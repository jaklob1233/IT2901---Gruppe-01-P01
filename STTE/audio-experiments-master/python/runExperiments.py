import math
import os.path

import yaml
import subprocess
import time
import shutil
from helper import command_helper
import logging
from logging import config as logging_config
from helper.path_helper import PathResolver
from helper.config_helper import ConfigHelper
from pydub import AudioSegment
from colorama import Back, Fore, Style
import threading


COLORS = {
    "WARNING": Fore.YELLOW,
    "INFO": Fore.CYAN,
    "DEBUG": Fore.BLUE,
    "CRITICAL": Fore.YELLOW,
    "ERROR": Fore.RED,
}


class ColoredFormatter(logging.Formatter):
    def __init__(self, *, format, use_color):
        logging.Formatter.__init__(self, fmt=format)
        self.use_color = use_color

    def format(self, record):
        msg = super().format(record)
        if self.use_color:
            levelname = record.levelname
            if hasattr(record, "color"):
                return f"{record.color}{msg}{Style.RESET_ALL}"
            if levelname in COLORS:
                return f"{COLORS[levelname]}{msg}{Style.RESET_ALL}"
        return msg


logger = logging.getLogger(__name__)


def run_experiments():
    config = ConfigHelper().get_config()

    # remove old results of this experiment
    if not config['skip_existing_results']:
        command_helper.run_local_command('rm -rf experiments/' + config['experiment_name'])

    samples = {}

    # specify sample names for each corpus
    for corpus in config['corpora']:
        samples_corpus = []
        if corpus['name'] == 'itu':
            if corpus['run_tests']:
                for l in corpus['languages']:
                    for g in corpus['gender']:
                        for i in range(1, corpus['samples_count'] + 1):
                            samples_corpus.append(l + "_" + g + str(i) + ".wav")
        elif corpus['name'] == 'abcmrt':
            if corpus['run_tests']:
                for s in corpus['speaker']:
                    for i in range(1, corpus['samples_count'] + 1):
                        for w in range(1, corpus['word_count'] + 1):
                            samples_corpus.append(s + "_b" + str(i) + '_w' + str(w) + ".wav")
        else:
            logger.error(f"Corpus {corpus['name']} not recognized")

        samples[corpus['name']] = samples_corpus

    # run experiments for all network profiles defined

    # stop radio emulator, linphone daemons and tcpdumps
    if not config['radio_emulator_already_running']:
        command_helper.run_ssh_command(config['radio_emulator_vm'], "/home/aut/stop_emulator.sh")
    stop_linphone_daemons(config['linphone_vms'])
    stop_tcpdump_on_radio_emulator(config['radio_emulator_vm'])
    time.sleep(10)

    for profile in config['network_profiles']:

        # skip profile if results already exist
        if config['skip_existing_results'] and os.path.isdir(
                'experiments/' + config['experiment_name'] + '/results/' + profile['name']):
            logger.info(f"skip profile {profile['name']} as results already exist")
            continue

        # copy audio files to experiment folder
        audio_samples_path = "experiments/" + config['experiment_name'] + "/original-audio/" + profile['name']
        copy_original_audio_files_to_experiment_folder(audio_samples_path, samples)

        # prepare audio files
        if profile['background_noise_file'] is not None:
            logger.info(
                f"pre-process audio files for profile '{profile['name']}' by adding background noise from '{profile['background_noise_file']}'")
            overlay_original_audio_with_background_noise(audio_samples_path, samples, profile['background_noise_file'])
        if config['add_silence_at_end_of_audio_files_milliseconds'] > 0:
            logger.info(
                f"pre-process audio files for profile '{profile['name']}' by adding silence ({config['add_silence_at_end_of_audio_files_milliseconds']} ms) at end of audio files")
            add_silence_at_end_of_audio_files(audio_samples_path, samples,
                                              config['add_silence_at_end_of_audio_files_milliseconds'])

        # run radio emulator with chosen profile
        if not config['radio_emulator_already_running']:
            sync_config_to_radio_emulator_vm(config['radio_emulator_vm'])
            command_helper.run_ssh_command_concurrent(config['radio_emulator_vm'],
                                                      "/home/aut/start_emulator.sh " + profile['scenario'])
            time.sleep(30)

        # run corpora samples
        for corpus in config['corpora']:
            if len(samples[corpus['name']]) != 0:
                logger.info(f"run {corpus['name']}")
                run_corpus_samples(config, config['experiment_name'], profile['name'], corpus['name'],
                                   samples[corpus['name']])
            else:
                logger.info(f"{corpus['name']} skipped")

        # stop radio emulator
        if not config['radio_emulator_already_running']:
            command_helper.run_ssh_command(config['radio_emulator_vm'], "/home/aut/stop_emulator.sh")
            time.sleep(10)

        # results from VM copied
        print(f"results from profile '{profile['name']}' copied, experiment with that profile finished")


def run_corpus_samples(config, experiment_name, profile_name, corpus_name, samples):
    logger = logging.getLogger(__name__)
    progress = 0

    try:
        for c in config['codecs']:
            logger.info("running new codec")
            # sync linphone-sdk config (codec c activated) and scripts to VMs
            sync_config_to_linphone_vms(config['linphone_vms'], c)

            # remove old results
            for host in config['linphone_vms']:
                command_helper.run_ssh_command(host, 'rm -rf /opt/linphone-sdk/received-audio')
                command_helper.run_ssh_command(host, 'mkdir /opt/linphone-sdk/received-audio')

            # remove old pcaps from radio emulator
            command_helper.run_ssh_command(config['radio_emulator_vm'],
                                           'rm -rf /home/aut/adapter/comsim-adapter/standalone/pcaps/*')

            # remove old logs from linphone-vms
            for host in config['linphone_vms']:
                command_helper.run_ssh_command(host,
                                               'rm -rf /opt/linphone-sdk/logs/*')

            # sync audio samples to vms
            sync_audio_samples_to_linphone_vms(config['linphone_vms'], experiment_name, profile_name, corpus_name)

            # start central logging, linphone daemons and tcpdumps
            start_central_logging(config, profile_name, corpus_name, c)
            start_linphone_daemons(config['linphone_vms'], "/opt/linphone-sdk/logs/linphone")
            start_tcpdump_on_radio_emulator(config['radio_emulator_vm'], config['radio_emulator_nodes_interfaces'],
                                            "/home/aut/adapter/comsim-adapter/standalone/pcaps")
            time.sleep(15)

            for s in samples:
                logger.info(f"start sample {s}")
                process2 = command_helper.run_ssh_command_concurrent(config['linphone_vms'][1],
                                                                     "/opt/linphone-sdk/bin/runLinphone2.sh " + s)
                time.sleep(2)
                process1 = command_helper.run_ssh_command_concurrent(config['linphone_vms'][0],
                                                                     "/opt/linphone-sdk/bin/runLinphone1.sh " + s)
                process1.communicate()
                process2.communicate()
                time.sleep(2)

                progress = progress + 1
                progress_corpus = math.floor(100 * progress / (len(config['codecs']) * len(samples)))
                logger.warning(f"Profile {profile_name}, corpus {corpus_name} (current codec {c}):\n|" +
                               progress_corpus * "■" + (100 - progress_corpus) * "-" + "|" + str(progress_corpus) + "%")

            # stop central logging, linphone daemons and tcpdumps
            stop_central_logging()
            stop_linphone_daemons(config['linphone_vms'])
            stop_tcpdump_on_radio_emulator(config['radio_emulator_vm'])
            time.sleep(5)

            # download received audio files, pcaps and logs
            download_received_audio_files(config, profile_name, corpus_name, c)
            download_logs_from_linphone_vms(config, profile_name, corpus_name, c)
            download_pcaps_from_radio_emulator(config, profile_name, corpus_name, c)
            download_logs_from_radio_emulator(config, profile_name, corpus_name, c)
    except Exception as e:
        logger.error(f"run_corpus_samples failed, error={e}")


def overlay_original_audio_with_background_noise(audio_samples_path, samples,
                                                 background_noise_file):
    background_sound = AudioSegment.from_file(f'background_noise/{background_noise_file}')

    for corpus_name in list(samples.keys()):
        for s in samples[corpus_name]:
            audio_file = audio_samples_path + '/' + corpus_name + '/' + s
            audio = AudioSegment.from_file(audio_file)

            combined = audio.overlay(background_sound)
            combined.export(audio_file, format='wav')


def add_silence_at_end_of_audio_files(audio_samples_path, samples, milliseconds):
    silence = AudioSegment.silent(milliseconds)

    for corpus_name in list(samples.keys()):
        for s in samples[corpus_name]:
            audio_file = audio_samples_path + '/' + corpus_name + '/' + s
            audio = AudioSegment.from_file(audio_file)

            combined = audio.append(silence, crossfade=0)
            combined.export(audio_file, format='wav')


def copy_original_audio_files_to_experiment_folder(audio_samples_path, samples):
    for corpus_name in list(samples.keys()):
        if len(samples[corpus_name]) != 0:
            command_helper.run_local_command(f'mkdir -p {audio_samples_path}/{corpus_name}')

            for s in samples[corpus_name]:
                command_helper.run_local_command(f'cp corpora/{corpus_name}/{s} {audio_samples_path}/{corpus_name}')


def download_received_audio_files(config, profile_name, corpus_name, codec):
    command_helper.run_local_command(
        'mkdir -p experiments/' + config[
            'experiment_name'] + '/results/' + profile_name + "/" + corpus_name + "/" + codec)

    password, user = command_helper.get_login(config['linphone_vms'][1])

    for host in config['linphone_vms']:
        subprocess.run(['sshpass', '-p', password, 'scp', '-r',
                        user + '@' + host + ':/opt/linphone-sdk/received-audio',
                        'experiments/' + config[
                            'experiment_name'] + '/results/' + profile_name + "/" + corpus_name + "/" + codec])


def download_logs_from_linphone_vms(config, profile_name, corpus_name, codec):
    password, user = command_helper.get_login(config['linphone_vms'][1])

    for host in config['linphone_vms']:
        subprocess.run(['sshpass', '-p', password, 'scp', '-r',
                        user + '@' + host + ':/opt/linphone-sdk/logs',
                        'experiments/' + config[
                            'experiment_name'] + '/results/' + profile_name + "/" + corpus_name + "/" + codec])


def download_logs_from_radio_emulator(config, profile_name, corpus_name, codec):
    host = config['radio_emulator_vm']
    password, user = command_helper.get_login(config['radio_emulator_vm'])
    command_helper.run_local_command(
        'mkdir -p experiments/' + config[
            'experiment_name'] + '/results/' + profile_name + "/" + corpus_name + "/" + codec + "/logs/radio_emulator_logs")
    subprocess.run(['sshpass', '-p', password, 'scp', '-r',
                    user + '@' + host + ':/home/aut/adapter/comsim-adapter/logs/*',
                    'experiments/' + config[
                        'experiment_name'] + '/results/' + profile_name + "/" + corpus_name + "/" + codec + "/logs/radio_emulator_logs"])


def download_pcaps_from_radio_emulator(config, profile_name, corpus_name, codec):
    host = config['radio_emulator_vm']
    password, user = command_helper.get_login(host)
    subprocess.run(['sshpass', '-p', password, 'scp', '-r',
                    user + '@' + host + ':/home/aut/adapter/comsim-adapter/standalone/pcaps',
                    'experiments/' + config[
                        'experiment_name'] + '/results/' + profile_name + "/" + corpus_name + "/" + codec])


def sync_config_to_linphone_vms(linphone_vms, codec):
    # linphone-sdk config and scripts to VMs
    shutil.copy("deployment-files/linphone/home/toor/.linphonec_" + codec, "deployment-files/linphone/home/toor/.linphonec")
    for host in linphone_vms:
        command_helper.run_local_command("./sync_config_to_linphone_vm.sh " + host)


def sync_audio_samples_to_linphone_vms(linphone_vms, experiment_name, profile_name, corpus_name):
    # sync audio samples to VMs
    for host in linphone_vms:
        command_helper.run_local_command(
            "./sync_audio_samples_to_linphone_vm.sh " + host + " " + "experiments/" + experiment_name
            + "/original-audio/" + profile_name + "/" + corpus_name)


def sync_config_to_radio_emulator_vm(radio_emulator_vm):
    command_helper.run_local_command("./sync_config_to_radio_emulator_vm.sh " + radio_emulator_vm)


def start_central_logging(config, profile_name, corpus_name, codec):
    path = 'experiments/' + config['experiment_name'] + '/results/' + profile_name + '/' + corpus_name + '/' + codec + '/logs'
    print("Norman: " + path)
    command_helper.run_local_command('mkdir -p ' + path)
    command_helper.run_local_command("./runCentralLogger.sh " + path)
    logger.info("Central Logger started.")


def stop_central_logging():
    command_helper.run_local_command("./stopCentralLogger.sh")
    logger.info("Central Logger stopped.")


def stop_linphone_daemon(host):
    """Stops Linphone Daemon on a given host."""
    command_helper.run_ssh_command(host, "/opt/linphone-sdk/bin/stopLinphoneDaemon.sh")
    logger.info(f"Linphone daemon on {host} stopped.")


def stop_linphone_daemons(linphone_vms):
    """Stops Linphone Daemons on all given hosts in parallel."""
    threads = []

    for host in linphone_vms:
        thread = threading.Thread(target=stop_linphone_daemon, args=(host,))
        threads.append(thread)
        thread.start()

    # Wait for all threads to complete
    for thread in threads:
        thread.join()

    logger.info("All Linphone daemons stopped.")


def start_linphone_daemons(linphone_vms, log_file_path):
    i = 0
    for host in linphone_vms:
        i = i + 1
        command_helper.run_ssh_command(host, 'mkdir -p ' + log_file_path)
        command_helper.run_ssh_command(host,
                                       "/opt/linphone-sdk/bin/runLinphoneDaemon.sh " + log_file_path + "/" + "linphone_"
                                       + str(i) + ".log")
        logger.info(f"linphone daemon {host} started.")


def stop_tcpdump_on_radio_emulator(host):
    command_helper.run_ssh_command(host, "/home/aut/adapter/comsim-adapter/standalone/stopTcpdump.sh")
    logger.info(f"tcpdump on {host} stopped.")


def start_tcpdump_on_radio_emulator(host, interfaces, file_path):
    i = 0
    for interface in interfaces:
        i = i + 1
        command_helper.run_ssh_command_concurrent(host,
                                                  "/home/aut/adapter/comsim-adapter/standalone/runTcpdump.sh "
                                                  + interface + " "
                                                  + file_path + "/node_" + str(i) + ".pcap")
        logger.info(f"tcpdump for node {i} on {host} started.")


def main():
    logging_configuration = yaml.safe_load(open(PathResolver.LOGGING_CONFIG))
    logging_config.dictConfig(logging_configuration)

    run_experiments()


if __name__ == "__main__":
    main()

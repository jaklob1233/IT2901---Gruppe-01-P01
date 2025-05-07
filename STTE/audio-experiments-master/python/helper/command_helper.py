import subprocess
from helper.config_helper import ConfigHelper

config = ConfigHelper().get_config()

def run_ssh_command(host, command):
    command_array = command.strip().split(' ')
    password, user = get_login(host)

    cmd = ["sshpass", "-p", password, "ssh", "-o", "StrictHostKeyChecking=accept-new", user + "@" + host] + command_array
    subprocess.run(cmd)


def run_ssh_command_concurrent(host, command):
    command_array = command.strip().split(' ')
    password, user = get_login(host)

    cmd = ["sshpass", "-p", password, "ssh", "-o", "StrictHostKeyChecking=accept-new", user + "@" + host] + command_array
    process = subprocess.Popen(cmd)
    return process


def get_login(host):
    if host == config['radio_emulator_vm']:
        user = config['radio_emulator_login'][0]
        password = config['radio_emulator_login'][1]
    else:
        user = config['linphone_vm_login'][0]
        password = config['linphone_vm_login'][1]
    return password, user


def run_local_command(command):
    command_array = command.strip().split(' ')
    cmd = command_array
    return subprocess.run(cmd)


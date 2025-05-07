#!/bin/sh
if [ -z "$1" ]; then
  echo "Missing argument linphone_vm_ip. Usage sync2vm.sh [linphone_vm_ip] [path_to_audio_samples]"
  exit 0
fi
if [ -z "$2" ]; then
  echo "Missing argument path_to_audio_samples. Usage sync2vm.sh [Linphone-VM IP] [path_to_audio_samples]; e.g. experiments/[experiment_name]/original-audio/narrowband/itu"
  exit 0
fi

LINPHONE_VM_IP=$1
PATH_TO_AUDIO_SAMPLES=$2

# sync audio-samples to VMs
rsync -v -rlt -z --stats --progress --mkpath --chmod=a=rwx -p "/home/toor/projects/IST-201/audio-experiments/$PATH_TO_AUDIO_SAMPLES/" "$LINPHONE_VM_IP::linphone-sdk/audio-samples"

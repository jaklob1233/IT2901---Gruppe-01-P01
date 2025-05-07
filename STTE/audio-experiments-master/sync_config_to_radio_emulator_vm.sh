#!/bin/sh
if [ -z "$1" ]; then
  echo "Missing argument radio_emulator_vm_ip. Usage sync2vm.sh [radio_emulator_vm_ip]"
  exit 0
fi

RADIO_EMULATOR_VM_IP=$1

# sync config files to VM
rsync -v -rlt -z --stats --progress --chmod=a=rwx -p "/home/toor/projects/IST-201/audio-experiments/deployment-files/radio_emulator/adapter/" "$RADIO_EMULATOR_VM_IP::adapter/"

#!/bin/sh
if [ -z "$1" ]; then
  echo "Missing argument linphone_vm_ip. Usage sync2vm.sh [linphone_vm_ip]"
  exit 0
fi

LINPHONE_VM_IP=$1

# sync scripts to VM
rsync -v -rlt -z --stats --progress --mkpath --chmod=a=rwx -p "/home/toor/projects/IST-201/audio-experiments/deployment-files/linphone/opt/linphone-sdk/" "$LINPHONE_VM_IP::linphone-sdk"

# sync config files of linphone-sdk and plugins to VM
rsync -v -rlt -z --stats --progress --mkpath --chmod=a=rwx -p "/home/toor/projects/IST-201/audio-experiments/deployment-files/linphone/home/toor/" "$LINPHONE_VM_IP::toor/"

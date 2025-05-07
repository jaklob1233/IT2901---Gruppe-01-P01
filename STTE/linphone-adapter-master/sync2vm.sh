#!/bin/sh

LINPHONE_VM_1_IP=192.168.100.11
LINPHONE_VM_2_IP=192.168.100.12

# copy run scripts to linphone-sdk
./copyDeploymentFiles.sh

# sync linphone-sdk to VMs
rsync -v -rlt -z --stats --progress --mkpath --chmod=a=rwx -p --delete "/opt/linphone-sdk/" "$LINPHONE_VM_1_IP::linphone-sdk"
rsync -v -rlt -z --stats --progress --mkpath --chmod=a=rwx -p --delete "/opt/linphone-sdk/" "$LINPHONE_VM_2_IP::linphone-sdk"

# sync config files of linphone, mslyra, msmelp and msmelpwb to VMs
rsync -v -rlt -z --stats --progress --mkpath --chmod=a=rwx -p "/home/toor/projects/IST-201/linphone-adapter/deployment-files/home/toor/" "$LINPHONE_VM_1_IP::toor/"
rsync -v -rlt -z --stats --progress --mkpath --chmod=a=rwx -p "/home/toor/projects/IST-201/linphone-adapter/deployment-files/home/toor/" "$LINPHONE_VM_2_IP::toor/"

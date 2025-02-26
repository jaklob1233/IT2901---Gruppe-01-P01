# AuT VM template for linphone-sdk

## Name
Linphone-sdk VM template

## Description
Describes how to create a Linphone-sdk VM template for AuT. 

## Creation of Linphone-sdk VM template

### 1. Create VM
Create Debian 12 VM with root/root and toor/toor
   
### 2. Install dependencies
- User rights
  su
  apt install sudo
  sudo usermod -aG sudo toor
- Install linphone-sdk dependencies (see install-linphone-sdk-dependencies.md)
- (Install Java 8)
- install rsync
  sudo apt install rsync
  sudo systemctl enable rsync

  sudo mkdir /opt/linphone-sdk
  sudo chown toor /opt/linphone-sdk

  use this config (etc/rsyncd.conf)
  [linphone-sdk]
  path = /opt/linphone-sdk
  list = true
  uid = toor
  gid = toor
  read only = false
  [toor]
  path = /home/toor
  list = true
  uid = toor
  gid = toor
  read only = false

### 3. Deploy linphone-sdk to VM
Sync linphone-sdk installation directory to VM (see linphone-adapter/README.md)

### 4. Import VM to ESXi

### 5. AuT integration
- Add AuT scripts to VM (changeIp, downloadAndRunAdapter)
- Take snapshot of VM
- Adapt (AuT) Linphone adapter/editor plugin if necessary
- adapt AuT config files regarding linphone template

## Authors and acknowledgment
Norman JANSEN, Fraunhofer FKIE

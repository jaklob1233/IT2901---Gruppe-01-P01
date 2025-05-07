# linphone-adapter

## Name
Linphone Adapter

## Description
This project integrates Google Lyra, Speechtext, MELPe and MELPWB into the Linphone-sdk and thus allows to run these codecs in VoIP calls for testing/comparing different codecs. 

## Installation

### 1. clone this project from gitlab
    $ git clone -b master git@gitlab.cc-asp.fraunhofer.de:ist-201/linphone-adapter.git --recurse-submodules
   
### 2. install own code and adaptations of code into linphone-sdk and lyra projects
This step is necessary since we don't have write access to the linphone-sdk repo and lyra repo. Thus, we could not commit our changes there.

    $ ./copyCodeToLinphoneSdkAndLyra.sh

### 3. build and install lyra and linphone-sdk
    $ ./buildProject.sh

### 4. copy deployment files (scripts) to deployment directory (deployment directory is /opt/linphone-sdk)
    $ ./copyDeploymentFiles.sh

### 5. sync deployment to two test VMs (this step includes step 4)
VMs with rsync server linking 'linphone-sdk' to '/usr/share/linphone-sdk' and 'toor' to '/home/toor' must be prepared.
I used Debian 12.
IPs must be adapted in 'sync2vms.sh' script.

    $ ./sync2vms.sh
       
## Further development
The project (linphone-adapter) can be opened and build in CLion to simplify the development of the mslyra plugin which I have written to integrate Lyra in linphone-sdk.

Use WSL as toolchain and build type 'RelWithDebInfo' in CLion and set these CMake options:

    -G Ninja -DENABLE_GPL_THIRD_PARTIES=ON -DCMAKE_INSTALL_PREFIX=/opt/linphone-sdk -DCMAKE_WARN_DEPRECATED=FALSE -DENABLE_DB_STORAGE=NO -DENABLE_VCARD=OFF -DENABLE_OPENH264=NO -DENABLE_MKV=NO -DENABLE_VIDEO=NO -DENABLE_NON_FREE_CODECS=ON -DENABLE_SILK=ON -DENABLE_LYRA=ON -DENABLE_SPEECHTEXT=ON -DENABLE_MELP=ON -DENABLE_MELPWB=ON -DENABLE_CODEC2=ON -DENABLE_G729=ON -DENABLE_CONSOLE_UI=ON -DCMAKE_INSTALL_PREFIX:PATH=/opt/linphone-sdk

Folder /opt/linphone-sdk must be created and ownership changed to toor. 

After editing the code of mslyra/msmelp/msmelpwb (in linphone-sdk), the corresponding script of the list below can be used to copy theses changes to the code-files directory: 

    $ ./copyMsLyraFromLinphoneSdkToLinphoneAdapter.sh
    $ ./copyMsMelpFromLinphoneSdkToLinphoneAdapter.sh
    $ ./copyMsMelpWbFromLinphoneSdkToLinphoneAdapter.sh

Then the changed code-files can be committed and pushed to gitlab. This step is necessary since we don't have write access to the linphone-sdk repo and lyra repo.

## Configuration of lyra/melp/melpwb plugin

The Lyra plugin searches for a configuration file using this strategy

+ a file specified with the **LIBMSLYRA_CONF** environment variable
+ a **libmslyra.conf** file in the standard linphone directory **$HOME/.config/linphone/**

The accepted options are:

| Option              |                                                    Description |
|---------------------|---------------------------------------------------------------:|
| dtx                 | use discontinuous transmission (DTX)   (**true** or **false**) |
| noise_preprocessing | apply noise pre-processing             (**true** or **false**) |
| fin                 |           replace micro by file content in S16_LE@16Khz format |
| fout                |                          record to file in S16_LE@16Khz format |

A template is provided under the **resources** directory.

The MELP/MELPWB plugins search for a configuration file using this strategy

+ a file specified with the **LIBMSMELP_CONF**/**LIBMSMELPWB_CONF** environment variable
+ a **libmsmelp.conf**/**libmsmelpwb.conf** file in the standard linphone directory **$HOME/.config/linphone/**

The accepted options are:

| Option |                                          Description |
|--------|-----------------------------------------------------:|
| npp    | apply noise pre-processing   (**true** or **false**) |
| pf     | apply post filtering         (**true** or **false**) |
| fin    | replace micro by file content in S16_LE@16Khz format |
| fout   | record to file in S16_LE@16Khz format                |

A template is provided under the **resources** directory.

## Usage
### In VM 2
    $ cd /opt/linphone-sdk
    $ runLinphoneTest2.sh

### In VM 1 (after running Linphone in VM 2)
    $ cd /opt/linphone-sdk
    $ runLinphoneTest1.sh

## Authors and acknowledgment
Norman JANSEN, Fraunhofer FKIE

# MELP plugin for mediastreamer2

This note describes how to build, configure and test the MELP plugin for mediastreamer2.

----



## Requirements

The following software are required:

|   Package    | Version |             |
|--------------|--------:|------------:|
| cmake        |  3.22.1 |    required |
| git          |  2.34.1 |    required |
| gcc          |  11.3.0 |    required |
| appimagetool |     N/A | recommended |



## Build

 + unzip the zip file **mst2-melp-1.0.0@cbcc50e8209.zip**.
```
$ unzip mst2-melp-1.0.0@cbcc50e8209.zip
```

A **mst2-melp-1.0.0@cbcc50e8209** directory is created containing:

```
.
+- CMakeLists.txt       cmake file to build the MELP plugin
|
+- libs
|  |
|  +- mediastreamer2    mediastreamer2 headers and symbols dependencies
|  |
|  +- melp              precompiled MELP for debian 12 and ubuntu 22.04
|
+- resources
|  |
|  +- libmsmelp.conf    template for optional configuration file
|
+- sources              actual plugin sources
```

_NB: the MELP library is selected according to the ID value from /etc/os-release_


  + Compile
```
$ mkdir path/to/build
$ cmake -B path/to/build path/to/project -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_INSTALL_PREFIX=/path/to/install
$ cmake --build path/to/build
```

_NB: the plugin is available in /path/to/install/lib directory._



## Configure

The plugin searches for a configuration file using this strategy

  + a file specified with the **LIBMSMELP_CONF** environment variable
  + a **libmsmelp.conf** file in the standard linphone directory **$HOME/.config/linphone/**

The accepted options are:

| Option |                                          Description |
|--------|-----------------------------------------------------:|
| npp    | apply noise pre-processing   (**true** or **false**) |
| pf     | apply post filtering         (**true** or **false**) |
| fin    | replace micro by file content in S16_LE@16Khz format |
| fout   | record to file in S16_LE@16Khz format                |

A template is provided under the **resources** directory.


## Test

Use this procedure to test the plugin.

  + Download linphone
```
$ wget https://download.linphone.org/releases/linux/app/Linphone-5.0.18.AppImage
```

  + Install the plugin into linphone and rebuild linphone
```
$ ./Linphone-5.0.18.AppImage --appimage-extract
$ cp /path/to/install/lib/libmsmelp.so squashfs-root/usr/plugins/mediastreamer
$ appimagetool squashfs-root/Linphone-5.0.18+melp.AppImage
```

   + Configure Linphone

Under "Preference > Network" menu, set **Enable ICE** and **STUN/TURN server** to local IPv4 address (e.g. 192.168.0.102) to allow direct IP call (no sip server).
Add an audio section to **~/.config/linphone/linphonerc**, assuming the last audio codec is numbered 12,
```
[audio_codec_13]
mime=MELP2400
rate=8000
channels=1
enabled=1
```

Restart Linphone, then in "Preference > Audio" menu, drag'n drop the audio codecs to order them by priority and use the switch to enable/disable the codecs.
Finally, start a call by typing in the "research bar" **sip:identifier@IPv4 address** (e.g. sip:someone@192.168.0.101 - the actual identifier is meaningless)





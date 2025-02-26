# MELP wideband plugin for mediastreamer2

This note describes how to build, configure and test the MELP wideband plugin for mediastreamer2.

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

 + unzip the zip file **mst2-melpwb-1.0.0@ff85621fe2d.zip**.
```
$ unzip mst2-melpwb-1.0.0@ff85621fe2d.zip
```

A **mst2-melpwb-1.0.0@ff85621fe2d** directory is created containing:

```
.
+- CMakeLists.txt       cmake file to build the MELP plugin
|
+- libs
|  |
|  +- mediastreamer2    mediastreamer2 headers and symbols dependencies
|  |
|  +- melpwb            precompiled MELP wideband for debian 12 and ubuntu 22.04
|
+- resources
|  |
|  +- libmsmelpwb.conf  template for optional configuration file
|
+- sources              actual plugin sources
```

_NB: the version of MELP wideband library is selected according to the ID value from /etc/os-release_


  + Compile
```
$ mkdir path/to/build
$ cmake -B path/to/build path/to/project -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_INSTALL_PREFIX=/path/to/install
$ cmake --build path/to/build --target install
```

_NB: the plugin is available in /path/to/install/lib directory._



## Configure (optional)

The plugin searches for a configuration file using this strategy

  + a file specified with the **LIBMSMELPWB_CONF** environment variable
  + a **libmsmelpwb.conf** file in the standard linphone directory **$HOME/.config/linphone/**

The accepted options are:

| Option  |                                          Description |
|---------|-----------------------------------------------------:|
| npp     | apply noise pre-processing   (**true** or **false**) |
| pf      | apply post filtering         (**true** or **false**) |
| fin     | replace micro by file content in S16_LE@16Khz format |
| fout    | record to file in S16_LE@16Khz format                |

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
$ cp /path/to/install/lib/libmsmelpwb.so squashfs-root/usr/plugins/mediastreamer
$ appimagetool squashfs-root/Linphone-5.0.18+melp.AppImage
```

   + Configure Linphone

Under "Preference > Network" menu, set **Enable ICE** and **STUN/TURN server** to local IPv4 address (e.g. 192.168.0.102) to allow direct IP call (no sip server).
Add an audio section to **~/.config/linphone/linphonerc**,
```
[audio_codec_0]
mime=MELPWB
rate=16000
channels=1
enabled=1
```

Restart Linphone, then in "Preference > Audio" menu, drag'n drop the audio codecs to order them by priority and use the switch to enable/disable the codecs.
Finally, start a call by typing in the "research bar" **sip:identifier@IPv4 address** (e.g. sip:someone@192.168.0.101 - the actual identifier is meaningless)





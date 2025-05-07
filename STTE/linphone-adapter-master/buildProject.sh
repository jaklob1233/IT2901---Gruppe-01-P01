#!/bin/sh
# build lyra
cd lyra
chmod 755 build.sh
./build.sh
cd ..

# build linphone-sdk
/usr/bin/cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo -G Ninja -DCMAKE_MAKE_PROGRAM=/usr/bin/ninja -DCMAKE_WARN_DEPRECATED=FALSE -DENABLE_DB_STORAGE=NO -DENABLE_VCARD=OFF -DENABLE_OPENH264=NO -DENABLE_MKV=NO -DENABLE_VIDEO=NO -DENABLE_NON_FREE_CODECS=ON -DENABLE_SILK=ON -DENABLE_LYRA=ON -DENABLE_MELP=ON -DENABLE_MELPWB=ON -S /home/toor/projects/IST-201/linphone-adapter/linphone-sdk -B /home/toor/projects/IST-201/linphone-adapter/linphone-sdk/cmake-build-relwithdebinfo
cd linphone-sdk/cmake-build-relwithdebinfo
cmake --build .

# install linphone-sdk
cmake --install .
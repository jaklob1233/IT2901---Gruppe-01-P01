# Dependencies needed for building and running linphone-sdk (e.g. in WSL2), collected (amongst others) from bc-dev-ubuntu-18-04-lts docker file
## Install development tools
    $ sudo apt-get update
    $ sudo apt-get install -y alien at autoconf bison ccache clang cmake doxygen elfutils g++ gdb git graphviz intltool libtool lsb-release make nasm ninja-build openssh-client patch python3-pip python3-pystache python3-six yasm

## Install linphone & flexisip dependencies development packages
    $ sudo apt-get install -y libasound2-dev libavcodec-dev libavutil-dev libbsd-dev libegl1-mesa-dev libglew-dev libgsm1-dev libjansson-dev libmariadb-dev-compat libmbedtls-dev libopus-dev libpq-dev libprotobuf-dev libpulse-dev libqt5svg5-dev libsnmp-dev libspeex-dev libspeexdsp-dev libsqlite3-dev libsrtp2-dev libssl-dev libswscale-dev libturbojpeg0-dev libv4l-dev libvpx-dev libxerces-c-dev libxml2-dev libxv-dev protobuf-compiler qttools5-dev qttools5-dev-tools xsdcxx

## Install mediastreamer test dependencies
    $ sudo apt-get install -y alsa-utils pulseaudio

## Install OpenApi server (pistache-server) dependencies
    $ sudo apt-get install -y libpistache-dev

## Install OpenApi client dependencies
    sudo apt install binutils
    sudo apt install build-essential
    wget http://curl.haxx.se/download/curl-7.58.0.tar.gz
    tar -xvf curl-7.58.0.tar.gz
    cd curl-7.58.0/
    ./configure
    make
    sudo make install

## apt clean
    $ sudo apt-get clean

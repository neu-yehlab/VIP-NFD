FROM ubuntu:18.04
RUN apt-get update && \
    apt-get install -y -qq apt-utils clang-6.0 clang-format-6.0 curl doxygen git go-bindata libc6-dev-i386 libelf-dev libnuma-dev libssl-dev liburcu-dev rake socat sudo yamllint software-properties-common wget
RUN add-apt-repository ppa:ubuntu-toolchain-r/test
RUN apt-get install -y -qq gcc-7 g++-7 gcc-8 g++-8 gcc-9 g++-9 cmake
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-9 90 --slave /usr/bin/g++ g++ /usr/bin/g++-9 --slave /usr/bin/gcov gcov /usr/bin/gcov-9
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-8 80 --slave /usr/bin/g++ g++ /usr/bin/g++-8 --slave /usr/bin/gcov gcov /usr/bin/gcov-8
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-7 70 --slave /usr/bin/g++ g++ /usr/bin/g++-7 --slave /usr/bin/gcov gcov /usr/bin/gcov-7
RUN curl -L https://dl.google.com/go/go1.13.linux-amd64.tar.gz | tar -C /usr/local -xz
RUN curl -L https://github.com/iovisor/ubpf/archive/644ad3ded2f015878f502765081e166ce8112baf.tar.gz | tar -C /tmp -xz && \
    cd /tmp/ubpf-*/vm && \
    make && \
    mkdir -p /usr/local/include /usr/local/lib && \
    cp inc/ubpf.h /usr/local/include/ && \
    cp libubpf.a /usr/local/lib/
RUN curl -sL https://deb.nodesource.com/setup_12.x | bash - && \
    apt-get install -y -qq nodejs && \
    npm install -g jayson
RUN apt-get update
RUN apt-get install -y -qq build-essential pkg-config libboost-all-dev \
    libsqlite3-dev libssl-dev libpcap-dev python-minimal psmisc vim
#RUN apt-get autoremove -y -qq libboost1.65-dev 
RUN cd /root/ && \
    wget https://dl.bintray.com/boostorg/release/1.71.0/source/boost_1_71_0.tar.bz2 && \
    tar --bzip2 -xf ./boost_1_71_0.tar.bz2 && \
    cd boost_1_71_0 && \
    bash ./bootstrap.sh --libdir=/usr/lib/x86_64-linux-gnu --includedir=/usr/include 
RUN cd /root/boost_1_71_0/ && \
    ./b2 install
#RUN rpm -i boost1_69_0/boost-license1_69_0-1.69.0-1.x86_64.rpm \
      # boost1_69_0/libboost_* boost1_69_0/boost-devel-1.69.0-1.x86_64.rpm
RUN cd /root/ && \
    git clone https://github.com/named-data/ndn-cxx.git && \
    cd ./ndn-cxx/ && \
    git checkout 0d748af32fd && \
    ./waf configure && \
    ./waf && \
    ./waf install && \
    ldconfig
RUN cd /root/ && \
    git clone https://github.com/neu-yehlab/VIP-NFD.git && \
    cd ./VIP-NFD/NFD-NFD-0.6.6 && \
    mkdir -p websocketpp && \
    curl -L https://github.com/cawka/websocketpp/archive/0.8.1-hotfix.tar.gz > websocketpp.tar.gz && \
    tar xf websocketpp.tar.gz -C websocketpp/ --strip 1 && \
    ./waf configure && \
    ./waf && \
    ./waf install && \
    cp /usr/local/etc/ndn/nfd.conf.sample /usr/local/etc/ndn/nfd.conf && \
    ldconfig
RUN cd /root/ && \
    git clone https://github.com/cmscaltech/sandie-ndn.git && \
    cd ./sandie-ndn && \
    git checkout sc19 && \
    cd ./xrootd-ndn-oss-plugin/ && \
    mkdir ./build && \ 
    cd ./build
    #cmake ../ && \
    #make xrdndn-consumer && make install xrdndn-consumer
    #make xrdndn-producer && make install xrdndn-producer

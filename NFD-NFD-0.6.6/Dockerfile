FROM ubuntu:18.04
RUN apt-get update && \
    apt-get install -y -qq clang-6.0 clang-format-6.0 curl doxygen git go-bindata libc6-dev-i386 libelf-dev libnuma-dev libssl-dev liburcu-dev rake socat sudo yamllint
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


FROM centos:centos7
ENV HOME /root

RUN yum -y install https://dl.fedoraproject.org/pub/epel/epel-release-latest-7.noarch.rpm
RUN rpm --import http://linuxsoft.cern.ch/wlcg/RPM-GPG-KEY-wlcg
RUN yum -y install http://linuxsoft.cern.ch/wlcg/centos7/x86_64/wlcg-repo-1.0.0-1.el7.noarch.rpm
RUN yum -y install alicexrdplugins

RUN yum -y install sudo vim htop git curl.x86_64 bind-utils.x86_64 \
        redhat-lsb.x86_64 bzip2.x86_64 libicu libpcap initscripts \
        libstdc++-devel net-tools cmake3.x86_64 xrootd-client.x86_64 \
        xrootd-client-devel.x86_64 xrootd-client-libs.x86_64 \
        xrootd-devel.x86_64 xrootd-libs.x86_64 xrootd-server.x86_64 \
        xrootd-server-devel.x86_64 xrootd-server-libs.x86_64 centos-release-scl \
        build-essential pkg-config libboost-all-dev \
        openssl-devel libssl-devel libpcap-devel sqlite-devel
RUN yum -y install devtoolset-8-gcc devtoolset-8-gcc-c++
  #  scl enable devtoolset-8 bash and add path of /usr/local/bin for nfd command
RUN sudo sh -c "printf '\
export PATH=\$PATH:/opt/rh/devtoolset-8/root/usr/bin\n\
export PATH=\$PATH:/usr/local/bin\n\
scl_source enable devtoolset-8\
' > /etc/profile.d/container.sh"

# Install NDN Software Stack
WORKDIR ${HOME}
RUN git clone https://github.com/cmscaltech/sandie-ndn.git
RUN git clone https://github.com/cmscaltech/sandie-ndn-repo.git

WORKDIR ${HOME}/sandie-ndn-repo/RPMS/x86_64/
RUN rpm -i boost1_69_0/boost-license1_69_0-1.69.0-1.x86_64.rpm \
        boost1_69_0/libboost_* boost1_69_0/boost-devel-1.69.0-1.x86_64.rpm \
        ndn-cxx-0.7.0/libndn-cxx-* ndn-tools-0.7/ndn-tools-0.7-10.el7.x86_64.rpm
#RUN ARCH=$( /bin/arch ) && \
#    subscription-manager repos --enable "codeready-builder-for-rhel-8-${ARCH}-rpms" && \
#    dnf config-manager --set-enabled powertools && \
#    yum install https://dl.fedoraproject.org/pub/epel/epel-release-latest-8.noarch.rpm
RUN source scl_source enable devtoolset-8 && \
    cd /root/ && \
    git clone https://github.com/neu-yehlab/VIP-NFD.git && \
    #cd ./VIP-NFD && \
    #git checkout 4bc0dd26c9e59044f0ef56f7a305e2263ac4fe2a && \
    #cd ./nfd-0.7.0 && \
    #mkdir -p websocketpp && \
    #curl -L https://github.com/cawka/websocketpp/archive/0.8.1-hotfix.tar.gz > websocketpp.tar.gz && \
    #tar xf websocketpp.tar.gz -C websocketpp/ --strip 1 && \
    #./waf configure && \
    #./waf && \
    #./waf install && \
    git clone https://github.com/named-data/NFD.git && \
    cd ./NFD && \
    git checkout NFD-0.7.0 && \
    cp -r ../VIP-NFD/nfd-0.7.0/daemon/fw/. ./daemon/fw && \
    cp -r ../VIP-NFD/nfd-0.7.0/daemon/table/. ./daemon/table && \
    mkdir -p websocketpp && \
    curl -L https://github.com/cawka/websocketpp/archive/0.8.1-hotfix.tar.gz > websocketpp.tar.gz && \
    tar xf websocketpp.tar.gz -C websocketpp/ --strip 1 && \
    ./waf configure && \
    ./waf && \
    ./waf install && \
    cp /root/VIP-NFD/nfd-0.7.0/nfd.conf.sample /usr/local/etc/ndn/nfd.conf && \
    cp -r /root/VIP-NFD/nfd-0.7.0/daemon/fw/CMS_File_Configuration /usr/local/etc/ndn/ && \
    ldconfig
RUN rpm -i xrdndn-consumer-0.2.0-4.el7.x86_64.rpm \
        xrdndn-producer-0.2.0-4.el7.x86_64.rpm xrootd-ndn-fs-0.2.0-4.el7.x86_64.rpm
WORKDIR ${HOME}/sandie-ndn

# Build xrdndn-consumer xrdndn-producer xrdndn-fs
RUN mkdir -p ${HOME}/sandie-ndn/xrootd-ndn-oss-plugin/build
WORKDIR ${HOME}/sandie-ndn/xrootd-ndn-oss-plugin/build
RUN source ~/.bashrc && cmake3 ../
RUN make && \ 
    make install

WORKDIR ${HOME}

# VIP-NFD
## Introduction 
This repo implements [VIP algorithm](https://dl.acm.org/doi/abs/10.1145/2660129.2660151?casa_token=0kABuDRdS8sAAAAA:qPlzF3Rg9s8yRwQpoqU3Nbggr-7_gAwqoG2fFuKSAPsjypPwL-ETqg-hL0-zaV2LaiDzggSR1xFi-A
) based on [NFD-0.7.0](https://github.com/named-data/NFD/tree/NFD-0.7.0). VIP algorithm is a kind of joint forwarding and caching algorithm, so it contains both an optional forwardeing strategy and a caching policy. The forwarding part is implemented in [/NFD-0.7.0/daemon/fw/](https://github.com/neu-yehlab/VIP-NFD/tree/master/NFD-NFD-0.7.0/daemon/fw) and the caching part is implemented in [/NFD-0.7.0/daemon/table/](https://github.com/neu-yehlab/VIP-NFD/tree/master/nfd-0.7.0/daemon/table). 

## Installation
VIP algorithm is implemented as optional forwarding strategy and caching policy in NFD. You need to install [NFD-0.7.0](https://github.com/named-data/NFD/tree/NFD-0.7.0) first.
The [NFD-0.7.0](https://github.com/named-data/NFD/tree/NFD-0.7.0) installation instructions can be found here: [Install.rst](https://github.com/named-data/NFD/blob/NFD-0.7.0/docs/INSTALL.rst).
After installing [NFD-0.7.0](https://github.com/named-data/NFD/tree/NFD-0.7.0), move the files in this repo to the corresponding directory and copy [CMS_File_Configuration] to 
```bash
/usr/local/etc/ndn/
```
* Notice: 
NFD is based on [ndn-cxx](https://github.com/named-data/ndn-cxx).
According to applications you use, Sometimes you need to change the max packet size in [ndn-cxx](https://github.com/named-data/ndn-cxx). if you want to change max packet size (8800 bytes ad default): `Change variable:  const size_t MAX_NDN_PACKET_SIZE in file: ndn-cxx/encoding/tlv.hpp`

* Use Docker:
You can also use docker to install it
```bash
sudo docker build . --no-cache -t nfddockerimg:3.0.1  --network=host
sudo docker run -d -it --name=VIP-NFD --cap-add=NET_ADMIN --net=host nfddockerimg:3.0.1
sudo docker exec -it VIP-NFD /bin/bash
```
You could change configure files under [CMS_File_Configuration](https://github.com/neu-yehlab/VIP-NFD/tree/master/nfd-0.7.0/daemon/fw/CMS_File_Configuration) to register chunk level names, data object level names, and data object size. You can also see some parameters hard coded in the file [VIP_configure.hpp](https://github.com/neu-yehlab/VIP-NFD/blob/master/nfd-0.7.0/daemon/fw/VIP_configure.hpp). The details will be introduced in the "Usage" part.

## Usage
Similar to [NFD-0.7.0](https://github.com/named-data/NFD/tree/NFD-0.7.0). Since VIP algorithm is implemented as an optional strategy, you can choose it in the configure file: [nfd.conf.sample.in](https://github.com/neu-yehlab/VIP-NFD/blob/master/nfd-0.7.0/nfd.conf.sample.in) by changing `cs_policy` and `strategy_choice` just like any other available strategy and copy it to `/usr/local/etc/ndn/nfd.conf`. This part is shown as follows:  
```bash
; The tables section configures the CS, PIT, FIB, Strategy Choice, and Measurements
tables
{
  ; ContentStore size limit in number of packets
  ; default is 65536, about 500MB with 8KB packet size
  cs_max_packets 10

  ; Set the CS replacement policy.
  ; Available policies are: priority_fifo, lru
  cs_policy VIP

  ; Set a policy to decide whether to cache or drop unsolicited Data.
  ; Available policies are: drop-all, admit-local, admit-network, admit-all
  cs_unsolicited_policy drop-all

  ; Set the forwarding strategy for the specified prefixes:
  ;   <prefix> <strategy>
  strategy_choice
  {
    /               /localhost/nfd/strategy/best-route
    /localhost      /localhost/nfd/strategy/multicast
    /localhost/nfd  /localhost/nfd/strategy/best-route
    /ndn/broadcast  /localhost/nfd/strategy/multicast
    /ndn/VIP        /localhost/nfd/strategy/VIP
    /ndn/asf        /localhost/nfd/strategy/asf
  }
```
Considering the variant sizes and hierarchical features for CMS data (file < datablock < dataset), there is a configure file folder [CMS_File_Configuration](https://github.com/neu-yehlab/VIP-NFD/tree/master/nfd-0.7.0/daemon/fw/CMS_File_Configuration) containing two additional configure files. You need to specify the mapping relationship between file names and datablock names in [name_map.txt](https://github.com/neu-yehlab/VIP-NFD/blob/master/nfd-0.7.0/daemon/fw/CMS_File_Configuration/name_map.txt). The [name_map.txt](https://github.com/neu-yehlab/VIP-NFD/blob/master/nfd-0.7.0/daemon/fw/CMS_File_Configuration/name_map.txt) has three columns, respectively indicating 1. packet name without sequence number 2. block name 3. isFirstpacket of the block. You need also register the datablock sizes in [content_size.txt](https://github.com/neu-yehlab/VIP-NFD/blob/master/nfd-0.7.0/daemon/fw/CMS_File_Configuration/content_size.txt). [content_size.txt](https://github.com/neu-yehlab/VIP-NFD/blob/master/nfd-0.7.0/daemon/fw/CMS_File_Configuration/content_size.txt) has two columns, respectively indicating data block name and block size in GB. After changing these configure files, cp th folder [CMS_File_Configuration](https://github.com/neu-yehlab/VIP-NFD/tree/master/nfd-0.7.0/daemon/fw/CMS_File_Configuration) to  `/usr/local/etc/ndn/`. Also, some VIP related parameters are hard coded in [VIP_configure.hpp](https://github.com/neu-yehlab/VIP-NFD/blob/master/nfd-0.7.0/daemon/fw/VIP_configure.hpp). Change them if necessary.

## test
start NFD, create face and insert fib. You could also choose tcp instead of udp here. 
``` bash
nfd-start
nfdc face create udp://<other-host>
nfdc route add /ndn udp://<other-host>
```

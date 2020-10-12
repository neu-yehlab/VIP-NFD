# VIP-NFD
## Introduction 
This repo implements [VIP algorithm](https://dl.acm.org/doi/abs/10.1145/2660129.2660151?casa_token=0kABuDRdS8sAAAAA:qPlzF3Rg9s8yRwQpoqU3Nbggr-7_gAwqoG2fFuKSAPsjypPwL-ETqg-hL0-zaV2LaiDzggSR1xFi-A
) in [NFD-0.6.6](https://github.com/named-data/NFD/tree/NFD-0.6.6). VIP algorithm is a kind of joint forwarding and caching algorithm, so it contains both an optional forwardeing strategy and a caching policy. The forwarding part is implemented in [/NFD-0.6.6/daemon/fw/](https://github.com/neu-yehlab/VIP-NFD/tree/master/NFD-NFD-0.6.6/daemon/fw) and the caching part is implemented in [/NFD-0.6.6/daemon/table/](https://github.com/neu-yehlab/VIP-NFD/tree/master/NFD-NFD-0.6.6/daemon/table). 

## Installation
The installation process is the same as ordinary NFD-0.6.6. In structions can be found here:\
`https://github.com/named-data/NFD/blob/NFD-0.6.6/docs/INSTALL.rst`
* Notice: 
NFD is based on [ndn-cxx](https://github.com/named-data/ndn-cxx).
According to applications you use, Sometimes you need to change the max packet size in [ndn-cxx](https://github.com/named-data/ndn-cxx). if you want to change max packet size (8800 bytes ad default): Change VARIABLE:  const size_t MAX_NDN_PACKET_SIZE at FILE: ./ndn-cxx/encoding/tlv.hpp


## Usage
Similar to [NFD-0.6.6](https://github.com/named-data/NFD/tree/NFD-0.6.6). Since VIP algorithm is implemented as an optional strategy, you can choose it in the configure file: [nfd.conf.sample.in](https://github.com/neu-yehlab/VIP-NFD/blob/master/NFD-NFD-0.6.6/nfd.conf.sample.in) just like any other available strategy with the following way: 
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
Considering the variant sizes and hierarchical features for CMS data (file < datablock < dataset), there is a configure file folder [CMS_File_Configuration](https://github.com/neu-yehlab/VIP-NFD/tree/master/NFD-NFD-0.6.6/daemon/fw/CMS_File_Configuration) containing two additional configure files. You need to specify the mapping relationship between file names and datablock names in [name_map.txt](https://github.com/neu-yehlab/VIP-NFD/blob/master/NFD-NFD-0.6.6/daemon/fw/CMS_File_Configuration/name_map.txt) and register the datablock sizes in [content_size.txt](https://github.com/neu-yehlab/VIP-NFD/blob/master/NFD-NFD-0.6.6/daemon/fw/CMS_File_Configuration/content_size.txt)

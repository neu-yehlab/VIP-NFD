//
//  VIP_configure.h
//  NFD
//
//  Created by 吴元昊 on 2019/9/22.
//  Copyright © 2019 吴元昊. All rights reserved.
//

#ifndef VIP_configure_HPP
#define VIP_configure_HPP

#define VIPAName "/ndn/VIP/Count/A"
#define VIPBName "/ndn/VIP/Count/B"

#define LINK_CAPACITY 1.0 //link capacity unit GB/period
#define PERIOD 8_s //psriodically send virtual packets and update VIP table
#define CS_LIMIT 2.0 //cache limit at data object/ data block level unit # of data blocks
#define SERVICE_RATE 1.0 //Rate to satisfy interest by cache unit GB/peroid
#define kDecayParam 0.01 //We use a exponentional decay average to replace traditional time average. Paremeter for exponentional decay.
#endif /* VIP_configure_hpp */
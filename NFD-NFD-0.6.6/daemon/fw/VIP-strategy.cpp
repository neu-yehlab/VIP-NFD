

/*
 * Copyright (c) 2014-2019,  Regents of the University of California,
 *                           Arizona Board of Regents,
 *                           Colorado State University,
 *                           University Pierre & Marie Curie, Sorbonne University,
 *                           Washington University in St. Louis,
 *                           Beijing Institute of Technology,
 *                           The University of Memphis.
 *
 * This file is part of NFD (Named Data Networking Forwarding Daemon).
 * See AUTHORS.md for complete list of NFD authors and contributors.
 *
 * NFD is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * NFD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * NFD, e.g., in COPYING.md file.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "VIP-strategy.hpp"
#include "algorithm.hpp"
#include <string>
#include "common/global.hpp"
#include <thread>
#include <ndn-cxx/util/random.hpp>
#include <utility>
#include <iostream>
#include <boost/asio/io_service.hpp>
#include<boost/thread.hpp>
#include <ndn-cxx/face.hpp>
#include <sstream>
#include <cmath>



namespace nfd {
    namespace fw {
        namespace VIP{
            
            VipTable VIPStrategy::m_VIPTable(100);
            std::unordered_map<std::string,std::pair<std::string,bool>> VIPStrategy::m_nameMapTable(100);
            std::unordered_map<std::string,double> VIPStrategy::m_contentSizeTable(100);
            //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
            std::pair<std::pair<std::string,bool>,bool> VIPStrategy::getContentName(std::string chunkName)
            {
                std::pair<std::pair<std::string,bool>,bool> out;
                auto it = m_nameMapTable.find(chunkName);
                if(it==m_nameMapTable.end())
                {
                    out = std::make_pair(std::pair<std::string,bool>("0",false),false);
                }
                else
                {
                    out = std::make_pair(it->second,true);
                }
                return out;
            }
            std::pair<double,bool> VIPStrategy::getContentSize(std::string contentName)
            {
                std::pair<double,bool> out;
                auto it = m_contentSizeTable.find(contentName);
                if(it==m_contentSizeTable.end())
                {
                    out = std::make_pair(0,false);
                }
                else
                {
                    out = std::make_pair(it->second,true);
                }
                return out;
            }
            void
            VIPStrategy::onDataA(const Interest& interest, const Data& data)
            {
                //std::cout<<"DATA_A: "<<data<<std::endl;//output:DATA_A
                /*
                 std::string dataAContent((const char*)data.getContent().value(),data.getContent().value_size());
                 std::cout<<dataAContent<<"\n\n"<<std::endl;
                 std::istringstream f(dataAContent);
                 std::string key;
                 std::string count;
                 std::getline(f, key, '\t');
                 std::getline(f,count,'\n');
                 double compare = 0;
                 std::string tempKey("0");//record the name with the maximum VIP count difference tempararily
                 if(key=="0")
                 {
                 return;
                 }
                 else
                 {
                 do
                 {
                 m_VIPTable.setNeighborCount(key, std::stoi(data.getName()[5].toUri()), std::stod(count));
                 if(abs(compare) < abs(m_VIPTable.getLocalCount(key)-std::stod(count)))
                 {
                 compare = m_VIPTable.getLocalCount(key)-std::stod(count);
                 tempKey = key;
                 }
                 }while (std::getline(f, key, '\t')&&std::getline(f,count,'\n'));
                 }
                 if(compare>0)
                 {
                 m_VIPTransTable.insert(std::pair<long,VIPTransEntry>(std::stol(data.getName()[5].toUri()),VIPTransEntry(tempKey,std::min(LINK_CAPACITY,m_VIPTable.getLocalCount(tempKey)))));
                 }
                 else
                 {
                 m_VIPTransTable.insert(std::pair<long,VIPTransEntry>(std::stol(data.getName()[5].toUri()),VIPTransEntry(tempKey,0)));
                 }
                 m_isVIPCountUpdated=true;
                 */
            }
            
            //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
            
            
            void VIPStrategy::setVIPDataProducer()
            {
                m_VIPDataface.setInterestFilter(m_VIPNameA,
                                                bind(&VIPStrategy::onInterestA, this, _1, _2),
                                                NULL,
                                                NULL);
                m_VIPDataface.setInterestFilter(m_VIPNameB,
                                                bind(&VIPStrategy::onInterestB, this, _1, _2),
                                                NULL,
                                                NULL);
                //boost::thread t([]{getGlobalIoService().run();});
            }
            
            //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
            
            Interest
            VIPStrategy::generateVIPCountInterestA()
            {
                
                auto nowTime = time::system_clock::to_time_t(time::system_clock::now());
                std::stringstream ss;
                ss << std::put_time(std::localtime(&nowTime), "%F %T");
                
                std::string nowTimeString = ss.str();
                //std::cout<<nowTimeString<<std::endl;//only for test
                Name VIPCountName(m_VIPNameA);
                VIPCountName.appendTimestamp();
                Interest VIPCountPacket(VIPCountName);
                VIPCountPacket.setInterestLifetime(PERIOD); // 2 seconds
                VIPCountPacket.setMustBeFresh(true);
                return VIPCountPacket;
            }
            
            //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
            
            void
            VIPStrategy::VIPCountSend(const Interest& interest)
            {
                m_VIPInterestface.expressInterest(interest,bind(&VIPStrategy::onDataA, this,  _1, _2),NULL,NULL);
                return;
            }
            
            //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
            
            void
            VIPStrategy::periodicEvent(const time::milliseconds& interval)
            {
                Interest interest = this->generateVIPCountInterestA();
                this->VIPCountSend(interest);
                
                nfd::cs::VIPcache::VIPPolicy::updateTable();
                /*
                for(auto i = m_VIPTransTable.begin();i!=m_VIPTransTable.end();++i)
                {
                    i->second = false;
                    std::cout<<"\n\n\n~~~~~~~~~~~\n"<<i->first<<"\n\n\n";
                }
                 */
                m_virtualCacheTable.sort(VIPStrategy::compare);
                //m_isVIPSendFinished=false;
                auto it =m_virtualCacheTable.begin();
                int tempCount = 0;
		std::cout<<"===========virtual_cache_table============"<<std::endl;
                while(it!=m_virtualCacheTable.end()&&tempCount<CS_LIMIT)
                {
                    std::cout<<"- "<<*it<<std::endl;
                    m_VIPTable.decLocalCount(*it, SERVICE_RATE);
                    ++it;
                    ++tempCount;
                }
                getScheduler().schedule(interval,[this,interval]{periodicEvent(interval);});
                return;
            }
            
            //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
            
            shared_ptr<Data>
            VIPStrategy::generateVIPCountDataA(Name nameIA)
            {
                
                
                std::string content = m_VIPTable.generateDataAContent();
                shared_ptr<Data> data = make_shared<Data>();
                data->setName(nameIA);
                data->setFreshnessPeriod(PERIOD); // 2 seconds
                data->setContent(reinterpret_cast<const uint8_t*>(content.data()), content.size());
                
                // Sign Data packet with default identity
                m_keyChain.sign(*data);
                // m_keyChain.sign(data, <identityName>);
                //m_keyChain.sign(data, <certificate>);
                return data;
            }
            
            //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
            
            void
            VIPStrategy::onInterestA(const ndn::InterestFilter& filter, const Interest& interest)
            {
                //std::cout << ">> on_IA: " << interest << std::endl;//only for test
                
                // Create new name, based on Interest's name
                Name interestName(interest.getName());
                // Create Data packet
                if(interestName.size()>=7)
                {
                    m_VIPDataface.put(*(this->generateVIPCountDataA(interestName)));
                    Name VIPCountNameB(m_VIPNameB);
                    VIPCountNameB.append(interestName[4]).append(interestName[6]).append(interestName[5]);
                    Interest VIPCountPacketB(VIPCountNameB);
                    VIPCountPacketB.setInterestLifetime(PERIOD); // set interestB lifetime 2 seconds
                    VIPCountPacketB.setMustBeFresh(true);
                    m_VIPInterestface.expressInterest(VIPCountPacketB, bind(&VIPStrategy::onDataB, this, _1, _2),NULL,NULL);
                    //std::cout<<"on_IA_SendIB"<<VIPCountNameB<<std::endl;
                }
                return;
            }
            
            //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
            
            void
            VIPStrategy::onDataB(const Interest& interest, const Data& data)
            {
                //std::cout<<">>DATA_B: "<<data<<'\n'<<std::endl;
                std::string dataBContent((const char*)data.getContent().value(),data.getContent().value_size());
                //std::cout<<dataBContent<<"\n\n"<<std::endl;
                std::istringstream f(dataBContent);
                std::string key;
                std::string count;
                std::getline(f, key, '\t');
                std::getline(f,count,'\n');
                
                if(key=="0")
                {
                    return;
                }
                else
                {
                    if(!m_VIPTable.checkEntry(key))
                    {
                        m_virtualCacheTable.push_back(key);
                    }
                        m_VIPTable.incLocalCount(key, stod(count));
                    
                }
            }
            
            //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
            shared_ptr<Data>
            VIPStrategy::generateVIPCountDataB(Name nameIB)
            {
                
                /*
                 if(it->second.getVIPTrans()>0)//VIP Count diference is positive
                 {
                 content = it->second.getKey()+"\t"+to_string(it->second.getVIPTrans())+"\n";
                 m_VIPTable.decLocalCount( it->second.getKey(), it->second.getVIPTrans());
                 }
                 else//VIP Count diference is nonpositive
                 {
                 content="0\t0\n";
                 }
                 */
                std::string content = m_VIPTable.generateDataBContent(stol(nameIB[6].toUri()));
                //std::cout<<"\n\n\n\n\n\n\n"<<content<<"\n\n\n\n\n\n\n\n\n"<<std::endl;
                shared_ptr<Data> data = make_shared<Data>();
                data-> setName(nameIB);
                data->setFreshnessPeriod(PERIOD); // 2 seconds
                
                data->setContent(reinterpret_cast<const uint8_t*>(content.data()), content.size());
                
                // Sign Data packet with default identity
                //m_keyChain.sign(*data);
                //m_keyChain.sign(data, <identityName>);
                //m_keyChain.sign(data, <certificate>);
                return data;
            }
            //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
            void
            VIPStrategy::onInterestB(const ndn::InterestFilter& filter, const Interest& interest)
            {
                
                Name interestName(interest.getName());
                
                
                
                //std::cout << ">>on_IB: " << interest << std::endl;
                
                // Create data name, based on Interest's name
                if(interestName.size()>=7)
                {
                    this->sendDataB(interestName);
                }
            }
            //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
            void
            VIPStrategy::sendDataB(Name iName)
            {
                //std::cout<<"\n\n\n\nSENDDATAB"<<std::endl;
                auto it = m_VIPTransTable.find(stol(iName[6].toUri()));
                if(it==m_VIPTransTable.end())
                {
                    std::string content="0\t0\n";
                    shared_ptr<Data> data = make_shared<Data>();
                    data-> setName(iName);
                    data->setFreshnessPeriod(PERIOD); // 2 seconds
                    data->setContent(reinterpret_cast<const uint8_t*>(content.data()), content.size());
                    // Sign Data packet with default identity
                    m_keyChain.sign(*data);
                    // m_keyChain.sign(data, <identityName>);
                    //m_keyChain.sign(data, <certificate>);
                    m_VIPDataface.put(*data);
                    //std::cout<<"DataB_Send:  0\t0\n"<<std::endl;
m_VIPTransTable.insert(std::pair<long,bool>(stol(iName[6].toUri()),false));
                }
                else if(it->second)
                {
                    shared_ptr<Data> data = this->generateVIPCountDataB(iName);
                    m_keyChain.sign(*data);
                    m_VIPDataface.put(*data);
                    //std::cout<<"DataB_Send:    "<<*data<<"\n"<<std::endl;
                    it->second=false;
                    return;
                }
                
                else
                {
                    getScheduler().schedule(30_ms,[this,iName]{VIPStrategy::sendDataB(iName);});
                    return;
                }
            }
            //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
            
            void
            VIPStrategy::afterReceiveInterest(const FaceEndpoint& ingress, const Interest& interest,
                                              const shared_ptr<pit::Entry>& pitEntry)
            {
                Name interestName = interest.getName();
                std::string interestString = interestName.toUri();
                //std::cout<<"************"<<m_interestFaceId<<"********\n\n"<<m_dataFaceId<<"\n\n"<<interestString<<std::endl;
                const fib::Entry& fibEntry = this->lookupFib(*pitEntry);
                if(std::regex_match(interestString, std::regex("^/ndn/VIP/Count/A/(.*)") ) && (m_interestFaceId*m_dataFaceId)!=0)//if get interestA
                {
                    
                    //std::cout<<"packet A"<<std::endl;
                    
                    if(ingress.face.getId()==m_interestFaceId)//if interestA comes from local controller
                    {
                        for (const auto& nexthop : fibEntry.getNextHops())//multicast to all possible faces except those of locoal controller
                        {
                            Face& outFace = nexthop.getFace();
                            if (!wouldViolateScope(ingress.face, interest, outFace) && canForwardToLegacy(*pitEntry, outFace) && outFace.getId()!=m_dataFaceId)
                            {
                                //std::cout<<"forwardingLocalA: "<<ingress.face.getId()<<std::endl;
                                m_VIPTransTable[outFace.getId()] = false;
                                Name tempInterestName = interestName;
                                tempInterestName.append(to_string(outFace.getId()));
                                const Interest interestA(tempInterestName);
                                this->sendInterest(pitEntry, FaceEndpoint(outFace, 0), interestA);
                                //std::cout<<"<<IA"<<tempInterestName<<std::endl;
                            }
                        }
                        
                    }
                    else//interest A comes from other NFD applications: send to locoal controller
                    {
                        //std::cout<<"forwardingRemoteA"<<std::endl;
                        interestName.append(to_string(ingress.face.getId()));
                        const Interest interestA(interestName);
                        this->sendInterest(pitEntry, FaceEndpoint(*(this->getFace(m_dataFaceId)), 0), interestA);
                        //std::cout<<"<<IA"<<interestName<<std::endl;
                        return;
                    }
                }
                
                else if(std::regex_match(interestString, std::regex("^/ndn/VIP/Count/B/(.*)"))&&(m_interestFaceId*m_dataFaceId)!=0)//interest B
                {
                    //std::cout<<"packet B"<<std::endl;
                    if(ingress.face.getId()==m_interestFaceId)//if comes from local controller
                    {
                        //std::cout<<interestName[5].toNumber()<<std::endl;
                        //std::cout<<"<<Local_IB"<<interestName<<std::endl;
                        //this->sendInterest(pitEntry, FaceEndpoint(*(this->getFace(m_dataFaceId)), 0), interest);
                        this->sendInterest(pitEntry, FaceEndpoint(*(this->getFace(std::stoull(interestName[5].toUri()))), 0), interest);
                        
                        return;
                        
                    }
                    else//if comes from other NFD/applications
                    {
                        this->sendInterest(pitEntry, FaceEndpoint(*(this->getFace(m_dataFaceId)), 0), interest);
                        //std::cout<<"<<Remote_IB"<<interestName<<std::endl;
                        return;
                    }
                    
                }
                
                else if(interestString=="/ndn/VIP/Calibrationinterest")
                {
                    m_interestFaceId=ingress.face.getId();
                    return;
                }
                else if(interestString=="/ndn/VIP/Calibrationdata")
                {
                    m_dataFaceId=ingress.face.getId();
                    return;
                }
                
                else if(VIPStrategy::getContentName(interestString).second)//actual interest
                {
                    //if(!VIPStrategy::getContentName(interestString).second) return;
                    std::string objName = VIPStrategy::getContentName(interestString).first.first;
                    //if(std::regex_match (interestString, std::regex("^(.*)/1$") )&&ingress.face.getScope() == ndn::nfd::FACE_SCOPE_LOCAL)
                    //increase VIP count if the application generate a new interest and NFD receives the first chunk interest for the
                    if( VIPStrategy::getContentName(interestString).first.second)
                    {
                        if(!m_VIPTable.checkEntry(objName))
                        {
                            m_virtualCacheTable.push_back(objName);
                        }
                        if(ingress.face.getScope() == ndn::nfd::FACE_SCOPE_LOCAL)
                        {
                        m_VIPTable.incLocalCount(objName, 1.0);//****************************
                            //std::cout<<"New endorgeous request for data: "<<objName<<"\n"<<std::endl;
                        }
                        else
                        {
                        //std::cout<<"New exorgeous request for data: "<<objName<<"\n"<<std::endl;
                        }
                    }
                    else //not the first chunk
                    {
                        //std::cout<<"Following requests for data: "<<objName<<"\n"<<std::endl;
                    }
                    //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
                   
                   
                    if(pitEntry->hasOutRecords())
                    {
                        return;
                    }
                    double maxTxAvg = 0;
                    unsigned long maxTxFaceId=0;
                    for (const auto& nexthop : fibEntry.getNextHops())
                    {
                        Face& outFace = nexthop.getFace();
                        if (!wouldViolateScope(ingress.face, interest, outFace) &&
                            canForwardToLegacy(*pitEntry, outFace))
                        {
                            double temp = m_VIPTable.getNeighborTxAvg(objName, outFace.getId());
                            if(temp>maxTxAvg)
                            {
                                maxTxAvg = temp;
                                maxTxFaceId = outFace.getId();
                            }
                            //this->sendInterest(pitEntry, FaceEndpoint(outFace, 0), interest);
                        }
                    }
                    if(maxTxAvg)
                    {
                        this->sendInterest(pitEntry, FaceEndpoint(*(this->getFace(maxTxFaceId)), 0), interest);
                    }
                    else
                    {
                        for (const auto& nexthop : fibEntry.getNextHops())
                        {
                            Face& outFace = nexthop.getFace();
                            if (!wouldViolateScope(ingress.face, interest, outFace) &&
                                canForwardToLegacy(*pitEntry, outFace))
                            {
                                this->sendInterest(pitEntry, FaceEndpoint(outFace, 0), interest);
                                return;
                            }
                        }
                    }
                    
                }
                else
                {
                    for (const auto& nexthop : fibEntry.getNextHops())
                                           {
                                               Face& outFace = nexthop.getFace();
                                               if (!wouldViolateScope(ingress.face, interest, outFace) &&
                                                   canForwardToLegacy(*pitEntry, outFace))
                                               {
                                                   this->sendInterest(pitEntry, FaceEndpoint(outFace, 0), interest);
                                                   return;
                                               }
                                           }
                }
                
            }
            
            //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
            
            void
            VIPStrategy::afterReceiveData(const shared_ptr<pit::Entry>& pitEntry,
                                          const FaceEndpoint& ingress, const Data& data)
            {
                Name dataName = data.getName();
                std::string dataString = dataName.toUri();
                //std::cout<<"afterReceiveData:"<<dataString<<"\n"<<std::endl;
                //if it is control packets
                if(dataName.size()>=7&&std::regex_match (dataString, std::regex("^/ndn/VIP/Count/(.*)") ))
                {
                    this->setExpiryTimer(pitEntry,PERIOD);
                    //std::cout<<"DATA_Send: "<<data<<std::endl;
                    if(ingress.face.getId()==m_dataFaceId)//if data comes from local controller
                    {
                        
                        this->sendData(pitEntry,data,FaceEndpoint(*(this->getFace(std::stoull(dataName[6].toUri()))), 0));
                        return;
                        
                    }
                    else//if data comes from remote NFD/applications
                    {
                        this->sendData(pitEntry,data,FaceEndpoint(*(this->getFace(m_interestFaceId)), 0));
                        if(std::regex_match(dataString, std::regex("^/ndn/VIP/Count/A/(.*)") ))
                        {
                            //std::cout<<"DATA_A: "<<data<<"CONTENT:"<<std::endl;
                            std::string dataAContent((const char*)data.getContent().value(),data.getContent().value_size());
                            //std::cout<<dataAContent<<"\n\n"<<std::endl;
                            std::istringstream f(dataAContent);
                            std::string key;
                            std::string count;
                            std::getline(f, key, '\t');
                            std::getline(f,count,'\n');
                            //double compare = 0;
                            std::string tempKey("0");//record the name with the maximum VIP count difference tempararily
                

                            if(key=="0")
                            {
                                return;
                            }
                            else
                            {
                                do
                                {
                                    if(!m_VIPTable.checkEntry(key))
                                    {
                                        m_virtualCacheTable.push_back(key);
                                    }
                                    m_VIPTable.setNeighborCount(key, std::stoi(data.getName()[5].toUri()), std::stod(count));//******************
                                }while (std::getline(f, key, '\t')&&std::getline(f,count,'\n'));
                            }
                                m_VIPTransTable[ingress.face.getId()]=true;
                            
                        }
                        return;
                    }
                    
                }
                //actual packets
                else
                {
                    
                    this->forwardCommonData(pitEntry, ingress, data);
                    
                }
                
            }
            
            void VIPStrategy::forwardCommonData(const shared_ptr<pit::Entry>& pitEntry,
                                                const FaceEndpoint& ingress, const Data& data)
            {
                if(1)//*******************************
                {
                    //std::cout<<"forwardCommonData"<<std::endl;
                    this->sendDataToAll(pitEntry, ingress, data);
                    return;
                }
                else
                {
                    getScheduler().schedule(20_ms,[this,pitEntry, ingress, data]{VIPStrategy::forwardCommonData(pitEntry, ingress, data);});
                    return;
                }
            }
            //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
            
            NFD_REGISTER_STRATEGY(VIPStrategy);
            
            //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
            
            VIPStrategy::VIPStrategy(Forwarder& forwarder,const Name& name)
            : Strategy(forwarder),m_VIPInterestface(getGlobalIoService()),m_VIPDataface(getGlobalIoService()),m_VIPNameA(VIPAName),m_VIPNameB(VIPBName)
            {
                /*
                 NeighborEntry neighborA_video1(123, 3, 1);
                 NeighborEntry neighborB_video1(124, 4, 1);
                 NeighborEntry neighborC_video1(125, 5, 1);
                 
                 NeighborEntry neighborA_video2(123, 3, 0.5);
                 NeighborEntry neighborB_video2(124, 2, 0.3);
                 NeighborEntry neighborC_video2(125, 1, 0.1);
                 
                 std::vector<NeighborEntry> neighbors_video1;
                 neighbors_video1.push_back(neighborA_video1);
                 neighbors_video1.push_back(neighborB_video1);
                 neighbors_video1.push_back(neighborC_video1);
                 
                 std::vector<NeighborEntry> neighbors_video2;
                 neighbors_video2.push_back(neighborA_video2);
                 neighbors_video2.push_back(neighborB_video2);
                 neighbors_video2.push_back(neighborC_video2);
                 
                 m_VIPTable.insert("/ndn/VIP/video1", 15, 100, neighbors_video1);
                 m_VIPTable.insert("/ndn/VIP/video2", 10, 5, neighbors_video2);
                m_virtualCacheTable.push_back("/ndn/VIP/video1");
                m_virtualCacheTable.push_back("/ndn/VIP/video2");
            //std::cout<<"\n/ndn/VIP/video1_local_VIP_Count"<<m_VIPTable.getLocalCount("/ndn/VIP/video1")<<std::endl;
                 //m_VIPTable.incLocalCount("/neu/vipvideo/video1", 22);
                 //m_VIPTable.decLocalCount("/neu/vipvideo/video1", 25);
                 */
        
                ParsedInstanceName parsed = parseInstanceName(name);
                if (!parsed.parameters.empty()) {
                    NDN_THROW(std::invalid_argument("VIPStrategy does not accept parameters"));
                }
                if (parsed.version && *parsed.version != getStrategyName()[-1].toVersion()) {
                    NDN_THROW(std::invalid_argument(
                                                    "VIPStrategy does not support version " + to_string(*parsed.version)));
                }
                this->setInstanceName(makeInstanceName(name, getStrategyName()));
                Name caliName1("/ndn/VIP/Calibrationdata");
                Name caliName2("/ndn/VIP/Calibrationinterest");
                Interest calibration1(caliName1);
                Interest calibration2(caliName2);
                std::ifstream iNameMap("./daemon/fw/CMS_File_Configuration/name_map.txt");
                std::ifstream iContSize("./daemon/fw/CMS_File_Configuration/content_size.txt");
                if(iNameMap&&iContSize)
                {
                    std::string sNameMap;
                    std::string sContSize;
                    while(getline(iNameMap,sNameMap))
                    {
                        std::istringstream st(sNameMap);
                        std::string chunkName;
                        std::string contentName;
                        bool isFirstChunk;
                        st>>chunkName>>contentName>>isFirstChunk;
                        m_nameMapTable.insert(std::pair<std::string,std::pair<std::string,bool>>(chunkName,std::pair<std::string,bool>(contentName,isFirstChunk)));
                        //std::cout<<chunkName<<std::endl;
                        //std::cout<<contentName<<std::endl;
                        //std::cout<<isFirstChunk<<std::endl;
                    }
                    while(getline(iContSize,sContSize))
                    {
                        std::size_t posContSize = sContSize.find('\t');
                        m_contentSizeTable.insert(std::pair<std::string,double>(sContSize.substr(0,posContSize),stod(sContSize.substr(posContSize+1))));
                    }
                }
                else
                {
                    std::cerr<<"CMS configutation file error"<<std::endl;
                }
                getScheduler().schedule(500_ms,[this,calibration1]{ m_VIPDataface.expressInterest(calibration1,NULL,NULL,NULL);});
                getScheduler().schedule(1000_ms,[this,calibration2]{ m_VIPInterestface.expressInterest(calibration2,NULL,NULL,NULL);});
                getScheduler().schedule(PERIOD,[this]{VIPStrategy::setVIPDataProducer();});
                getScheduler().schedule(3_s,[this]{VIPStrategy::periodicEvent(PERIOD);});
                
            }
            
            //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
            const Name&
            VIPStrategy::getStrategyName()
            {
                static Name strategyName("/localhost/nfd/strategy/VIP/%FD%01");
                return strategyName;
            }
            
            //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
            
        }// namespace VIP
    } // namespace fw
} // namespace nfd











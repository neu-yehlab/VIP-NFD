//
//  cs-policy-VIP.cpp
//  NFD
//
//  Created by 吴元昊 on 2019/8/22.
//  Copyright © 2019 吴元昊. All rights reserved.
//


/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2014-2016,  Regents of the University of California,
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
#include "cs-policy-VIP.hpp"
#include "cs.hpp"

namespace nfd {
    namespace cs {
        namespace VIPcache {
            
            const std::string VIPPolicy::POLICY_NAME = "VIP";
            csVIPTable VIPPolicy::m_csVIPTable;
            NFD_REGISTER_CS_POLICY(VIPPolicy);
            
            VIPPolicy::VIPPolicy()
            : Policy(POLICY_NAME)
            {
            }
            
            void
            VIPPolicy::updateTable()//update CS table
            {
                
                //std::cout<<vt.getRxVipAvg("/neu/vipvideo/video1")<<std::endl;
                std::cout<<"=========================Contents Cached==============================="<<std::endl;
                for(auto it = m_csVIPTable.begin();it!=m_csVIPTable.end();++it)
                {
                    if(it->m_objectName=="") continue;
                    //std::cout<<"update"<<it->m_objectName<<"\n"<<std::endl;
                    m_csVIPTable.modify(it, [it](csVIPEntry& p)
                                        {
                                            p.m_cacheScore=fw::VIP::VIPStrategy::getRxVipAvg(it->m_objectName);
                                            //p.m_VIPCount=nfd::fw::VIP::VIPStrategy::getLocalCount(it->m_objectName);
                                        });
                
               std::cout<<"Content Name: "<<it->m_objectName<<"   ---   VIP Count: "<<fw::VIP::VIPStrategy::getLocalCount(it->m_objectName)<<"   ---   Cache Score: "<<fw::VIP::VIPStrategy::getRxVipAvg(it->m_objectName)<<std::endl;
                }
            }
            
            void
            VIPPolicy::doAfterInsert(EntryRef i)
            {
                std::string dataName = i->getName().toUri();
		/*
		else if(std::regex_match (dataName, std::regex("^/ndn/VIP/Count/(.*)")))//no cache for control packet
                {
                    std::cout<<"\n\nCS:New_VIP_Control_Data_Name:"<<dataName<<"\n"<<std::endl;
                    this->emitSignal(beforeEvict, i);
                }
                */            
                //else if(std::regex_match (dataName, std::regex("^/ndn/VIP/(.*)")))//cache the packets that use VIP algorithm.
                if(std::regex_match (dataName, std::regex("^/ndn/xrootd/(.*)")))
                {
                    std::string contentName;
                    if(fw::VIP::VIPStrategy::getContentName(dataName).second)//check the content name of received chunk; if it exists in the mapping table, get the name
                    {
                        contentName = fw::VIP::VIPStrategy::getContentName(dataName).first.first;
                    }
                    else if(fw::VIP::VIPStrategy::getContentName(dataName.substr(0,dataName.rfind("/"))).second)//if the prefix name exists
                    {
                        contentName = fw::VIP::VIPStrategy::getContentName(dataName.substr(0,dataName.rfind("/"))).first.first;
                    }
                    else//unregistered
                    {
                        //std::cerr<<"unregistered file Name"<<std::endl;
                        this->emitSignal(beforeEvict, i);
                        return;
                    }
                                
                    auto& index0 = m_csVIPTable.get<0>();
                    auto& index1 = m_csVIPTable.get<1>();
                    auto it = index0.find(contentName);
                    if(it!=index0.end())//content already cached
                    {
                        //std::cout<<"following actual data block insert\n"<<std::endl;
                        //std::cout<<"_________________________\nchunk_size:  "<<this->getCs()->size()<<"\nblock_size:  "<<m_contentCacheSize<<"\n\n\n"<<std::endl;
                        m_csVIPTable.modify(it, [i,dataName](csVIPEntry& p)
                                            {
                            p.m_fileLevelTable.insert(std::pair<std::string,EntryRef>(dataName,i));
                                                            });
                        //BOOST_ASSERT(this->getCs()->size() <= CS_LIMIT);//enough space, just cache this chunk
                    }
                    else if(fw::VIP::VIPStrategy::getContentName(dataName).first.second)//content not cached yet and is first chunk
                    {
                        ++m_contentCacheSize;
                        
                        if((m_contentCacheSize)<= CS_LIMIT)//enough space
                        {
                            index0.insert(csVIPEntry(contentName,nfd::fw::VIP::VIPStrategy::getRxVipAvg(contentName),i));
                        }
                        else//no enough space, need compare and evict
                        {
                            if(index1.begin()->m_cacheScore < nfd::fw::VIP::VIPStrategy::getRxVipAvg(contentName))//insert and evict old content
                            {
                                //std::cout<<"repalace old content"<<std::endl;
                                this->evictEntries();
                                index0.insert(csVIPEntry(contentName,nfd::fw::VIP::VIPStrategy::getRxVipAvg(contentName),i));
                            }
                            else// not popular enough
                            {
                                //std::cout<<"not popular enough"<<std::endl;
                                this->emitSignal(beforeEvict, i);
                            }
                            --m_contentCacheSize;
                        }
                        //std::cout<<"New actual data block insert\n"<<std::endl;
                        //std::cout<<"_________________________\nchunk_size:  "<<this->getCs()->size()<<"\nblock_size:  "<<m_contentCacheSize<<"\n\n\n"<<std::endl;
                    }
                    else
                    {
                        //std::cout<<"following low score actual data block insert\n"<<std::endl;
                        
                        this->emitSignal(beforeEvict, i);
                        //std::cout<<"_________________________\nchunk_size:  "<<this->getCs()->size()<<"\nblock_size:  "<<m_contentCacheSize<<"\n\n\n"<<std::endl;
                    }
                    //std:string obName = dataName.substr(0，dataName.rfind("/"));
                }
		else if(std::regex_match (dataName, std::regex("^/ndn/VIP/Count/(.*)")))//no cache for control packet
                {
                    std::cout<<"\n\nCS:New_VIP_Control_Data_Name:"<<dataName<<"\n"<<std::endl;
                    this->emitSignal(beforeEvict, i);
                }
            }
           
            
            
            
            void
            VIPPolicy::doBeforeErase(EntryRef i)
            {
                std::string dataName = i->getName().toUri();
                std::string contentName;
                if(fw::VIP::VIPStrategy::getContentName(dataName).second)//check the content name of erased chunk
                {
                    contentName = fw::VIP::VIPStrategy::getContentName(dataName).first.first;
                }
                else if (fw::VIP::VIPStrategy::getContentName(dataName.substr(0,dataName.rfind('/'))).second)
                {
                contentName = fw::VIP::VIPStrategy::getContentName(dataName.substr(0,dataName.rfind('/'))).first.first;
                }
                else
                {
                    return;
                }
                auto it = m_csVIPTable.get<0>().find(contentName);
                m_csVIPTable.modify(it, [dataName](csVIPEntry& p)
                                    {
                                        p.m_fileLevelTable.erase(dataName);
                                    });
                if(it->m_fileLevelTable.size()==0)
                    m_csVIPTable.get<0>().erase(it);
                --m_contentCacheSize;
                return;
            }
            
            void
            VIPPolicy::doBeforeUse(EntryRef i)
            {
                return;
            }
            
            void
            VIPPolicy::doAfterRefresh(EntryRef i)
            {
                return;
            }
            
            void
            VIPPolicy::evictEntries()
            {
                std::cout<<"\n\n\nevict\n\n\n"<<std::endl;
                BOOST_ASSERT(this->getCs() != nullptr);
                auto it = m_csVIPTable.get<1>().begin();
                if(it!=m_csVIPTable.get<1>().end())
                {
                    BOOST_ASSERT(!m_csVIPTable.empty());
                    //std::cout<<"========================= Chunk_Evict ===================="<<std::endl;
                    for(auto iter = it->m_fileLevelTable.begin(); iter != it->m_fileLevelTable.end(); ++iter)
                    { 
                        //std::cout<<"Evicted Chunk Name: "<<iter->second->getName().toUri()<<std::endl;
                        this->emitSignal(beforeEvict, iter->second);
                    
                    }
                    m_csVIPTable.get<1>().erase(it);
                }
              
                
            }
            
            
            
            
        } // namespace VIP
    } // namespace cs
} // namespace nfd


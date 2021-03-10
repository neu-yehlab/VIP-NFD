//
//  cs-policy-VIP.hpp
//  NFD

#ifndef cs_policy_VIP_hpp
#define cs_policy_VIP_hpp

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
#include  <iostream>
#include <fw/vip_table.hpp>
#include "cs-policy.hpp"
#include <regex>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>
#include <fw/VIP-strategy.hpp>
#include <fw/VIP_configure.hpp>
//#include <core/common.hpp>
namespace nfd {
    namespace cs {
        namespace VIPcache {
           /* 
            struct EntryItComparator
            {
                bool
                operator()(const EntryRef& a, const EntryRef& b) const
                {
                    return *a < *b;
                }
            };
	    */
            struct csVIPEntry
            {
                
                inline csVIPEntry(std::string objectName_)
                {
                    m_objectName = objectName_;
                   
                }
                
                inline csVIPEntry(std::string objectName_, Policy::EntryRef it_)
                {
                    m_objectName = objectName_;
                    m_fileLevelTable={{it_->getName().toUri(),it_}};
                    
                }
                inline csVIPEntry(std::string objectName_,double rxVIPAvg_)
                {
                    m_objectName=objectName_;
                    m_cacheScore=rxVIPAvg_;
                    //m_VIPCount=VIPCount_;
                }
                inline csVIPEntry(std::string objectName_,double rxVIPAvg_,Policy::EntryRef it_)
                {
                    m_objectName=objectName_;
                    m_cacheScore=rxVIPAvg_;
                    //m_VIPCount=VIPCount_;
                    m_fileLevelTable={{it_->getName().toUri(),it_}};
                   
                }
                std::string m_objectName;
                std::map<std::string,Policy::EntryRef> m_fileLevelTable;
                double m_cacheScore=0;
                //double m_VIPCount=0;
            };
            
            typedef boost::multi_index_container<
            csVIPEntry,
            boost::multi_index::indexed_by
            <
            boost::multi_index::hashed_unique
                <
                boost::multi_index::member<csVIPEntry,std::string,&csVIPEntry::m_objectName>
                >,
            boost::multi_index::ordered_non_unique
                <
                boost::multi_index::member<csVIPEntry, double, &csVIPEntry::m_cacheScore>
                >
            >
            > csVIPTable;
            
        
            class VIPPolicy : public Policy
            {
            public:
                VIPPolicy();
                
            public:
                static const std::string POLICY_NAME;
                static void updateTable();
            private:
                virtual void
                doAfterInsert(EntryRef i) override;
                
                virtual void
                doAfterRefresh(EntryRef i) override;
                
                virtual void
                doBeforeErase(EntryRef i) override;
                
                virtual void
                doBeforeUse(EntryRef i) override;
                
                virtual void
                evictEntries() override;
                                
            private:
                 static csVIPTable m_csVIPTable;
                 long m_contentCacheSize=0;
                 Name m_VIPCountPrefix;
                 Name m_xrootdPrefix;
                //static int m_i;
            };
            
        } // namespace VIPcache
        
       using VIPcache::VIPPolicy;
        
    } // namespace cs
} // namespace nfd

#endif // NFD_DAEMON_TABLE_CS_POLICY_LRU_HPP

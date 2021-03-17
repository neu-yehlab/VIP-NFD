/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
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

#ifndef NFD_DAEMON_FW_VIP_STRATEGY_HPP
#define NFD_DAEMON_FW_VIP_STRATEGY_HPP

#include <table/cs-policy-VIP.hpp>
#include <ndn-cxx/face.hpp>
#include "strategy.hpp"
#include "iostream"
#include <string>
#include <algorithm>
#include "common/global.hpp"
#include <thread>
#include <ndn-cxx/util/random.hpp>
#include <boost/asio/io_service.hpp>
#include <map>
#include <regex>
#include "vip_table.hpp"
#include <unordered_map>
#include <fstream>
#include "VIP_configure.hpp"



namespace nfd {
    namespace fw {
        namespace VIP{
            //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
            /*
            struct VIPTransEntry
            {
            public:
                VIPTransEntry(std::string s,bool isUpdated)
                {
                    m_key = s;
                    m_isVIPCountUpdated=isUpdated;
                }
                std::string getKey()
                {
                    return m_key;
                }
             
                double  getVIPTrans()
                {
                    return m_VIPSend;
                }
             
                bool isVIPCountUpdated()
                {
                    return m_isVIPCountUpdated;
                }
                void setVIPCountUpdated(bool isUpdated_)
                {
                    m_isVIPCountUpdated =isUpdated_;
                }
            private:
                std::string m_key="0";
                //double m_VIPSend=0;
                bool m_isVIPCountUpdated=false;
                
            };
            */
            
            class VIPStrategy : public Strategy
            {
            public:
                explicit
                VIPStrategy(Forwarder& forwarder,const Name& name = getStrategyName());
                void afterContentStoreHit(const shared_ptr<pit::Entry>& pitEntry,
                                          const FaceEndpoint& ingress, const Data& data) override;
                void afterReceiveInterest(const FaceEndpoint& ingress,const Interest& interest,const shared_ptr<pit::Entry>& pitEntry) override;
                //multicast VIPInterestA, unicast for VIPInterestB
                void afterReceiveData(const shared_ptr<pit::Entry>& pitEntry, const FaceEndpoint& ingress, const Data& data) override;
                void setVIPDataProducer(std::string s);
                static const Name& getStrategyName();
                void setVIPDataProducer();
                void sendDataB(Name iName);
                void
                VIPBDataSend();
                void
                periodicEvent(const time::milliseconds& interval);
                //periodically send VIP Interest.
                void
                VIPCountSend(const Interest& interest);
                Interest
                generateVIPCountInterestA();
                Interest
                generateVIPCountInterestB();
                shared_ptr<Data>
                generateVIPCountDataA(Name nameIA);
                shared_ptr<Data>
                generateVIPCountDataB(Name nameIB);
                static std::pair<std::pair<std::string,bool>,bool> getContentName(Name chunkName);
                static std::pair<double,bool> getContentSize(std::string contentName);
                inline static void incLocalCount(std::string key, double amount)
                {
                    return m_VIPTable.incLocalCount(key, amount);
                }
                //inline static std::pair<std::string, bool> get//?????
                inline static void updateRxAvg(std::string key,const long timestamp,const double vip_amount)
                {
                    return m_VIPTable.updateRxAvg(key, timestamp, vip_amount);
                }
                inline static double getRxVipAvg(const std::string key)
                {
                    return m_VIPTable.getRxVipAvg(key);
                }
                
                inline static double getLocalCount(const std::string key)
                {
                    return m_VIPTable.getLocalCount(key);
                }
                inline static void decLocalCount(const std::string key, const double amount)
                {
                    return m_VIPTable.decLocalCount(key,amount);
                }
                void forwardCommonData(const shared_ptr<pit::Entry>& pitEntry,
                                                    const FaceEndpoint& ingress, const Data& data);
                inline static bool compare(std::string A, std::string B)
                {
                    //return (m_VIPTable.getRxVipAvg(A))*(getContentSize(A).first)>(m_VIPTable.getRxVipAvg(B))*(getContentSize(B).first);
                    return (m_VIPTable.getRxVipAvg(A))>(m_VIPTable.getRxVipAvg(B));
                }
            private:
               
                void
                onInterestA(const ndn::InterestFilter& filter, const Interest& interest);
                //make use of generateVIPCountDataA(), send data back;
                void
                onInterestB(const ndn::InterestFilter& filter, const Interest& interest);
                //make use of generateVIPCountDataB(),send data back;
                void
                onDataA(const Interest& interest, const Data& data);
                void
                onDataB(const Interest& interest, const Data& data);
                
                
                void
                onRegisterFailed(const Name& prefix, const std::string& reason);
                ndn::KeyChain m_keyChain;//not defined yet
                void
                onNack(const Interest& interest, const lp::Nack& nack);//not defined yet
                void
                onTimeout(const Interest& interest);//not defined yet, it's about retransmission and other things
                
                //bool m_isVIPCountUpdated=false;
                bool m_isVIPSendFinished=false;
                std::map<long,bool> m_VIPTransTable;
                ndn::Face m_VIPInterestface;
                ndn::Face m_VIPDataface;
                std::string m_VIPNameA;
                std::string m_VIPNameB;
                uint64_t m_interestFaceId=0;//interest sending and data receiving face of the controller
                uint64_t m_dataFaceId=0;//data sending face of the controller
                static VipTable m_VIPTable;
                std::list<std::string> m_virtualCacheTable;//how to add elements and avoid repeating。 Element adding has not finished.
                static std::unordered_map<Name, std::pair<std::string,bool>> m_nameMapTable;
                static std::unordered_map<std::string,double> m_contentSizeTable;
                Name m_VIPCountPrefix;
                Name m_VIPCountAPrefix;
                Name m_VIPCountBPrefix;
                Name m_calibrationInterestName;
                Name m_calibrationDataName;               
                
            };
            
            
            /** \brief VIP-strategy
             *  This strategy acts in virtual plane: forwarding virtual packets based on VIP algorithm, and
             *  uodate the VIP Count and other information in the VIPTable
             *  in actual plane: forwarding actual packets based on VIP Table information
             */
            
        }//namespace VIP
    } // namespace fw
} // namespace nfd

#endif // NFD_DAEMON_FW_VIP_STRATEGY_HPP


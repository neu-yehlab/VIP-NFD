/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2018 Regents of the University of California.
 *
 * This file is part of ndn-cxx library (NDN C++ library with eXperimental eXtensions).
 *
 * ndn-cxx library is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later version.
 *
 * ndn-cxx library is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 * You should have received copies of the GNU General Public License and GNU Lesser
 * General Public License along with ndn-cxx, e.g., in COPYING.md file.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
 *
 * @author Alexander Afanasyev <http://lasr.cs.ucla.edu/afanasyev/index.html>
 */

#include <ndn-cxx/face.hpp>
#include <ndn-cxx/security/key-chain.hpp>

#include <iostream>

// Enclosing code in ndn simplifies coding (can also use `using namespace ndn`)
namespace ndn {
// Additional nested namespaces can be used to prevent/limit name conflicts
namespace examples {

class Producer : noncopyable
{
public:
  void
  run()
  {
    m_face.setInterestFilter("/ndn/VIP/video2/",
                             bind(&Producer::onInterest3, this, _1, _2),
                             RegisterPrefixSuccessCallback(),
                             bind(&Producer::onRegisterFailed, this, _1, _2));
    m_face.setInterestFilter("/ndn/VIP/video1/",
    bind(&Producer::onInterest3, this, _1, _2),
    RegisterPrefixSuccessCallback(),
    bind(&Producer::onRegisterFailed, this, _1, _2));
      
      
       m_face.setInterestFilter("/ndn/VIP/Count/A",
                               bind(&Producer::onInterest, this, _1, _2),
                               RegisterPrefixSuccessCallback(),
                               bind(&Producer::onRegisterFailed, this, _1, _2));
     
    m_face.processEvents();
  }

private:
  void
  onInterest(const InterestFilter& filter, const Interest& interest)
  {
    
    //std::cout << "<< I: " << interest << std::endl;
   
    // Create new name, based on Interest's name
    Name dataName(interest.getName());
    for (const auto& component : dataName) {
      std::cout << "-";
      component.toUri(std::cout);
    }
      std::string s = "7777";   //std::cout<<"<< N: "<< dataName << std::endl;
    dataName
      .append(s); // add "testApp" component to Interest name

/*
    static const std::string content = "HELLO KITTY";

    // Create Data packet
    shared_ptr<Data> data = make_shared<Data>();
    data->setName(dataName);
    data->setFreshnessPeriod(10_s); // 10 seconds

    data->setContent(reinterpret_cast<const uint8_t*>(content.data()), content.size());
   
    // Sign Data packet with default identity
    m_keyChain.sign(*data);
    // m_keyChain.sign(data, <identityName>);
    // m_keyChain.sign(data, <certificate>);
*/
    // Return Data packet to the requester
      std::string content("/ndn/VIP/video1\t1\n/ndn/VIP/video2\t1\n/ndn/VIP/animal\t1\n");
      shared_ptr<Data> data = make_shared<Data>();
      data->setName(dataName);
      data->setFreshnessPeriod(2_s); // 2 seconds
      
      data->setContent(reinterpret_cast<const uint8_t*>(content.data()), content.size());
      
      // Sign Data packet with default identity
      m_keyChain.sign(*data);
    //std::cout << "\n\n\n\n\n\n\n\n>> D: " << *data << std::endl;
    //m_face.put(*data);
      //std::cout << ">> D: " << *data << std::endl;
      m_face.put(*data);
      Interest interest2("ndn/VIP/Count/B/11223344/7777/267");
      m_face.expressInterest(interest2,bind(&Producer::onData, this, _1, _2),NULL,NULL);
  }

    void
    onData(const Interest& interest, const Data& data)
    {
        //std::cout<<"\n\n\n\nDATA: "<<data<<'\n'<<"CONTENT:"<<std::endl;
        //std::string s((const char*)data.getContent().value(),data.getContent().value_size());
        //std::cout<<s<<std::endl;
        /*
        for(uint8_t i =0; i<data.getContent().value_size();++i)
        {
            //std::cout <<"DATA: "<< data << std::endl;
            //std::cout<<"CONTENT_TYPE: "<<data.getContentType()<<std::endl;
            std::cout<< *(data.getContent().value()+i);
        }
         */
        std::cout<<std::endl;
    }
    void
    onInterest3(const InterestFilter& filter, const Interest& interest)
    {
        std::cout << "<< I: " << interest << std::endl;
        
        // Create new name, based on Interest's name
        Name dataName(interest.getName());
        for (const auto& component : dataName) {
            std::cout << "-";
            component.toUri(std::cout);
        }
        //std::cout<<"<< N: "<< dataName << std::endl;
        //dataName.append("testApp"); // add "testApp" component to Interest name
        
        
        static const std::string content(10000,'a');
        
        // Create Data packet
        shared_ptr<Data> data = make_shared<Data>();
        data->setName(dataName);
        data->setFreshnessPeriod(10_s); // 10 seconds
        
        data->setContent(reinterpret_cast<const uint8_t*>(content.data()), content.size());
        
        // Sign Data packet with default identity
        m_keyChain.sign(*data);
        // m_keyChain.sign(data, <identityName>);
        // m_keyChain.sign(data, <certificate>);
        
        // Return Data packet to the requester
        std::cout << ">> D: " << *data << std::endl;
        m_face.put(*data);
    }
  void
  onRegisterFailed(const Name& prefix, const std::string& reason)
  {
    std::cerr << "ERROR: Failed to register prefix \""
              << prefix << "\" in local hub's daemon (" << reason << ")"
              << std::endl;
    m_face.shutdown();
  }

private:
  Face m_face;
  KeyChain m_keyChain;
};

} // namespace examples
} // namespace ndn

int
main(int argc, char** argv)
{
  ndn::examples::Producer producer;
  try {
    producer.run();
  }
  catch (const std::exception& e) {
    std::cerr << "ERROR: " << e.what() << std::endl;
  }
  return 0;
}

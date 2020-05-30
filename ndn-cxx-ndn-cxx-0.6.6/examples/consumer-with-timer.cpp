/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2019 Regents of the University of California.
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
#include <ndn-cxx/util/scheduler.hpp>

#include <boost/asio/io_service.hpp>
#include <iostream>

// Enclosing code in ndn simplifies coding (can also use `using namespace ndn`)
namespace ndn {
// Additional nested namespaces can be used to prevent/limit name conflicts
namespace examples {

class ConsumerWithTimer
{
public:
  ConsumerWithTimer()
    : m_face(m_ioService) // Create face with io_service object
    , m_scheduler(m_ioService)
  {
  }

  void
  run()
  {
      //Interest interest(Name("/ndn/VIP/Count/A/1234567/123"));
      Interest interestSet[6]={
      Interest(Name("/ndn/VIP/video2/1")),
      Interest(Name("/ndn/VIP/video2/2")),
      Interest(Name("/ndn/VIP/video2/3")),
      Interest(Name("/ndn/VIP/video1/1")),
      Interest(Name("/ndn/VIP/video1/2")),
          Interest(Name("/ndn/VIP/video1/3"))};
      /*for(int i = 0;i<6;++i)
      {
          interestSet[i].setInterestLifetime(2_s); // 2 seconds
          interestSet[i].setMustBeFresh(true);

    m_face.expressInterest(interestSet[i],
                           bind(&ConsumerWithTimer::onData, this, _1, _2),
                           bind(&ConsumerWithTimer::onNack, this, _1, _2),
                           bind(&ConsumerWithTimer::onTimeout, this, _1));
          std::cout << "Sending " << interestSet[i] << std::endl;
      }
    */
      interestSet[0].setInterestLifetime(2_s); // 2 seconds
            interestSet[0].setMustBeFresh(true);

      m_face.expressInterest(interestSet[0],
                             bind(&ConsumerWithTimer::onData, this, _1, _2),
                             bind(&ConsumerWithTimer::onNack, this, _1, _2),
                             bind(&ConsumerWithTimer::onTimeout, this, _1));
            std::cout << "Sending " << interestSet[0] << std::endl;
    // Schedule a new event
    m_scheduler.schedule(1_s, [this] { delayedInterest(); });

    // m_ioService.run() will block until all events finished or m_ioService.stop() is called
    m_ioService.run();

    // Alternatively, m_face.processEvents() can also be called.
    // processEvents will block until the requested data received or timeout occurs.
    // m_face.processEvents();
  }

private:
  void
  onData(const Interest& interest, const Data& data)
  {
      std::cout<<"DATA: "<<data<<'\n'<<"CONTENT:"<<std::endl;
      std::string s((const char*)data.getContent().value(),data.getContent().value_size());
      std::cout<<s<<std::endl;
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
  onNack(const Interest& interest, const lp::Nack& nack)
  {
    std::cout << "received Nack with reason " << nack.getReason()
              << " for interest " << interest << std::endl;
  }

  void
  onTimeout(const Interest& interest)
  {
    std::cout << "Timeout " << interest << std::endl;
  }

  void
  delayedInterest()
  {
    std::cout << "One more Interest, delayed by the scheduler" << std::endl;

    Interest interest(Name("/ndn/VIP/video2/2"));
    interest.setInterestLifetime(5_s); // 2 seconds
    interest.setMustBeFresh(true);

    m_face.expressInterest(interest,
                           bind(&ConsumerWithTimer::onData, this, _1, _2),
                           bind(&ConsumerWithTimer::onNack, this, _1, _2),
                           bind(&ConsumerWithTimer::onTimeout, this, _1));

    std::cout << "Sending " << interest << std::endl;
  }

private:
  // Explicitly create io_service object, which can be shared between Face and Scheduler
  boost::asio::io_service m_ioService;
  Face m_face;
  Scheduler m_scheduler;
};

} // namespace examples
} // namespace ndn

int
main(int argc, char** argv)
{
  ndn::examples::ConsumerWithTimer consumer;
  try {
    consumer.run();
  }
  catch (const std::exception& e) {
    std::cerr << "ERROR: " << e.what() << std::endl;
  }
  return 0;
}

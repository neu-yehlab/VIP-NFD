
#include "vip_table.hpp"
#include <iostream>
#include <stdlib.h>
#include <stdint.h>
#include <string>
#include <string.h>
namespace nfd {
    namespace fw {
        namespace VIP{
//static uint16_t vipASegment = 0;
// TO DO: Check for hash conflicts (create getEntry and use it everywhere), implement checkObject function, implement update function

uint16_t nextPrime(uint16_t n){ // A very hacky nextPrime function that should be enough for our purposes
  uint16_t primes[9] = {131, 257, 521, 1031, 2053, 4099, 8209, 16411, 32771};
  for(int i=0; i<9; i++) {
    if (primes[i] > n) return primes[i];
  }
  std::cout << "Catalog size can't be extended further.\n"; // TO DO: Proper error handling here.
  return primes[8];
}

VipTable::VipTable(const uint16_t table_size) : catalog_size_(0) {
  uint16_t prime_table_size = nextPrime(table_size);
  vip_table_ = new VipEntry *[prime_table_size]();
  table_size_ = prime_table_size;
}

VipTable::~VipTable() {
  for (int i = 0; i < table_size_; ++i) {
    VipEntry *entry = vip_table_[i];
    while (entry != NULL) {
        VipEntry *prev = entry;
        entry = entry->getNext();
        delete prev;
    }
    vip_table_[i] = NULL;
  }
  // destroy the hash table
  delete [] vip_table_;
}

uint16_t VipTable::getHash(const std::string key) {
  size_t len = key.size();
  char k[len+1];
  strcpy(k,key.c_str());
  uint16_t hash_val = CityHash64(k, len) % table_size_;
  return hash_val;
}

uint16_t VipTable::getSize() {
  return catalog_size_;
}

bool VipTable::checkEntry(const std::string key) {
  uint16_t hash_val = getHash(key);
  VipEntry* entry = vip_table_[hash_val];

  while(entry != NULL) {
    if(entry->getKey() == key) {
      return true;
    }
    entry = entry->getNext();
  }
  return false;
}

VipEntry* VipTable::getEntry(const std::string key) {
  if(checkEntry(key)) {
    uint16_t hash_val = getHash(key);
    VipEntry* entry = vip_table_[hash_val];

    while(entry->getKey() != key) {
      entry = entry->getNext();
    }
    return entry;
  }
  std::cout << "Entry does not exist.\n"; // TO DO : Proper error handling is a must here. All following functions depend on this function.
  return NULL;
}

double VipTable::getLocalCount(const std::string key) {
  VipEntry* entry = getEntry(key);
    if(entry != NULL) {
  return entry->getLocalCount();
}
    else
    {
        std::vector<NeighborEntry> neighbours;
        insert(key, 0, 0, neighbours);
	return 0;
    }
}
void VipTable::incLocalCount(const std::string key, const double amount) {
    uint16_t hash_val = getHash(key);
    VipEntry *prev = NULL;
    VipEntry *entry = vip_table_[hash_val];
    
    while (entry != NULL && entry->getKey() != key) {
        prev = entry;
        entry = entry->getNext();
    }
    
    if (entry==NULL) {
        catalog_size_++;
        auto now = std::chrono::system_clock::now();
        auto now_ms = std::chrono::time_point_cast<std::chrono::seconds>(now);
        auto epoch = now_ms.time_since_epoch();
        auto value = std::chrono::duration_cast<std::chrono::seconds>(epoch);
        long duration = value.count();
        entry = new VipEntry(key, amount, amount,duration);// when some content has already been inserted in other nodes and its VIP counts have changed but it's still not inserted in the local node
        // the neighbor_vector part may come in handy, may need copy constructor
        if (prev==NULL) {
            vip_table_[hash_val] = entry;
        }
        else {
            prev->setNext(entry);
        }
        if (catalog_size_ > 0.75 * table_size_) {
            resize();
        }
    }
    else {
        //std::cout<<"\n\n\n\n\n\nincrease VIP local Count"<< amount<<std::endl;
        entry->incLocalCount(amount);
        // content was already in table, no need to adjust neighbor vector since it is already being updated
    }
}

void VipTable::decLocalCount(const std::string key, const double amount) {
uint16_t hash_val = getHash(key);
    VipEntry *prev = NULL;
    VipEntry *entry = vip_table_[hash_val];
    
    while (entry != NULL && entry->getKey() != key) {
        prev = entry;
        entry = entry->getNext();
    }
    
    if (entry==NULL) {
        catalog_size_++;
        auto now = std::chrono::system_clock::now();
        auto now_ms = std::chrono::time_point_cast<std::chrono::seconds>(now);
        auto epoch = now_ms.time_since_epoch();
        auto value = std::chrono::duration_cast<std::chrono::seconds>(epoch);
        long duration = value.count();
        entry = new VipEntry(key, 0, 0,duration); // when some content has already been inserted in other nodes and its VIP counts have changed but it's still not inserted in the local node
        // the neighbor_vector part may come in handy, may need copy constructor
        if (prev==NULL) {
            vip_table_[hash_val] = entry;
        }
        else {
            prev->setNext(entry);
        }
        if (catalog_size_ > 0.75 * table_size_) {
            resize();
        }
    }
    else {
        entry->decLocalCount(amount);
        // content was already in table, no need to adjust neighbor vector since it is already being updated
    }
}

void VipTable::resetLocalCounts(std::vector<std::string> key_list) {
  for(std::vector<std::string>::iterator it = key_list.begin(); it != key_list.end(); ++it) {
        VipEntry* entry = getEntry(*it);
        entry->setLocalCount(0);
  }
}

void VipTable::resetRxVipAvgs(std::vector<std::string> key_list){
  for(std::vector<std::string>::iterator it = key_list.begin(); it != key_list.end(); ++it) {
        VipEntry* entry = getEntry(*it);
        entry->setRxAvg(0);
  }
}

void VipTable::resetNeighborCount(const std::string key, const int face_id){
  VipEntry* entry = getEntry(key);
  entry->setNeighborCount(face_id, 0);
}

void VipTable::resetNeighborTxAvg(const std::string key, const int face_id){
  VipEntry* entry = getEntry(key);
  entry->setNeighborTxAvg(face_id, 0);
}

double VipTable::getRxVipAvg(const std::string key) {
  VipEntry* entry = getEntry(key);
    if(entry != NULL) {
  return entry->getRxAvg();
    }
    else
    {
          std::vector<NeighborEntry> neighbours;
             insert(key, 0, 0, neighbours);
	     return 0;
    }
}

double VipTable::getNeighborCount(const std::string key, const int face_id) {
  VipEntry* entry = getEntry(key);
    if(entry != NULL) {
    return entry->getNeighborCount(face_id);
    }
   else
    {
     
         std::vector<NeighborEntry> neighbours;
                       auto now = std::chrono::system_clock::now();
                            auto now_ms = std::chrono::time_point_cast<std::chrono::seconds>(now);
                            auto epoch = now_ms.time_since_epoch();
                            auto value = std::chrono::duration_cast<std::chrono::seconds>(epoch);
                            long duration = value.count();
                       NeighborEntry neighbour(face_id, 0, 0,duration);
                       neighbours.push_back(neighbour);
        insert(key, 0, 0, neighbours);
        return 0;
    }
  
}

double VipTable::getNeighborTxAvg(const std::string key, const int face_id) {
  VipEntry* entry = getEntry(key);
   if(entry != NULL) {
      return entry->getNeighborTxAvg(face_id);
    }
   else
    {
        std::vector<NeighborEntry> neighbours;
                             auto now = std::chrono::system_clock::now();
                                  auto now_ms = std::chrono::time_point_cast<std::chrono::seconds>(now);
                                  auto epoch = now_ms.time_since_epoch();
                                  auto value = std::chrono::duration_cast<std::chrono::seconds>(epoch);
                                  long duration = value.count();
                             NeighborEntry neighbour(face_id, 0, 0,duration);
                             neighbours.push_back(neighbour);
            insert(key, 0, 0, neighbours);
        return 0;
    }
  
}

void VipTable::insert(const std::string key, const double local_vip_count, const double rx_vip_avg, const std::vector<NeighborEntry> neighbor_vector) {
  uint16_t hash_val = getHash(key);
  VipEntry *prev = NULL;
  VipEntry *entry = vip_table_[hash_val];

  while (entry != NULL && entry->getKey() != key) {
    prev = entry;
    entry = entry->getNext();
  }
    auto now = std::chrono::system_clock::now();
    auto now_ms = std::chrono::time_point_cast<std::chrono::seconds>(now);
    auto epoch = now_ms.time_since_epoch();
    auto value = std::chrono::duration_cast<std::chrono::seconds>(epoch);
    long duration = value.count();
  if (entry==NULL) {
    catalog_size_++;
      
    entry = new VipEntry(key, local_vip_count, rx_vip_avg, neighbor_vector,duration); // when some content has already been inserted in other nodes and its VIP counts have changed but it's still not inserted in the local node
                                                      // the neighbor_vector part may come in handy, may need copy constructor
    if (prev==NULL) {
        vip_table_[hash_val] = entry;
    }
    else {
        prev->setNext(entry);
    }
    if (catalog_size_ > 0.75 * table_size_) {
     resize();
    }
  }
  else {
      entry->setLocalCount(local_vip_count);
      entry->setRxAvg(rx_vip_avg);
      //entry->setLastTimeStamp(duration);
      // content was already in table, no need to adjust neighbor vector since it is already being updated
  }
}

void VipTable::erase(const std::string key) {
  uint16_t hash_val = getHash(key);
  VipEntry *prev = NULL;
  VipEntry *entry = vip_table_[hash_val];

  while (entry != NULL && entry->getKey() != key) {
    prev = entry;
    entry = entry->getNext();
  }

  if (entry==NULL) {
    // key not found
    return;
  }
  else {
    if (prev == NULL) {
        // remove first bucket of the list
        vip_table_[hash_val] = entry->getNext();
    }
    else {
        prev->setNext(entry->getNext());
    }
    delete entry;
  }
}

void VipTable::resize() {
  uint16_t old_table_size = table_size_;
  table_size_ = nextPrime(table_size_ + 1); // Due to the lookup nature of the hacky nextPrime function, +1 instead of *2
  VipEntry **old_table = vip_table_;
  vip_table_ = new VipEntry *[table_size_];
  for (int i = 0; i < table_size_; i++) {
        vip_table_[i] = NULL;
  }
  for (int hash = 0; hash < old_table_size; hash++) {
    if (old_table[hash] != NULL) {
      VipEntry *old_entry;
      VipEntry *entry = old_table[hash];
      while (entry != NULL) {
           insert(entry->getKey(), entry->getLocalCount(), entry->getRxAvg(), entry->getNeighborValues());
           old_entry = entry;
           entry = entry->getNext();
           delete old_entry;
      }
    }
  }
  delete[] old_table;
}

uint16_t VipTable::getVIPASeg(const int face_id)
{
  std::map< const int, uint16_t >::iterator iter;
  if((iter=vip_A_segment_.find(face_id))!=vip_A_segment_.end())
  {
    return iter->second;
  }
  else
  {
    vip_A_segment_.insert(std::pair<const int, uint16_t>(face_id,0));
    return 0;
  }
}


void VipTable::resetVIPASeg()
{
  std::map< const int, uint16_t >::iterator iter;
  for(iter=vip_A_segment_.begin();iter!=vip_A_segment_.end();iter++)
  {
    iter->second = 0;
  } 
}
   
void VipTable::setVIPASeg(const int face_id, uint16_t seg)
{
  std::map< const int, uint16_t >::iterator iter;
  if((iter=vip_A_segment_.find(face_id))!=vip_A_segment_.end())
  {
    iter->second = seg;
  }
  else
  {
    vip_A_segment_.insert(std::pair<const int, uint16_t>(face_id,seg));
  }

}
            
            std::string VipTable::generateDataAContent(const int face_id){
                if(catalog_size_==0){
                    return "0\t0\n";
                }
                else{
                    std::string content;
                    
                    for(int i = this->getVIPASeg(face_id); i<table_size_;++i){
                        if(vip_table_[i]!=NULL){//have VipEntry arry
                            std::string append = (vip_table_[i]->getKey())+"\t"+std::to_string(vip_table_[i]->getLocalCount())+"\n";
                            //content = content+(vip_table_[i]->getKey())+"\t"+std::to_string(vip_table_[i]->getLocalCount())+"\n";
                            if(content.size()+append.size()> 8800)
                            {
                              this->setVIPASeg(face_id,(uint16_t)i);
                              return content; 
                            }
                            else
                            {
                              content+=append; 
                            }
                        }
                    }
                    this->setVIPASeg(face_id,0);
                    return content;
                }
            }
            
            
            std::string VipTable::generateDataBContent(long face_id){
                //std::cout<<"\n\n\n\n\n\n\ngenerateDataBContent\n\n\n\n\n"<<std::endl;
                if(catalog_size_==0){
                    //std::cout<<"\ntable is empty"<<std::endl;
                    return "0\t0\n";
                }
                else{
                    std::string content="0\t0\n";
                    double maxDiff = 0;
                    std::string maxDiffName="0";
                    for(int i = 0; i<table_size_;++i){
                        if(vip_table_[i]!=NULL){//have VipEntry arry
                            double temp = vip_table_[i]->getLocalCount()-vip_table_[i]->getNeighborCount(face_id);
                            //std::cout<<"Face_Id:  "<<face_id<<"   ---   Content Name: "<<vip_table_[i]->getKey()<<"   ---   VIPCount Difference: " <<temp<<std::endl;
                            if(abs(temp)>abs(maxDiff)){
                                maxDiff =temp;
                                maxDiffName = vip_table_[i]->getKey();
                            }
                        }
                    }
                    if(maxDiff>0){
                        if(this->getLocalCount(maxDiffName)<=LINK_CAPACITY)
                        {
                            content = maxDiffName + "\t" + std::to_string(this->getLocalCount(maxDiffName))+"\n";
                            this->decLocalCount(maxDiffName,this->getLocalCount(maxDiffName));
                            auto now = std::chrono::system_clock::now();
                            auto now_ms = std::chrono::time_point_cast<std::chrono::seconds>(now);
                            auto epoch = now_ms.time_since_epoch();
                            auto value = std::chrono::duration_cast<std::chrono::seconds>(epoch);
                            long duration = value.count();
                            this->updateNeighborTxAvg(maxDiffName, face_id, duration, -(this->getLocalCount(maxDiffName)));
                            
                        }
                        else
                        {
                            content = maxDiffName + "\t" + std::to_string(LINK_CAPACITY)+"\n";
                            this->decLocalCount(maxDiffName,LINK_CAPACITY);
                            auto now = std::chrono::system_clock::now();
                            auto now_ms = std::chrono::time_point_cast<std::chrono::seconds>(now);
                            auto epoch = now_ms.time_since_epoch();
                            auto value = std::chrono::duration_cast<std::chrono::seconds>(epoch);
                            long duration = value.count();
                            this->updateNeighborTxAvg(maxDiffName, face_id, duration, -LINK_CAPACITY);
                            
                        }
			
                    }
                    return content;
                }
            }
            void VipTable::setNeighborTxAvg(const std::string key, const int face_id, const double tx_vip_avg){
                uint16_t hash_val=getHash(key);
                VipEntry* entry = vip_table_[hash_val];
                while(entry != NULL) {
                    if(entry->getKey() == key) {
                        vip_table_[hash_val]->setNeighborTxAvg(face_id, tx_vip_avg);
                        return;
                    }
                    entry = entry->getNext();
                }
                std::vector<NeighborEntry> neighbours;
                auto now = std::chrono::system_clock::now();
                     auto now_ms = std::chrono::time_point_cast<std::chrono::seconds>(now);
                     auto epoch = now_ms.time_since_epoch();
                     auto value = std::chrono::duration_cast<std::chrono::seconds>(epoch);
                     long duration = value.count();
                NeighborEntry neighbour(face_id, 0, tx_vip_avg,duration);
                neighbours.push_back(neighbour);
                insert(key, 0, 0, neighbours);
            }
            
            
            void VipTable::setNeighborCount(const std::string key, const int face_id, const double vip_count) {
                uint16_t hash_val=getHash(key);
                VipEntry* entry = vip_table_[hash_val];
                while(entry != NULL) {
                    if(entry->getKey() == key) {
                        vip_table_[hash_val]->setNeighborCount(face_id, vip_count);
                        return;
                    }
                    entry = entry->getNext();
                }
                std::vector<NeighborEntry> neighbours;
                auto now = std::chrono::system_clock::now();
                     auto now_ms = std::chrono::time_point_cast<std::chrono::seconds>(now);
                     auto epoch = now_ms.time_since_epoch();
                     auto value = std::chrono::duration_cast<std::chrono::seconds>(epoch);
                     long duration = value.count();
                NeighborEntry neighbour(face_id, vip_count, 0,duration);
                neighbours.push_back(neighbour);
                this->insert(key, 0, 0, neighbours);
            }
        void VipTable::updateRxAvg(const std::string key, const long timestamp, const double vip_amount){
          VipEntry* entry = getEntry(key);
            if(entry!=NULL)
            {
          entry->updateRxAvg(timestamp, vip_amount);
            }
            else
            {
                std::vector<NeighborEntry> neighbours;
                            insert(key, 0, 0, neighbours);
            }
        }
        void VipTable::updateNeighborTxAvg(const std::string key, const int face_id, const long timestamp, const double vip_amount){
          VipEntry* entry = getEntry(key);
            if(entry!=NULL)
            {
          entry->updateNeighborTxAvg(face_id, timestamp, vip_amount);
            }
            else{
                std::vector<NeighborEntry> neighbours;
                            insert(key, 0, 0, neighbours);
            }
        }
             }}}

#include "vip_entry.hpp"
namespace nfd {
    namespace fw {
        namespace VIP{
VipEntry::VipEntry(const std::string key, const double local_vip_count, const double rx_vip_avg, const std::vector<NeighborEntry> neighbor_vector) :
key_(key), local_vip_count_(local_vip_count), rx_vip_avg_(rx_vip_avg), neighbor_vector_(neighbor_vector), next_(NULL) {}

VipEntry::VipEntry(const std::string key, const double local_vip_count, const double rx_vip_avg) :
key_(key), local_vip_count_(local_vip_count), rx_vip_avg_(rx_vip_avg), next_(NULL) {}
            
std::string VipEntry::getKey() const {
  return key_;
}

void VipEntry::incLocalCount(const double amount) {
  local_vip_count_ = local_vip_count_ + amount;
}

void VipEntry::decLocalCount(const double amount) {
  local_vip_count_ = (local_vip_count_<amount)?0:(local_vip_count_ - amount);
}

void VipEntry::setLocalCount(const double local_vip_count) {
  local_vip_count_ = local_vip_count;
}

double VipEntry::getLocalCount() const {
  return local_vip_count_;
}

void VipEntry::setRxAvg(const double rx_vip_avg) {
  rx_vip_avg_ = rx_vip_avg;
}

double VipEntry::getRxAvg() const {
  return rx_vip_avg_;
}

/*!
\brief This function takes a face ID and VIP count, finds the neighbor with that face ID
in the neighbor vector of the VIP entry (corresponding to the given data name in the)
current node's VIP table and updates that neighbor's current known VIP count with
the input VIP count.

A search predicate is set up to compare the input face_id to the passed neighbor's face_id.
std::find_if() function is then used with this search predicate to find a matching face_id value
inside the neighbor_vector_ of the current VipEntry. If such a match is found, the iterator
returned will point to the matching neighbor's NeighborEntry. Using this iterator, the vip_count
of the neighbor is updated. If no match is found, an error is generated. There can't be more
than one match, since face IDs are unique for each neighbor in the FIB.
*/
void VipEntry::setNeighborCount(const int face_id, double vip_count) {
  auto pred = [face_id](const NeighborEntry& neighbor) {
    return neighbor.face_id == face_id;
  };
  std::vector<NeighborEntry>::iterator it = std::find_if(neighbor_vector_.begin(),neighbor_vector_.end(),pred);
  if (it!=neighbor_vector_.end()) {
    it->vip_count = vip_count;
  }
  else {
      NeighborEntry neighbor_Entry(face_id, vip_count, 0);
      neighbor_vector_.push_back(neighbor_Entry);
    // TO DO: Appropriate error / exception handling
    //std::cout << "Attempt to update neighbor VIP count failed: no neighbor with given face ID exists.\n";
  }
}

void VipEntry::setNeighborTxAvg(const int face_id, const double tx_vip_avg) {
  auto pred = [face_id](const NeighborEntry& neighbor) {
    return neighbor.face_id == face_id;
  };
  std::vector<NeighborEntry>::iterator it = std::find_if(neighbor_vector_.begin(),neighbor_vector_.end(),pred);
  if (it!=neighbor_vector_.end()) {
    it->tx_vip_avg = tx_vip_avg;
  }
  else {
      NeighborEntry neighbor_Entry(face_id, 0, tx_vip_avg);
      neighbor_vector_.push_back(neighbor_Entry);
    // TO DO: Appropriate error / exception handling
    //std::cout << "Attempt to update transmitted VIP average to neighbor failed: no neighbor with given face ID exists.\n";
  }
}

std::vector<NeighborEntry> VipEntry::getNeighborValues() const {
  return neighbor_vector_;
}

double VipEntry::getNeighborCount(const int face_id){
  auto pred = [face_id](const NeighborEntry& neighbor) {
    return neighbor.face_id == face_id;
  };
  std::vector<NeighborEntry>::iterator it = std::find_if(neighbor_vector_.begin(),neighbor_vector_.end(),pred);
  if (it!=neighbor_vector_.end()) {
    return it->vip_count;
  }
  else {
    // TO DO: Appropriate error / exception handling
      NeighborEntry neighbor_Entry(face_id, 0, 0);
      neighbor_vector_.push_back(neighbor_Entry);
    //std::cout << "Attempt to get neighbor VIP count failed: no neighbor with given face ID exists.\n";
    return 0;
  }
}

double VipEntry::getNeighborTxAvg(const int face_id){
  auto pred = [face_id](const NeighborEntry& neighbor) {
    return neighbor.face_id == face_id;
  };
  std::vector<NeighborEntry>::iterator it = std::find_if(neighbor_vector_.begin(),neighbor_vector_.end(),pred);
  if (it!=neighbor_vector_.end()) {
    return it->tx_vip_avg;
  }
  else {
    // TO DO: Appropriate error / exception handling
      NeighborEntry neighbor_Entry(face_id, 0, 0);
      neighbor_vector_.push_back(neighbor_Entry);
    //std::cout << "Attempt to get transmitted VIP average to neighbor failed: no neighbor with given face ID exists.\n";
    return 0;
  }
}


void VipEntry::setNext(VipEntry *next) {
  VipEntry::next_ = next;
}

VipEntry * VipEntry::getNext() const {
  return next_;
}
        }}}

#ifndef NEIGHBOR_ENTRY_HPP
#define NEIGHBOR_ENTRY_HPP

#include <iostream>
namespace nfd {
    namespace fw {
        namespace VIP{
struct NeighborEntry {
  const int face_id;
  double vip_count;
  double tx_vip_avg;
      long last_timestamp;
  NeighborEntry(const int faceid, const double vipcount, const double txvipavg,const long last_timestamp);
  bool operator==(const NeighborEntry& neighbor);
  friend std::ostream& operator<<(std::ostream& o, const NeighborEntry& neighbor);
};
      }}}
#endif

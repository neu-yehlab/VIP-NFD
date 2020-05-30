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
  NeighborEntry(const int faceid, const double vipcount, const double txvipavg);
  bool operator==(const NeighborEntry& neighbor);
  friend std::ostream& operator<<(std::ostream& o, const NeighborEntry& neighbor);
};
      }}}
#endif

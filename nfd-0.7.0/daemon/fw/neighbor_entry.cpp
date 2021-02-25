#include "neighbor_entry.hpp"
namespace nfd {
    namespace fw {
        namespace VIP{
NeighborEntry::NeighborEntry(const int face_id, const double vip_count, const double tx_vip_avg, const long last_timestamp)
: face_id(face_id), vip_count(vip_count), tx_vip_avg(tx_vip_avg), last_timestamp(last_timestamp) {}

bool NeighborEntry::operator==(const NeighborEntry& neighbor) {
    return (face_id == neighbor.face_id && vip_count == neighbor.vip_count && tx_vip_avg == neighbor.tx_vip_avg && last_timestamp == neighbor.last_timestamp);
}


std::ostream& operator<<(std::ostream& o, const NeighborEntry& neighbor){
    o << "Neighbor's face id: " << neighbor.face_id << "\tNeighbor's VIP count: " << neighbor.vip_count << "\tAverage VIP transmitted to neighbor" << neighbor.tx_vip_avg << "\n";
    return o;
}
        }}}



#ifndef VIP_ENTRY_HPP
#define VIP_ENTRY_HPP

#include <string>
#include <vector>
#include <iterator>
#include <algorithm>
#include <cmath>
#include "neighbor_entry.hpp"
#include "VIP_configure.hpp"
#include <chrono>
namespace nfd {
    namespace fw {
        namespace VIP{
class VipEntry {
  public:
    VipEntry(const std::string key, const double local_vip_count, const double rx_vip_avg, const std::vector<NeighborEntry> neighbor_vector,const long timestamp);
    VipEntry(const std::string key, const double local_vip_count, const double rx_vip_avg,const long timestamp);
    std::string getKey() const;
    void incLocalCount(const double amount);
    void decLocalCount(const double amount);
    void setLocalCount(const double local_vip_count);
    double getLocalCount() const;
    void setRxAvg(const double rx_vip_avg);
    //void setLastTimeStamp(const long time_stamp);
    // void updateRxAvg(const double amount, const time_t timestamp);
    double getRxAvg() const;
    // void setNeighborValues(std::vector<NeighborEntry> neighbor_vector); // this is probably not needed
    void setNeighborCount(const int face_id, const double vip_count);
    void setNeighborTxAvg(const int face_id, const double tx_vip_avg);
    // void updateNeighborTxAvg(const int face_id, const double amount);
    std::vector<NeighborEntry> getNeighborValues() const;
    double getNeighborCount(const int face_id);
    double getNeighborTxAvg(const int face_id);
    void setNext(VipEntry *next);
    void updateRxAvg(const long timestamp, const double vip_amount);
    void updateNeighborTxAvg(const int face_id, const long timestamp, const double vip_amount);
    VipEntry * getNext() const;
  private:
    std::string key_;
    double local_vip_count_;
    double rx_vip_avg_;
    std::vector<NeighborEntry> neighbor_vector_;
    long last_timestamp_;
    VipEntry *next_;
};
}}}
#endif

#ifndef VIP_TABLE_HPP
#define VIP_TABLE_HPP

#include "vip_entry.hpp"
#include "common/city-hash.hpp"
#include <stdint.h>
#include <string>
#include "algorithm.hpp"
#include "VIP_configure.hpp"
namespace nfd {
    namespace fw {
        namespace VIP{
class VipTable {
  public:
    VipTable(const uint16_t table_size);
    ~VipTable();
    uint16_t getHash(const std::string key);
    uint16_t getSize();
    bool checkEntry(const std::string key);
    VipEntry* getEntry(const std::string key);
    double getLocalCount(const std::string key);
    void incLocalCount(const std::string key, const double amount);
    void decLocalCount(const std::string key, const double amount);
    void resetLocalCounts(std::vector<std::string> key_list);
    void resetRxVipAvgs(std::vector<std::string> key_list);
    void resetNeighborCount(const std::string key, const int face_id);
    void resetNeighborTxAvg(const std::string key, const int face_id);
    double getRxVipAvg(const std::string key);
    double getNeighborCount(const std::string key, const int face_id);
    double getNeighborTxAvg(const std::string key, const int face_id);
    void insert(const std::string key, const double local_vip_count, const double rx_vip_avg, const std::vector<NeighborEntry> neighbor_vector);
    void erase(const std::string key);
    void resize();
    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    void setNeighborCount(const std::string key, const int face_id, const double vip_count);
    void setNeighborTxAvg(const std::string key, const int face_id, const double tx_vip_avg);
    std::string generateDataAContent();
    std::string generateDataBContent(long face_id);
    void updateRxAvg(const std::string key, const double rx_vip_avg);
    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
  private:
    VipEntry **vip_table_;
    uint16_t table_size_;
    uint16_t catalog_size_;
};
   }}}
#endif

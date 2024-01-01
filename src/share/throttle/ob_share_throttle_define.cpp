/**
 * Copyright (c) 2021 OceanBase
 * OceanBase CE is licensed under Mulan PubL v2.
 * You can use this software according to the terms and conditions of the Mulan PubL v2.
 * You may obtain a copy of Mulan PubL v2 at:
 *          http://license.coscl.org.cn/MulanPubL-2.0
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PubL v2 for more details.
 */

#include "ob_share_throttle_define.h"

#include "observer/omt/ob_tenant_config_mgr.h"
#include "lib/alloc/alloc_func.h"

namespace oceanbase {

namespace share {

int64_t FakeAllocatorForTxShare::resource_unit_size()
{
  static const int64_t SHARE_RESOURCE_UNIT_SIZE = 2L * 1024L * 1024L; /* 2MB */
  return SHARE_RESOURCE_UNIT_SIZE;
}

void FakeAllocatorForTxShare::init_throttle_config(int64_t &resource_limit,
                                                   int64_t &trigger_percentage,
                                                   int64_t &max_duration)
{
  // define some default value
  const int64_t SR_LIMIT_PERCENTAGE = 60;
  const int64_t SR_THROTTLE_TRIGGER_PERCENTAGE = 60;
  const int64_t SR_THROTTLE_MAX_DURATION = 2LL * 60LL * 60LL * 1000LL * 1000LL;  // 2 hours

  int64_t total_memory = lib::get_tenant_memory_limit(MTL_ID());

  omt::ObTenantConfigGuard tenant_config(TENANT_CONF(MTL_ID()));
  if (tenant_config.is_valid()) {
    int64_t share_mem_limit = tenant_config->_tx_share_memory_limit_percentage;
    // if _tx_share_memory_limit_percentage equals 1, use (memstore_limit_percentage + 10) as default value
    if (0 == share_mem_limit) {
      share_mem_limit = tenant_config->memstore_limit_percentage + 10;
    }
    resource_limit = total_memory * share_mem_limit / 100LL;
    trigger_percentage = tenant_config->writing_throttling_trigger_percentage;
    max_duration = tenant_config->writing_throttling_maximum_duration;
  } else {
    resource_limit = total_memory * SR_LIMIT_PERCENTAGE / 100;
    trigger_percentage = SR_THROTTLE_TRIGGER_PERCENTAGE;
    max_duration = SR_THROTTLE_MAX_DURATION;
  }
}

/**
 * @brief Because the TxShare may can not use enough memory as the config specified, it need dynamic modify
 * resource_limit according to the memory remained of the tenant.
 *
 * @param[in] holding_size this allocator current holding memory size
 * @param[in] config_specify_resource_limit the memory limit which _tx_share_memory_limit_percentage specified
 * @param[out] resource_limit the real limit now
 * @param[out] last_update_limit_ts last update ts (for performance optimization)
 * @param[out] is_updated to decide if update_decay_factor() is needed
 */
void FakeAllocatorForTxShare::adaptive_update_limit(const int64_t tenant_id,
                                                    const int64_t holding_size,
                                                    const int64_t config_specify_resource_limit,
                                                    int64_t &resource_limit,
                                                    int64_t &last_update_limit_ts,
                                                    bool &is_updated)
{
  static const int64_t UPDATE_LIMIT_INTERVAL = 100LL * 1000LL; // 100 ms
  static const int64_t USABLE_REMAIN_MEMORY_PERCETAGE = 60;
  static const int64_t MAX_UNUSABLE_MEMORY = 2LL * 1024LL * 1024LL * 1024LL; // 2 GB

  int64_t cur_ts = ObClockGenerator::getClock();
  int64_t old_ts = last_update_limit_ts;
  if ((cur_ts - old_ts > UPDATE_LIMIT_INTERVAL) && ATOMIC_BCAS(&last_update_limit_ts, old_ts, cur_ts)) {
    int64_t remain_memory = lib::get_tenant_memory_remain(tenant_id);
    int64_t usable_remain_memory = remain_memory / 100 * USABLE_REMAIN_MEMORY_PERCETAGE;
    if (remain_memory > MAX_UNUSABLE_MEMORY) {
      usable_remain_memory = std::max(usable_remain_memory, remain_memory - MAX_UNUSABLE_MEMORY);
    }

    is_updated = false;
    if (holding_size + usable_remain_memory < config_specify_resource_limit) {
      resource_limit = holding_size + usable_remain_memory;
      is_updated = true;
    } else if (resource_limit != config_specify_resource_limit) {
      resource_limit = config_specify_resource_limit;
      is_updated = true;
    } else {
      // do nothing
    }

    if (is_updated && REACH_TIME_INTERVAL(10LL * 1000LL * 1000LL)) {
      SHARE_LOG(INFO,
                "adaptive update",
                "Tenant ID", tenant_id,
                "Config Specify Resource Limit(MB)", config_specify_resource_limit / 1024 / 1024,
                "TxShare Current Memory Limit(MB)", resource_limit / 1024 / 1024,
                "Holding Memory(MB)", holding_size / 1024 / 1024,
                "Tenant Remain Memory(MB)", remain_memory / 1024 / 1024,
                "Usable Remain Memory(MB)", usable_remain_memory / 1024 /1024,
                "Last Update Limit Timestamp", last_update_limit_ts,
                "Is Updated", is_updated);
    }
  }
}

}  // namespace share
}  // namespace oceanbase
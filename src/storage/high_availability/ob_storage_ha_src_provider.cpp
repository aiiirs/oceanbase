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

#define USING_LOG_PREFIX STORAGE
#include "ob_storage_ha_src_provider.h"
#include "share/location_cache/ob_location_service.h"
#include "observer/ob_server_event_history_table_operator.h"
#include "storage/tx_storage/ob_ls_handle.h"
#include "storage/tx_storage/ob_ls_service.h"
#include "storage/high_availability/ob_storage_ha_utils.h"
#include "storage/ob_locality_manager.h"

namespace oceanbase {
using namespace share;
namespace storage {
ERRSIM_POINT_DEF(EN_FORCE_NOT_CHOOSE_C_REPLICA);

/**
 * ------------------------------ObStorageHAGetMemberHelper---------------------
 */
ObStorageHAGetMemberHelper::ObStorageHAGetMemberHelper()
  : is_inited_(false),
    storage_rpc_(nullptr)
{
}

ObStorageHAGetMemberHelper::~ObStorageHAGetMemberHelper()
{
}

int ObStorageHAGetMemberHelper::init(storage::ObStorageRpc *storage_rpc)
{
  int ret = OB_SUCCESS;
  if (is_inited_) {
    ret = OB_INIT_TWICE;
    LOG_WARN("ObStorageHAGetMemberHelper init twice", K(ret));
  } else if (OB_ISNULL(storage_rpc)) {
    ret = OB_INVALID_ARGUMENT;
    LOG_WARN("invalid argument", K(ret), KP(storage_rpc));
  } else {
    storage_rpc_ = storage_rpc;
    is_inited_ = true;
  }
  return ret;
}

int ObStorageHAGetMemberHelper::get_ls_member_list(const uint64_t tenant_id,
    const share::ObLSID &ls_id, common::ObIArray<common::ObAddr> &addr_list)
{
  int ret = OB_SUCCESS;
  addr_list.reset();
  common::GlobalLearnerList learner_list;
  common::ObAddr leader_addr;
  if (IS_NOT_INIT) {
    ret = OB_NOT_INIT;
    LOG_WARN("ObStorageHAGetMemberHelper do not init", K(ret));
  } else if (OB_INVALID_ID == tenant_id || !ls_id.is_valid()) {
    ret = OB_INVALID_ARGUMENT;
    LOG_WARN("get invalid args", K(ret), K(tenant_id), K(ls_id));
  } else if (OB_FAIL(get_ls_leader(tenant_id, ls_id, leader_addr))) {
    LOG_WARN("failed to get ls leader", K(ret), K(tenant_id), K(ls_id));
  } else if (OB_FAIL(fetch_ls_member_list_and_learner_list_(tenant_id, ls_id, false/*need_learner_list*/, leader_addr,
      learner_list, addr_list))) {
    LOG_WARN("failed to fetch ls member list", K(ret), K(tenant_id), K(ls_id), K(leader_addr));
  }
  return ret;
}

int ObStorageHAGetMemberHelper::get_ls_member_list_and_learner_list(
    const uint64_t tenant_id, const share::ObLSID &ls_id, const bool need_learner_list,
    common::ObAddr &leader_addr, common::GlobalLearnerList &learner_list,
    common::ObIArray<common::ObAddr> &member_list)
{
  int ret = OB_SUCCESS;
  leader_addr.reset();
  member_list.reset();
  learner_list.reset();
  common::ObArray<common::ObAddr> learner_addr_array;
  if (IS_NOT_INIT) {
    ret = OB_NOT_INIT;
    LOG_WARN("ObStorageHAGetMemberHelper do not init", K(ret));
  } else if (OB_INVALID_ID == tenant_id || !ls_id.is_valid()) {
    ret = OB_INVALID_ARGUMENT;
    LOG_WARN("get invalid args", K(ret), K(tenant_id), K(ls_id));
  } else if (OB_FAIL(get_ls_leader(tenant_id, ls_id, leader_addr))) {
    LOG_WARN("failed to get ls leader", K(ret), K(tenant_id), K(ls_id));
  } else if (OB_FAIL(fetch_ls_member_list_and_learner_list_(tenant_id, ls_id, need_learner_list,
      leader_addr, learner_list, member_list))) {
    LOG_WARN("failed to fetch ls member list and learner list", K(ret), K(tenant_id), K(ls_id),
        K(leader_addr), K(need_learner_list));
  }
  return ret;
}

int ObStorageHAGetMemberHelper::get_ls_member_list_and_learner_list_(
    const uint64_t tenant_id,
    const share::ObLSID &ls_id,
    const bool need_learner_list,
    common::ObAddr &leader_addr,
    common::GlobalLearnerList &learner_list,
    common::ObIArray<common::ObAddr> &member_list)
{
  int ret = OB_SUCCESS;
  member_list.reset();
  learner_list.reset();
  ObLSHandle ls_handle;
  ObLS *ls = nullptr;
  ObStorageHASrcInfo src_info;
  src_info.src_addr_ = leader_addr;
  src_info.cluster_id_ = GCONF.cluster_id;
  obrpc::ObFetchLSMemberListInfo member_info;
  obrpc::ObFetchLSMemberAndLearnerListInfo member_and_learner_info;
  ObLSService *ls_service = nullptr;
  if (OB_FAIL(get_ls(ls_id, ls_handle))) {
    LOG_WARN("failed to get ls handle", K(ret), K(ls_id));
  } else if (OB_ISNULL(ls = ls_handle.get_ls())) {
    ret = OB_ERR_UNEXPECTED;
    LOG_WARN("ls should not be NULL", K(ret), KP(ls), K(tenant_id), K(ls_id));
  } else if (need_learner_list) {
    if (OB_FAIL(storage_rpc_->fetch_ls_member_and_learner_list(tenant_id, ls_id, src_info, member_and_learner_info))) {
      LOG_WARN("failed to fetch ls member list and learner list request", K(ret), K(tenant_id), K(src_info), K(ls_id));
      //overwrite ret
      member_and_learner_info.reset();
      if (OB_FAIL(ls->get_log_handler()->get_election_leader(src_info.src_addr_))) {
        LOG_WARN("failed to get election leader", K(ret), K(tenant_id), K(ls_id));
      } else {
        leader_addr = src_info.src_addr_;
        if (OB_FAIL(storage_rpc_->fetch_ls_member_and_learner_list(tenant_id, ls_id, src_info, member_and_learner_info))) {
          LOG_WARN("failed to post ls member list and learner list request", K(ret), K(tenant_id), K(src_info), K(ls_id));
        }
      }
    }
    if (OB_FAIL(ret)) {
    } else if (OB_FAIL(member_and_learner_info.member_list_.get_addr_array(member_list))) {
      LOG_WARN("failed to get member addr array", K(ret), K(member_and_learner_info));
    } else if (OB_FAIL(learner_list.deep_copy(member_and_learner_info.learner_list_))) {
      LOG_WARN("failed to get learner addr array", K(ret), K(member_and_learner_info));
    }
  } else {
    if (OB_FAIL(storage_rpc_->post_ls_member_list_request(tenant_id, src_info, ls_id, member_info))) {
      LOG_WARN("failed to post ls member list request", K(ret), K(tenant_id), K(src_info), K(ls_id));
      //overwrite ret
      member_info.reset();
      if (OB_FAIL(ls->get_log_handler()->get_election_leader(src_info.src_addr_))) {
        LOG_WARN("failed to get election leader", K(ret), K(tenant_id), K(ls_id));
      } else {
        leader_addr = src_info.src_addr_;
        if (OB_FAIL(storage_rpc_->post_ls_member_list_request(tenant_id, src_info, ls_id, member_info))) {
          LOG_WARN("failed to post ls member list request", K(ret), K(tenant_id), K(src_info), K(ls_id));
        }
      }
    }
    if (OB_FAIL(ret)) {
    } else if (OB_FAIL(member_info.member_list_.get_addr_array(member_list))) {
      LOG_WARN("failed to get member addr array", K(ret), K(member_info));
    } else {
      FLOG_INFO("fetch ls member list", K(tenant_id), K(ls_id), K(src_info), K(member_and_learner_info),
          K(member_info), K(member_list), K(learner_list));
    }
  }
  return ret;
}

int ObStorageHAGetMemberHelper::get_ls_leader(const uint64_t tenant_id, const share::ObLSID &ls_id, common::ObAddr &leader)
{
  int ret = OB_SUCCESS;
  leader.reset();
  if (IS_NOT_INIT) {
    ret = OB_NOT_INIT;
    LOG_WARN("ObStorageHAGetMemberHelper do not init", K(ret));
  } else if (OB_INVALID_ID == tenant_id || !ls_id.is_valid()) {
    ret = OB_INVALID_ARGUMENT;
    LOG_WARN("get invalid args", K(ret), K(tenant_id), K(ls_id));
  } else if (OB_FAIL(ObStorageHAUtils::get_ls_leader(tenant_id, ls_id, leader))) {
    LOG_WARN("failed to get ls leader", K(ret), K(tenant_id), K(ls_id));
  }
  return ret;
}

int ObStorageHAGetMemberHelper::fetch_ls_member_list_and_learner_list_(const uint64_t tenant_id, const share::ObLSID &ls_id,
    const bool need_learner_list, common::ObAddr &leader_addr,
    common::GlobalLearnerList &learner_list,
    common::ObIArray<common::ObAddr> &member_list)
{
  int ret = OB_SUCCESS;
  member_list.reset();
  learner_list.reset();
  if (OB_ISNULL(storage_rpc_)) {
    ret = OB_ERR_UNEXPECTED;
    LOG_WARN("storage rpc should not be null", K(ret));
  } else if (OB_INVALID_ID == tenant_id || !ls_id.is_valid() || !leader_addr.is_valid()) {
    ret = OB_INVALID_ARGUMENT;
    LOG_WARN("get invalid args", K(ret), K(tenant_id), K(ls_id), K(leader_addr));
  } else if (OB_FAIL(get_ls_member_list_and_learner_list_(tenant_id, ls_id,
      need_learner_list, leader_addr, learner_list, member_list))) {
    LOG_WARN("failed to get ls member list and learner list", K(ret), K(tenant_id), K(ls_id),
        K(need_learner_list), K(leader_addr));
  }
  return ret;
}

int ObStorageHAGetMemberHelper::get_ls(const share::ObLSID &ls_id, ObLSHandle &ls_handle)
{
  int ret = OB_SUCCESS;
  ls_handle.reset();
  if (IS_NOT_INIT) {
    ret = OB_NOT_INIT;
    LOG_WARN("ObStorageHAGetMemberHelper do not init", K(ret));
  } else if (!ls_id.is_valid()) {
    ret = OB_INVALID_ARGUMENT;
    LOG_WARN("get invalid args", K(ret), K(ls_id));
  } else if (OB_FAIL(ObStorageHADagUtils::get_ls(ls_id, ls_handle))) {
    LOG_WARN("failed to get ls", K(ret), K(ls_id));
  }
  return ret;
}

bool ObStorageHAGetMemberHelper::check_tenant_primary()
{
  return MTL_TENANT_ROLE_CACHE_IS_PRIMARY();
}

int ObStorageHAGetMemberHelper::filter_dest_replica_(
    const common::ObReplicaMember &dst,
    common::GlobalLearnerList &learner_list)
{
  int ret = OB_SUCCESS;
  if (!dst.is_valid() || !learner_list.is_valid()) {
    ret = OB_INVALID_ARGUMENT;
    LOG_WARN("invalid argument!", K(ret), K(dst), K(learner_list));
  } else if (!learner_list.contains(dst.get_server())) {
    ret = OB_ERR_UNEXPECTED;
    LOG_WARN("learner list must include dst", K(ret), K(learner_list), K(dst));
  } else if (OB_FAIL(learner_list.remove_learner(dst.get_server()))) {
    LOG_WARN("failed to remove learner", K(ret), K(learner_list), K(dst));
  }
  return ret;
}

int ObStorageHAGetMemberHelper::check_is_first_c_replica_(
    const common::ObReplicaMember &dst,
    const common::GlobalLearnerList &learner_list,
    const bool &need_learner_list,
    bool &is_first_c_replica)
{
  int ret = OB_SUCCESS;
  is_first_c_replica = false;
  if (!dst.is_valid() || (need_learner_list && !learner_list.is_valid())) {
    ret = OB_INVALID_ARGUMENT;
    LOG_WARN("invalid argument!", K(ret), K(dst), K(need_learner_list), K(learner_list));
  } else if (!need_learner_list || !dst.is_columnstore()) {
    is_first_c_replica = false;
  } else {
    int64_t learner_count = learner_list.get_member_number();
    int64_t idx = 0;
    for (idx = 0; OB_SUCC(ret) && idx < learner_count; ++idx) {
      common::ObMember member;
      if (OB_FAIL(learner_list.get_learner(idx, member))) {
        LOG_WARN("failed to get learner", K(ret), K(idx), K(learner_list));
      } else if (member.is_columnstore()) {
        // stop at first c replica
        if (member.get_server() == dst.get_server()) {
          is_first_c_replica = true;
        } else {
          is_first_c_replica = false;
        }
        break;
      }

      if (OB_SUCC(ret) && idx == learner_count) {
        ret = OB_ERR_UNEXPECTED;
        LOG_WARN("learner list does not contain dst", K(ret), K(learner_list), K(dst));
      }
    }
  }
  return ret;
}

int ObStorageHAGetMemberHelper::get_member_list_by_replica_type(
    const uint64_t tenant_id, const share::ObLSID &ls_id, const common::ObReplicaMember &dst,
    ObLSMemberListInfo &info, bool &is_first_c_replica)
{
  int ret = OB_SUCCESS;
  bool need_learner_list = false;
  common::ObArray<common::ObAddr> learner_addr_array;
  info.reset();
  if (IS_NOT_INIT) {
    ret = OB_NOT_INIT;
    LOG_WARN("ObStorageHAGetMemberHelper do not init", K(ret));
  } else if (OB_INVALID_ID == tenant_id || !ls_id.is_valid()
             || !dst.is_valid()) {
    ret = OB_INVALID_ARGUMENT;
    LOG_WARN("get invalid args", K(ret), K(tenant_id), K(ls_id), K(dst));
  } else {
    if (common::ObReplicaType::REPLICA_TYPE_FULL == dst.get_replica_type()) {
      need_learner_list = false;
    } else if (ObReplicaTypeCheck::is_non_paxos_replica(dst.get_replica_type())) {
      need_learner_list = true;
    } else {
      ret = OB_ERR_UNEXPECTED;
      LOG_WARN("unexpected replica type", K(ret), "replica_type", dst.get_replica_type());
    }
    if (FAILEDx(get_ls_member_list_and_learner_list(tenant_id, ls_id, need_learner_list,
        info.leader_addr_, info.learner_list_, info.member_list_))) {
      LOG_WARN("failed to fetch ls leader member list and learner list", K(ret), K(tenant_id), K(ls_id),
          K(need_learner_list));
    } else if (OB_FAIL(check_is_first_c_replica_(dst, info.learner_list_, need_learner_list, is_first_c_replica))) {
      LOG_WARN("failed to check is first c replica", K(ret), K(dst), K(need_learner_list), K(info));
    } else if (need_learner_list && OB_FAIL(filter_dest_replica_(dst, info.learner_list_))) {
      LOG_WARN("failed to filter dest replica", K(ret), K(info), K(dst), K(is_first_c_replica));
    } else if (info.learner_list_.is_valid()) {
      if (OB_FAIL(info.learner_list_.get_addr_array(learner_addr_array))) {
        LOG_WARN("failed to get addr array from learner list", K(ret), K(info), K(is_first_c_replica));
      } else if (OB_FAIL(common::append(info.member_list_, learner_addr_array))) {
        LOG_WARN("failed to append addr list", K(ret), K(info), K(learner_addr_array), K(is_first_c_replica));
      }
    }
    LOG_INFO("get member info", K(ret), K(info), K(is_first_c_replica));
  }
  return ret;
}
/**
 * ------------------------------ObStorageHASrcProvider---------------------
 */
ObStorageHASrcProvider::ObStorageHASrcProvider()
  : is_inited_(false),
    member_list_info_(),
    is_first_c_replica_(false),
    tenant_id_(OB_INVALID_ID),
    ls_id_(),
    type_(ObMigrationOpType::MAX_LS_OP),
    local_clog_checkpoint_scn_(),
    palf_parent_checkpoint_scn_(),
    member_helper_(nullptr),
    storage_rpc_(nullptr),
    policy_type_(ChooseSourcePolicy::IDC)
{}

ObStorageHASrcProvider::~ObStorageHASrcProvider()
{
  member_helper_ = nullptr;
  storage_rpc_ = nullptr;
}

int ObStorageHASrcProvider::init(const ObMigrationChooseSrcHelperInitParam &param,
    const ChooseSourcePolicy policy_type,
    storage::ObStorageRpc *storage_rpc,
    ObStorageHAGetMemberHelper *member_helper)
{
  int ret = OB_SUCCESS;
  if (is_inited_) {
    ret = OB_INIT_TWICE;
    LOG_WARN("storage ha src provider init twice", K(ret));
  } else if (!param.is_valid() || OB_ISNULL(storage_rpc)
      || policy_type < ChooseSourcePolicy::IDC
      || policy_type >= ChooseSourcePolicy::MAX_POLICY
      || OB_ISNULL(member_helper)) {
    ret = OB_INVALID_ARGUMENT;
    LOG_WARN("get invalid argument", K(ret), K(param),
        KP(storage_rpc), K(policy_type), KP(member_helper));
  } else {
    storage_rpc_ = storage_rpc;
    member_helper_ = member_helper;
    if (OB_FAIL(init_palf_parent_checkpoint_scn_(param.tenant_id_, param.ls_id_,
        param.local_clog_checkpoint_scn_, param.arg_.dst_.get_replica_type()))) {
      LOG_WARN("failed to init palf parent checkpoint scn", K(ret), K(param), KP(storage_rpc_));
    } else if (OB_FAIL(member_list_info_.assign(param.info_))) {
      LOG_WARN("failed to assign member list info", K(ret), K(param));
    } else {
      tenant_id_ = param.tenant_id_;
      ls_id_ = param.ls_id_;
      type_ = param.arg_.type_;
      local_clog_checkpoint_scn_ = param.local_clog_checkpoint_scn_;
      policy_type_ = policy_type;
      is_first_c_replica_ = param.is_first_c_replica_;
    }
  }
  return ret;
}

int ObStorageHASrcProvider::fetch_ls_meta_info_(const uint64_t tenant_id, const share::ObLSID &ls_id,
    const common::ObAddr &member_addr, obrpc::ObFetchLSMetaInfoResp &ls_meta_info)
{
  int ret = OB_SUCCESS;
  ls_meta_info.reset();
  ObStorageHASrcInfo src_info;
  src_info.src_addr_ = member_addr;
  src_info.cluster_id_ = GCONF.cluster_id;
  if (OB_ISNULL(storage_rpc_)) {
    ret = OB_ERR_UNEXPECTED;
    LOG_WARN("storage rpc should not be null", K(ret));
  } else if (OB_INVALID_ID == tenant_id || !ls_id.is_valid() || !member_addr.is_valid()) {
    ret = OB_INVALID_ARGUMENT;
    LOG_WARN("get invalid args", K(ret), K(tenant_id), K(ls_id), K(member_addr));
  } else {
    uint32_t renew_count = 0;
    const uint32_t max_renew_count = 3;
    const int64_t retry_us = 200 * 1000;
    const int64_t start_ts = ObTimeUtility::current_time();
    const int64_t DEFAULT_GET_LS_META_RPC_TIMEOUT = 1 * 60 * 1000 * 1000L;
    do {
      if (OB_FAIL(storage_rpc_->post_ls_meta_info_request(tenant_id, src_info, ls_id, ls_meta_info))) {
        if (renew_count++ < max_renew_count) {  // retry three times
          LOG_WARN("failed to post ls info request", K(ret), K(tenant_id), K(ls_id), K(src_info), KP(storage_rpc_));
          if (ObTimeUtility::current_time() - start_ts > DEFAULT_GET_LS_META_RPC_TIMEOUT) {
            renew_count = max_renew_count;
          } else {
            ob_usleep(retry_us);
          }
        }
      } else {
        LOG_INFO("succeed to get ls meta", K(tenant_id), K(ls_id), K(ls_meta_info));
        break;
      }
    } while (renew_count < max_renew_count);
  }
  return ret;
}

int ObStorageHASrcProvider::check_replica_type_for_c_replica_(
    const common::ObAddr &addr,
    const common::ObReplicaMember &dst,
    const common::GlobalLearnerList &learner_list,
    bool &is_replica_type_valid)
{
  int ret = OB_SUCCESS;
  ObMember src;
  is_replica_type_valid = false;
  if (!addr.is_valid() || !dst.is_valid() || !learner_list.is_valid()) {
    ret = OB_INVALID_ARGUMENT;
    LOG_WARN("invalid argument!", K(ret), K(addr), K(dst), K(learner_list));
  } else if (OB_FAIL(learner_list.get_learner_by_addr(addr, src))) {
    LOG_WARN("failed to get learner by addr", KR(ret), K(addr));
  } else if (src.is_columnstore()) {
    is_replica_type_valid = true;
  }
  return ret;
}

int ObStorageHASrcProvider::check_replica_type_for_normal_replica_(
    const common::ObAddr &addr,
    const common::ObReplicaMember &dst,
    const common::GlobalLearnerList &learner_list,
    bool &is_replica_type_valid)
{
  int ret = OB_SUCCESS;
  if (!addr.is_valid() || !dst.is_valid()) {
    ret = OB_INVALID_ARGUMENT;
    LOG_WARN("invalid argument!", K(ret), K(addr), K(dst));
  } else if (learner_list.is_valid() && learner_list.contains(addr)) {
    // src is R/C
    ObMember src;
    if (OB_FAIL(learner_list.get_learner_by_addr(addr, src))) {
      LOG_WARN("failed to get learner by addr", KR(ret), K(addr));
    } else if (src.is_columnstore()) {
      // src is C, dst can only be C as well
      is_replica_type_valid = REPLICA_TYPE_COLUMNSTORE == dst.get_replica_type();
#ifdef ERRSIM
      if (OB_SUCCESS != EN_FORCE_NOT_CHOOSE_C_REPLICA) {
        is_replica_type_valid = false;
      }
#endif
    } else {
      // src is R, dst can be non-paxos replica-type (R or C)
      if (common::ObReplicaType::REPLICA_TYPE_FULL == dst.get_replica_type()) { // dst is F
        is_replica_type_valid = false;
      } else if (ObReplicaTypeCheck::is_non_paxos_replica(dst.get_replica_type())) {
        is_replica_type_valid = true;
      } else {
        ret = OB_ERR_UNEXPECTED;
        LOG_WARN("unexpected dst replica type", K(ret), K(dst), K(learner_list));
      }
    }
  } else {
    // src is F, dst can be all replica-type (F/R/C)
    if (ObReplicaTypeCheck::is_replica_type_valid(dst.get_replica_type())) {
      is_replica_type_valid = true;
    } else {
      ret = OB_ERR_UNEXPECTED;
      LOG_WARN("unexpected dst replica type", K(ret), K(dst), K(learner_list));
    }
  }
  return ret;
}

int ObStorageHASrcProvider::check_replica_type_(
    const common::ObAddr &addr,
    const common::ObReplicaMember &dst,
    const common::GlobalLearnerList &learner_list,
    bool &is_replica_type_valid)
{
  int ret = OB_SUCCESS;
  is_replica_type_valid = false;
  if (!addr.is_valid() || !dst.is_valid()) {
    ret = OB_INVALID_ARGUMENT;
    LOG_WARN("invalid argument!", K(ret), K(addr), K(dst));
  } else {
    switch (policy_type_) {
      case ChooseSourcePolicy::C_TYPE_REPLICA: {
        // TODO(zhixing.yh) When the learner concurrency can be controlled, this function will be overwritten
        is_replica_type_valid = true;
        // if (OB_FAIL(check_replica_type_for_c_replica_(addr, dst,
        //     learner_list, is_replica_type_valid))) {
        //   LOG_WARN("failed to check replica type for c replica", K(ret), K(addr), K(dst), K(learner_list));
        // }
        break;
      };
      case ChooseSourcePolicy::IDC:
      case ChooseSourcePolicy::REGION:
      case ChooseSourcePolicy::CHECKPOINT:
      case ChooseSourcePolicy::RECOMMEND: {
        if (OB_FAIL(check_replica_type_for_normal_replica_(addr, dst, learner_list, is_replica_type_valid))) {
          LOG_WARN("failed to check replica type", K(ret), K(addr), K(dst), K(learner_list));
        }
        break;
      }
      default: {
        ret = OB_ERR_UNEXPECTED;
        LOG_WARN("unexpected policy type", K(ret), K(policy_type_));
      }
    }
  }
  return ret;
}

int ObStorageHASrcProvider::init_palf_parent_checkpoint_scn_(const uint64_t tenant_id, const share::ObLSID &ls_id,
    const share::SCN &local_clog_checkpoint_scn, const common::ObReplicaType replica_type)
{
  int ret = OB_SUCCESS;
  // local_clog_checkpoint_scn is min means firstly run migration.
  // local_clog_checkpoint_scn is not min scn when Rebuild and migretion retry.
  // The first migration run does not determine the parent checkpoint scn,
  // Rebuild and migretion retry will compare parent checkpoint scn and replica checkpoint scn
  if (local_clog_checkpoint_scn.is_min()) {
    palf_parent_checkpoint_scn_.set_min();
    LOG_INFO("palf_parent_checkpoint_scn_ set min", K(local_clog_checkpoint_scn));
  } else {
    uint32_t renew_count = 0;
    const uint32_t max_renew_count = 3;
    const int64_t retry_us = 200 * 1000;
    const int64_t start_ts = ObTimeUtility::current_time();
    const int64_t DEFAULT_GET_PARENT_CHECKPOINT_TIMEOUT = 1 * 60 * 1000 * 1000L;
    do {
      if (OB_FAIL(get_palf_parent_checkpoint_scn_from_rpc_(tenant_id, ls_id, replica_type, palf_parent_checkpoint_scn_))) {
        if (renew_count++ < max_renew_count) {  // retry three times
          LOG_WARN("failed to get parent checkpoint scn", K(ret), K(tenant_id), K(ls_id), K(replica_type), KP(storage_rpc_));
          if (ObTimeUtility::current_time() - start_ts > DEFAULT_GET_PARENT_CHECKPOINT_TIMEOUT) {
            renew_count = max_renew_count;
          } else {
            ob_usleep(retry_us);
          }
        }
      } else {
        LOG_INFO("get parent checkpoint scn", K(tenant_id), K(ls_id));
        break;
      }
    } while (renew_count < max_renew_count);
    // if get parent fail or rpc fail, it will overwrite ret and set scn is min.
    // For ensuring migration can run and ensuring upgrade compatibility
    if (OB_FAIL(ret)) {
      palf_parent_checkpoint_scn_.set_min();
      ret = OB_SUCCESS;
      LOG_INFO("after retry palf_parent_checkpoint_scn_ set min", K(local_clog_checkpoint_scn));
    }
  }
  return ret;
}

int ObStorageHASrcProvider::get_palf_parent_checkpoint_scn_from_rpc_(const uint64_t tenant_id, const share::ObLSID &ls_id,
    const common::ObReplicaType replica_type, share::SCN &parent_checkpoint_scn)
{
  int ret = OB_SUCCESS;
  common::ObAddr parent_addr;
  obrpc::ObFetchLSMetaInfoResp ls_info;
  parent_checkpoint_scn.reset();
  if (OB_INVALID_ID == tenant_id || !ls_id.is_valid()) {
    ret = OB_INVALID_ARGUMENT;
    LOG_WARN("get invalid args", K(ret), K(tenant_id), K(ls_id));
  } else if (OB_FAIL(get_palf_parent_addr_(tenant_id, ls_id, replica_type, parent_addr))) {
    LOG_WARN("failed to get palf parent addr", K(ret), K(tenant_id), K(ls_id), K(replica_type));
  } else if (OB_FAIL(fetch_ls_meta_info_(tenant_id, ls_id, parent_addr, ls_info))) {
    LOG_WARN("failed to fetch palf parent ls meta", K(ret), K(tenant_id), K(ls_id), K(parent_addr), KP(storage_rpc_));
  } else {
    parent_checkpoint_scn = ls_info.ls_meta_package_.ls_meta_.get_clog_checkpoint_scn();
    LOG_INFO("succeed to get palf parent checkpoint scn", K(tenant_id), K(ls_id), K(parent_addr), K(parent_checkpoint_scn));
  }
  return ret;
}

int ObStorageHASrcProvider::get_palf_parent_addr_(const uint64_t tenant_id, const share::ObLSID &ls_id,
    const common::ObReplicaType replica_type, common::ObAddr &parent_addr)
{
  int ret = OB_SUCCESS;
  ObLSHandle ls_handle;
  ObLS *ls = nullptr;
  parent_addr.reset();
  if (OB_FAIL(member_helper_->get_ls(ls_id, ls_handle))) {
    LOG_WARN("failed to get ls", K(ret), K(ls_id));
  } else if (OB_ISNULL(ls = ls_handle.get_ls())) {
    ret = OB_ERR_UNEXPECTED;
    LOG_WARN("ls should not be NULL", K(ret), KP(ls), K(ls_id));
  } else if (common::ObReplicaType::REPLICA_TYPE_FULL == replica_type) {
    if (OB_FAIL(member_helper_->get_ls_leader(tenant_id, ls_id, parent_addr))) {
      LOG_WARN("failed to get leader addr", K(ret), K(tenant_id), K(ls_id));
    }
  } else if (ObReplicaTypeCheck::is_non_paxos_replica(replica_type)) {
    if (OB_FAIL(ls->get_log_handler()->get_parent(parent_addr))) {
      LOG_WARN("failed to get parent addr", K(ret), K(tenant_id), K(ls_id));
    }
  } else {
    ret = OB_ERR_UNEXPECTED;
    LOG_WARN("unexpected dst replica type", K(ret), K(replica_type));
  }
  return ret;
}

int ObStorageHASrcProvider::check_replica_validity(
    const common::ObAddr &addr, const common::ObReplicaMember &dst,
    const common::GlobalLearnerList &learner_list, obrpc::ObFetchLSMetaInfoResp &ls_info)
{
  int ret = OB_SUCCESS;
  ls_info.reset();
  bool is_replica_type_valid;
  if (!is_inited_) {
    ret = OB_NOT_INIT;
    LOG_WARN("ObStorageHASrcProvider is not init.", K(ret));
  } else if (!addr.is_valid() || !dst.is_valid()) {
    ret = OB_INVALID_ARGUMENT;
    LOG_WARN("invalid argument!", K(ret), K(addr), K(dst));
  } else if (OB_FAIL(check_replica_type_(addr, dst, learner_list, is_replica_type_valid))) {
    LOG_WARN("failed to check replica type", K(ret), K(tenant_id_), K(ls_id_), K(addr),
        K(dst), K(learner_list));
  } else if (!is_replica_type_valid) {
    ret = OB_DATA_SOURCE_NOT_VALID;
    LOG_WARN("do not choose this src, replica type check failed", K(ret), K(tenant_id_), K(ls_id_), K(addr), K(dst), K(learner_list), K(ls_info));
  } else if (OB_FAIL(fetch_ls_meta_info_(tenant_id_, ls_id_, addr, ls_info))) {
    LOG_WARN("failed to fetch ls meta info", K(ret), K(tenant_id_), K(ls_id_), K(addr));
  } else if (!ls_info.is_valid()) {
    ret = OB_ERR_UNEXPECTED;
    LOG_WARN("ls_info is invalid!", K(ret), K(tenant_id_), K(ls_id_), K(addr), K(ls_info));
  } else if (OB_FAIL(ObStorageHAUtils::check_replica_validity(ls_info))) {
    LOG_WARN("failed to check replica validity", K(ret), K(ls_info));
  } else if (local_clog_checkpoint_scn_ > ls_info.ls_meta_package_.ls_meta_.get_clog_checkpoint_scn()) {
    ret = OB_DATA_SOURCE_NOT_VALID;
    LOG_WARN("do not choose this src, local checkpoint scn check failed", K(ret), K(tenant_id_), K(ls_id_), K(addr), K(dst), K(learner_list),
        K(local_clog_checkpoint_scn_), K(ls_info));
  } else if (palf_parent_checkpoint_scn_ > ls_info.ls_meta_package_.ls_meta_.get_clog_checkpoint_scn()) {
    ret = OB_DATA_SOURCE_NOT_VALID;
    LOG_WARN("do not choose this src, parent checkpoint scn check failed", K(ret), K(tenant_id_), K(ls_id_), K(addr), K(dst), K(learner_list),
        K(palf_parent_checkpoint_scn_), K(ls_info));
  }
  return ret;
}

const char *ObStorageHASrcProvider::ObChooseSourcePolicyStr[static_cast<int64_t>(ChooseSourcePolicy::MAX_POLICY)] = {
  "idc",
  "region",
  "checkpoint",
  "recommend",
  "c_type_replica"
};

const char *ObStorageHASrcProvider::get_policy_str(const ChooseSourcePolicy policy_type)
{
  const char *str = "";
  if (policy_type >= ChooseSourcePolicy::MAX_POLICY || policy_type < ChooseSourcePolicy::IDC) {
    str = "invalid_type";
  } else {
    str = ObChooseSourcePolicyStr[static_cast<int64_t>(policy_type)];
  }
  return str;
}

int ObStorageHASrcProvider::check_tenant_primary(bool &is_primary)
{
  int ret = OB_SUCCESS;
  is_primary = false;
  if (!is_inited_) {
    ret = OB_NOT_INIT;
    LOG_WARN("ObStorageHASrcProvider is not init.", K(ret));
  } else {
    is_primary = member_helper_->check_tenant_primary();
  }
  return ret;
}
/**
 * ------------------------------ObMigrationSrcByLocationProvider---------------------
 */
ObMigrationSrcByLocationProvider::ObMigrationSrcByLocationProvider()
  : ObStorageHASrcProvider(),
    locality_manager_(nullptr)
{
}

ObMigrationSrcByLocationProvider::~ObMigrationSrcByLocationProvider()
{
}

int ObMigrationSrcByLocationProvider::init(const ObMigrationChooseSrcHelperInitParam &param,
    const ChooseSourcePolicy policy_type,
    storage::ObStorageRpc *storage_rpc,
    ObStorageHAGetMemberHelper *member_helper)
{
  int ret = OB_SUCCESS;
  if (is_inited_) {
    ret = OB_INIT_TWICE;
    LOG_WARN("ObMigrationSrcByLocationProvider init twice", K(ret));
  } else if (!param.is_valid() || OB_ISNULL(storage_rpc)
      || policy_type < ChooseSourcePolicy::IDC
      || policy_type >= ChooseSourcePolicy::MAX_POLICY
      || OB_ISNULL(member_helper)) {
    ret = OB_INVALID_ARGUMENT;
    LOG_WARN("get invalid argument", K(ret), K(param), K(policy_type), KP(storage_rpc), KP(member_helper));
  } else if (OB_FAIL(ObStorageHASrcProvider::init(param, policy_type, storage_rpc, member_helper))) {
    LOG_WARN("failed to init src provider", K(ret), K(param), KP(storage_rpc), K(policy_type), KP(member_helper));
  } else {
    locality_manager_ = GCTX.locality_manager_;
    is_inited_ = true;
  }
  return ret;
}

int ObMigrationSrcByLocationProvider::choose_ob_src(
    const ObMigrationOpArg &arg, common::ObAddr &chosen_src_addr)
{
  int ret = OB_SUCCESS;
  chosen_src_addr.reset();
  if (!is_inited_) {
    ret = OB_NOT_INIT;
    LOG_WARN("ObMigrationSrcByLocationProvider is not init.", K(ret));
  } else if (!arg.is_valid()) {
    ret = OB_INVALID_ARGUMENT;
    LOG_WARN("invalid argument!", K(ret), K(arg));
  } else if (OB_FAIL(inner_choose_ob_src(
      member_list_info_.leader_addr_, member_list_info_.learner_list_,
      member_list_info_.member_list_, arg, chosen_src_addr))) {
    LOG_WARN("failed to inner choose ob src", K(ret), "tenant_id", get_tenant_id(), "ls_id", get_ls_id(),
        K(member_list_info_), K(arg));
  }
  return ret;
}

int ObMigrationSrcByLocationProvider::inner_choose_ob_src(
    const common::ObAddr &leader_addr, const common::GlobalLearnerList &learner_list,
    const common::ObIArray<common::ObAddr> &addr_list, const ObMigrationOpArg &arg,
    common::ObAddr &choosen_src_addr)
{
  int ret = OB_SUCCESS;
  common::ObArray<common::ObAddr> sorted_addr_list;
  int64_t idc_end_index = 0;
  int64_t region_end_index = 0;
  choosen_src_addr.reset();
  if (!is_inited_) {
    ret = OB_NOT_INIT;
    LOG_WARN("ObMigrationSrcByLocationProvider is not init.", K(ret));
  } else if (!leader_addr.is_valid() || addr_list.empty() || !arg.is_valid()) {
    ret = OB_INVALID_ARGUMENT;
    LOG_WARN("invalid argument!", K(ret), K(addr_list), K(arg));
  } else if (OB_FAIL(divide_addr_list(addr_list, arg.dst_, sorted_addr_list, idc_end_index, region_end_index))) {
    LOG_WARN("failed to divide addr", K(ret), K(addr_list), K(arg.dst_));
  } else {
    bool is_idc_policy = ChooseSourcePolicy::IDC == get_policy_type() ? true : false;
    int64_t region_start_index = is_idc_policy ? idc_end_index + 1 : 0;
    if (is_idc_policy && OB_FAIL(find_src(sorted_addr_list, 0/*idc_start_index*/, idc_end_index,
        learner_list, leader_addr, arg.dst_, choosen_src_addr))) { // find in same idc
      LOG_WARN("failed to find source in same idc scope", K(ret), K(sorted_addr_list),
          K(idc_end_index), K(learner_list), K(leader_addr), K(arg.dst_));
    } else if (!choosen_src_addr.is_valid() && OB_FAIL(find_src(sorted_addr_list, region_start_index, region_end_index,
        learner_list, leader_addr, arg.dst_, choosen_src_addr))) { // find in same region
      LOG_WARN("failed to find source in same region scope", K(ret), K(sorted_addr_list),
          K(region_start_index), K(region_end_index), K(learner_list), K(leader_addr), K(arg.dst_));
    } else if (!choosen_src_addr.is_valid() && OB_FAIL(find_src(sorted_addr_list, region_end_index + 1, addr_list.count() - 1,
        learner_list, leader_addr, arg.dst_, choosen_src_addr))) { // find in different region
      LOG_WARN("failed to find source in different region scope", K(ret), K(sorted_addr_list),
          K(region_end_index), K(addr_list.count()), K(learner_list), K(leader_addr), K(arg.dst_));
    } else if (!choosen_src_addr.is_valid()) {
      ret = OB_DATA_SOURCE_NOT_EXIST;
      LOG_WARN("all region no available data source exist", K(ret), "tenant_id", get_tenant_id(),
          "ls_id", get_ls_id(), K(leader_addr), K(learner_list), K(addr_list), K(arg));
    }
  }
  return ret;
}

int ObMigrationSrcByLocationProvider::divide_addr_list(
    const common::ObIArray<common::ObAddr> &addr_list,
    const common::ObReplicaMember &dst,
    common::ObIArray<common::ObAddr> &sorted_addr_list,
    int64_t &idc_end_index,
    int64_t &region_end_index)
{
  int ret = OB_SUCCESS;
  sorted_addr_list.reset();
  common::ObRegion dst_region;
  common::ObIDC dst_idc;
  common::ObRegion src_region;
  common::ObIDC src_idc;
  int64_t same_idc_count = 0;
  int64_t same_region_count = 0;
  common::ObArray<common::ObAddr> same_idc_addr;
  common::ObArray<common::ObAddr> same_region_addr;
  common::ObArray<common::ObAddr> different_region_addr;
  idc_end_index = 0;
  region_end_index = 0;
  if (!is_inited_) {
    ret = OB_NOT_INIT;
    LOG_WARN("ObMigrationSrcByLocationProvider is not init.", K(ret));
  } else if (addr_list.empty() || !dst.is_valid()) {
    ret = OB_INVALID_ARGUMENT;
    LOG_WARN("invalid argument!", K(ret), K(addr_list), K(dst));
  } else if (OB_ISNULL(locality_manager_)) {
    ret = OB_ERR_UNEXPECTED;
    LOG_ERROR("locality manager is null", K(ret), KP(locality_manager_));
  } else if (OB_FAIL(get_server_region_and_idc_(dst.get_server(), dst_region, dst_idc))) {
    LOG_WARN("failed to get dst idc", K(ret), "addr", dst.get_server());
  } else {
    LOG_INFO("succeed to get dst region and idc", K(dst_idc), K(dst_region));
    for (int64_t i = 0; OB_SUCC(ret) && i < addr_list.count(); ++i) {
      if (OB_FAIL(get_server_region_and_idc_(addr_list.at(i), src_region, src_idc))) {
        LOG_WARN("failed to get src region and idc", K(ret), "addr", addr_list.at(i), K(dst));
      } else if (src_region == dst_region && src_idc == dst_idc) { // get distinct region server
        same_idc_count++;
        if (OB_FAIL(same_idc_addr.push_back(addr_list.at(i)))) {
          LOG_WARN("failed to add distinct region addr", K(ret), "addr", addr_list.at(i), K(dst));
        }
      } else if (src_region == dst_region) {
        same_region_count++;
        if (OB_FAIL(same_region_addr.push_back(addr_list.at(i)))) {
          LOG_WARN("failed to add distinct region addr", K(ret), "addr", addr_list.at(i), K(dst));
        }
      } else if (src_region != dst_region) {
        if (OB_FAIL(different_region_addr.push_back(addr_list.at(i)))) {
          LOG_WARN("failed to add distinct region addr", K(ret), "addr", addr_list.at(i), K(dst));
        }
      } else {
        ret = OB_ERR_UNEXPECTED;
        LOG_WARN("unexpected error", K(ret), K(src_idc), K(dst_idc), K(src_region), K(dst));
      }
    }
    if (OB_FAIL(ret)) {
      // do nothing
    } else if (OB_FAIL(common::append(sorted_addr_list, same_idc_addr))) {
      LOG_WARN("failed to append same_idc_addr to sorted_addr_list", K(ret), K(sorted_addr_list), K(same_idc_addr));
    } else if (OB_FAIL(common::append(sorted_addr_list, same_region_addr))) {
      LOG_WARN("failed to append same_region_addr to sorted_addr_list", K(ret), K(sorted_addr_list), K(same_region_addr));
    } else if (OB_FAIL(common::append(sorted_addr_list, different_region_addr))) {
      LOG_WARN("failed to append different_region_addr to sorted_addr_list", K(ret), K(sorted_addr_list), K(different_region_addr));
    } else {
      idc_end_index = same_idc_count - 1;
      region_end_index = same_idc_count + same_region_count - 1;
      LOG_INFO("succeed to divide addr list", K(ret), K(sorted_addr_list), K(same_idc_addr),
          K(same_region_addr), K(different_region_addr), K(idc_end_index), K(region_end_index));
    }
  }
  return ret;
}

int ObMigrationSrcByLocationProvider::find_src(
    const common::ObIArray<common::ObAddr> &addr_list,
    const int64_t start_index,
    const int64_t end_index,
    const common::GlobalLearnerList &learner_list,
    const common::ObAddr &leader_addr,
    const common::ObReplicaMember &dst,
    common::ObAddr &choosen_src_addr)
{
  int ret = OB_SUCCESS;
  int tmp_ret = OB_SUCCESS;
  int64_t choose_member_idx = -1;
  bool is_leader = false;
  obrpc::ObFetchLSMetaInfoResp ls_info;
  common::ObArray<int64_t> candidate_addr_list;
  int64_t leader_index = -1;
  choosen_src_addr.reset();
  bool is_primary = false;
  LOG_INFO("start find source", K(start_index), K(end_index));
  if (!is_inited_) {
    ret = OB_NOT_INIT;
    LOG_WARN("ObMigrationSrcByLocationProvider is not init.", K(ret));
  } else if (addr_list.empty() || start_index < 0 || end_index < -1
    || !leader_addr.is_valid() || !dst.is_valid()) {
    ret = OB_INVALID_ARGUMENT;
    LOG_WARN("invalid argument!", K(ret), K(addr_list), K(start_index), K(end_index), K(leader_addr), K(dst));
  } else if (OB_FAIL(check_tenant_primary(is_primary))) {
    LOG_WARN("failed to check tenant primary", K(ret), "tenant_id", get_tenant_id());
  } else {
    for (int64_t i = start_index; OB_SUCC(ret) && i <= end_index && i < addr_list.count(); ++i) {
      if (OB_TMP_FAIL(check_replica_validity(addr_list.at(i), dst, learner_list, ls_info))) {
        if (OB_DATA_SOURCE_NOT_EXIST == tmp_ret) {
          // overwrite ret
          ret = tmp_ret;
          LOG_WARN("failed to check replica validity", K(ret), K(tmp_ret), "tenant_id", get_tenant_id(),
              "ls_id", get_ls_id(), "addr", addr_list.at(i), K(dst), K(learner_list));
          break;
        } else {
          LOG_WARN("this address is not suitable.", K(ret), K(tmp_ret), "tenant_id", get_tenant_id(),
              "ls_id", get_ls_id(), "addr", addr_list.at(i), K(dst), K(learner_list));
        }
      } else {
        if (addr_list.at(i) != leader_addr || !is_primary) {
          if (OB_FAIL(candidate_addr_list.push_back(i))) {
            LOG_WARN("failed to push back to candidate_addr_list", K(ret), "index", i, K(addr_list));
          }
        } else {
          leader_index = i;
        }
      }
    }
    if (OB_SUCC(ret)) {
      if (candidate_addr_list.empty() && -1 == leader_index) {
        LOG_INFO("no available data source exist in this area", K(ret), "tenant_id", get_tenant_id(),
            "ls_id", get_ls_id(), K(addr_list), K(dst), K(learner_list));
      } else if (!candidate_addr_list.empty()) {
        int64_t num = candidate_addr_list.count();
        choosen_src_addr = addr_list.at(candidate_addr_list.at(rand() % num));
        LOG_INFO("found available data follower source in this area", "tenant_id", get_tenant_id(),
            "ls_id", get_ls_id(), K(addr_list), K(dst), K(learner_list), K(choosen_src_addr));
      } else {
        choosen_src_addr = addr_list.at(leader_index);
        LOG_INFO("found available data leader source in this area", "tenant_id", get_tenant_id(),
            "ls_id", get_ls_id(), K(addr_list), K(dst), K(learner_list), K(choosen_src_addr), K(leader_index));
      }
    }
  }
  return ret;
}

int ObMigrationSrcByLocationProvider::get_server_region_and_idc_(
    const common::ObAddr &addr, common::ObRegion &region, common::ObIDC &idc)
{
  int ret = OB_SUCCESS;
  region.reset();
  idc.reset();
  if (OB_ISNULL(locality_manager_)) {
    ret = OB_ERR_UNEXPECTED;
    LOG_ERROR("locality manager is null", K(ret), KP(locality_manager_));
  } else if (OB_FAIL(locality_manager_->get_server_region(addr, region))) {
    LOG_WARN("failed to get src region", K(ret), K(addr));
  } else if (OB_FAIL(locality_manager_->get_server_idc(addr, idc))) {
    LOG_WARN("failed to get src idc", K(ret), K(addr));
  } else {
    LOG_INFO("succeed to get region and idc", K(addr), K(region), K(idc));
  }
  return ret;
}

void ObMigrationSrcByLocationProvider::set_locality_manager_(ObLocalityManager *locality_manager)
{
  int ret = OB_SUCCESS;
  if (OB_ISNULL(locality_manager)) {
    ret = OB_INVALID_ARGUMENT;
    LOG_WARN("argument invalid, locality_manager is null", K(ret));
  } else {
    locality_manager_ = locality_manager;
  }
}
/**
 * ------------------------------ObMigrationSrcByCheckpointProvider---------------------
 */
ObMigrationSrcByCheckpointProvider::ObMigrationSrcByCheckpointProvider()
  : ObStorageHASrcProvider()
{
}

ObMigrationSrcByCheckpointProvider::~ObMigrationSrcByCheckpointProvider()
{
}

int ObMigrationSrcByCheckpointProvider::init(const ObMigrationChooseSrcHelperInitParam &param,
    const ChooseSourcePolicy policy_type,
    storage::ObStorageRpc *storage_rpc,
    ObStorageHAGetMemberHelper *member_helper)
{
  int ret = OB_SUCCESS;
  if (is_inited_) {
    ret = OB_INIT_TWICE;
    LOG_WARN("ObMigrationSrcByCheckpointProvider init twice", K(ret));
  } else if (!param.is_valid() || OB_ISNULL(storage_rpc)
      || policy_type < ChooseSourcePolicy::IDC
      || policy_type >= ChooseSourcePolicy::MAX_POLICY
      || OB_ISNULL(member_helper)) {
    ret = OB_INVALID_ARGUMENT;
    LOG_WARN("get invalid argument", K(ret), K(param), K(policy_type), KP(storage_rpc), KP(member_helper));
  } else if (OB_FAIL(ObStorageHASrcProvider::init(param, policy_type, storage_rpc, member_helper))) {
    LOG_WARN("failed to init src provider", K(ret), K(param), KP(storage_rpc), K(policy_type), KP(member_helper));
  } else {
    is_inited_ = true;
  }
  return ret;
}

int ObMigrationSrcByCheckpointProvider::choose_ob_src(
    const ObMigrationOpArg &arg, common::ObAddr &chosen_src_addr)
{
  int ret = OB_SUCCESS;
  chosen_src_addr.reset();
  if (!is_inited_) {
    ret = OB_NOT_INIT;
    LOG_WARN("ObMigrationSrcByCheckpointProvider is not init.", K(ret));
  } else if (!arg.is_valid()) {
    ret = OB_INVALID_ARGUMENT;
    LOG_WARN("invalid argument!", K(ret), K(arg));
  } else if (OB_FAIL(inner_choose_ob_src_(
      member_list_info_.leader_addr_, member_list_info_.learner_list_,
      member_list_info_.member_list_, arg, chosen_src_addr))) {
    LOG_WARN("failed to inner choose ob src", K(ret), "tenant_id", get_tenant_id(), "ls_id", get_ls_id(),
        K(member_list_info_), K(arg));
  }
  return ret;
}

int ObMigrationSrcByCheckpointProvider::inner_choose_ob_src_(
    const common::ObAddr &leader_addr, const common::GlobalLearnerList &learner_list,
    const common::ObIArray<common::ObAddr> &addr_list, const ObMigrationOpArg &arg,
    common::ObAddr &choosen_src_addr)
{
  int ret = OB_SUCCESS;
  int tmp_ret = OB_SUCCESS;
  choosen_src_addr.reset();
  share::SCN max_clog_checkpoint_scn = share::SCN::min_scn();
  int64_t choose_member_idx = -1;
  obrpc::ObFetchLSMetaInfoResp ls_info;
  if (!is_inited_) {
    ret = OB_NOT_INIT;
    LOG_WARN("ObMigrationSrcByCheckpointProvider is not init.", K(ret));
  } else if (!leader_addr.is_valid() || addr_list.empty() || !arg.is_valid()) {
    ret = OB_INVALID_ARGUMENT;
    LOG_WARN("invalid argument!", K(ret), K(leader_addr), K(addr_list), K(arg));
  } else {
    for (int64_t i = 0; OB_SUCC(ret) && i < addr_list.count(); ++i) {
      if (OB_TMP_FAIL(check_replica_validity(addr_list.at(i), arg.dst_, learner_list, ls_info))) {
        // OB_DATA_SOURCE_NOT_EXIST make migration exit. It is used to return when check restore fail.
        // Only check restore fail use OB_DATA_SOURCE_NOT_EXIST currently
        if (OB_DATA_SOURCE_NOT_EXIST == tmp_ret) {
          // overwrite ret
          ret = tmp_ret;
          LOG_WARN("failed to check replica validity", K(ret), K(tmp_ret), "tenant_id", get_tenant_id(),
              "ls_id", get_ls_id(), "addr", addr_list.at(i), K(arg.dst_), K(learner_list));
          break;
        } else {
          LOG_WARN("this address is not suitable.", K(ret), K(tmp_ret), "tenant_id", get_tenant_id(),
              "ls_id", get_ls_id(), "addr", addr_list.at(i), K(arg.dst_), K(learner_list));
        }
      } else {
        if (ls_info.ls_meta_package_.ls_meta_.get_clog_checkpoint_scn() >= max_clog_checkpoint_scn) {
          max_clog_checkpoint_scn = ls_info.ls_meta_package_.ls_meta_.get_clog_checkpoint_scn();
          choose_member_idx = i;
        }
      }
    }
    if (OB_SUCC(ret)) {
      if (-1 == choose_member_idx) {
        ret = OB_DATA_SOURCE_NOT_EXIST;
        LOG_WARN("no available data source exist", K(ret), "tenant_id", get_tenant_id(),
            "ls_id", get_ls_id(), K(learner_list), K(addr_list));
      } else {
        choosen_src_addr = addr_list.at(choose_member_idx);
      }
    }
  }
  return ret;
}
/**
 * ------------------------------ObRSRecommendSrcProvider---------------------
 */
ObRSRecommendSrcProvider::ObRSRecommendSrcProvider()
  : ObStorageHASrcProvider()
{
}

ObRSRecommendSrcProvider::~ObRSRecommendSrcProvider()
{
}

int ObRSRecommendSrcProvider::init(const ObMigrationChooseSrcHelperInitParam &param,
    const ChooseSourcePolicy policy_type,
    storage::ObStorageRpc *storage_rpc,
    ObStorageHAGetMemberHelper *member_helper)
{
  int ret = OB_SUCCESS;
  if (is_inited_) {
    ret = OB_INIT_TWICE;
    LOG_WARN("ObRSRecommendSrcProvider init twice", K(ret));
  } else if (!param.is_valid() || OB_ISNULL(storage_rpc)
      || policy_type < ChooseSourcePolicy::IDC
      || policy_type >= ChooseSourcePolicy::MAX_POLICY
      || OB_ISNULL(member_helper)) {
    ret = OB_INVALID_ARGUMENT;
    LOG_WARN("get invalid argument", K(ret), K(param), K(policy_type),
        KP(storage_rpc), KP(member_helper));
  } else if (OB_FAIL(ObStorageHASrcProvider::init(param, policy_type,
      storage_rpc, member_helper))) {
    LOG_WARN("failed to init src provider", K(ret), K(param), KP(storage_rpc), K(policy_type), KP(member_helper));
  } else {
    is_inited_ = true;
  }
  return ret;
}

int ObRSRecommendSrcProvider::check_replica_validity_(
    const int64_t cluster_id,
    const common::ObIArray<common::ObAddr> &addr_list,
    const common::ObAddr &addr, const common::ObReplicaMember &dst,
    const common::GlobalLearnerList &learner_list)
{
  int ret = OB_SUCCESS;
  obrpc::ObFetchLSMetaInfoResp ls_info;
  int64_t gconf_cluster_id = GCONF.cluster_id;
  if (addr_list.empty() || !addr.is_valid() || !dst.is_valid() || cluster_id != gconf_cluster_id) {
    ret = OB_INVALID_ARGUMENT;
    LOG_WARN("invalid argument!", K(ret), K(addr_list), K(addr), K(dst),
        K(cluster_id), K(gconf_cluster_id));
  } else if (OB_FAIL(check_replica_validity(addr, dst, learner_list, ls_info))) {
    LOG_WARN("failed to check replica validity", K(ret), K(addr), K(dst), K(learner_list));
  } else {
    bool is_exist = false;
    for (int64_t i = 0; OB_SUCC(ret) && i < addr_list.count(); i++) {
      if (addr_list.at(i) == addr) {
        is_exist = true;
        break;
      }
    }
    if (!is_exist) {
      ret = OB_DATA_SOURCE_NOT_EXIST;
      LOG_WARN("addr not in addr_list", K(ret), K(addr), K(addr_list));
    }
  }
  return ret;
}

int ObRSRecommendSrcProvider::choose_ob_src(
    const ObMigrationOpArg &arg, common::ObAddr &chosen_src_addr)
{
  int ret = OB_SUCCESS;
  chosen_src_addr.reset();
  if (!is_inited_) {
    ret = OB_NOT_INIT;
    LOG_WARN("ObRSRecommendSrcProvider is not init.", K(ret));
  } else if (!arg.is_valid()) {
    ret = OB_INVALID_ARGUMENT;
    LOG_WARN("invalid argument!", K(ret), K(arg));
  } else if (OB_FAIL(check_replica_validity_(arg.cluster_id_, member_list_info_.member_list_,
      arg.data_src_.get_server(), arg.dst_, member_list_info_.learner_list_))) {
    LOG_WARN("failed to check replica validity", K(ret), K(member_list_info_), K(arg));
  } else {
    chosen_src_addr = arg.data_src_.get_server();
  }
  return ret;
}
/**
 * ------------------------------ObCTypeReplicaSrcProvider---------------------
 */
ObCTypeReplicaSrcProvider::ObCTypeReplicaSrcProvider()
  : ObStorageHASrcProvider()
{
}

ObCTypeReplicaSrcProvider::~ObCTypeReplicaSrcProvider()
{
}

int ObCTypeReplicaSrcProvider::init(const ObMigrationChooseSrcHelperInitParam &param,
    const ChooseSourcePolicy policy_type,
    storage::ObStorageRpc *storage_rpc,
    ObStorageHAGetMemberHelper *member_helper)
{
  int ret = OB_SUCCESS;
  if (is_inited_) {
    ret = OB_INIT_TWICE;
    LOG_WARN("ObCTypeReplicaSrcProvider init twice", K(ret));
  } else if (!param.is_valid() || OB_ISNULL(storage_rpc)
      || policy_type < ChooseSourcePolicy::IDC
      || policy_type >= ChooseSourcePolicy::MAX_POLICY
      || OB_ISNULL(member_helper)) {
    ret = OB_INVALID_ARGUMENT;
    LOG_WARN("get invalid argument", K(ret), K(param), K(policy_type),
        KP(storage_rpc), KP(member_helper));
  } else if (OB_FAIL(ObStorageHASrcProvider::init(param, policy_type,
      storage_rpc, member_helper))) {
    LOG_WARN("failed to init src provider", K(ret), K(param), KP(storage_rpc), K(policy_type), KP(member_helper));
  } else {
    is_inited_ = true;
  }
  return ret;
}

int ObCTypeReplicaSrcProvider::get_c_replica_(
    const common::ObAddr &addr,
    const common::GlobalLearnerList &learner_list,
    ObIArray<common::ObAddr> &c_replica_list) const
{
  int ret = OB_SUCCESS;
  ObMember src;
  if (!addr.is_valid() || !learner_list.is_valid()) {
    ret = OB_INVALID_ARGUMENT;
    LOG_WARN("invalid argument!", K(ret), K(addr), K(learner_list));
  } else if (!learner_list.contains(addr)) {
  } else if (OB_FAIL(learner_list.get_learner_by_addr(addr, src))) {
    LOG_WARN("failed to get learner by addr", KR(ret), K(addr));
  } else if (src.is_columnstore() && OB_FAIL(c_replica_list.push_back(addr))) {
    LOG_WARN("failed to push back addr", K(ret), K(addr), K(src), K(learner_list));
  }
  return ret;
}

int ObCTypeReplicaSrcProvider::inner_choose_ob_src_(
    const common::ObIArray<common::ObAddr> &addr_list,
    const common::GlobalLearnerList &learner_list,
    const ObMigrationOpArg &arg, common::ObAddr &choosen_src_addr)
{
  int ret = OB_SUCCESS;
  int tmp_ret = OB_SUCCESS;
  choosen_src_addr.reset();
  common::ObArray<int64_t> candidate_addr_list;
  obrpc::ObFetchLSMetaInfoResp ls_info;
  ObArray<common::ObAddr> all_c_replica_list;
  ObArray<common::ObAddr> valid_c_replica_list;
  if (!is_inited_) {
    ret = OB_NOT_INIT;
    LOG_WARN("ObCTypeReplicaSrcProvider is not init.", K(ret));
  } else if (addr_list.empty() || !learner_list.is_valid() || !arg.is_valid()) {
    ret = OB_INVALID_ARGUMENT;
    LOG_WARN("invalid argument!", K(ret), K(addr_list), K(learner_list), K(arg));
  } else {
    for (int64_t i = 0; OB_SUCC(ret) && i < addr_list.count(); ++i) {
      if (OB_FAIL(get_c_replica_(addr_list.at(i), learner_list, all_c_replica_list))) {
        LOG_WARN("failed to get all c replica", K(ret), "addr", addr_list.at(i), K(learner_list));
      } else if (OB_TMP_FAIL(check_replica_validity(addr_list.at(i), arg.dst_, learner_list, ls_info))) {
        if (OB_DATA_SOURCE_NOT_EXIST == tmp_ret) {
          // overwrite ret
          ret = tmp_ret;
          LOG_WARN("failed to check replica validity", K(ret), K(tmp_ret), "tenant_id", get_tenant_id(),
              "ls_id", get_ls_id(), "addr", addr_list.at(i), K(arg.dst_), K(learner_list));
          break;
        } else {
          LOG_WARN("this address is not suitable.", K(ret), K(tmp_ret), "tenant_id", get_tenant_id(),
              "ls_id", get_ls_id(), "addr", addr_list.at(i), K(arg.dst_), K(learner_list));
        }
      } else if (OB_FAIL(candidate_addr_list.push_back(i))) {
        LOG_WARN("failed to push back to candidate_addr_list", K(ret), "index", i, "addr", addr_list.at(i));
      } else if (OB_FAIL(get_c_replica_(addr_list.at(i), learner_list, valid_c_replica_list))) {
        LOG_WARN("failed to get valid c replica", K(ret), "addr", addr_list.at(i), K(learner_list));
      }
    }
    if (OB_SUCC(ret)) {
      if (candidate_addr_list.empty() && valid_c_replica_list.empty()) {
        ret = OB_DATA_SOURCE_NOT_EXIST;
        LOG_INFO("no available data source exist in this area", K(ret), "tenant_id", get_tenant_id(),
            "ls_id", get_ls_id(), K(arg.dst_), K(addr_list), K(learner_list));
      } else if (candidate_addr_list.empty() && !valid_c_replica_list.empty()) {
        ret = OB_ERR_UNEXPECTED;
        LOG_WARN("this case is unexpected", K(ret), K(valid_c_replica_list), "tenant_id", get_tenant_id(),
            "ls_id", get_ls_id(), K(arg.dst_), K(addr_list), K(learner_list));
      } else if (!valid_c_replica_list.empty()) {
        // if C replica exist, choose one of them
        int64_t num = valid_c_replica_list.count();
        choosen_src_addr = valid_c_replica_list.at(rand() % num);
        LOG_INFO("found available C replica source in this area", "tenant_id", get_tenant_id(),
            "ls_id", get_ls_id(), K(arg.dst_), K(learner_list), K(choosen_src_addr), K(addr_list), K(valid_c_replica_list));
      } else if (!all_c_replica_list.empty() && !is_first_c_replica_) {
        // C replica exist, but all C replica is not available. If dst is not first C replica, fail this migration
        ret = OB_DATA_SOURCE_NOT_EXIST;
        LOG_INFO("C replica exist, but all C replica is not available", K(ret), "tenant_id", get_tenant_id(),
            "ls_id", get_ls_id(), K(arg.dst_), K(learner_list), K(addr_list), K(all_c_replica_list), K(valid_c_replica_list), K(is_first_c_replica_));
      } else {
        // If no C replica exist or dst is first C replica and other C replica invalid, choose other F/R replica
        int64_t num = candidate_addr_list.count();
        choosen_src_addr = addr_list.at(candidate_addr_list.at(rand() % num));
        LOG_INFO("no available C replica, but found other available source in this area", "tenant_id", get_tenant_id(),
            "ls_id", get_ls_id(), K(arg.dst_), K(learner_list), K(choosen_src_addr), K(addr_list), K(is_first_c_replica_));
      }
    }
  }
  return ret;
}

int ObCTypeReplicaSrcProvider::choose_ob_src(
    const ObMigrationOpArg &arg, common::ObAddr &chosen_src_addr)
{
  int ret = OB_SUCCESS;
  chosen_src_addr.reset();
  if (!is_inited_) {
    ret = OB_NOT_INIT;
    LOG_WARN("ObCTypeReplicaSrcProvider is not init.", K(ret));
  } else if (!arg.is_valid()) {
    ret = OB_INVALID_ARGUMENT;
    LOG_WARN("invalid argument!", K(ret), K(arg));
  } else if (OB_FAIL(inner_choose_ob_src_(
      member_list_info_.member_list_, member_list_info_.learner_list_, arg, chosen_src_addr))) {
    LOG_WARN("failed to inner choose ob src", K(ret), "tenant_id", get_tenant_id(), "ls_id", get_ls_id(),
        K(member_list_info_), K(arg));
  }
  return ret;
}
/**
 * ------------------------------ObStorageHAChooseSrcHelper---------------------
 */
ObStorageHAChooseSrcHelper::ObStorageHAChooseSrcHelper()
  : provider_(nullptr),
    storage_rpc_(nullptr),
    allocator_(),
    is_inited_(false)
{
}

ObStorageHAChooseSrcHelper::~ObStorageHAChooseSrcHelper()
{
  if (OB_NOT_NULL(provider_)) {
    provider_->~ObStorageHASrcProvider();
    provider_ = nullptr;
  }
  storage_rpc_ = nullptr;
}

int ObStorageHAChooseSrcHelper::init(const ObMigrationChooseSrcHelperInitParam &param,
    const ObStorageHASrcProvider::ChooseSourcePolicy policy, storage::ObStorageRpc *storage_rpc,
    ObStorageHAGetMemberHelper *member_helper)
{
  int ret = OB_SUCCESS;
  if (is_inited_) {
    ret = OB_INIT_TWICE;
    LOG_WARN("storage ha src helper init twice", K(ret));
  } else if (!param.is_valid()
      || policy >= ObStorageHASrcProvider::ChooseSourcePolicy::MAX_POLICY
      || policy < ObStorageHASrcProvider::ChooseSourcePolicy::IDC
      || OB_ISNULL(storage_rpc)
      || OB_ISNULL(member_helper)) {
    ret = OB_INVALID_ARGUMENT;
    LOG_WARN("invalid argument!", K(ret), K(param), KP(storage_rpc), KP(member_helper));
  } else {
    switch (policy) {
      case ObStorageHASrcProvider::ChooseSourcePolicy::IDC:
      case ObStorageHASrcProvider::ChooseSourcePolicy::REGION: {
        if (OB_FAIL(init_choose_source_by_location_provider_(param, policy,
            storage_rpc, member_helper))) {
          LOG_WARN("failed to init choose source by location provider", K(ret), K(param), K(policy));
        }
        break;
      }
      case ObStorageHASrcProvider::ChooseSourcePolicy::CHECKPOINT: {
        if (OB_FAIL(init_choose_source_by_checkpoint_provider_(param,
            storage_rpc, member_helper))) {
          LOG_WARN("failed to init choose source by checkpoint provider", K(ret), K(param), K(policy));
        }
        break;
      }
      case ObStorageHASrcProvider::ChooseSourcePolicy::RECOMMEND: {
        if (OB_FAIL(init_rs_recommend_source_provider_(param,
            storage_rpc, member_helper))) {
          LOG_WARN("failed to init choose source by rs recommend provider", K(ret), K(param), K(policy));
        }
        break;
      }
      case ObStorageHASrcProvider::ChooseSourcePolicy::C_TYPE_REPLICA: {
        if (OB_FAIL(init_c_type_replica_source_provider_(param,
            storage_rpc, member_helper))) {
          LOG_WARN("failed to init c replica choose source provider", K(ret), K(param), K(policy));
        }
        break;
      }
      default: {
        ret = OB_ERR_UNEXPECTED;
        LOG_WARN("unexpected choose source policy", K(ret), K(param), K(policy));
      }
    }
  }

  if (OB_FAIL(ret)) {
    // do nothing
  } else {
    storage_rpc_ = storage_rpc;
    is_inited_ = true;
  }
  return ret;
}

int ObStorageHAChooseSrcHelper::get_available_src(const ObMigrationOpArg &arg, ObStorageHASrcInfo &src_info)
{
  int ret = OB_SUCCESS;
  common::ObAddr chosen_src_addr;
  if (!is_inited_) {
    ret = OB_NOT_INIT;
    LOG_WARN("src helper not init", K(ret));
  } else if (OB_FAIL(provider_->choose_ob_src(arg, chosen_src_addr))) {
    LOG_WARN("failed to choose ob src", K(ret), K(arg), K(src_info));
  } else {
    src_info.src_addr_ = chosen_src_addr;
    src_info.cluster_id_ = GCONF.cluster_id;
    LOG_INFO("succeed to choose src", K(src_info));
    errsim_test_(arg, src_info);
  }
  SERVER_EVENT_ADD("storage_ha", "choose_src",
                   "tenant_id", provider_->get_tenant_id(),
                   "ls_id", provider_->get_ls_id().id(),
                   "src_addr", src_info.src_addr_,
                   "dst_addr", arg.dst_.get_server(),
                   "op_type", ObMigrationOpType::get_str(provider_->get_type()));
  return ret;
}

int ObStorageHAChooseSrcHelper::init_rs_recommend_source_provider_(
    const ObMigrationChooseSrcHelperInitParam &param,
    storage::ObStorageRpc *storage_rpc, ObStorageHAGetMemberHelper *member_helper)
{
  int ret = OB_SUCCESS;
  void *buf = nullptr;
  ObRSRecommendSrcProvider *recommend_src_provider = nullptr;
  const ObStorageHASrcProvider::ChooseSourcePolicy policy = ObStorageHASrcProvider::RECOMMEND;
  if (OB_ISNULL(buf = allocator_.alloc(sizeof(ObRSRecommendSrcProvider)))) {
    ret = OB_ALLOCATE_MEMORY_FAILED;
    LOG_WARN("fail to alloc memory", K(ret));
  } else if (OB_ISNULL(recommend_src_provider = (new (buf) ObRSRecommendSrcProvider()))) {
    ret = OB_ERR_UNEXPECTED;
    LOG_WARN("provider is nullptr", K(ret));
  } else if (OB_FAIL(recommend_src_provider->init(param, policy, storage_rpc, member_helper))) {
    LOG_WARN("failed to init rs recommend src provider", K(ret), K(param), K(policy), KP(storage_rpc), KP(member_helper));
  } else {
    provider_ = recommend_src_provider;
    recommend_src_provider = nullptr;
  }
  if (OB_NOT_NULL(recommend_src_provider)) {
    recommend_src_provider->~ObRSRecommendSrcProvider();
    recommend_src_provider = nullptr;
  }
  buf = nullptr;
  return ret;
}

int ObStorageHAChooseSrcHelper::init_choose_source_by_location_provider_(
    const ObMigrationChooseSrcHelperInitParam &param,
    const ObStorageHASrcProvider::ChooseSourcePolicy policy,
    storage::ObStorageRpc *storage_rpc, ObStorageHAGetMemberHelper *member_helper)
{
  int ret = OB_SUCCESS;
  void *buf = nullptr;
  ObMigrationSrcByLocationProvider *choose_src_by_location_provider = nullptr;
  if (OB_ISNULL(buf = allocator_.alloc(sizeof(ObMigrationSrcByLocationProvider)))) {
    ret = OB_ALLOCATE_MEMORY_FAILED;
    LOG_WARN("fail to alloc memory", K(ret));
  } else if (OB_ISNULL(choose_src_by_location_provider = (new (buf) ObMigrationSrcByLocationProvider()))) {
    ret = OB_ERR_UNEXPECTED;
    LOG_WARN("provider is nullptr", K(ret));
  } else if (OB_FAIL(choose_src_by_location_provider->init(param, policy, storage_rpc, member_helper))) {
    LOG_WARN("failed to init src by location provider", K(ret), K(param), K(policy), KP(storage_rpc), KP(member_helper));
  } else {
    provider_ = choose_src_by_location_provider;
    choose_src_by_location_provider = nullptr;
  }
  if (OB_NOT_NULL(choose_src_by_location_provider)) {
    choose_src_by_location_provider->~ObMigrationSrcByLocationProvider();
    choose_src_by_location_provider = nullptr;
  }
  buf = nullptr;
  return ret;
}

int ObStorageHAChooseSrcHelper::init_choose_source_by_checkpoint_provider_(
    const ObMigrationChooseSrcHelperInitParam &param,
    storage::ObStorageRpc *storage_rpc, ObStorageHAGetMemberHelper *member_helper)
{
  int ret = OB_SUCCESS;
  void *buf = nullptr;
  ObMigrationSrcByCheckpointProvider *choose_src_by_checkpoint_provider = nullptr;
  const ObStorageHASrcProvider::ChooseSourcePolicy policy = ObStorageHASrcProvider::CHECKPOINT;
  if (OB_ISNULL(buf = allocator_.alloc(sizeof(ObMigrationSrcByCheckpointProvider)))) {
    ret = OB_ALLOCATE_MEMORY_FAILED;
    LOG_WARN("fail to alloc memory", K(ret));
  } else if (OB_ISNULL(choose_src_by_checkpoint_provider = (new (buf) ObMigrationSrcByCheckpointProvider()))) {
    ret = OB_ERR_UNEXPECTED;
    LOG_WARN("provider is nullptr", K(ret));
  } else if (OB_FAIL(choose_src_by_checkpoint_provider->init(param, policy, storage_rpc, member_helper))) {
    LOG_WARN("failed to init src by checkpoint provider", K(ret), K(param), K(policy), KP(storage_rpc), KP(member_helper));
  } else {
    provider_ = choose_src_by_checkpoint_provider;
    choose_src_by_checkpoint_provider = nullptr;
  }
  if (OB_NOT_NULL(choose_src_by_checkpoint_provider)) {
    choose_src_by_checkpoint_provider->~ObMigrationSrcByCheckpointProvider();
    choose_src_by_checkpoint_provider = nullptr;
  }
  buf = nullptr;
  return ret;
}

int ObStorageHAChooseSrcHelper::init_c_type_replica_source_provider_(
    const ObMigrationChooseSrcHelperInitParam &param,
    storage::ObStorageRpc *storage_rpc, ObStorageHAGetMemberHelper *member_helper)
{
  int ret = OB_SUCCESS;
  void *buf = nullptr;
  ObCTypeReplicaSrcProvider *c_replica_choose_src_provider = nullptr;
  const ObStorageHASrcProvider::ChooseSourcePolicy policy = ObStorageHASrcProvider::C_TYPE_REPLICA;
  if (OB_ISNULL(buf = allocator_.alloc(sizeof(ObCTypeReplicaSrcProvider)))) {
    ret = OB_ALLOCATE_MEMORY_FAILED;
    LOG_WARN("fail to alloc memory", K(ret));
  } else if (OB_ISNULL(c_replica_choose_src_provider = (new (buf) ObCTypeReplicaSrcProvider()))) {
    ret = OB_ERR_UNEXPECTED;
    LOG_WARN("provider is nullptr", K(ret));
  } else if (OB_FAIL(c_replica_choose_src_provider->init(param, policy, storage_rpc, member_helper))) {
    LOG_WARN("failed to init c replica src provider", K(ret), K(param), K(policy), KP(storage_rpc), KP(member_helper));
  } else {
    provider_ = c_replica_choose_src_provider;
    c_replica_choose_src_provider = nullptr;
  }
  if (OB_NOT_NULL(c_replica_choose_src_provider)) {
    c_replica_choose_src_provider->~ObCTypeReplicaSrcProvider();
    c_replica_choose_src_provider = nullptr;
  }
  buf = nullptr;
  return ret;
}

int ObStorageHAChooseSrcHelper::get_policy_type(
    const ObMigrationOpArg &arg,
    const uint64_t tenant_id,
    bool enable_choose_source_policy,
    const char *policy_str,
    const common::GlobalLearnerList &learner_list,
    ObStorageHASrcProvider::ChooseSourcePolicy &policy)
{
  int ret = OB_SUCCESS;
  policy = ObStorageHASrcProvider::ChooseSourcePolicy::IDC;
  bool use_c_replica_policy = false;
  if (!arg.is_valid()
      || OB_INVALID_TENANT_ID == tenant_id) {
    ret = OB_INVALID_ARGUMENT;
    LOG_WARN("invalid argument", K(ret), K(arg), K(tenant_id), K(learner_list));
  } else if (OB_FAIL(check_c_replica_migration_policy_(tenant_id, arg.ls_id_, arg.dst_, learner_list, use_c_replica_policy))) {
    LOG_WARN("failed to check c replica choose source", K(ret), K(tenant_id), K(arg.ls_id_), K(arg.dst_), K(learner_list));
  } else if (use_c_replica_policy) {
    policy = ObStorageHASrcProvider::ChooseSourcePolicy::C_TYPE_REPLICA;
    LOG_INFO("c replica choose source", K(tenant_id), K(arg.ls_id_));
  } else if (arg.data_src_.is_valid() && ObMigrationOpType::TYPE::REBUILD_LS_OP != arg.type_) { // TODO (zhixing.yh) modify condition after repairing compat
    policy = ObStorageHASrcProvider::ChooseSourcePolicy::RECOMMEND;
    LOG_INFO("rs recommend source", K(arg.data_src_), K(tenant_id));
  } else if (!enable_choose_source_policy) {
    policy = ObStorageHASrcProvider::ChooseSourcePolicy::CHECKPOINT;
    LOG_INFO("set checkpoint policy", K(tenant_id));
  } else if (0 == strcmp(policy_str, ObStorageHASrcProvider::get_policy_str(ObStorageHASrcProvider::ChooseSourcePolicy::IDC))) {
    policy = ObStorageHASrcProvider::ChooseSourcePolicy::IDC;
    LOG_INFO("set idc policy", K(tenant_id));
  } else if (0 == strcmp(policy_str, ObStorageHASrcProvider::get_policy_str(ObStorageHASrcProvider::ChooseSourcePolicy::REGION))) {
    policy = ObStorageHASrcProvider::ChooseSourcePolicy::REGION;
    LOG_INFO("set region policy", K(tenant_id));
  } else {
    ret = OB_ERR_UNEXPECTED;
    LOG_WARN("unexpected type", K(ret), K(tenant_id), K(policy_str));
  }
  return ret;
}

void ObStorageHAChooseSrcHelper::errsim_test_(const ObMigrationOpArg &arg, ObStorageHASrcInfo &src_info)
{
  int ret = OB_SUCCESS;
#ifdef ERRSIM
  if (ObMigrationOpType::ADD_LS_OP == arg.type_ || ObMigrationOpType::MIGRATE_LS_OP == arg.type_) {
    const ObString &errsim_server = GCONF.errsim_migration_src_server_addr.str();
    if (!errsim_server.empty()) {
      common::ObAddr tmp_errsim_addr;
      if (OB_FAIL(tmp_errsim_addr.parse_from_string(errsim_server))) {
        LOG_WARN("failed to parse from string", K(ret), K(errsim_server));
      } else {
        src_info.src_addr_ = tmp_errsim_addr;
        src_info.cluster_id_ = GCONF.cluster_id;
        LOG_INFO("storage ha choose errsim src", K(tmp_errsim_addr));
      }
    }
  }
#endif
}

int ObStorageHAChooseSrcHelper::check_exist_c_replica_(const uint64_t tenant_id,
    const share::ObLSID &ls_id, const common::GlobalLearnerList &learner_list, bool &exist_c_replica)
{
  int ret = OB_SUCCESS;
  exist_c_replica = false;
  if (OB_INVALID_TENANT_ID == tenant_id || !ls_id.is_valid() || !learner_list.is_valid()) {
    ret = OB_INVALID_ARGUMENT;
    LOG_WARN("invalid argument", K(ret), K(tenant_id), K(ls_id), K(learner_list));
  } else {
    for (int64_t i = 0; OB_SUCC(ret) && i < learner_list.get_member_number(); i++) {
      ObMember learner;
      if (OB_FAIL(learner_list.get_learner(i, learner))) {
        LOG_WARN("failed to get learner", K(ret), K(learner_list), K(i));
      } else if (learner.is_columnstore()) {
        exist_c_replica = true;
        LOG_INFO("succeed to find c replica", K(tenant_id), K(ls_id), K(learner_list));
        break;
      }
    }
  }
  return ret;
}

int ObStorageHAChooseSrcHelper::check_c_replica_migration_policy_(const uint64_t tenant_id, const share::ObLSID &ls_id,
    const common::ObReplicaMember &dst, const common::GlobalLearnerList &learner_list, bool &use_c_replica_policy)
{
  int ret = OB_SUCCESS;
  use_c_replica_policy = false;
  bool exist_c_replica = false;
  if (OB_INVALID_TENANT_ID == tenant_id || !ls_id.is_valid() || !dst.is_valid()) {
    ret = OB_INVALID_ARGUMENT;
    LOG_WARN("invalid argument", K(ret), K(tenant_id), K(ls_id), K(dst), K(learner_list));
  } else if (REPLICA_TYPE_COLUMNSTORE != dst.get_replica_type()) {
    use_c_replica_policy = false;
  } else if (!learner_list.is_valid()) {
    use_c_replica_policy = false;
  } else if (OB_FAIL(check_exist_c_replica_(tenant_id, ls_id, learner_list, exist_c_replica))) {
    LOG_WARN("failed to check exist c replica", K(ret), K(tenant_id), K(ls_id), K(learner_list));
  } else if (exist_c_replica) {
    use_c_replica_policy = true;
  } else {
    use_c_replica_policy = false;
  }
#ifdef ERRSIM
  if (OB_SUCC(ret)) {
    if(OB_SUCCESS != EN_FORCE_NOT_CHOOSE_C_REPLICA) {
      use_c_replica_policy = false;
    }
  }
#endif
  return ret;
}
}  // namespace storage
}  // namespace oceanbase

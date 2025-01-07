/**
 * Copyright (c) 2024 OceanBase
 * OceanBase CE is licensed under Mulan PubL v2.
 * You can use this software according to the terms and conditions of the Mulan PubL v2.
 * You may obtain a copy of Mulan PubL v2 at:
 *          http://license.coscl.org.cn/MulanPubL-2.0
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PubL v2 for more details.
 */

#ifndef OB_DAS_VEC_DEFINE_H_
#define OB_DAS_VEC_DEFINE_H_

#include "ob_das_attach_define.h"
#include "src/share/vector_index/ob_vector_index_util.h"

namespace oceanbase
{
namespace sql
{

struct ObDASVecAuxScanCtDef : ObDASAttachCtDef
{
  OB_UNIS_VERSION(1);
public:
  ObDASVecAuxScanCtDef(common::ObIAllocator &alloc)
    : ObDASAttachCtDef(alloc, DAS_OP_VEC_SCAN),
      inv_scan_vec_id_col_(nullptr),
      vec_index_param_(),
      dim_(0),
      access_pk_(false) {}
  const ObDASScanCtDef *get_inv_idx_scan_ctdef() const
  {
    const  ObDASScanCtDef *idx_scan_ctdef = nullptr;
    if (children_cnt_ > 0 && children_ != nullptr) {
      idx_scan_ctdef = static_cast<const  ObDASScanCtDef*>(children_[get_inv_scan_idx()]);
    }
    return idx_scan_ctdef;
  }

  const ObDASScanCtDef *get_delta_tbl_ctdef() const
  {
    const ObDASScanCtDef *delta_tbl_ctdef = nullptr;
    const int64_t ctdef_idx = get_delta_tbl_idx();
    if (children_cnt_ > ctdef_idx && ctdef_idx > 0 && children_ != nullptr) {
      const ObDASScanCtDef *child = static_cast<const ObDASScanCtDef *>(children_[ctdef_idx]);
      if (child->ir_scan_type_ == ObTSCIRScanType::OB_VEC_DELTA_BUF_SCAN) {
        delta_tbl_ctdef = child;
      }
    }
    return delta_tbl_ctdef;
  }
  const ObDASScanCtDef *get_index_id_tbl_ctdef() const
  {
    const ObDASScanCtDef *index_id_tbl_ctdef = nullptr;
    const int64_t ctdef_idx = get_index_id_tbl_idx();
    if (children_cnt_ > ctdef_idx && ctdef_idx > 0 && children_ != nullptr) {
      const ObDASScanCtDef *child = static_cast<const ObDASScanCtDef *>(children_[ctdef_idx]);
      if (child->ir_scan_type_ == ObTSCIRScanType::OB_VEC_IDX_ID_SCAN) {
        index_id_tbl_ctdef = child;
      }
    }
    return index_id_tbl_ctdef;
  }
  const ObDASScanCtDef *get_snapshot_tbl_ctdef() const
  {
    const ObDASScanCtDef *snapshot_tbl_ctdef = nullptr;
    const int64_t ctdef_idx = get_snapshot_tbl_idx();
    if (children_cnt_ > ctdef_idx && ctdef_idx > 0 && children_ != nullptr) {
      const ObDASScanCtDef *child = static_cast<const ObDASScanCtDef *>(children_[ctdef_idx]);
      if (child->ir_scan_type_ == ObTSCIRScanType::OB_VEC_SNAPSHOT_SCAN) {
        snapshot_tbl_ctdef = child;
      }
    }
    return snapshot_tbl_ctdef;
  }

  const ObDASScanCtDef *get_com_aux_tbl_ctdef() const
  {
    const ObDASScanCtDef *com_aux_tbl_ctdef = nullptr;
    const int64_t ctdef_idx = get_com_aux_tbl_idx();
    if (children_cnt_ > ctdef_idx && ctdef_idx > 0 && children_ != nullptr) {
      const ObDASScanCtDef *child = static_cast<const ObDASScanCtDef *>(children_[ctdef_idx]);
      if (child->ir_scan_type_ == ObTSCIRScanType::OB_VEC_COM_AUX_SCAN) {
        com_aux_tbl_ctdef = child;
      }
    }
    return com_aux_tbl_ctdef;
  }

  int64_t get_inv_scan_idx() const { return ObVecAuxTableIdx::INV_SCAN_IDX; }
  int64_t get_delta_tbl_idx() const { return ObVecAuxTableIdx::DELTA_BUFFER_TBL_IDX; }
  int64_t get_index_id_tbl_idx() const { return ObVecAuxTableIdx::INDEX_ID_TBL_IDX; }
  int64_t get_snapshot_tbl_idx() const { return ObVecAuxTableIdx::SNAPSHOT_TBL_IDX; }
  int64_t get_com_aux_tbl_idx() const { return ObVecAuxTableIdx::COM_AUX_TBL_IDX; }

  INHERIT_TO_STRING_KV("ObDASBaseCtDef", ObDASBaseCtDef,
                       KPC_(inv_scan_vec_id_col), K_(vec_index_param), K_(dim));

  ObExpr *inv_scan_vec_id_col_;
  ObString vec_index_param_;
  int64_t dim_;
  ObVecIndexType vec_type_;
  ObVectorIndexAlgorithmType algorithm_type_;
  double selectivity_;
  double row_count_;
  bool access_pk_;
};

struct ObDASVecAuxScanRtDef : ObDASAttachRtDef
{
  OB_UNIS_VERSION(1);
public:
  ObDASVecAuxScanRtDef()
    : ObDASAttachRtDef(DAS_OP_VEC_SCAN) {}

  virtual ~ObDASVecAuxScanRtDef() {}

  ObDASScanRtDef *get_inv_idx_scan_rtdef()
  {
    const ObDASVecAuxScanCtDef *ctdef = static_cast<const ObDASVecAuxScanCtDef *>(ctdef_);
    const int64_t rtdef_idx = ctdef->get_inv_scan_idx();
    ObDASScanRtDef *idx_scan_rtdef = nullptr;
    if (children_cnt_ > rtdef_idx && children_ != nullptr) {
      idx_scan_rtdef = static_cast<ObDASScanRtDef*>(children_[rtdef_idx]);
    }
    return idx_scan_rtdef;
  }
  ObDASScanRtDef *get_delta_tbl_rtdef() const
  {
    const ObDASVecAuxScanCtDef *ctdef = static_cast<const ObDASVecAuxScanCtDef *>(ctdef_);
    const int64_t rtdef_idx = ctdef->get_delta_tbl_idx();
    ObDASScanRtDef *rtdef = nullptr;
    if (children_cnt_ > rtdef_idx && children_ != nullptr) {
      rtdef = static_cast<ObDASScanRtDef*>(children_[rtdef_idx]);
    }
    return rtdef;
  }
  ObDASScanRtDef *get_index_id_tbl_rtdef() const
  {
    const ObDASVecAuxScanCtDef *ctdef = static_cast<const ObDASVecAuxScanCtDef *>(ctdef_);
    const int64_t rtdef_idx = ctdef->get_index_id_tbl_idx();
    ObDASScanRtDef *rtdef = nullptr;
    if (children_cnt_ > rtdef_idx && children_ != nullptr) {
      rtdef = static_cast<ObDASScanRtDef*>(children_[rtdef_idx]);
    }
    return rtdef;
  }
  ObDASScanRtDef *get_snapshot_tbl_rtdef() const
  {
    const ObDASVecAuxScanCtDef *ctdef = static_cast<const ObDASVecAuxScanCtDef *>(ctdef_);
    const int64_t rtdef_idx = ctdef->get_snapshot_tbl_idx();
    ObDASScanRtDef *rtdef = nullptr;
    if (children_cnt_ > rtdef_idx && children_ != nullptr) {
      rtdef = static_cast<ObDASScanRtDef*>(children_[rtdef_idx]);
    }
    return rtdef;
  }

  ObDASScanRtDef *get_com_aux_tbl_rtdef() const
  {
    const ObDASVecAuxScanCtDef *ctdef = static_cast<const ObDASVecAuxScanCtDef *>(ctdef_);
    const int64_t rtdef_idx = ctdef->get_com_aux_tbl_idx();
    ObDASScanRtDef *rtdef = nullptr;
    if (children_cnt_ > rtdef_idx && children_ != nullptr) {
      rtdef = static_cast<ObDASScanRtDef*>(children_[rtdef_idx]);
    }
    return rtdef;
  }
};

} // namespace sql
} // namespace oceanbase

#endif

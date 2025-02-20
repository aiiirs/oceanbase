/**
 * Copyright (c) 2023 OceanBase
 * OceanBase CE is licensed under Mulan PubL v2.
 * You can use this software according to the terms and conditions of the Mulan PubL v2.
 * You may obtain a copy of Mulan PubL v2 at:
 *          http://license.coscl.org.cn/MulanPubL-2.0
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PubL v2 for more details.
 */

#ifndef OCEANBASE_LIB_OB_VECTOR_COSINE_DISTANCE_H_
#define OCEANBASE_LIB_OB_VECTOR_COSINE_DISTANCE_H_

#include "lib/utility/ob_print_utils.h"
#include "lib/oblog/ob_log.h"
#include "lib/ob_define.h"
#include "common/object/ob_obj_compare.h"

namespace oceanbase
{
namespace common
{
template <typename T>
struct ObVectorCosineDistance
{
  static int cosine_similarity_func(const T *a, const T *b, const int64_t len, double &similarity);
  static int cosine_distance_func(const T *a, const T *b, const int64_t len, double &distance);

  // normal func
  OB_INLINE static int cosine_similarity_normal(const T *a, const T *b, const int64_t len, double &similarity);
  OB_INLINE static int cosine_calculate_normal(const T *a, const T *b, const int64_t len, double &ip, double &abs_dist_a, double &abs_dist_b);
  OB_INLINE static double get_cosine_distance(double similarity);
  // TODO(@jingshui) add simd func
};

template <typename T>
int ObVectorCosineDistance<T>::cosine_similarity_func(const T *a, const T *b, const int64_t len, double &similarity)
{
  return cosine_similarity_normal(a, b, len, similarity);
}

template <typename T>
int ObVectorCosineDistance<T>::cosine_distance_func(const T *a, const T *b, const int64_t len, double &distance) {
  int ret = OB_SUCCESS;
  double similarity = 0;
  if (OB_FAIL(cosine_similarity_func(a, b, len, similarity))) {
    if (OB_ERR_NULL_VALUE != ret) {
      LIB_LOG(WARN, "failed to cal cosine similaity", K(ret));
    }
  } else {
    distance = get_cosine_distance(similarity);
  }
  return ret;
}

template <typename T>
OB_INLINE int ObVectorCosineDistance<T>::cosine_calculate_normal(const T *a, const T *b, const int64_t len, double &ip, double &abs_dist_a, double &abs_dist_b)
{
  int ret = OB_SUCCESS;
  for (int64_t i = 0; OB_SUCC(ret) && i < len; ++i) {
    ip += a[i] * b[i];
    abs_dist_a += a[i] * a[i];
    abs_dist_b += b[i] * b[i];
    if (OB_UNLIKELY(0 != ::isinf(ip) || 0 != ::isinf(abs_dist_a) || 0 != ::isinf(abs_dist_b))) {
      ret = OB_NUMERIC_OVERFLOW;
      LIB_LOG(WARN, "value is overflow", K(ret), K(ip), K(abs_dist_a), K(abs_dist_b));
    }
  }
  return ret;
}

template <typename T>
OB_INLINE int ObVectorCosineDistance<T>::cosine_similarity_normal(const T *a, const T *b, const int64_t len, double &similarity)
{
  int ret = OB_SUCCESS;
  double ip = 0;
  double abs_dist_a = 0;
  double abs_dist_b = 0;
  similarity = 0;
  if (OB_FAIL(cosine_calculate_normal(a, b, len, ip, abs_dist_a, abs_dist_b))) {
    LIB_LOG(WARN, "failed to cal cosine", K(ret), K(ip));
  } else if (0 == abs_dist_a || 0 == abs_dist_b) {
    ret = OB_ERR_NULL_VALUE;
  } else {
    similarity = ip / (sqrt(abs_dist_a * abs_dist_b));
  }
  return ret;
}

template <typename T>
OB_INLINE double ObVectorCosineDistance<T>::get_cosine_distance(double similarity)
{
  if (similarity > 1.0) {
    similarity = 1.0;
  } else if (similarity < -1.0) {
    similarity = -1.0;
  }
  return 1.0 - similarity;
}
} // common
} // oceanbase
#endif

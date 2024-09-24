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

#include "observer/omt/ob_tenant_config_mgr.h"
#include "lib/stat/ob_diagnose_info.h"
#include "common/ob_smart_var.h"
#include "storage/ob_file_system_router.h"
#include "share/ob_task_define.h"
#include "ob_tmp_file_cache.h"
#include "storage/blocksstable/ob_block_manager.h"
#include "storage/tmp_file/ob_tmp_file_global.h"
#include "storage/tmp_file/ob_tmp_file_manager.h"

using namespace oceanbase::storage;
using namespace oceanbase::share;

namespace oceanbase
{
namespace tmp_file
{
/* -------------------------- ObTmpBlockCacheKey --------------------------- */
ObTmpBlockCacheKey::ObTmpBlockCacheKey()
  : block_id_(-1), tenant_id_(OB_INVALID_TENANT_ID)
{
}

ObTmpBlockCacheKey::ObTmpBlockCacheKey(const int64_t block_id, const uint64_t tenant_id)
  : block_id_(block_id), tenant_id_(tenant_id)
{
}

ObTmpBlockCacheKey::~ObTmpBlockCacheKey()
{
}

bool ObTmpBlockCacheKey::operator ==(const ObIKVCacheKey &other) const
{
  const ObTmpBlockCacheKey &other_key = reinterpret_cast<const ObTmpBlockCacheKey &> (other);
  return block_id_ == other_key.block_id_ && tenant_id_ == other_key.tenant_id_;
}

uint64_t ObTmpBlockCacheKey::get_tenant_id() const
{
  return tenant_id_;
}

uint64_t ObTmpBlockCacheKey::hash() const
{
  return murmurhash(this, sizeof(ObTmpBlockCacheKey), 0);
}

int64_t ObTmpBlockCacheKey::size() const
{
  return sizeof(*this);
}

int ObTmpBlockCacheKey::deep_copy(char *buf, const int64_t buf_len, ObIKVCacheKey *&key) const
{
  int ret = OB_SUCCESS;
  if (OB_ISNULL(buf) || OB_UNLIKELY(buf_len < size())) {
    ret = OB_INVALID_ARGUMENT;
    STORAGE_LOG(WARN, "Invalid argument, ", KR(ret));
  } else if (OB_UNLIKELY(!is_valid())) {
    ret = OB_INVALID_DATA;
    STORAGE_LOG(WARN, "invalid tmp block cache key, ", KPC(this), KR(ret));
  } else {
    key = new (buf) ObTmpBlockCacheKey(block_id_, tenant_id_);
  }
  return ret;
}

bool ObTmpBlockCacheKey::is_valid() const
{
  return OB_LIKELY(block_id_ >= 0 && tenant_id_ > 0 && size() > 0);
}

/* -------------------------- ObTmpBlockCacheValue --------------------------- */
ObTmpBlockCacheValue::ObTmpBlockCacheValue(char *buf)
  : buf_(buf), size_(OB_DEFAULT_MACRO_BLOCK_SIZE)
{
}

ObTmpBlockCacheValue::~ObTmpBlockCacheValue()
{
}

int64_t ObTmpBlockCacheValue::size() const
{
  return sizeof(*this) + size_;
}

int ObTmpBlockCacheValue::deep_copy(char *buf, const int64_t buf_len, ObIKVCacheValue *&value) const
{
  int ret = OB_SUCCESS;
  if (OB_ISNULL(buf) || OB_UNLIKELY(buf_len < size())) {
    ret = OB_INVALID_ARGUMENT;
    STORAGE_LOG(WARN, "invalid arguments", KR(ret), KP(buf), K(buf_len),
                      "request_size", size());
  } else if (OB_UNLIKELY(!is_valid())) {
    ret = OB_INVALID_DATA;
    STORAGE_LOG(WARN, "invalid tmp block cache value", KR(ret));
  } else {
    ObTmpBlockCacheValue *blk_cache_value = new (buf) ObTmpBlockCacheValue(buf + sizeof(*this));
    MEMCPY(buf + sizeof(*this), buf_, size() - sizeof(*this));
    blk_cache_value->size_ = size_;
    value = blk_cache_value;
  }
  return ret;
}

/* -------------------------- ObTmpBlockCache --------------------------- */

ObTmpBlockCache &ObTmpBlockCache::get_instance()
{
  static ObTmpBlockCache instance;
  return instance;
}

int ObTmpBlockCache::init(const char *cache_name, const int64_t priority)
{
  int ret = OB_SUCCESS;
  if (OB_FAIL((common::ObKVCache<ObTmpBlockCacheKey, ObTmpBlockCacheValue>::init(
      cache_name, priority)))) {
    STORAGE_LOG(WARN, "Fail to init kv cache, ", KR(ret));
  }
  return ret;
}

void ObTmpBlockCache::destroy()
{
  common::ObKVCache<ObTmpBlockCacheKey, ObTmpBlockCacheValue>::destroy();
}

int ObTmpBlockCache::get_block(const ObTmpBlockCacheKey &key, ObTmpBlockValueHandle &handle)
{
  int ret = OB_SUCCESS;
  const ObTmpBlockCacheValue *value = NULL;
  if (OB_UNLIKELY(!key.is_valid())) {
    ret = OB_INVALID_ARGUMENT;
    STORAGE_LOG(WARN, "invalid arguments", KR(ret), K(key));
  } else if (OB_FAIL(get(key, value, handle.handle_))) {
    if (OB_UNLIKELY(OB_ENTRY_NOT_EXIST != ret)) {
      STORAGE_LOG(WARN, "fail to get key from block cache", KR(ret), K(key));
    } else {
      EVENT_INC(ObStatEventIds::TMP_BLOCK_CACHE_MISS);
    }
  } else {
    if (OB_ISNULL(value)) {
      ret = OB_ERR_UNEXPECTED;
      STORAGE_LOG(WARN, "unexpected error, the value must not be NULL", KR(ret));
    } else {
      handle.value_ = const_cast<ObTmpBlockCacheValue *>(value);
      EVENT_INC(ObStatEventIds::TMP_BLOCK_CACHE_HIT);
    }
  }
  return ret;
}

int ObTmpBlockCache::put_block(ObKVCacheInstHandle &inst_handle,
                               ObKVCachePair *&kvpair,
                               ObTmpBlockValueHandle &block_handle)
{
  int ret = OB_SUCCESS;
  if (OB_UNLIKELY(!inst_handle.is_valid() || nullptr == kvpair || !block_handle.is_valid())) {
    ret = OB_INVALID_ARGUMENT;
    STORAGE_LOG(WARN, "invalid argument", KR(ret), K(inst_handle), KP(kvpair), K(block_handle));
  } else if (OB_FAIL(put_kvpair(inst_handle, kvpair, block_handle.handle_, false/*overwrite*/))) {
    STORAGE_LOG(WARN, "fail to put tmp block to block cache", KR(ret));
  }

  return ret;
}

int ObTmpBlockCache::prealloc_block(const ObTmpBlockCacheKey &key, ObKVCacheInstHandle &inst_handle,
                                    ObKVCachePair *&kvpair,
                                    ObTmpBlockValueHandle &block_handle)
{
  int ret = OB_SUCCESS;
  inst_handle.reset();
  kvpair = nullptr;
  block_handle.reset();
  if (OB_UNLIKELY(!key.is_valid())) {
    ret = OB_INVALID_ARGUMENT;
    STORAGE_LOG(WARN, "invalid argument", KR(ret), K(key));
  } else if (OB_FAIL(alloc(key.get_tenant_id(), key.size(),
                     sizeof(ObTmpBlockCacheValue) + OB_DEFAULT_MACRO_BLOCK_SIZE,
                     kvpair, block_handle.handle_, inst_handle))) {
    STORAGE_LOG(WARN, "failed to alloc kvcache buf", KR(ret), K(key));
  } else if (OB_FAIL(key.deep_copy(reinterpret_cast<char *>(kvpair->key_),
                     key.size(), kvpair->key_))) {
    STORAGE_LOG(WARN, "failed to deep copy key", KR(ret), K(key));
  } else {
    char *buf = reinterpret_cast<char *>(kvpair->value_);
    block_handle.value_ = new (buf) ObTmpBlockCacheValue(buf + sizeof(ObTmpBlockCacheValue));
  }

  if (OB_FAIL(ret)) {
    block_handle.reset();
    inst_handle.reset();
    kvpair = NULL;
  }
  return ret;
}

/* -------------------------- ObTmpPageCacheKey --------------------------- */
ObTmpPageCacheKey::ObTmpPageCacheKey()
  : block_id_(-1), page_id_(-1), tenant_id_(OB_INVALID_TENANT_ID)
{
}

ObTmpPageCacheKey::ObTmpPageCacheKey(const int64_t block_id, const int64_t page_id,
    const uint64_t tenant_id)
  : block_id_(block_id), page_id_(page_id), tenant_id_(tenant_id)
{
}

ObTmpPageCacheKey::~ObTmpPageCacheKey()
{
}

bool ObTmpPageCacheKey::operator ==(const ObIKVCacheKey &other) const
{
  const ObTmpPageCacheKey &other_key = reinterpret_cast<const ObTmpPageCacheKey &> (other);
  return block_id_ == other_key.block_id_
         && page_id_ == other_key.page_id_
         && tenant_id_ == other_key.tenant_id_;
}

uint64_t ObTmpPageCacheKey::get_tenant_id() const
{
  return tenant_id_;
}

uint64_t ObTmpPageCacheKey::hash() const
{
  return murmurhash(this, sizeof(ObTmpPageCacheKey), 0);
}

int64_t ObTmpPageCacheKey::size() const
{
  return sizeof(*this);
}

int ObTmpPageCacheKey::deep_copy(char *buf, const int64_t buf_len, ObIKVCacheKey *&key) const
{
  int ret = OB_SUCCESS;
  if (OB_ISNULL(buf) || OB_UNLIKELY(buf_len < size())) {
    ret = OB_INVALID_ARGUMENT;
    STORAGE_LOG(WARN, "Invalid argument, ", KR(ret));
  } else if (OB_UNLIKELY(!is_valid())) {
    ret = OB_INVALID_DATA;
    STORAGE_LOG(WARN, "invalid tmp page cache key, ", KPC(this), KR(ret));
  } else {
    key = new (buf) ObTmpPageCacheKey(block_id_, page_id_, tenant_id_);
  }
  return ret;
}

bool ObTmpPageCacheKey::is_valid() const
{
  return OB_LIKELY(block_id_ >= 0 && page_id_ >= 0 && tenant_id_ > 0 && size() > 0);
}

/* -------------------------- ObTmpPageCacheValue --------------------------- */
ObTmpPageCacheValue::ObTmpPageCacheValue(char *buf)
  : buf_(buf), size_(ObTmpFileGlobal::PAGE_SIZE)
{
}

ObTmpPageCacheValue::~ObTmpPageCacheValue()
{
}

int64_t ObTmpPageCacheValue::size() const
{
  return sizeof(*this) + size_;
}

int ObTmpPageCacheValue::deep_copy(char *buf, const int64_t buf_len, ObIKVCacheValue *&value) const
{
  int ret = OB_SUCCESS;
  if (OB_ISNULL(buf) || OB_UNLIKELY(buf_len < size())) {
    ret = OB_INVALID_ARGUMENT;
    STORAGE_LOG(WARN, "invalid arguments", KR(ret), KP(buf), K(buf_len),
                      "request_size", size());
  } else if (OB_UNLIKELY(!is_valid())) {
    ret = OB_INVALID_DATA;
    STORAGE_LOG(WARN, "invalid tmp page cache value", KR(ret));
  } else {
    ObTmpPageCacheValue *page_cache_value = new (buf) ObTmpPageCacheValue(buf + sizeof(*this));
    MEMCPY(buf + sizeof(*this), buf_, size() - sizeof(*this));
    page_cache_value->size_ = size_;
    value = page_cache_value;
  }
  return ret;
}

/* -------------------------- ObTmpPageCache --------------------------- */

ObTmpPageCache &ObTmpPageCache::get_instance()
{
  static ObTmpPageCache instance;
  return instance;
}

int ObTmpPageCache::init(const char *cache_name, const int64_t priority)
{
  int ret = OB_SUCCESS;
  if (OB_FAIL((common::ObKVCache<ObTmpPageCacheKey, ObTmpPageCacheValue>::init(
      cache_name, priority)))) {
    STORAGE_LOG(WARN, "Fail to init kv cache, ", KR(ret));
  }
  return ret;
}

void ObTmpPageCache::destroy()
{
  common::ObKVCache<ObTmpPageCacheKey, ObTmpPageCacheValue>::destroy();
}

int ObTmpPageCache::get_page(const ObTmpPageCacheKey &key, ObTmpPageValueHandle &handle)
{
  int ret = OB_SUCCESS;
  const ObTmpPageCacheValue *value = NULL;
  if (OB_UNLIKELY(!key.is_valid())) {
    ret = OB_INVALID_ARGUMENT;
    STORAGE_LOG(WARN, "invalid arguments", KR(ret), K(key));
  } else if (OB_FAIL(get(key, value, handle.handle_))) {
    if (OB_UNLIKELY(OB_ENTRY_NOT_EXIST != ret)) {
      STORAGE_LOG(WARN, "fail to get key from page cache", KR(ret), K(key));
    } else {
      EVENT_INC(ObStatEventIds::TMP_PAGE_CACHE_MISS);
    }
  } else {
    if (OB_ISNULL(value)) {
      ret = OB_ERR_UNEXPECTED;
      STORAGE_LOG(WARN, "unexpected error, the value must not be NULL", KR(ret));
    } else {
      handle.value_ = const_cast<ObTmpPageCacheValue *>(value);
      EVENT_INC(ObStatEventIds::TMP_PAGE_CACHE_HIT);
    }
  }
  return ret;
}

void ObTmpPageCache::try_put_page_to_cache(const ObTmpPageCacheKey &key,
                                           const ObTmpPageCacheValue &value)
{
  int ret = OB_SUCCESS;
  if (OB_UNLIKELY(!key.is_valid() || !value.is_valid())) {
    ret = OB_INVALID_ARGUMENT;
    STORAGE_LOG(WARN, "invalid arguments", KR(ret), K(key), K(value));
  } else if (OB_FAIL(put(key, value, true/*overwrite*/))) {
    STORAGE_LOG(WARN, "fail to put tmp page into cache", KR(ret), K(key), K(value));
  }
}

int ObTmpPageCache::load_page(const ObTmpPageCacheKey &key,
                              ObIAllocator *callback_allocator,
                              ObTmpPageValueHandle &p_handle)
{
  int ret = OB_SUCCESS;
  ObKVCacheInstHandle inst_handle;
  ObKVCachePair *kvpair = NULL;
  p_handle.reset();
  if (OB_UNLIKELY(!key.is_valid())) {
    ret = OB_INVALID_ARGUMENT;
    STORAGE_LOG(WARN, "invalid argument", KR(ret), K(key));
  } else if (OB_ISNULL(callback_allocator)) {
    ret = OB_INVALID_ARGUMENT;
    STORAGE_LOG(WARN, "callback_allocator is unexpected nullptr", KR(ret), K(key));
  } else if (OB_FAIL(alloc(key.get_tenant_id(), key.size(),
      sizeof(ObTmpPageCacheValue) + ObTmpFileGlobal::PAGE_SIZE,
      kvpair, p_handle.handle_, inst_handle))) {
    STORAGE_LOG(WARN, "failed to alloc kvcache buf", KR(ret), K(key));
  } else if (OB_FAIL(key.deep_copy(reinterpret_cast<char *>(kvpair->key_),
      key.size(), kvpair->key_))) {
    STORAGE_LOG(WARN, "failed to deep copy key", KR(ret), K(key));
  } else {
    char *buf = reinterpret_cast<char *>(kvpair->value_);
    p_handle.value_ = new (buf) ObTmpPageCacheValue(buf + sizeof(ObTmpPageCacheValue));
  }
  if (OB_SUCC(ret)) {
    ObTmpFileBlockManager &block_manager = MTL(ObTenantTmpFileManager*)->get_sn_file_manager().get_tmp_file_block_manager();
    blocksstable::ObMacroBlockHandle mb_handle;
    blocksstable::MacroBlockId macro_block_id;
    //TODO: io_desc and io_timeout_ms value settings
    common::ObIOFlag io_desc;
    io_desc.set_wait_event(ObWaitEventIds::DB_FILE_DATA_READ);
    int64_t io_timeout_ms = 10 * 1000; // 10s
    if (OB_FAIL(block_manager.get_macro_block_id(key.get_block_id(), macro_block_id))) {
      STORAGE_LOG(WARN, "failed to get macro block id", KR(ret), K(key));
    } else if (OB_FAIL(direct_read(macro_block_id, ObTmpFileGlobal::PAGE_SIZE,
        key.get_page_id() * ObTmpFileGlobal::PAGE_SIZE,
        io_desc, io_timeout_ms, *callback_allocator, mb_handle))) {
      STORAGE_LOG(WARN, "failed to alloc kvcache buf", KR(ret), K(key));
    } else if (OB_FAIL(mb_handle.wait())) {
      STORAGE_LOG(WARN, "fail to do handle read wait", KR(ret), K(key));
    } else {
      MEMCPY(p_handle.value_->get_buffer(), mb_handle.get_buffer(), ObTmpFileGlobal::PAGE_SIZE);
    }
  }
  if (FAILEDx(put_kvpair(inst_handle, kvpair, p_handle.handle_, false/*overwrite*/))) {
    if (OB_ENTRY_EXIST == ret) {
      ret = OB_SUCCESS;
    } else {
      STORAGE_LOG(WARN, "fail to put tmp page to page cache", KR(ret), K(key));
    }
  }
  if (OB_FAIL(ret)) {
    p_handle.reset();
    inst_handle.reset();
    kvpair = NULL;
  }
  return ret;
}

// only read pages from disk
int ObTmpPageCache::direct_read(const blocksstable::MacroBlockId macro_block_id,
                                const int64_t read_size,
                                const int64_t begin_offset_in_block,
                                const common::ObIOFlag io_desc,
                                const int64_t io_timeout_ms,
                                common::ObIAllocator &callback_allocator,
                                blocksstable::ObMacroBlockHandle &mb_handle)
{
  int ret = OB_SUCCESS;
  void *buf = nullptr;
  ObTmpDirectReadPageIOCallback *callback = nullptr;
  if (OB_UNLIKELY(!macro_block_id.is_valid() || read_size <= 0 || begin_offset_in_block < 0 ||
                  (begin_offset_in_block + read_size > OB_DEFAULT_MACRO_BLOCK_SIZE) ||
                  !io_desc.is_valid() || io_timeout_ms < 0)) {
    ret = OB_INVALID_ARGUMENT;
    STORAGE_LOG(WARN, "Invalid arguments", KR(ret), K(macro_block_id), K(read_size),
                                           K(begin_offset_in_block), K(io_desc), K(io_timeout_ms));
  } else if (OB_ISNULL(buf = callback_allocator.alloc(sizeof(ObTmpDirectReadPageIOCallback)))) {
    ret = OB_ALLOCATE_MEMORY_FAILED;
    STORAGE_LOG(WARN, "allocate callback memory failed", KR(ret));
  } else {
    callback = new (buf) ObTmpDirectReadPageIOCallback;
    callback->cache_ = this;
    callback->allocator_ = &callback_allocator;
    if (OB_FAIL(inner_read_io_(macro_block_id, read_size, begin_offset_in_block,
                               io_desc, io_timeout_ms, callback, mb_handle))) {
      STORAGE_LOG(WARN, "fail to inner read io", KR(ret), K(macro_block_id), K(read_size),
                  K(begin_offset_in_block), K(io_desc), K(io_timeout_ms));
    }
    // There is no need to handle error cases (freeing the memory of the
    // callback) because inner_read_io_ will handle error cases and free the
    // memory of the callback.
  }
  return ret;
}

// read pages from disk and put them into kv_cache
int ObTmpPageCache::cached_read(const common::ObIArray<ObTmpPageCacheKey> &page_keys,
                                const blocksstable::MacroBlockId macro_block_id,
                                const int64_t begin_offset_in_block,
                                const common::ObIOFlag io_desc,
                                const int64_t io_timeout_ms,
                                common::ObIAllocator &callback_allocator,
                                blocksstable::ObMacroBlockHandle &mb_handle)
{
  int ret = OB_SUCCESS;
  void *buf = nullptr;
  ObTmpCachedReadPageIOCallback *callback = nullptr;
  const int64_t read_size = page_keys.count() * ObTmpFileGlobal::PAGE_SIZE;
  if (OB_UNLIKELY(page_keys.count() <= 0)) {
    ret = OB_INVALID_ARGUMENT;
    STORAGE_LOG(WARN, "Invalid arguments", KR(ret), K(page_keys.count()));
  } else if (OB_UNLIKELY(!macro_block_id.is_valid() || begin_offset_in_block < 0 ||
                         !io_desc.is_valid() || io_timeout_ms < 0)) {
    ret = OB_INVALID_ARGUMENT;
    STORAGE_LOG(WARN, "Invalid arguments", KR(ret), K(macro_block_id), K(begin_offset_in_block),
                                           K(io_desc), K(io_timeout_ms));
  } else if (OB_UNLIKELY(begin_offset_in_block % ObTmpFileGlobal::PAGE_SIZE != 0)) {
    ret = OB_INVALID_ARGUMENT;
    STORAGE_LOG(WARN, "cached_read should read completed pages", KR(ret), K(begin_offset_in_block));
  } else if (OB_UNLIKELY(begin_offset_in_block + read_size > OB_DEFAULT_MACRO_BLOCK_SIZE)) {
    ret = OB_INVALID_ARGUMENT;
    STORAGE_LOG(WARN, "invalid range", KR(ret), K(read_size), K(begin_offset_in_block));
  } else if (OB_ISNULL(buf = callback_allocator.alloc(sizeof(ObTmpCachedReadPageIOCallback)))) {
    ret = OB_ALLOCATE_MEMORY_FAILED;
    STORAGE_LOG(WARN, "allocate callback memory failed", KR(ret), K(macro_block_id));
  } else {
    callback = new (buf) ObTmpCachedReadPageIOCallback;
    callback->cache_ = this;
    callback->allocator_ = &callback_allocator;
    if (OB_FAIL(callback->page_keys_.assign(page_keys))) {
      STORAGE_LOG(WARN, "fail to assign page keys", KR(ret), K(page_keys.count()));
      callback->~ObTmpCachedReadPageIOCallback();
      callback_allocator.free(callback);
    } else if (OB_FAIL(inner_read_io_(macro_block_id, read_size, begin_offset_in_block,
                                      io_desc, io_timeout_ms, callback, mb_handle))) {
      STORAGE_LOG(WARN, "fail to inner read io", KR(ret), K(macro_block_id), K(read_size),
                  K(begin_offset_in_block), K(io_desc), K(io_timeout_ms));
    }
    // There is no need to handle error cases (freeing the memory of the
    // callback) because inner_read_io_ will handle error cases and free the
    // memory of the callback.
  }
  return ret;
}

// TODO: wanyue.wy
// refactor logic of callback.
// currently, the buffer of callback is freed in different places.
// if async_read_() is failed, it will be released in here;
// otherwise, it will be released after ObTmpFileIOCtx::do_read_wait_() has been called.
// (callback is bound with ObIOResult of io_handle, do_read_wait_() will call reset() of io_handle to
//  destroy callback and its data buf)
// we need to refactor it by removing callback allocator and directly copy read io buf to user buf
// rather than using a callback buf
int ObTmpPageCache::inner_read_io_(const blocksstable::MacroBlockId macro_block_id,
                                   const int64_t read_size,
                                   const int64_t begin_offset_in_block,
                                   const common::ObIOFlag io_desc,
                                   const int64_t io_timeout_ms,
                                   ObITmpPageIOCallback *callback,
                                   blocksstable::ObMacroBlockHandle &handle)
{
  int ret = OB_SUCCESS;
  if (OB_FAIL(async_read_(macro_block_id, read_size, begin_offset_in_block,
                          io_desc, io_timeout_ms, callback, handle))) {
    STORAGE_LOG(WARN, "fail to read tmp page from io", KR(ret), K(macro_block_id), K(read_size),
                K(begin_offset_in_block), K(io_desc), K(io_timeout_ms), KP(callback));
  }

  // if read successful, callback will be freed after user calls ObSNTmpFileIOHandle::wait()
  // for copying data from callback's buf to user's buf
  // thus, here just free memory of the failed cases
  if (OB_FAIL(ret) && OB_NOT_NULL(callback) && OB_NOT_NULL(callback->get_allocator())) {
    common::ObIAllocator *allocator = callback->get_allocator();
    callback->~ObITmpPageIOCallback();
    allocator->free(callback);
  }

  return ret;
}

int ObTmpPageCache::async_read_(const blocksstable::MacroBlockId macro_block_id,
                                const int64_t read_size,
                                const int64_t begin_offset_in_block,
                                const common::ObIOFlag io_desc,
                                const int64_t io_timeout_ms,
                                ObITmpPageIOCallback *callback,
                                blocksstable::ObMacroBlockHandle &handle)
{
  int ret = OB_SUCCESS;
  blocksstable::ObMacroBlockReadInfo read_info;
  read_info.macro_block_id_ = macro_block_id;
  read_info.size_ = read_size;
  read_info.offset_ = begin_offset_in_block;
  read_info.io_desc_ = io_desc;
  read_info.io_timeout_ms_ = io_timeout_ms;
  read_info.io_callback_ = callback;
  read_info.io_desc_.set_resource_group_id(THIS_WORKER.get_group_id());
  read_info.io_desc_.set_sys_module_id(ObIOModule::TMP_TENANT_MEM_BLOCK_IO);

  if (OB_FAIL(handle.async_read(read_info))) {
    STORAGE_LOG(WARN, "fail to async read block", KR(ret), K(read_info));
  }
  return ret;
}

ObTmpPageCache::ObITmpPageIOCallback::ObITmpPageIOCallback(const common::ObIOCallbackType type)
  : common::ObIOCallback(type), cache_(NULL), allocator_(NULL), data_buf_(NULL)
{
  static_assert(sizeof(*this) <= CALLBACK_BUF_SIZE, "IOCallback buf size not enough");
}

ObTmpPageCache::ObITmpPageIOCallback::~ObITmpPageIOCallback()
{
  if (NULL != allocator_ && NULL != data_buf_) {
    allocator_->free(data_buf_);
    data_buf_ = NULL;
  }
  allocator_ = NULL;
}

int ObTmpPageCache::ObITmpPageIOCallback::alloc_data_buf(const char *io_data_buffer, const int64_t data_size)
{
  int ret = alloc_and_copy_data(io_data_buffer, data_size, allocator_, data_buf_);
  return ret;
}

int ObTmpPageCache::ObITmpPageIOCallback::process_page(
    const ObTmpPageCacheKey &key, const ObTmpPageCacheValue &value)
{
  int ret = OB_SUCCESS;
  if (OB_UNLIKELY(!key.is_valid() || !value.is_valid())) {
    ret = OB_INVALID_ARGUMENT;
    STORAGE_LOG(WARN, "invalid arguments", KR(ret), K(key), K(value));
  } else if (OB_FAIL(cache_->put(key, value, true/*overwrite*/))) {
    STORAGE_LOG(WARN, "fail to put tmp page into cache", KR(ret), K(key), K(value));
  }
  return ret;
}

ObTmpPageCache::ObTmpCachedReadPageIOCallback::ObTmpCachedReadPageIOCallback()
  : ObITmpPageIOCallback(ObIOCallbackType::TMP_MULTI_PAGE_CALLBACK), page_keys_()
{
  static_assert(sizeof(*this) <= CALLBACK_BUF_SIZE, "IOCallback buf size not enough");
  page_keys_.set_attr(ObMemAttr(MTL_ID(), "TFCacheRead"));
}

ObTmpPageCache::ObTmpCachedReadPageIOCallback::~ObTmpCachedReadPageIOCallback()
{
  page_keys_.reset();
}

int ObTmpPageCache::ObTmpCachedReadPageIOCallback::inner_process(const char *data_buffer, const int64_t size)
{
  int ret = OB_SUCCESS;
  ObTimeGuard time_guard("TmpCachedReadPage_Callback_Process", 100000); //100ms
  if (OB_ISNULL(cache_) || OB_ISNULL(allocator_)) {
    ret = OB_ERR_UNEXPECTED;
    STORAGE_LOG(WARN, "Invalid tmp page cache callback allocator", KR(ret), KP(cache_), KP(allocator_));
  } else if (OB_UNLIKELY(size <= 0 || data_buffer == nullptr)) {
    ret = OB_INVALID_DATA;
    STORAGE_LOG(WARN, "invalid data buffer size", KR(ret), K(size), KP(data_buffer));
  } else if (OB_UNLIKELY(page_keys_.count() * ObTmpFileGlobal::PAGE_SIZE != size)) {
    ret = OB_INVALID_DATA;
    STORAGE_LOG(WARN, "invalid data buffer size", KR(ret), K(size), K(page_keys_.count()));
  } else if (OB_FAIL(alloc_data_buf(data_buffer, size))) {
    STORAGE_LOG(WARN, "Fail to allocate memory, ", KR(ret), K(size));
  } else if (FALSE_IT(time_guard.click("alloc_data_buf"))) {
  } else {
    for (int32_t i = 0; OB_SUCC(ret) && i < page_keys_.count(); i++) {
      ObTmpPageCacheValue value(nullptr);
      value.set_buffer(data_buf_ + i * ObTmpFileGlobal::PAGE_SIZE);
      if (OB_FAIL(process_page(page_keys_.at(i), value))) {
        STORAGE_LOG(WARN, "fail to process tmp page cache in callback", KR(ret));
      }
    }
    time_guard.click("process_page");
  }
  if (OB_FAIL(ret) && NULL != allocator_ && NULL != data_buf_) {
    allocator_->free(data_buf_);
    data_buf_ = NULL;
  }
  return ret;
}

int ObTmpPageCache::ObTmpDirectReadPageIOCallback::inner_process(const char *data_buffer, const int64_t size)
{
  int ret = OB_SUCCESS;
  ObTimeGuard time_guard("ObTmpDirectReadPageIOCallback", 100000); //100ms
  if (OB_ISNULL(cache_) || OB_ISNULL(allocator_)) {
    ret = OB_ERR_UNEXPECTED;
    STORAGE_LOG(WARN, "Invalid tmp page cache callback allocator", KR(ret), KP(cache_), KP(allocator_));
  } else if (OB_UNLIKELY(size <= 0 || data_buffer == nullptr)) {
    ret = OB_INVALID_DATA;
    STORAGE_LOG(WARN, "invalid data buffer size", KR(ret), K(size), KP(data_buffer));
  } else if (OB_FAIL(alloc_data_buf(data_buffer, size))) {
    STORAGE_LOG(WARN, "Fail to allocate memory, ", KR(ret), K(size));
  } else if (FALSE_IT(time_guard.click("alloc_data_buf"))) {
  }
  if (OB_FAIL(ret) && NULL != allocator_ && NULL != data_buf_) {
    allocator_->free(data_buf_);
    data_buf_ = NULL;
  }
  return ret;
}

}  // end namespace tmp_file
}  // end namespace oceanbase

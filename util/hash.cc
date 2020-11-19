// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "util/hash.h"

#include <cstring>

#include "util/coding.h"

// The FALLTHROUGH_INTENDED macro can be used to annotate implicit fall-through
// between switch labels. The real definition should be provided externally.
// This one is a fallback version for unsupported compilers.
#ifndef FALLTHROUGH_INTENDED
#define FALLTHROUGH_INTENDED \
  do {                       \
  } while (0)
#endif   //这里的FALLTHROUGH_INTENDED宏没有实际作用
/*
每次按照四字节长度读取字节流中的数据 w，并使用普通的哈希函数计算哈希值。
计算过程中使用 uint32_t 的自然溢出特性。四字节读取则为了加速，
最终可能剩下 3/2/1 个多余的字节，使用 switch 语句补充计算，以实现最好的性能。
leveldb中哈希表和布隆过滤器会使用到该哈希函数，未看懂如何实现的
*/
namespace leveldb {

uint32_t Hash(const char* data, size_t n, uint32_t seed) {
  // Similar to murmur hash 和非加密型hash相似，适用于基于hash进行查找的场景
  const uint32_t m = 0xc6a4a793;
  const uint32_t r = 24;
  const char* limit = data + n;
  uint32_t h = seed ^ (n * m);

  // Pick up four bytes at a time
  while (data + 4 <= limit) {
    uint32_t w = DecodeFixed32(data);
    data += 4;
    h += w;
    h *= m;
    h ^= (h >> 16);
  }

  // Pick up remaining bytes
  switch (limit - data) {
    case 3:
      h += static_cast<uint8_t>(data[2]) << 16;
      FALLTHROUGH_INTENDED;
    case 2:
      h += static_cast<uint8_t>(data[1]) << 8;
      FALLTHROUGH_INTENDED;
    case 1:
      h += static_cast<uint8_t>(data[0]);
      h *= m;
      h ^= (h >> r);
      break;
  }
  return h;
}

}  // namespace leveldb

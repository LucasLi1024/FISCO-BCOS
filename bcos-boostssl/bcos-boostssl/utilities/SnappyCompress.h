/*
 *  Copyright (C) 2021 FISCO BCOS.
 *  SPDX-License-Identifier: Apache-2.0
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 * @file P2PCompressMessage.h
 * @author: lucasli
 * @date 2021-12-22
 */

#pragma once
#include "Common.h"
#include "snappy.h"

namespace bcos
{
namespace boostssl
{
namespace utilities
{
class SnappyCompress
{
public:
    static size_t compress(bytesConstRef inputData, bytes& compressedData);
    static size_t uncompress(bytesConstRef compressedData, bytes& uncompressedData);
};
} // namespace utilities
} // namespace boostssl
} // namespace bcos
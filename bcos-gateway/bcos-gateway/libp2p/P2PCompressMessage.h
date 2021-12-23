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

#include <bcos-gateway/libp2p/P2PMessage.h>

namespace bcos
{
namespace gateway
{
class P2PCompressMessage : public P2PMessage
{
public:
    using Ptr = std::shared_ptr<P2PCompressMessage>;

    P2PCompressMessage() : P2PMessage() {}

    virtual ~P2PCompressMessage() {}

public:
    bool encodePayload(bytes& _payload) override;
    bool encode(bytes& _buffer) override;
    ssize_t decode(bytesConstRef _buffer) override;

    bool compressEnabled() const { return m_compressEnabled; };
    void setCompressEnabled(bool _CompressEnabled){ m_compressEnabled = _CompressEnabled };

private:
     bool compress(std::shared_ptr<bytes>);
      // control network package compress
     bool m_compressEnabled;
     std::shared_ptr<bytes> m_buffer;
}
} // namespace gateway
} // namespace bcos
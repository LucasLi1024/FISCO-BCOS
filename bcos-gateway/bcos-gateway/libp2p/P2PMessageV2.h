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
 * @file P2PMessageV2.h
 * @brief: extend srcNodeID and dstNodeID for message forward
 * @author: yujiechen
 * @date 2021-05-04
 */
#pragma once
#include "P2PMessage.h"

namespace bcos
{
namespace gateway
{
class P2PMessageV2 : public P2PMessage
{
public:
    using Ptr = std::shared_ptr<P2PMessageV2>;
    P2PMessageV2() : P2PMessage() {}
    ~P2PMessageV2() override {}

    virtual int16_t ttl() const { return m_ttl; }
    virtual void setTTL(int16_t _ttl) { m_ttl = _ttl; }

protected:
    ssize_t decodeHeader(bytesConstRef _buffer) override;
    bool encodeHeader(bytes& _buffer) override;

protected:
    int16_t m_ttl = 10;
};

class P2PMessageFactoryV2 : public MessageFactory
{
public:
    using Ptr = std::shared_ptr<P2PMessageFactoryV2>;
    P2PMessageFactoryV2() = default;
    ~P2PMessageFactoryV2() override {}
    Message::Ptr buildMessage() override
    {
        auto message = std::make_shared<P2PMessageV2>();
        message->setCompressType(m_compressType);
        return message;
    }

    // todo: for test compress performance, wait to be removed
    std::string const& compressType() const { return m_compressType; }
    void setCompressType(std::string _compressType) { m_compressType = _compressType; }

private:
    // todo: for test compress performance, wait to be removed
    std::string m_compressType;
};
}  // namespace gateway
}  // namespace bcos
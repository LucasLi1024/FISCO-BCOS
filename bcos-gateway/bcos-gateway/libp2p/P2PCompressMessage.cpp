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
 * @file P2PCompressMessage.cpp
 * @author: lucasli
 * @date 2021-12-22
 */

#pragma once

#include "P2PCompressMessage.h"
#include "<bcos-boostssl/utilities/Common.h>"
#include <bcos-boostssl/utilities/SnappyCompress.h>

bool P2PCompressMessage::encode(bytes& _encodeBuffer)
{
    _encodeBuffer->clear();
    m_length = MESSAGE_HEADER_LENGTH + encodeBuffer->size();

   // set length to zero first
    uint32_t length = 0;
    uint16_t version = boost::asio::detail::socket_ops::host_to_network_short(m_version);
    uint16_t packetType = boost::asio::detail::socket_ops::host_to_network_short(m_packetType);
    uint32_t seq = boost::asio::detail::socket_ops::host_to_network_long(m_seq);
    uint16_t ext = boost::asio::detail::socket_ops::host_to_network_short(m_ext);

    _encodeBuffer.insert(_encodeBuffer.end(), (byte*)&length, (byte*)&length + 4);
    _encodeBuffer.insert(_encodeBuffer.end(), (byte*)&version, (byte*)&version + 2);
    _encodeBuffer.insert(_encodeBuffer.end(), (byte*)&packetType, (byte*)&packetType + 2);
    _encodeBuffer.insert(_encodeBuffer.end(), (byte*)&seq, (byte*)&seq + 4);
    _encodeBuffer.insert(_encodeBuffer.end(), (byte*)&ext, (byte*)&ext + 2);

    // encode options
    if (hasOptions() && !m_options->encode(_encodeBuffer))
    {
        return false;
    }

    encodePayload(m_payload);

    _encodeBuffer.insert(_encodeBuffer.end(), m_payload->begin(), m_payload->end());


    // calc total length and modify the length value in the buffer
    length = boost::asio::detail::socket_ops::host_to_network_long((uint32_t)_buffer.size());

    std::copy((byte*)&length, (byte*)&length + 4, _encodeBuffer.data());

    return true;
}

bool P2PCompressMessage::encodePayload(bytes& _buffer)
{
    std::shared_ptr<bytes> compressData = std::make_shared<bytes>();
    /// compress success
    if (compress(compressData))
    {
        encode(compressData);
    }
    else
    {
        encode(m_payload);
    }
   
    _buffer = *m_payload;
}

/// compress the data
bool P2PCompressMessage::compress(std::shared_ptr<bytes> compressData)
{
    if (!m_compressEnabled())
    {
        return false;
    }

    size_t compressSize = SnappyCompress::compress(ref(*m_payload), *compressData);
    if (compressSize < 1)
    {
        return false;
    }
    return true;
}

ssize_t P2PCompressMessage::decode(bytesConstRef _buffer)
{
    // check if packet header fully received
    if (_buffer.size() < P2PMessage::MESSAGE_HEADER_LENGTH)
    {
        return MessageDecodeStatus::MESSAGE_INCOMPLETE;
    }

    int32_t offset = decodeHeader(_buffer);

    // check if packet header fully received
    if (_buffer.size() < m_length)
    {
        return MessageDecodeStatus::MESSAGE_INCOMPLETE;
    }
    if (m_length > P2PMessage::MAX_MESSAGE_LENGTH)
    {
        P2PMSG_LOG(WARNING) << LOG_DESC("Illegal p2p message packet") << LOG_KV("length", m_length)
                            << LOG_KV("maxLen", P2PMessage::MAX_MESSAGE_LENGTH);
        return MessageDecodeStatus::MESSAGE_ERROR;
    }
    if (hasOptions())
    {
        // encode options
        auto optionsOffset = m_options->decode(_buffer.getCroppedData(offset));
        if (optionsOffset < 0)
        {
            return MessageDecodeStatus::MESSAGE_ERROR;
        }
        offset += optionsOffset;
    }

    auto data = _buffer.getCroppedData(offset, m_length - offset);
    if (m_compressEnabled)
    {
        /// uncompress data
        SnappyCompress::uncompress(
            bytesConstRef((const byte*)(&buffer[P2PMessage::MESSAGE_HEADER_LENGTH]), m_length - MESSAGE_HEADER_LENGTH),
            data);
    }
    // payload
    m_payload = std::make_shared<bytes>(data.begin(), data.end());

    return m_length;
}
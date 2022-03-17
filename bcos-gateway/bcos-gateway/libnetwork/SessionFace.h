
/** @file Session.h
 * @author Gav Wood <i@gavwood.com>
 * @author Alex Leverington <nessence@gmail.com>
 * @date 2014
 * @author toxotguo
 * @date 2018
 *
 * @author: yujiechen
 * @date: 2018-09-19
 * @modification: remove addNote interface
 */

#pragma once
#include <bcos-gateway/libnetwork/Common.h>
#include <bcos-gateway/libnetwork/Message.h>
#include <bcos-boostssl/interfaces/MessageFace.h>
#include <bcos-boostssl/interfaces/NodeInfo.h>
#include <memory>

#include <boost/asio.hpp>

namespace bcos
{
namespace gateway
{
class SocketFace;

using SessionCallbackFunc = std::function<void(NetworkException, bcos::boostssl::MessageFace::Ptr)>;
struct ResponseCallback : public std::enable_shared_from_this<ResponseCallback>
{
    using Ptr = std::shared_ptr<ResponseCallback>;

    uint64_t m_startTime;
    SessionCallbackFunc callback;
    std::shared_ptr<boost::asio::deadline_timer> timeoutHandler;
};

class SessionFace
{
public:
    virtual ~SessionFace(){};

    using Ptr = std::shared_ptr<SessionFace>;

    virtual void start() = 0;
    virtual void disconnect(DisconnectReason) = 0;

    virtual void asyncSendMessage(
        boostssl::MessageFace::Ptr, Options = Options(), SessionCallbackFunc = SessionCallbackFunc()) = 0;

    virtual std::shared_ptr<SocketFace> socket() = 0;

    virtual void setMessageHandler(
        std::function<void(NetworkException, std::shared_ptr<SessionFace>, boostssl::MessageFace::Ptr)>
            messageHandler) = 0;

    virtual boostssl::NodeIPEndpoint nodeIPEndpoint() const = 0;

    virtual bool actived() const = 0;
};
}  // namespace gateway
}  // namespace bcos

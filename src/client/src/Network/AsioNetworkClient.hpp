#pragma once

#include "INetworkClient.hpp"
#include <asio.hpp>
#include <string>
#include <system_error>
#include <vector>

namespace rtype {
class AsioNetworkClient : public INetworkClient {
public:
    AsioNetworkClient(const std::string &ip, unsigned short port);
    ~AsioNetworkClient() override = default;

    void open() override;
    void setNonBlocking(bool nonBlocking) override;
    void send(const std::vector<unsigned char> &data) override;
    size_t receive(std::vector<unsigned char> &buffer) override;
    bool isWouldBlock() const override;
    void poll() override;

private:
    asio::io_context _ioContext;
    asio::ip::udp::socket _socket;
    asio::ip::udp::endpoint _serverEndpoint;
    bool _wasWouldBlock = false;
};
}

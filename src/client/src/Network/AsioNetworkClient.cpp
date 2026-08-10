#include "AsioNetworkClient.hpp"

namespace rtype {

    AsioNetworkClient::AsioNetworkClient(const std::string& ip, unsigned short port)
        : _socket(_ioContext),
          _serverEndpoint(asio::ip::address::from_string(ip), port)
    {
    }

    void AsioNetworkClient::open()
    {
        _socket.open(asio::ip::udp::v4());
    }

    void AsioNetworkClient::setNonBlocking(bool nonBlocking)
    {
        _socket.non_blocking(nonBlocking);
    }

    void AsioNetworkClient::send(const std::vector<unsigned char>& data)
    {
        try {
            _socket.send_to(asio::buffer(data), _serverEndpoint);
        } catch (const std::system_error &) {
            // Ignore for now
        }
    }

    size_t AsioNetworkClient::receive(std::vector<unsigned char>& buffer)
    {
        _wasWouldBlock = false;
        try {
            asio::ip::udp::endpoint senderEndpoint;
            return _socket.receive_from(asio::buffer(buffer), senderEndpoint);
        } catch (const std::system_error &e) {
            if (e.code() == asio::error::would_block) {
                _wasWouldBlock = true;
            }
            return 0;
        }
    }

    bool AsioNetworkClient::isWouldBlock() const
    {
        return _wasWouldBlock;
    }

    void AsioNetworkClient::poll()
    {
        _ioContext.poll();
        if (_ioContext.stopped()) {
            _ioContext.restart();
        }
    }
}

#pragma once

#include <string>
#include <vector>

namespace rtype {
class INetworkClient {
public:
    virtual ~INetworkClient() = default;

    virtual void open() = 0;
    virtual void setNonBlocking(bool nonBlocking) = 0;
    virtual void send(const std::vector<unsigned char> &data) = 0;
    virtual size_t receive(std::vector<unsigned char> &buffer) = 0;
    virtual bool isWouldBlock() const = 0;
    virtual void poll() = 0;
};
}

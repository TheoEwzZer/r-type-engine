/*
** EPITECH PROJECT, 2024
** rtype
** File description:
** Network
*/

#pragma once

#include "Entity.hpp"
#include <asio.hpp>
#include <map>
#include <memory>
#include <vector>

using namespace std;
using namespace asio;
using namespace asio::ip;

namespace rtype {
class Network {
public:
    Network(io_context &ioContext, const unsigned short port);
    void receive(
        vector<unsigned char> &recvBuffer, udp::endpoint &senderEndpoint);
    void sendTo(
        const vector<unsigned char> &data, const udp::endpoint &endpoint);
    void sendAll(const vector<unsigned char> &data);
    void setNonBlocking(const bool mode);
    [[nodiscard]] udp::socket &getSocket();

    [[nodiscard]] map<udp::endpoint, shared_ptr<ecs::Entity>, less<>> &
    getClients()
    {
        return clients;
    }

    [[nodiscard]] vector<udp::endpoint> &getSpectators() { return spectators; }

private:
    udp::socket socket;
    map<udp::endpoint, shared_ptr<ecs::Entity>, less<>> clients;
    vector<udp::endpoint> spectators;
};
}

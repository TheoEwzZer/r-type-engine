/*
** EPITECH PROJECT, 2024
** rtype
** File description:
** Network
*/

#include "Network.hpp"

rtype::Network::Network(io_context &ioContext, const unsigned short port) :
    socket(ioContext, udp::endpoint(udp::v4(), port))
{
}

void rtype::Network::receive(
    vector<unsigned char> &recvBuffer, udp::endpoint &senderEndpoint)
{
    socket.receive_from(buffer(recvBuffer), senderEndpoint);
}

void rtype::Network::sendTo(
    const vector<unsigned char> &data, const udp::endpoint &endpoint)
{
    socket.send_to(buffer(data), endpoint);
}

void rtype::Network::sendAll(const vector<unsigned char> &data)
{
    for (const auto &[endpoint, entity] : clients) {
        socket.send_to(buffer(data), endpoint);
    }
    for (const auto &endpoint : spectators) {
        socket.send_to(buffer(data), endpoint);
    }
}

void rtype::Network::setNonBlocking(const bool mode)
{
    socket.non_blocking(mode);
}

udp::socket &rtype::Network::getSocket() { return socket; }

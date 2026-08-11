/*
** EPITECH PROJECT, 2024
** rtype
** File description:
** Protocol
*/

#include "Protocol.hpp"
#include <cstdint>
#include <iostream>

using namespace rtype;
using enum rtype::Direction;

inline uint64_t toNetworkEndian64(uint64_t value)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    value = ((value & 0x0000'0000'0000'00FFULL) << 56
        | (value & 0x0000'0000'0000'FF00ULL) << 40
        | (value & 0x0000'0000'00FF'0000ULL) << 24
        | (value & 0x0000'0000'FF00'0000ULL) << 8
        | (value & 0x0000'00FF'0000'0000ULL) >> 8
        | (value & 0x0000'FF00'0000'0000ULL) >> 24
        | (value & 0x00FF'0000'0000'0000ULL) >> 40
        | (value & 0xFF00'0000'0000'0000ULL) >> 56);
#endif
    return value;
}

inline uint64_t fromNetworkEndian64(uint64_t value)
{
    return toNetworkEndian64(value);
}

vector<unsigned char> BinaryProtocol::compressData(
    const vector<unsigned char> &data)
{
    uLongf destSize = compressBound(data.size());
    vector<unsigned char> outBuffer(destSize + sizeof(uint64_t) + 1);
    if (compress(outBuffer.data() + sizeof(uint64_t) + 1, &destSize,
            data.data(), data.size())
        != Z_OK) {
        cerr << "Compression failed." << endl;
        return data;
    }
    uint64_t originalSize64 = static_cast<uint64_t>(data.size());
    originalSize64 = toNetworkEndian64(originalSize64);
    ::memcpy(outBuffer.data() + 1, &originalSize64, sizeof(uint64_t));
    outBuffer.resize(destSize + sizeof(uint64_t) + 1);
    float ratio = 100.0f
        - (static_cast<float>(destSize) * 100.0f
            / static_cast<float>(data.size()));
    if (ratio < 0) {
        vector<unsigned char> uncompressedData(1 + data.size());
        uncompressedData[0] = 0;
        ::memcpy(uncompressedData.data() + 1, data.data(), data.size());
        return uncompressedData;
    }
    outBuffer[0] = 1;
    return outBuffer;
}

vector<unsigned char> BinaryProtocol::decompressData(
    const vector<unsigned char> &data)
{
    if (data[0] == 0) {
        vector<unsigned char> uncompressedData(data.begin() + 1, data.end());
        return uncompressedData;
    }

    uint64_t originalSize64 = 0;
    ::memcpy(&originalSize64, data.data() + 1, sizeof(uint64_t));
    originalSize64 = fromNetworkEndian64(originalSize64);
    size_t originalSize = static_cast<size_t>(originalSize64);
    vector<unsigned char> outBuffer(originalSize);
    uLongf destSize = static_cast<uLongf>(originalSize);
    if (uncompress(outBuffer.data(), &destSize,
            data.data() + sizeof(uint64_t) + 1,
            data.size() - sizeof(uint64_t) - 1)
        != Z_OK) {
        cerr << "Decompression failed." << endl;
        return {};
    }
    if (destSize != originalSize) {
        cerr << "Decompressed size (" << destSize
             << ") does not match expected size (" << originalSize << ")."
             << endl;
        return {};
    }
    return outBuffer;
}

vector<unsigned char> BinaryProtocol::serializeSpriteList(
    const vector<Sprite> &sprites)
{
    vector<unsigned char> buffer(sprites.size() * sizeof(Sprite));
    unsigned int offset = 0;
    for (auto sprite : sprites) {
        sprite.spritesheetIndex = toNetworkEndian(sprite.spritesheetIndex);
        sprite.x = toNetworkEndian(sprite.x);
        sprite.y = toNetworkEndian(sprite.y);
        sprite.width = toNetworkEndian(sprite.width);
        sprite.height = toNetworkEndian(sprite.height);
        sprite.gameX = toNetworkEndian(sprite.gameX);
        sprite.gameY = toNetworkEndian(sprite.gameY);
        sprite.scaleX = toNetworkEndian(sprite.scaleX);
        sprite.scaleY = toNetworkEndian(sprite.scaleY);
        sprite.id = toNetworkEndian(sprite.id);
        sprite.rotation = toNetworkEndian(sprite.rotation);
        ::memcpy(buffer.data() + offset, &sprite, sizeof(Sprite));
        offset += sizeof(Sprite);
    }
    vector<unsigned char> compressedData = compressData(buffer);
    vector<unsigned char> finalBuffer(1 + compressedData.size());
    finalBuffer[0] = static_cast<uint8_t>(Event::SPRITE_UPDATE);
    ::memcpy(
        finalBuffer.data() + 1, compressedData.data(), compressedData.size());

    return finalBuffer;
}

vector<Sprite> BinaryProtocol::deserializeSpriteList(
    const vector<unsigned char> &buffer)
{
    if (buffer.empty()
        || buffer[0] != static_cast<uint8_t>(Event::SPRITE_UPDATE)) {
        return {};
    }
    vector<unsigned char> compressedData(buffer.begin() + 1, buffer.end());
    vector<unsigned char> decompressed = decompressData(compressedData);
    vector<Sprite> sprites;
    if ((decompressed.size() % sizeof(Sprite)) != 0) {
        return sprites;
    }
    const size_t count = decompressed.size() / sizeof(Sprite);
    sprites.reserve(count);
    for (size_t i = 0; i < count; ++i) {
        Sprite sprite {};
        ::memcpy(&sprite, decompressed.data() + (i * sizeof(Sprite)),
            sizeof(Sprite));
        sprite.spritesheetIndex = fromNetworkEndian(sprite.spritesheetIndex);
        sprite.x = fromNetworkEndian(sprite.x);
        sprite.y = fromNetworkEndian(sprite.y);
        sprite.width = fromNetworkEndian(sprite.width);
        sprite.height = fromNetworkEndian(sprite.height);
        sprite.gameX = fromNetworkEndian(sprite.gameX);
        sprite.gameY = fromNetworkEndian(sprite.gameY);
        sprite.scaleX = fromNetworkEndian(sprite.scaleX);
        sprite.scaleY = fromNetworkEndian(sprite.scaleY);
        sprite.id = fromNetworkEndian(sprite.id);
        sprite.rotation = fromNetworkEndian(sprite.rotation);
        sprites.emplace_back(sprite);
    }
    return sprites;
}

vector<unsigned char> BinaryProtocol::serializePlayerEvent(
    const PlayerEvent &event)
{
    vector<unsigned char> buffer(sizeof(PlayerEvent));
    PlayerEvent tempEvent = event;
    tempEvent.playerId = toNetworkEndian(tempEvent.playerId);
    tempEvent.packetId = toNetworkEndian(tempEvent.packetId);
    ::memcpy(buffer.data(), &tempEvent, sizeof(PlayerEvent));
    return buffer;
}

PlayerEvent BinaryProtocol::deserializePlayerEvent(
    const vector<unsigned char> &buffer)
{
    if (buffer.size() < sizeof(PlayerEvent)) {
        return PlayerEvent { Event::MOVE, 0, 0 };
    }
    PlayerEvent event {};
    ::memcpy(&event, buffer.data(), sizeof(PlayerEvent));
    event.playerId = fromNetworkEndian(event.playerId);
    event.packetId = fromNetworkEndian(event.packetId);
    return event;
}

vector<unsigned char> BinaryProtocol::serializePlayerEventMove(
    const PlayerEventMove &event)
{
    vector<unsigned char> buffer(sizeof(PlayerEventMove));
    PlayerEventMove tempEvent = event;
    tempEvent.playerId = toNetworkEndian(tempEvent.playerId);
    ::memcpy(buffer.data(), &tempEvent, sizeof(PlayerEventMove));
    return buffer;
}

PlayerEventMove BinaryProtocol::deserializePlayerEventMove(
    const vector<unsigned char> &buffer)
{
    if (buffer.size() < sizeof(PlayerEventMove)) {
        return PlayerEventMove { Direction::NONE, Direction::NONE, 0 };
    }
    PlayerEventMove event {};
    ::memcpy(&event, buffer.data(), sizeof(PlayerEventMove));
    event.playerId = fromNetworkEndian(event.playerId);
    return event;
}

vector<unsigned char> BinaryProtocol::serializePlayerEventLife(
    const PlayerEventLife &event)
{
    vector<unsigned char> buffer(sizeof(PlayerEventLife));
    PlayerEventLife tempEvent = event;
    tempEvent.playerId = toNetworkEndian(tempEvent.playerId);
    tempEvent.lives = toNetworkEndian(tempEvent.lives);
    tempEvent.packetId = toNetworkEndian(tempEvent.packetId);
    ::memcpy(buffer.data(), &tempEvent, sizeof(PlayerEventLife));
    return buffer;
}

PlayerEventLife BinaryProtocol::deserializePlayerEventLife(
    const vector<unsigned char> &buffer)
{
    if (buffer.size() < sizeof(PlayerEventLife)) {
        return PlayerEventLife { 0, 0, 0 };
    }
    PlayerEventLife event {};
    ::memcpy(&event, buffer.data(), sizeof(PlayerEventLife));
    event.playerId = fromNetworkEndian(event.playerId);
    event.lives = fromNetworkEndian(event.lives);
    event.packetId = fromNetworkEndian(event.packetId);
    return event;
}

vector<unsigned char> BinaryProtocol::serializePlayerEventLevel(
    const PlayerEventLevel &event)
{
    vector<unsigned char> buffer(sizeof(PlayerEventLevel));
    PlayerEventLevel tempEvent = event;
    tempEvent.level = toNetworkEndian(tempEvent.level);
    tempEvent.packetId = toNetworkEndian(tempEvent.packetId);
    ::memcpy(buffer.data(), &tempEvent, sizeof(PlayerEventLevel));
    return buffer;
}

PlayerEventLevel BinaryProtocol::deserializePlayerEventLevel(
    const vector<unsigned char> &buffer)
{
    if (buffer.size() < sizeof(PlayerEventLevel)) {
        return PlayerEventLevel { 1, 0 };
    }
    PlayerEventLevel event {};
    ::memcpy(&event, buffer.data(), sizeof(PlayerEventLevel));
    event.level = fromNetworkEndian(event.level);
    event.packetId = fromNetworkEndian(event.packetId);
    return event;
}

vector<unsigned char> BinaryProtocol::serializePacketAck(const PacketAck &ack)
{
    vector<unsigned char> buffer(sizeof(PacketAck));
    PacketAck tempAck = ack;
    tempAck.packetId = toNetworkEndian(tempAck.packetId);
    ::memcpy(buffer.data(), &tempAck, sizeof(PacketAck));
    return buffer;
}

PacketAck BinaryProtocol::deserializePacketAck(
    const vector<unsigned char> &buffer)
{
    if (buffer.size() < sizeof(PacketAck)) {
        return PacketAck { Event::ACK, 0 };
    }
    PacketAck ack {};
    ::memcpy(&ack, buffer.data(), sizeof(PacketAck));
    ack.packetId = fromNetworkEndian(ack.packetId);
    return ack;
}

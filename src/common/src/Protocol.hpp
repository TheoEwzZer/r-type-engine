/*
** EPITECH PROJECT, 2024
** rtype
** File description:
** Protocol
*/

#pragma once
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>
#include <zlib.h>

using namespace std;

namespace rtype {

#ifdef __BIG_ENDIAN__
constexpr bool IS_BIG_ENDIAN = true;
#else
constexpr bool IS_BIG_ENDIAN = false;
#endif

inline uint16_t swapEndian16(uint16_t val)
{
    return static_cast<uint16_t>(
        ((val & 0x00FF) << 8) | ((val & 0xFF00) >> 8));
}

inline uint32_t swapEndian32(uint32_t val)
{
    return ((val & 0x0000'00FF) << 24) | ((val & 0x0000'FF00) << 8)
        | ((val & 0x00FF'0000) >> 8) | ((val & 0xFF00'0000) >> 24);
}

inline float swapEndianFloat(float val)
{
    uint32_t temp;
    ::memcpy(&temp, &val, sizeof(temp));
    temp = swapEndian32(temp);
    ::memcpy(&val, &temp, sizeof(val));
    return val;
}

template <typename T> inline T toNetworkEndian(T val)
{
    if constexpr (sizeof(T) == 2) {
        return IS_BIG_ENDIAN ? val : swapEndian16(val);
    } else if constexpr (sizeof(T) == 4) {
        // For floats or 32-bit ints
        if constexpr (is_same_v<T, float>) {
            return IS_BIG_ENDIAN ? val : swapEndianFloat(val);
        } else {
            return IS_BIG_ENDIAN ? val : swapEndian32(val);
        }
    }
    return val;
}

template <typename T> inline T fromNetworkEndian(T val)
{
    return toNetworkEndian(val);
}

enum class Event : unsigned char {
    MOVE,
    CHARGED_SHOOT,
    SHOOT,
    SCORE_UPDATE,
    JOIN,
    DESTROY,
    BOSS_FIGHT,
    DETACH_ATTACH_FORCE,
    SPECTATOR,
    SPRITE_UPDATE,
    ACK
};

enum class Direction : unsigned char { UP, DOWN, LEFT, RIGHT, NONE };

#pragma pack(push, 1)

struct Sprite {
    unsigned short spritesheetIndex;
    unsigned short x;
    unsigned short y;
    unsigned short width;
    unsigned short height;
    int gameX;
    int gameY;
    float scaleX;
    float scaleY;
    unsigned int id;
    unsigned short rotation;
};

struct PlayerEvent {
    Event event;
    unsigned int playerId;
    uint32_t packetId;
};

struct PacketAck {
    Event event; // Event::ACK
    uint32_t packetId;
};

struct PlayerEventMove {
    Direction dx;
    Direction dy;
    unsigned int playerId;
};

struct PlayerEventLife {
    unsigned int playerId;
    int lives;
    uint32_t packetId;
};

struct PlayerEventLevel {
    unsigned char level;
    uint32_t packetId;
};

#pragma pack(pop)

class BinaryProtocol {
public:
    static vector<unsigned char> compressData(
        const vector<unsigned char> &data);
    static vector<unsigned char> decompressData(
        const vector<unsigned char> &data);

    static vector<unsigned char> serializeSpriteList(
        const vector<Sprite> &sprites);
    static vector<Sprite> deserializeSpriteList(
        const vector<unsigned char> &buffer);
    static vector<unsigned char> serializePlayerEvent(
        const PlayerEvent &event);
    static PlayerEvent deserializePlayerEvent(
        const vector<unsigned char> &buffer);
    static vector<unsigned char> serializePlayerEventMove(
        const PlayerEventMove &event);
    static PlayerEventMove deserializePlayerEventMove(
        const vector<unsigned char> &buffer);
    static vector<unsigned char> serializePlayerEventLife(
        const PlayerEventLife &event);
    static PlayerEventLife deserializePlayerEventLife(
        const vector<unsigned char> &buffer);
    static vector<unsigned char> serializePlayerEventLevel(
        const PlayerEventLevel &event);
    static PlayerEventLevel deserializePlayerEventLevel(
        const vector<unsigned char> &buffer);
    static vector<unsigned char> serializePacketAck(
        const PacketAck &ack);
    static PacketAck deserializePacketAck(
        const vector<unsigned char> &buffer);
};
}

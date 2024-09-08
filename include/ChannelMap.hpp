#include <map>

#ifndef CHANNEL_MAP_HPP
#define CHANNEL_MAP_HPP

struct ChannelPins {
    uint8_t inputPin;
    uint8_t outputPin;
};

std::map<uint8_t, ChannelPins> channelMap = {
    {1, {3, 4}},
    {2, {5, 16}},
    {3, {12, 17}},
    {4, {14, 18}},
    {5, {15, 19}},
    {6, {32, 21}},
    {7, {33, 22}},
    {8, {34, 23}},
    {9, {35, 25}},
    {10, {36, 26}},
    {11, {39, 27}}
};

#endif // CHANNEL_MAP_HPP

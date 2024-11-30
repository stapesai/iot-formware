#ifndef CHANNEL_MAP_HPP
#define CHANNEL_MAP_HPP

#include <map>

struct ChannelPins
{
	uint8_t inputPin;
	uint8_t outputPin;
};

extern std::map<uint8_t, ChannelPins> channelMap; // Declare as extern

#endif // CHANNEL_MAP_HPP
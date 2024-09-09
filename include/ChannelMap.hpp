// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

/*
 * @file include/ChannelMap.hpp
 * @brief This file defines the mapping between channel numbers and their 
 *        corresponding input and output GPIO pins.
 * 
 * The channelMap is a map where the key is the channel number and the value 
 * is a ChannelPins struct containing the input and output pin numbers for that channel.
 */
#ifndef CHANNEL_MAP_HPP
#define CHANNEL_MAP_HPP

#include <map>

// Structure to store input and output pins for a channel
struct ChannelPins
{
	uint8_t inputPin;
	uint8_t outputPin;
};

// Declare the channel map as extern to be defined in ChannelMap.cpp
extern std::map<uint8_t, ChannelPins> channelMap;

#endif // CHANNEL_MAP_HPP
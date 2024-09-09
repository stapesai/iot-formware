// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

// Path: src/ChannelMap.cpp

#include <ChannelMap.hpp>

// Define the channel map here (only once)
std::map<uint8_t, ChannelPins> channelMap = {{1, {3, 4}}, // Channel 1: Input = GPIO3, Output = GPIO4
											 {2, {5, 16}}, // Channel 2: Input = GPIO5, Output = GPIO16
											 {3, {12, 17}}, // Channel 3: Input = GPIO12, Output = GPIO17
											 {4, {14, 18}},
											 {5, {15, 19}},
											 {6, {32, 21}},
											 {7, {33, 22}},
											 {8, {34, 23}},
											 {9, {35, 25}},
											 {10, {36, 26}},
											 {11, {39, 27}}};

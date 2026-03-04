/*
 * FrameManager.cpp
 *
 *  Created on: Jan 13, 2026
 *      Author: dptrias
 */
#ifndef FRAMEMANAGER_H__
#define FRAMEMANAGER_H__

#include "receiver.h"
#include "transmitter.h"

struct UplinkFrame
{
    uint8_t cmd;
    uint8_t content;

};

struct DownlinkFrame
{
    uint8_t cmd;
    uint8_t content;

};

class FrameManager {
public:
    FrameManager();
    ~FrameManager();

    void initialize();
    void shutdown();

    void createFrame(int frameId);
    void destroyFrame(int frameId);

    bool frameExists(int frameId) const;
private:
    std::map<int, UplinkFrame> uplinkFrames;
    std::map<int, DownlinkFrame> downlinkFrames;
};

#endif /* FRAMEMANAGER_H__ */
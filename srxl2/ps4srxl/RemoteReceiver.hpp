#pragma once
#include "SrxlDevice.hpp"

class RemoteReceiver : public SrxlMaster
{
public:
  RemoteReceiver(SoftwareSerial &serial, SrxlDeviceID id, int time_frame = 22) : SrxlMaster(serial, id, time_frame) {}
};

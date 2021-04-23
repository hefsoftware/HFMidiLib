/*
 * Copyright 2021 Marzocchi Alessandro
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "driver.h"
#include "framework.h"
#include <QDebug>
using namespace HFMidi;
bool Driver::addDriverToFramework(HFMidi::Driver *driver)
{
  return Framework::framework()->addDriver(driver);
}

HFMidi::Driver::Driver()
{

}

QSharedPointer<Device> Driver::openDevice(const QString &device)
{
  QSharedPointer<Device> devicePtr=m_devices.value(device);
  if(!devicePtr)
  {
    devicePtr=createDevice(device);
    addDeviceToCache(device, devicePtr);
  }
  return devicePtr;
}

QSharedPointer<Port> Driver::openPort(const QString &device, const QString &port)
{
  QSharedPointer<Port> ret;
  auto devicePtr=openDevice(device);
  if(devicePtr)
    ret=devicePtr->openPort(port);
  return ret;
}

void Driver::addDeviceToCache(const QString &deviceId, QWeakPointer<Device> devicePtr)
{
  if(devicePtr)
    m_devices.insert(deviceId, devicePtr);
}

QSharedPointer<Device> Driver::createDevice(const QString &device)
{
  Q_UNUSED(device);
  return nullptr;
}

bool Driver::initialize()
{
  return false;
}

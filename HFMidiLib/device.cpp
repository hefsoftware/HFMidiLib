/*
 * Copyright 2021 Marzocchi Alessandro
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "device.h"
using namespace HFMidi;
Device::Device(QObject *parent) : QObject(parent)
{

}

QSharedPointer<Port> Device::openPort(const QString &id)
{
  QSharedPointer<Port> ret=m_ports.value(id);
  if(!ret)
  {
    ret=createPort(id);
    addPortToCache(id, ret);
  }
  return ret;

}

QSharedPointer<Port> Device::createPort(const QString &id)
{
  Q_UNUSED(id);
  return nullptr;
}

void Device::addPortToCache(const QString &portId, QWeakPointer<Port> portPtr)
{
  if(portPtr)
    m_ports.insert(portId, portPtr);
}

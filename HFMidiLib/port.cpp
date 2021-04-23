/*
 * Copyright 2021 Marzocchi Alessandro
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "port.h"
using namespace HFMidi;

Port::Port()
{

}

void Port::sendEvent(const QByteArray &data)
{
  sendData(data);
}

bool Port::sendData(const QByteArray &data)
{
  Q_UNUSED(data);
  return false;
}

void Port::receivedData(const QByteArray &data)
{
  //  qDebug()<<data.toHex();
  emit midiEvent(data);
}

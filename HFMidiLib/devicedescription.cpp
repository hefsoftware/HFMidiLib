/*
 * Copyright 2021 Marzocchi Alessandro
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "devicedescription.h"
using namespace HFMidi;

DeviceDescription::DeviceDescription(const QString &driver): m_driver(driver)
{

}

QString DeviceDescription::deviceId() const
{
  return m_id;
}

QString DeviceDescription::fullId()
{
  return QString("%1//%2").arg(m_driver, m_id);
}

QString DeviceDescription::portId(const QString &port)
{
  return QString("%1//%2//%3").arg(m_driver, m_id, port);
}

void DeviceDescription::setDeviceId(const QString &id)
{
  m_id = id;
}

QString DeviceDescription::description() const
{
  return m_description;
}

void DeviceDescription::setDescription(const QString &description)
{
  m_description = description;
}

int DeviceDescription::numInputPorts() const
{
  int ret=0;
  for(int i=0;i<m_ports.size();i++)
    ret=m_ports[i].isInput();
  return ret;
}


int DeviceDescription::numOutputPorts() const
{
  int ret=0;
  for(int i=0;i<m_ports.size();i++)
    ret=m_ports[i].isOutput();
  return ret;
}

int DeviceDescription::numPorts() const
{
  return m_ports.size();
}

void DeviceDescription::addPort(const PortDescription &port)
{
  m_ports.append(port);
}

QList<PortDescription> DeviceDescription::inputPorts()
{
  QList<PortDescription> ret;
  for(int i=0;i<m_ports.size();i++)
    if(m_ports[i].isInput())
      ret.append(m_ports[i]);
  return ret;
}

QList<PortDescription> DeviceDescription::outputPorts()
{
  QList<PortDescription> ret;
  for(int i=0;i<m_ports.size();i++)
    if(!m_ports[i].isInput())
      ret.append(m_ports[i]);
  return ret;
}

QList<PortDescription> DeviceDescription::ports()
{
  return m_ports;
}

const QList<PortDescription> &DeviceDescription::portsRef() const
{
  return m_ports;
}

void DeviceDescription::addExtra(const QString &id, const QVariant &value)
{
  m_extra.insert(id, value);
}

const QMap<QString, QVariant> &DeviceDescription::extra() const
{
    return m_extra;
}



QString DeviceDescription::driverId() const
{
    return m_driver;
}

void DeviceDescription::setDriverId(const QString &driver)
{
  m_driver = driver;
}


QString PortDescription::deviceId() const
{
  return m_deviceId;
}

void PortDescription::setDeviceId(const QString &device)
{
  m_deviceId = device;
}

bool PortDescription::isInput() const
{
  return m_input;
}

void PortDescription::setInput(bool input)
{
  m_input = input;
}

bool PortDescription::isOutput() const
{
  return m_output;
}

void PortDescription::setOutput(bool output)
{
  m_output = output;
}

PortDescription::PortDescription(const QString &driver, const QString &device): m_driverId(driver), m_deviceId(device), m_input(false), m_output(false)
{

}

QString PortDescription::fullId()
{
  return QString("%1//%2//%3").arg(m_driverId, m_deviceId, m_portId);
}

QString PortDescription::driverId() const
{
  return m_driverId;
}

void PortDescription::setDriverId(const QString &driver)
{
  m_driverId = driver;
}

QString PortDescription::portId() const
{
  return m_portId;
}

void PortDescription::setPortId(const QString &id)
{
  m_portId = id;
}

QString PortDescription::description() const
{
  return m_description;
}

void PortDescription::setDescription(const QString &description)
{
  m_description = description;
}

QMap<QString, QVariant> PortDescription::extra() const
{
  return m_extra;
}

void PortDescription::setExtra(const QMap<QString, QVariant> &extra)
{
  m_extra = extra;
}

void PortDescription::addExtra(const QString &id, const QVariant &value)
{
  m_extra.insert(id, value);
}


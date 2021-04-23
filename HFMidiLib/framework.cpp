/*
 * Copyright 2021 Marzocchi Alessandro
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "framework.h"
#include "driver.h"
#include <QDebug>
using namespace HFMidi;

Framework *Framework::m_framework=nullptr;


bool Framework::addDriver(Driver *driver)
{
  bool ret=false;
  if(driver && !m_drivers.contains(driver->driverId()))
  {
    if(m_initialized)
      ret=driver->initialize();
    else
      ret=true;
    if(ret)
    {
      m_drivers.insert(driver->driverId(), driver);
      connect(driver, &Driver::deviceListChanged, this, &Framework::deviceListChanged);
    }
    else
      delete driver;
  }
  return ret;
}

QList<DeviceDescription> Framework::devices()
{
  if(!m_initialized)
    initialize();
  QList<DeviceDescription> ret;
  for(auto it=m_drivers.constBegin(); it!=m_drivers.constEnd(); it++)
    ret.append(it.value()->devices());
  return ret;
}

void Framework::initialize()
{
  m_initialized=true;
  for(auto it=m_drivers.begin(); it!=m_drivers.end();)
  {
    if(it.value()->initialize())
    {
      qDebug()<<"Driver"<<it.value()->driverId()<<"init ok";
      it++;
    }
    else
      m_drivers.erase(it);
  }
}

QSharedPointer<Port> Framework::openPort(const QString &id)
{
  if(!m_initialized)
    initialize();
  QSharedPointer<Port> ret;
  QStringList l=id.split("//");
  if(l.size()==3)
  {
    auto driver=unescapeString(l[0]), device=unescapeString(l[1]), port=unescapeString(l[2]);
    if(m_drivers.contains(driver))
      ret=m_drivers[driver]->openPort(device, port);
  }
  return ret;
}

Framework::Framework()
{
  m_initialized=false;
}

QSharedPointer<Port> HFMidi::openPort(const QString &id)
{
  return Framework::framework()->openPort(id);
}


/*
 * Copyright 2021 Marzocchi Alessandro
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#pragma once

#include <QObject>
#include <QMap>
#include <QSharedPointer>
#include "port.h"
namespace HFMidi
{
  class Driver;
  /**
   * @brief The Device class rapresent a MIDI device
   */
  class Device : public QObject
  {
    Q_OBJECT
  public:
    explicit Device(QObject *parent = nullptr);
    /**
     * @brief openPort Opens a port on this device given its id
     * On most cases this function should be reimplemented only in case a port can be referenced with two different IDs.
     * If id for a port is unique and there is a device open with given id a reference to it will be returned.
     * Otherwise the default implementation will call createPort to open the port.
     * @param device Unescaped id of the device
     * @return Opened device or NULL on fault
     */
    virtual QSharedPointer<Port> openPort(const QString &id);
  protected:
    /**
     * @brief createDevice Creates the handler for a device handled by this driver
     * @param device Device id
     * @return Pointer to the create device on success, null on fault
     */
    virtual QSharedPointer<Port> createPort(const QString &id);
  protected:
    void addPortToCache(const QString &portId, QWeakPointer<Port> portPtr);
    QMap<QString, QWeakPointer<Port> > m_ports;
  };
}

/*
 * Copyright 2021 Marzocchi Alessandro
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#pragma once

#include <QObject>
#include "device.h"
#include "port.h"
#include "devicedescription.h"
#include <QSharedPointer>
namespace HFMidi
{
  class DeviceDescription;
  class Framework;
  /**
   * @brief The Driver class gives the framework access to a set of MIDI devices.
   * It provides enumeration of devices available for the driver and the opening of one of the device.
   * To create a new driver one should implement driverId and devices function, plus createDevice.
   */
  class Driver: public QObject
  {
    Q_OBJECT
    friend class Framework;
  public:
    /**
     * @brief addDriverToFramework Adds a driver to the framework
     * This is a convenience function equivalent to calling Framework::framework()->addDriver(driver)
     * \see Framework::framework()
     * @param driver The driver to add to the framework
     * @return True on success
     */
    static bool addDriverToFramework(Driver *driver);
    Driver();
    /**
     * @brief driverId Returns the id for this driver
     * @return Unescaped driver id string
     */
    virtual QString driverId()=0;
    /**
     * @brief devices Enumerates the devices handles for this driver
     * @return A list of descriptors for each device
     */
    virtual QList<DeviceDescription> devices()=0;
    /**
     * @brief openPort Opens a device on this driver given its id
     * On most cases this function should be reimplemented only in case a device can be referenced with two different IDs.
     * If id for a device is unique and there is a device open with given id a reference to it will be returned.
     * Otherwise the deault implementation will call createDevice to open the device.
     * @param device Unescaped id of the device
     * @return Opened device or NULL on fault
     */
    virtual QSharedPointer<Device> openDevice(const QString &device);

    /**
     * @brief openPort Opens a port on this device given its device and port id
     * @param device Device id to open
     * @param port Port id to open
     * @return Port id on success, nulll on fault
     */
    QSharedPointer<Port> openPort(const QString &device, const QString &port);
  signals:
    /**
     * @brief deviceListChanged Signals that there was a change on devices/ports handled by the driver
     */
    void deviceListChanged();
  protected:
    QMap<QString, QWeakPointer<Device> > m_devices;
    void addDeviceToCache(const QString &deviceId, QWeakPointer<Device> devicePtr);
    /**
     * @brief createDevice Creates the handler for a device handled by this driver
     * @param device Device id
     * @return Pointer to the create device on success, null on fault
     */
    virtual QSharedPointer<Device> createDevice(const QString &device);
    /**
     * @brief initialize Initializes the driver
     * This function will be called once by the framework before accessing the functions of the driver.
     * @return True on success
     */
    virtual bool initialize();
  };
}

/*
 * Copyright 2021 Marzocchi Alessandro
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#pragma once

#include "devicedescription.h"
#include <QObject>
#include <QMap>
namespace HFMidi
{
  class Driver;
  class Port;
  /**
   * @brief The Framework class is the public interface for accessing MIDI devices
   * The ID to a port is a string of type "{Driver}/{Device}/{Port}". All 3 parts are escaped with function available on this class
   */
  class Framework: public QObject
  {
    Q_OBJECT
  public:
    /**
     * @brief framework Returns the singleton returning the program HFMidi framework
     * @return a pointer to the framework
     */
    static inline Framework *framework() { if(!m_framework)m_framework=new Framework; return m_framework; }
    /**
     * @brief addDriver Adds a driver to the framework
     * @param driver A pointer to the driver to be added. Note: on function success the ownership of the driver is taken by the framework.
     * @return True on success
     */
    bool addDriver(Driver *driver);
    /**
     * @brief devices Enumerates the devices available for the program
     * @return A list of descriptor containing device data
     */
    QList<DeviceDescription> devices();
    /**
     * @brief initialize Initializes the driver.
     * Note: the function is called automatically on first call of a function that accesses any driver (e.g. devices())
     */
    void initialize();
    /**
     * @brief openPort Opens a MIDI port given its full ID.
     * @param id Is an ID returned by PortDescription::fullId
     * @return A shared pointer to the port if successfull, NULL otherwise
     */
    QSharedPointer<HFMidi::Port> openPort(const QString &id);

    /**
     * @brief escapeString Escapes a string of an ID fragment
     * @param str The string to escape (e.g. "Foo/device")
     * @return The escaped string (e.g. "Foo\/device")
     */
    static QString escapeString(const QString &str) {return str; }
    /**
     * @brief escapeString Unescapes a string of an ID fragment
     * @param str The string to escape (e.g. "Foo\/device")
     * @return The escaped string (e.g. "Foo/device")
     */
    static QString unescapeString(const QString &str) {return str; }
  signals:
    /**
     * @brief deviceListChanged This function signals that a change occoured in Device/ports availability.
     */
    void deviceListChanged();
  protected:
    QMap<QString, Driver *> m_drivers;
    static Framework *m_framework;
    bool m_initialized;
    Framework();
  };
  /**
   * @brief Equivalent to \sa Framework::framework
   */
  inline Framework *framework() {return Framework::framework();}
}

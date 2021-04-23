/*
 * Copyright 2021 Marzocchi Alessandro
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#pragma once

#include "../driver.h"
#include "../device.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>
#include <QAbstractNativeEventFilter>
namespace HFMidi
{
  /**
   * @brief The DriverWin class is a driver for winmm
   */
  class DriverWin: public Driver
  {
    struct DeviceId
    {
      QString manufacturer;
      QString product;
      QString name;
      QString toString();
      bool operator ==(const DeviceId &other){ return manufacturer==other.manufacturer && product==other.product && name==other.name; }
    };

    Q_OBJECT
  public:
    QString driverId() override;
    QList<DeviceDescription> devices() override;
  protected:
    static bool getDeviceId(const QString &id, DeviceId &data);
    static DeviceId getDeviceId(const MIDIINCAPS2W &devCaps);
    static DeviceId getDeviceId(const MIDIOUTCAPS2W &devCaps);
    virtual QSharedPointer<Device> createDevice(const QString &device) override;
    QSharedPointer<Device> createInputDevice(const QString &device);
    QSharedPointer<Device> createOutputDevice(const QString &device);
    bool initialize() override;
  };
  /**
   * @brief The WinEventDeviceChangeFilter class Is a filter on events to check for WM_DEVICECHANGE events that could indicate that MIDI device available are changed.
   */
  class WinEventDeviceChangeFilter: public QAbstractNativeEventFilter
  {
  public:
    WinEventDeviceChangeFilter(DriverWin *driver);
    bool nativeEventFilter(const QByteArray &eventType, void *message, long *result) override;
  protected:
    DriverWin *m_driver;
  };
  class WinPortIn;
  /**
   * @brief The WinDeviceIn class implements an interface to an input device
   */
  class WinDeviceIn: public Device
  {
    friend class WinPortIn;
  public:
    WinDeviceIn(int id);
    ~WinDeviceIn();
    QWeakPointer<WinDeviceIn> thisRef() const;
    void setThisRef(const QWeakPointer<WinDeviceIn> &thisRef);
    inline bool isValid() const { return m_valid; }
  protected:
    static void CALLBACK midiInCallback(HMIDIIN hMidiIn, UINT wMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2);

    bool m_valid;
    HMIDIIN m_handle;
    QWeakPointer<WinDeviceIn> m_thisRef;
    QList<MIDIHDR *> m_headers;
    WinPortIn *m_portIn;
    QSharedPointer<Port> createPort(const QString &id) override;
  };
  class WinPortOut;
  /**
   * @brief The WinDeviceOut class implements an interface to an output device
   */
  class WinDeviceOut: public Device
  {
    friend class WinPortOut;
  public:
    WinDeviceOut(int id);
    ~WinDeviceOut();
    QWeakPointer<WinDeviceOut> thisRef() const;
    void setThisRef(const QWeakPointer<WinDeviceOut> &thisRef);
    inline bool isValid() const { return m_valid; }

  protected:
    static void CALLBACK midiOutCallback(HMIDIOUT hMidiOut, UINT wMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2);

    bool m_valid;
    HMIDIOUT m_handle;
    WinPortOut *m_portOut;
    QWeakPointer<WinDeviceOut> m_thisRef;

    QSharedPointer<Port> createPort(const QString &id) override;
  };
  /**
   * @brief The WinPortIn class handles a MIDI input port on Windows.
   * It holds a static reference to the device it uses so it will not be destroyed until this port exists.
   */
  class WinPortIn: public Port
  {
    friend class WinDeviceIn;
  public:
    WinPortIn(QSharedPointer<WinDeviceIn> device);
    ~WinPortIn();
  protected:
    QSharedPointer<WinDeviceIn> m_device;
  };
  /**
   * @brief The WinPortIn class handles a MIDI output port on Windows.
   * It holds a static reference to the device it uses so it will not be destroyed until this port exists.
   */
  class WinPortOut: public Port
  {
    friend class WinDeviceOut;
  public:
    WinPortOut(QSharedPointer<WinDeviceOut> device);
    ~WinPortOut();
    bool sendData(const QByteArray &data) override;
  protected:
    QSharedPointer<WinDeviceOut> m_device;
  };
}

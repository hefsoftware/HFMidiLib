#pragma once

#include "../driver.h"
#include "../device.h"
#include <QThread>
#include <QSet>
#include <alsa/asoundlib.h>
#include <alsa/seq.h>

namespace Midi
{
  class AlsaDevice;
  /**
   * @brief The DriverAlsa class is a driver implementation for connecting to ALSA system
   */
  class DriverAlsa: public Driver
  {
    friend class AlsaDevice;
    Q_OBJECT
    typedef QPair<int,int> DevicePortId;
  public:
    DriverAlsa();
    QString driverId() override;
    QList<DeviceDescription> devices() override;
    bool getOpenId(const QString &device, const QString &port, int &deviceId, int &portId);
    static inline DriverAlsa *instance()
    {
      if(!m_instance)
        m_instance=new DriverAlsa;
      return m_instance;
    }

  protected slots:
    void onPortAdded(int device, int port);
    void onPortRemoved(int device, int port);
    void onPortChanged(int device, int port);

  protected:
    static bool splitId(const QString &fullId, int &i, QString &name);
    QSharedPointer<Device> createDevice(const QString &device) override;
    QSharedPointer<AlsaDevice> createDeviceAlsa(const QString &device);
    bool initialize() override;
    PortDescription getAnnouncePortId();

    static DriverAlsa *m_instance;
    snd_seq_t* m_handle;
    QSharedPointer<Port> m_announcePort;
    QSet<DevicePortId> m_interestingPorts;
    bool m_interestingPortInitialized;
  };
  class AlsaPort;
  /**
   * @brief The AlsaDevice class handles a device in ALSA
   */
  class AlsaDevice: public Device
  {
    friend class AlsaPort;
    friend class DriverAlsa;
  public:
    ~AlsaDevice();
    static QSharedPointer<AlsaDevice> newDevice(const QString &id);
  protected:
    AlsaDevice(const QString &id);
    QWeakPointer<AlsaDevice> m_thisRef;
    QString m_id;
    QSharedPointer<Port> createPort(const QString &id) override;
    QSharedPointer<AlsaPort> createPortAlsa(const QString &id, snd_seq_t *handle=nullptr);
  };
  class AlsaPort;
  /**
   * @brief The AlsaThread class implements a thread to connect to a MIDI input port in ALSA
   */
  class AlsaThread: public QThread
  {
    friend class AlsaPort;
    friend class AlsaDriver;
    Q_OBJECT
  protected:
    AlsaThread(snd_seq_t *handle, int portId, int pipeRead);
    void run();
    snd_seq_t *m_handle;
    int m_portId;
    int m_pipeRead;
  signals:
    void portAdded(int device, int port);
    void portRemoved(int device, int port);
    void portChanged(int device, int port);
  signals:
    void midiEvent(QByteArray data);
  };
  /**
   * @brief The WinPortIn class handles a MIDI input or output port in ALSA.
   * It holds a static reference to the device it uses so it will not be destroyed until this port exists.
   */

  class AlsaPort: public Port
  {
    Q_OBJECT
    friend class DriverAlsa;
  public:
    AlsaPort(const QSharedPointer<AlsaDevice> &device, snd_seq_t *handle, bool ownsHandle, int portId, bool isInput);
    ~AlsaPort();
    bool sendData(const QByteArray &data) override;
  protected:
    AlsaThread *handleThread(){return m_thread;}
    QSharedPointer<AlsaDevice> m_device;
    snd_midi_event_t *m_midiCoder;
    int m_pipeWrite;
    snd_seq_t *m_handle;
    bool m_ownsHandle;
    int m_id;
    bool m_isInput;
    AlsaThread *m_thread;
  };
}

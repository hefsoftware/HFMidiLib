/*
 * Copyright 2021 Marzocchi Alessandro
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#pragma once

#include <QStringList>
#include <QVariant>
namespace HFMidi
{

  class PortDescription
  {
  public:
    PortDescription(const QString &driver=QString(), const QString &device=QString());
    QString fullId();
    QString driverId() const;
    void setDriverId(const QString &driver);
    QString deviceId() const;
    void setDeviceId(const QString &device);
    QString portId() const;
    void setPortId(const QString &id);
    QString description() const;
    void setDescription(const QString &description);
    QMap<QString, QVariant> extra() const;
    void setExtra(const QMap<QString, QVariant> &extra);
    void addExtra(const QString &id, const QVariant &value);

    bool isInput() const;
    void setInput(bool input);
    bool isOutput() const;
    void setOutput(bool input);
    inline bool isValid() const {return !m_driverId.isEmpty() && !m_deviceId.isEmpty() && !m_portId.isEmpty();}
  protected:
    QString m_driverId;
    QString m_deviceId;
    QString m_portId;
    bool m_input;
    bool m_output;
    QString m_description;
//    QStringList m_inputPorts;
//    QStringList m_outputPorts;
    QMap<QString, QVariant> m_extra;
  };
  class DeviceDescription
  {
  public:
    DeviceDescription(const QString &driver=QString());
    QString driverId() const;
    void setDriverId(const QString &driver);
    QString deviceId() const;
    QString fullId();
    QString portId(const QString &port);
    void setDeviceId(const QString &id);
    QString description() const;
    void setDescription(const QString &description);
    int numInputPorts() const;
    int numOutputPorts() const;
    int numPorts() const;
    void addPort(const PortDescription &port);
    QList<PortDescription> inputPorts();
    QList<PortDescription> outputPorts();
    QList<PortDescription> ports();
    const QList<PortDescription> &portsRef() const;
    void addExtra(const QString &id, const QVariant &value);

    const QMap<QString, QVariant> &extra() const;

  protected:
    QString m_driver;
    QString m_id;
    QString m_description;
    QList<PortDescription> m_ports;
//    QStringList m_inputPorts;
//    QStringList m_outputPorts;
    QMap<QString, QVariant> m_extra;
  };
}

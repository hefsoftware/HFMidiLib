/*
 * Copyright 2021 Marzocchi Alessandro
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "mididevicestree.h"
#include "HFMidiLib/framework.h"
#include <QMimeData>
#include <QDebug>
MidiDevicesTree::MidiDevicesTree(QWidget *parent) : QTreeWidget(parent)
{
  updateDevices();
  connect(HFMidi::framework(), &HFMidi::Framework::deviceListChanged, this, &MidiDevicesTree::updateDevices);
}

void MidiDevicesTree::updateDevices()
{
  clear();
  auto devices=HFMidi::Framework::framework()->devices();
  QMultiMap<QString, HFMidi::DeviceDescription> devs;

  Q_FOREACH(auto d, devices)
    devs.insert(d.description(), d);
  for(auto it=devs.constBegin(); it!=devs.constEnd(); it++)
  {
    auto dev=it.value();
    QTreeWidgetItem *ti=new QTreeWidgetItem(QStringList({it.key()}));
    QString toolTip;
    toolTip=extraToHtml(dev.extra(), "Device", dev.deviceId());
    bool canDrag=false;
    if(!dev.numPorts())
      continue;
    if(dev.numPorts()==1)
    {
      canDrag=true;
      if(dev.numInputPorts())
      {
        auto p=dev.portsRef().first();
        ti->setText(0, QString("%1/%2").arg(ti->text(0), p.description()));
        ti->setData(0, Qt::UserRole, QString("in")+p.fullId());
        ti->setIcon(0, QIcon(":/input.png"));
        toolTip=extraToHtml(dev.portsRef().first().extra(), "Port", p.portId(), toolTip);
      }
      else if(dev.numOutputPorts())
      {
        auto p=dev.portsRef().first();
        ti->setText(0, QString("%1/%2").arg(ti->text(0), p.description()));
        ti->setData(0, Qt::UserRole, QString("out")+p.fullId());
        ti->setIcon(0, QIcon(":/output.png"));
        toolTip=extraToHtml(dev.portsRef().first().extra(), "Port", p.portId(), toolTip);
      }
    }
    else
    {
      Q_FOREACH(auto p, dev.portsRef())
      {
        if(p.isInput())
        {
          QTreeWidgetItem *ti2=new QTreeWidgetItem(ti, QStringList(p.description()));
          ti2->setData(0, Qt::UserRole, QString("in")+p.fullId());
          ti2->setToolTip(0, extraToHtml(dev.portsRef().first().extra(), "Port", p.portId()));
          ti2->setIcon(0, QIcon(":/input.png"));
        }
        else if(p.isOutput())
        {
          QTreeWidgetItem *ti2=new QTreeWidgetItem(ti, QStringList(p.description()));
          ti2->setData(0, Qt::UserRole, QString("out")+p.fullId());
          ti2->setToolTip(0, extraToHtml(dev.portsRef().first().extra(), "Port", p.portId()));
          ti2->setIcon(0, QIcon(":/output.png"));
        }
      }
    }
    if(!toolTip.isEmpty())
      ti->setToolTip(0, QString("<table>%1</table>").arg(toolTip));
    if(canDrag)
      ti->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled);
    else
      ti->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    addTopLevelItem(ti);
    ti->setExpanded(true);
  }
}

QString MidiDevicesTree::extraToHtml(const QMap<QString, QVariant> &data, const QString &header, const QString &headerValue, const QString &curTooltip)
{
  QString toolTip(curTooltip);
  if(!data.isEmpty())
  {
    if(!toolTip.isEmpty())
      toolTip+=tr("<tr><td>&nbsp;</td><td>&nbsp;</td></tr>");
    toolTip+=tr("<tr><td><b><i>%1</i></b></td><td><b>%2</b></td></tr>").arg(header.toHtmlEscaped(), headerValue.toHtmlEscaped());
    for(auto it=data.constBegin(); it!=data.constEnd(); it++)
      toolTip+=tr("<tr><td><i>%1</i></td><td>%2</td></tr>").arg(it.key().toHtmlEscaped(), it.value().toString().toHtmlEscaped());
  }
  return toolTip;
}


QMimeData *MidiDevicesTree::mimeData(const QList<QTreeWidgetItem *> items) const
{
  QMimeData *ret=nullptr;
  if(items.size()==1)
  {
    auto data=items.first()->data(0, Qt::UserRole).toString();
    if(!data.isEmpty())
    {
      if(data.startsWith("in"))
      {
        ret=new QMimeData;
        ret->setData("mididev/input",data.midRef(2).toUtf8());
      }
      else if(data.startsWith("out"))
      {
        ret=new QMimeData;
        ret->setData("mididev/output",data.midRef(3).toUtf8());
      }
    }
  }
  return ret;
}

/*
 * Copyright 2021 Marzocchi Alessandro
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "midiwin.h"
#include <dbt.h>
#include <QSet>
#include <QApplication>
#include <QUuid>
#include <QDebug>
using namespace HFMidi;

static bool initDriverOk=Driver::addDriverToFramework(new DriverWin);
QString DriverWin::driverId()
{
  return "win";
}

QList<DeviceDescription> DriverWin::devices()
{
  QList<DeviceDescription> ret;
  int numDevs;
  numDevs = midiOutGetNumDevs();
  for (int i = 0; i < numDevs; i++) {
     MIDIOUTCAPS2W devCaps;
     memset(&devCaps, 0, sizeof(devCaps));
     if(!midiOutGetDevCapsW(i, (MIDIOUTCAPSW *)&devCaps, sizeof(devCaps)))
     {
       auto curId=getDeviceId(devCaps);
       DeviceDescription desc(driverId());
       desc.setDeviceId("out"+curId.toString());
       desc.setDescription(QString::fromWCharArray(devCaps.szPname));
       desc.addExtra("MID", QString::number(devCaps.wMid,16));
       desc.addExtra("PID", QString::number(devCaps.wPid,16));
       desc.addExtra("Manufacturer", curId.manufacturer);
       desc.addExtra("Product", curId.product);
       desc.addExtra("Name", curId.name);
       desc.addExtra("Technology", devCaps.wTechnology);
       desc.addExtra("Voicers", devCaps.wVoices);
       desc.addExtra("Notes", devCaps.wNotes);
       desc.addExtra("Channel mask", devCaps.wChannelMask);
       desc.addExtra("id",curId.toString());
       PortDescription port(driverId(), desc.deviceId());
       port.setPortId("1");
       port.setDescription("Port");
       port.setOutput(true);
       desc.addPort(port);
       ret.append(desc);
     }
  }

  numDevs = midiInGetNumDevs();
  for (int i = 0; i < numDevs; i++) {
     MIDIINCAPS2W devCaps;
     memset(&devCaps, 0, sizeof(devCaps));
     if(!midiInGetDevCapsW(i, (MIDIINCAPSW *)&devCaps, sizeof(devCaps)))
     {
       auto curId=getDeviceId(devCaps);
       DeviceDescription desc(driverId());
       desc.setDeviceId("in"+curId.toString());
       desc.setDescription(QString::fromWCharArray(devCaps.szPname));
       desc.addExtra("MID", QString::number(devCaps.wMid,16));
       desc.addExtra("PID", QString::number(devCaps.wPid,16));
       desc.addExtra("Manufacturer", curId.manufacturer);
       desc.addExtra("Product", curId.product);
       desc.addExtra("Name", curId.name);
       desc.addExtra("id",curId.toString());
       PortDescription port(driverId(), desc.deviceId());
       port.setPortId("1");
       port.setDescription("Port");
       port.setInput(true);
       desc.addPort(port);
       ret.append(desc);
     }
  }
  return ret;
}

QString DriverWin::DeviceId::toString()
{
  return QString("%1:%2:%3").arg(manufacturer, product, name);
}

bool DriverWin::getDeviceId(const QString &id, DriverWin::DeviceId &data)
{
  auto idData=id.split(":");
  bool ret=false;
  if(idData.size()==3)
  {
    ret=true;
    data.manufacturer=idData[0];
    data.product=idData[1];
    data.name=idData[2];
  }
  return ret;
}

DriverWin::DeviceId DriverWin::getDeviceId(const MIDIINCAPS2W &devCaps)
{
  DeviceId ret;
  auto manGuid=QUuid(devCaps.ManufacturerGuid);
  auto prodGuid=QUuid(devCaps.ProductGuid);
  auto nameGuid=QUuid(devCaps.NameGuid);
  ret.manufacturer=manGuid.isNull()?QString::number(devCaps.wMid,16):manGuid.toString();
  ret.product=prodGuid.isNull()?QString::number(devCaps.wPid,16):manGuid.toString();
  ret.name=nameGuid.isNull()?QString::fromWCharArray(devCaps.szPname):nameGuid.toString();
  return ret;
}

DriverWin::DeviceId DriverWin::getDeviceId(const MIDIOUTCAPS2W &devCaps)
{
  DeviceId ret;
  auto manGuid=QUuid(devCaps.ManufacturerGuid);
  auto prodGuid=QUuid(devCaps.ProductGuid);
  auto nameGuid=QUuid(devCaps.NameGuid);
  ret.manufacturer=manGuid.isNull()?QString::number(devCaps.wMid,16):manGuid.toString();
  ret.product=prodGuid.isNull()?QString::number(devCaps.wPid,16):manGuid.toString();
  ret.name=nameGuid.isNull()?QString::fromWCharArray(devCaps.szPname):nameGuid.toString();
  return ret;
}

QSharedPointer<Device> DriverWin::createDevice(const QString &device)
{
  QSharedPointer<Device> ret;
  if(device.startsWith("in"))
    ret=createInputDevice(device.mid(2));
  else if(device.startsWith("out"))
    ret=createOutputDevice(device.mid(3));
  return ret;
}

QSharedPointer<Device> DriverWin::createInputDevice(const QString &device)
{
  QSharedPointer<WinDeviceIn> ret=nullptr;
  DeviceId id;
  if(getDeviceId(device, id))
  {
    int numDevs = midiInGetNumDevs();
    for (int i = 0; i < numDevs; i++) {
       MIDIINCAPS2W devCaps;
       memset(&devCaps, 0, sizeof(devCaps));
       if(!midiInGetDevCapsW(i, (MIDIINCAPSW *)&devCaps, sizeof(devCaps)) && id==getDeviceId(devCaps))
       {
         WinDeviceIn *device=new WinDeviceIn(i);
         if(!device->isValid())
         {
           delete device;
           device=nullptr;
         }
         else
         {
           ret=QSharedPointer<WinDeviceIn>(device);
           ret->setThisRef(ret);
           break;
         }
       }
     }
  }
  return ret;
}

QSharedPointer<Device> DriverWin::createOutputDevice(const QString &device)
{
  QSharedPointer<WinDeviceOut> ret=nullptr;
  DeviceId id;
  if(getDeviceId(device, id))
  {
    int numDevs = midiOutGetNumDevs();
    for (int i = 0; i < numDevs; i++) {
       MIDIOUTCAPS2W devCaps;
       memset(&devCaps, 0, sizeof(devCaps));
       if(!midiOutGetDevCapsW(i, (MIDIOUTCAPSW *)&devCaps, sizeof(devCaps)) && id==getDeviceId(devCaps))
       {
         auto curId=getDeviceId(devCaps);
         WinDeviceOut *device=new WinDeviceOut(i);
         if(!device->isValid())
         {
           delete device;
           device=nullptr;
         }
         else
         {
           ret=QSharedPointer<WinDeviceOut>(device);
           ret->setThisRef(ret);
           break;
         }
       }
     }
  }
  return ret;

}


WinEventDeviceChangeFilter::WinEventDeviceChangeFilter(DriverWin *driver): m_driver(driver)
{

}

bool HFMidi::WinEventDeviceChangeFilter::nativeEventFilter(const QByteArray &eventType, void *message, long *result)
{
  Q_UNUSED(eventType);
  Q_UNUSED(result);
  static QSet<UINT> alreadyReceived;
  MSG* msg = static_cast<MSG *>(message);
  if(msg->message==WM_DEVICECHANGE)
  {
    switch(msg->wParam)
    {
    case DBT_DEVNODES_CHANGED:
      emit m_driver->deviceListChanged();
      break;
    }
  }
  return false;
}



bool HFMidi::DriverWin::initialize()
{
  QApplication::instance()->installNativeEventFilter(new WinEventDeviceChangeFilter(this));
  return true;
}

void CALLBACK WinDeviceIn::midiInCallback(HMIDIIN hMidiIn, UINT wMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2)
{
  WinDeviceIn* device= reinterpret_cast<WinDeviceIn*>(dwInstance);

  switch (wMsg)
  {
  case MIM_DATA:
  case MIM_MOREDATA:
      //  dwParam1 is the midi event with status in the low byte of the low word
      //  dwParam2 is the event time in ms since the start of midi in
      // data: LOBYTE(dwParam1), HIBYTE(dwParam1), LOBYTE(HIWORD(dwParam1))
      {
        BYTE data[3]={LOBYTE(dwParam1), HIBYTE(dwParam1), LOBYTE(HIWORD(dwParam1))};
        if(device->m_portIn)
          device->m_portIn->receivedData(QByteArray((char *)data, 3));
      }
      break;
  case MIM_CLOSE:
  case MIM_OPEN:
  case MIM_ERROR:
      break;
  case MIM_LONGDATA:
    if(device->m_valid)
    {
      MIDIHDR *hdr = (LPMIDIHDR) dwParam1;
      qDebug()<<"Long data"<<hdr->dwBytesRecorded;
      if(device->m_portIn)
        device->m_portIn->receivedData(QByteArray(hdr->lpData, hdr->dwBytesRecorded));
      midiInAddBuffer(hMidiIn, hdr, sizeof(*hdr));
    }
    break;
  case MIM_LONGERROR:
    if(device->m_valid)
    {
      MIDIHDR *hdr = (LPMIDIHDR) dwParam1;
      midiInAddBuffer(hMidiIn, hdr, sizeof(*hdr));
    }
    break;
  default:
    qDebug()<<"Unhandled message"<<wMsg;
  }
}
WinDeviceIn::WinDeviceIn(int id): m_valid(false), m_portIn(nullptr)
{
  m_handle=nullptr;
  if(MMSYSERR_NOERROR==midiInOpen(&m_handle,
                 id,
                 reinterpret_cast<DWORD_PTR>(&midiInCallback),
                 reinterpret_cast<DWORD_PTR>(this),
                 CALLBACK_FUNCTION | MIDI_IO_STATUS)
     )
  {
    m_valid=true;
    for(int i=0;i<10;i++)
    {
      MIDIHDR *hdr=new MIDIHDR;
      memset(hdr, 0, sizeof(*hdr));
      hdr->lpData=new char[512];
      hdr->dwBufferLength=512;
      if(MMSYSERR_NOERROR==midiInPrepareHeader(m_handle, hdr, sizeof(*hdr)) && MMSYSERR_NOERROR==midiInAddBuffer(m_handle, hdr, sizeof(*hdr)))
        m_headers.append(hdr);
      else
        m_valid=false;
    }
    if(m_valid)
      m_valid=(MMSYSERR_NOERROR==midiInStart(m_handle));
  }
}

WinDeviceIn::~WinDeviceIn()
{
  m_valid=false; // This is important, in this way midi headers will not be added back to the queue by callback
  if(midiInReset(m_handle)==MMSYSERR_NOERROR && midiInStop(m_handle)==MMSYSERR_NOERROR )
  {
    while(!m_headers.isEmpty())
    {
      auto cur=m_headers.takeFirst();
      midiInUnprepareHeader(m_handle, cur, sizeof(*cur));
      delete cur->lpData;
      delete cur;
    }
    if(m_handle)
      midiInClose(m_handle);
  }
}

QWeakPointer<WinDeviceIn> WinDeviceIn::thisRef() const
{
  return m_thisRef;
}

void WinDeviceIn::setThisRef(const QWeakPointer<WinDeviceIn> &thisRef)
{
  m_thisRef = thisRef;
}

void CALLBACK WinDeviceOut::midiOutCallback(HMIDIOUT hMidiOut, UINT wMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2)
{
  Q_UNUSED(dwInstance);
  Q_UNUSED(dwParam2);
  switch (wMsg)
  {
  case MOM_DONE:
    {
    auto header=reinterpret_cast<MIDIHDR *>(dwParam1);
    if(MMSYSERR_NOERROR==midiOutUnprepareHeader(hMidiOut, header, sizeof(*header)))
    {
      delete header->lpData;
      delete header;
    }
    }
    break;
  }
}

WinDeviceOut::WinDeviceOut(int id): m_valid(false)
{
  m_handle=nullptr;

  if(MMSYSERR_NOERROR==midiOutOpen(&m_handle,
                                   id,
                                   reinterpret_cast<DWORD_PTR>(&midiOutCallback),
                                   reinterpret_cast<DWORD_PTR>(this),
                                   CALLBACK_FUNCTION))
  {
    m_valid=true;
    qDebug()<<"Opened MIDI output device";
  }
}

WinDeviceOut::~WinDeviceOut()
{
  if(m_valid && midiOutReset(m_handle)==MMSYSERR_NOERROR && midiOutClose(m_handle)==MMSYSERR_NOERROR)
  {
    qDebug()<<"Closed MIDI device";
  }
}

QWeakPointer<WinDeviceOut> WinDeviceOut::thisRef() const
{
  return m_thisRef;
}

void WinDeviceOut::setThisRef(const QWeakPointer<WinDeviceOut> &thisRef)
{
  m_thisRef = thisRef;
}

QSharedPointer<Port> WinDeviceOut::createPort(const QString &id)
{
  QSharedPointer<Port> ret;
  if(id=="1")
  {
    ret=QSharedPointer<Port>(m_portOut=new WinPortOut(m_thisRef.toStrongRef()));
  }
  return ret;
}


QSharedPointer<Port> HFMidi::WinDeviceIn::createPort(const QString &id)
{
  QSharedPointer<Port> ret;
  if(id=="1")
  {
    ret=QSharedPointer<Port>(m_portIn=new WinPortIn(m_thisRef.toStrongRef()));
  }
  return ret;
}

WinPortIn::WinPortIn(QSharedPointer<WinDeviceIn> device): m_device(device)
{
}

WinPortIn::~WinPortIn()
{
  m_device->m_portIn=nullptr;
}

WinPortOut::WinPortOut(QSharedPointer<WinDeviceOut> device): m_device(device)
{

}

WinPortOut::~WinPortOut()
{
  m_device->m_portOut=nullptr;
}


bool HFMidi::WinPortOut::sendData(const QByteArray &data)
{
  bool ret=false;
  switch(data.size())
  {
  case 0:
    break;
  case 1:
    ret=(MMSYSERR_NOERROR==midiOutShortMsg(m_device->m_handle, (uint8_t)data[0])); break;
  case 2:
    ret=(MMSYSERR_NOERROR==midiOutShortMsg(m_device->m_handle, (((uint8_t)data[1])<<8) | ((uint8_t)data[0]))); break;
  case 3:
    ret=(MMSYSERR_NOERROR==midiOutShortMsg(m_device->m_handle, (((uint8_t)data[2])<<16) | (((uint8_t)data[1])<<8) | ((uint8_t)data[0]))); break;
  default:
    {
      MIDIHDR *header;
      header=new MIDIHDR;
      memset(header, 0, sizeof(*header));

      auto len=data.length();
      header->lpData = new char[len];
      memcpy(header->lpData, data.data(), len);
      header->dwBufferLength = len;
      if(MMSYSERR_NOERROR==midiOutPrepareHeader(m_device->m_handle, header, sizeof(*header)) && MMSYSERR_NOERROR==midiOutLongMsg(m_device->m_handle, header, sizeof(*header)))
      {
        ret=true;
        qDebug()<<"Send long message"<<data.size()<<data.toHex()<<header<<len;
      }
      else
      {
        delete header->lpData;
        delete header;
      }
    }
    break;
  }
  return true;
}

#include "midilinux.h"
#include <QDebug>
#include <alsa/seq_midi_event.h>
#include <poll.h>
/**
aseqdump
0:1   Client start               client 136
0:1   Port start                 136:0
0:1   Port subscribed            0:1 -> 136:0
*/

using namespace Midi;
DriverAlsa *DriverAlsa::m_instance=nullptr;
static bool initDriverOk=Driver::addDriverToFramework(DriverAlsa::instance());

DriverAlsa::DriverAlsa(): m_handle(nullptr)
{
  m_interestingPortInitialized=false;
}

QString DriverAlsa::driverId()
{
  return "linux";
}
PortDescription DriverAlsa::getAnnouncePortId()
{
  PortDescription ret;
  if (m_handle) {
    snd_seq_client_info_t* cinfo;
    snd_seq_client_info_alloca(&cinfo);
    snd_seq_port_info_t* pinfo;
    snd_seq_port_info_alloca(&pinfo);
    if(snd_seq_get_any_client_info(m_handle, SND_SEQ_CLIENT_SYSTEM, cinfo)==0 && snd_seq_get_any_port_info(m_handle, SND_SEQ_CLIENT_SYSTEM, SND_SEQ_PORT_SYSTEM_ANNOUNCE, pinfo)==0)
    {
      qDebug()<<"Got client and port info ok";
      ret.setDriverId(driverId());
      ret.setDeviceId(QString("%2:%1").arg(snd_seq_client_info_get_name(cinfo)).arg(snd_seq_client_info_get_client(cinfo)));
      ret.setPortId(QString("in%2:%1").arg(snd_seq_port_info_get_name(pinfo)).arg(snd_seq_port_info_get_port(pinfo)));
    }
  }
  return ret;
}
QList<DeviceDescription> DriverAlsa::devices()
{
  QList<DeviceDescription> ret;
  if (m_handle) {
    int err;
    snd_seq_client_info_t* cinfo;
    snd_seq_client_info_alloca(&cinfo);
    if(cinfo)
    {
      err=snd_seq_get_any_client_info(m_handle, 0, cinfo);
      int i=0;
      while(err>=0)
      {
        i++;
        int clientId = snd_seq_client_info_get_client(cinfo);
        DeviceDescription device(driverId());
        device.setDeviceId(QString("%2:%1").arg(snd_seq_client_info_get_name(cinfo)).arg(clientId));
        device.setDescription(snd_seq_client_info_get_name(cinfo));
        device.addExtra("Name", snd_seq_client_info_get_name(cinfo));
        if(snd_seq_client_info_get_card(cinfo)>=0)
          device.addExtra("Card", snd_seq_client_info_get_card(cinfo));
        if(snd_seq_client_info_get_pid(cinfo)>=0)
          device.addExtra("Pid", snd_seq_client_info_get_pid(cinfo));
        device.addExtra("Type", snd_seq_client_info_get_type(cinfo));
        device.addExtra("Client", snd_seq_client_info_get_client(cinfo));
        QStringList inputs;
        QStringList outputs;
        snd_seq_port_info_t* pinfo;
        snd_seq_port_info_alloca(&pinfo);
        if(pinfo)
        {
          snd_seq_port_info_set_client(pinfo, clientId);
          snd_seq_port_info_set_port(pinfo, -1);
          while (snd_seq_query_next_port(m_handle, pinfo) >= 0) {
            PortDescription port(driverId(), device.deviceId());
            int cap=snd_seq_port_info_get_capability(pinfo);
            int portId=snd_seq_port_info_get_port(pinfo);
            QString id=QString("%2:%1").arg(snd_seq_port_info_get_name(pinfo)).arg(portId);
            port.setDescription(snd_seq_port_info_get_name(pinfo));
            if(snd_seq_port_info_get_type(pinfo))
              port.addExtra("Type", QString::number(snd_seq_port_info_get_type(pinfo), 16));
            port.addExtra("Capabilities", cap);
            if(snd_seq_port_info_get_midi_channels(pinfo))
              port.addExtra("Channels", snd_seq_port_info_get_midi_channels(pinfo));
            if(snd_seq_port_info_get_midi_voices(pinfo))
              port.addExtra("Voices", snd_seq_port_info_get_midi_voices(pinfo));
            if(snd_seq_port_info_get_synth_voices(pinfo))
              port.addExtra("Synth voices", snd_seq_port_info_get_synth_voices(pinfo));
            if((cap&SND_SEQ_PORT_CAP_SUBS_READ || cap&SND_SEQ_PORT_CAP_SUBS_WRITE)&&!m_interestingPortInitialized)
              m_interestingPorts.insert(DevicePortId(clientId, portId));
            if(cap&SND_SEQ_PORT_CAP_SUBS_READ)
            {
              port.setPortId("in"+id);
              port.setInput(true);
              device.addPort(port);
            }
            if(cap&SND_SEQ_PORT_CAP_SUBS_WRITE)
            {
              port.setPortId("out"+id);
              port.setInput(false);
              device.addPort(port);
            }
          }
        }
        ret.append(device);
        err=snd_seq_query_next_client(m_handle, cinfo);
      }
    }
  }
  m_interestingPortInitialized=true;
  return ret;
}

bool DriverAlsa::getOpenId(const QString &device, const QString &port, int &deviceId, int &portId)
{
  QString inDeviceName, inPortName;
  int inDeviceId, inPortId;
  deviceId=portId=-1;
  if(splitId(device, inDeviceId, inDeviceName) && splitId(port, inPortId, inPortName))
  {
    int bestDeviceScore=0;
    snd_seq_client_info_t* cinfo;
    snd_seq_client_info_alloca(&cinfo);
    if(cinfo)
    {
      int err=snd_seq_get_any_client_info(m_handle, 0, cinfo);
      for(;err>=0;)
      {
        int score=0;
        int curDevId=snd_seq_client_info_get_client(cinfo);
        score+=10*(inDeviceName==snd_seq_client_info_get_name(cinfo));
        score+=(inDeviceId==curDevId);
        if(score>bestDeviceScore)
        {
          deviceId=curDevId;
          bestDeviceScore=score;
        }
        err=snd_seq_query_next_client(m_handle, cinfo);
      }
      if(deviceId>=0)
      {
        snd_seq_port_info_t* pinfo;
        snd_seq_port_info_alloca(&pinfo);
        if(pinfo)
        {
          portId=-1;
          int bestPortScore=0;
          snd_seq_port_info_set_client(pinfo, deviceId);
          snd_seq_port_info_set_port(pinfo, -1);
          while (snd_seq_query_next_port(m_handle, pinfo) >= 0) {
            int curPortId=snd_seq_port_info_get_port(pinfo);
            int score=0;
            score+=10*(inPortName==snd_seq_port_info_get_name(pinfo));
            score+=(inPortId==curPortId);
            if(score>bestPortScore)
            {
              portId=curPortId;
              bestPortScore=score;
            }
          }
        }
      }
    }
  }
  return (deviceId>=0) && (portId>=0);
}

void DriverAlsa::onPortAdded(int device, int port)
{
  if(m_interestingPortInitialized)
  {
    snd_seq_port_info_t* pinfo;
    snd_seq_port_info_alloca(&pinfo);
    if(snd_seq_get_any_port_info(m_handle, device, port, pinfo)==0)
    {
      auto cap=snd_seq_port_info_get_capability(pinfo);
      bool isInteresting=(cap&SND_SEQ_PORT_CAP_SUBS_READ) || (cap&SND_SEQ_PORT_CAP_SUBS_WRITE);
      DevicePortId id(device, port);
      if(isInteresting && !m_interestingPorts.contains(id))
      {
        m_interestingPorts.insert(id);
        emit deviceListChanged();
      }
    }
  }
}

void DriverAlsa::onPortRemoved(int device, int port)
{
  DevicePortId id(device, port);
  if(m_interestingPortInitialized && m_interestingPorts.contains(id))
  {
    m_interestingPorts.remove(id);
    emit deviceListChanged();
  }
}

void DriverAlsa::onPortChanged(int device, int port)
{
  if(m_interestingPortInitialized)
  {
    snd_seq_port_info_t* pinfo;
    snd_seq_port_info_alloca(&pinfo);
    if(snd_seq_get_any_port_info(m_handle, device, port, pinfo)==0)
    {
      auto cap=snd_seq_port_info_get_capability(pinfo);
      bool isInteresting=(cap&SND_SEQ_PORT_CAP_SUBS_READ) || (cap&SND_SEQ_PORT_CAP_SUBS_WRITE);
      DevicePortId id(device, port);
      if(isInteresting && !m_interestingPorts.contains(id))
      {
        m_interestingPorts.insert(id);
        emit deviceListChanged();
      }
      else if(!isInteresting && m_interestingPorts.contains(id))
      {
        m_interestingPorts.remove(id);
        emit deviceListChanged();
      }
    }
  }
}



bool DriverAlsa::splitId(const QString &fullId, int &i, QString &name)
{
  bool ret=false;
  int j=fullId.indexOf(':');
  if(j>=1)
  {
    i=fullId.leftRef(j).toInt(&ret);
    name=fullId.mid(j+1);
  }
  return ret;
}

QSharedPointer<AlsaDevice> DriverAlsa::createDeviceAlsa(const QString &device)
{
  return QSharedPointer<AlsaDevice>(AlsaDevice::newDevice(device));
}
QSharedPointer<Device> DriverAlsa::createDevice(const QString &device)
{
  return createDeviceAlsa(device);
}

bool DriverAlsa::initialize()
{
  int err;
  err = snd_seq_open(&m_handle, "default", SND_SEQ_OPEN_DUPLEX, 0); //SND_SEQ_NONBLOCK);
  snd_seq_set_client_name(m_handle, "MyMidi");
  auto announcePort=getAnnouncePortId();
  if(announcePort.isValid())
  {
    qDebug()<<"Announce"<<announcePort.deviceId()<<announcePort.portId();
    auto systemDevice=createDeviceAlsa(announcePort.deviceId());
    if(systemDevice)
    {
      auto port=systemDevice->createPortAlsa(announcePort.portId(), m_handle);
      if(port)
      {
        qDebug()<<"Got announce port";
        auto thread=port->handleThread();
        if(thread)
        {
          qDebug()<<"Got thread";
          connect(thread, &AlsaThread::portAdded, this, &DriverAlsa::onPortAdded, Qt::QueuedConnection);
          connect(thread, &AlsaThread::portRemoved, this, &DriverAlsa::onPortRemoved, Qt::QueuedConnection);
          connect(thread, &AlsaThread::portChanged, this, &DriverAlsa::onPortChanged, Qt::QueuedConnection);
        }
        m_announcePort=port;
      }
    }
  }
  return (err>=0);
}

AlsaDevice::AlsaDevice(const QString &id): m_id(id)
{
  qDebug()<<"Creating alsa device"<<m_id;
}

QSharedPointer<Port> AlsaDevice::createPort(const QString &id)
{
  return createPortAlsa(id);
}

AlsaDevice::~AlsaDevice()
{
  qDebug()<<"Destroying alsa device"<<m_id;
}

QSharedPointer<AlsaDevice> AlsaDevice::newDevice(const QString &id)
{
  auto ret=QSharedPointer<AlsaDevice>(new AlsaDevice(id));
  ret->m_thisRef=ret.toWeakRef();
  return ret;
}

QSharedPointer<AlsaPort> AlsaDevice::createPortAlsa(const QString &id, snd_seq_t *handle)
{
  QSharedPointer<AlsaPort> ret;
  QString portName;
  bool isInput=false, isOutput=false;
  if(id.startsWith("in"))
  {
    portName=id.mid(2);
    isInput=true;
  }
  else if(id.startsWith("out"))
  {
    portName=id.mid(3);
    isOutput=true;
  }
  int deviceNum=-1, portNum=-1;
  if((isInput || isOutput) && DriverAlsa::instance()->getOpenId(m_id, portName, deviceNum, portNum))
  {
    bool connectOk=false;
    bool ownsHandle=!handle;
    if(handle || snd_seq_open(&handle, "default", SND_SEQ_OPEN_DUPLEX, 0)==0)
    {
      int portId=-1;
      snd_seq_nonblock(handle, 1);
      snd_seq_set_client_name(handle, QString("MyMidi%1").arg(id).toUtf8().data());

      portId=snd_seq_create_simple_port(handle, id.toUtf8().data(), isInput?SND_SEQ_PORT_CAP_WRITE:SND_SEQ_PORT_CAP_READ, SND_SEQ_PORT_TYPE_MIDI_GENERIC);
      if(isOutput)
        connectOk=(snd_seq_connect_to(handle, portId, deviceNum, portNum)>=0);
      else
        connectOk=(snd_seq_connect_from(handle, portId, deviceNum, portNum)>=0);
      if(!connectOk && portId>=0)
      {
        snd_seq_delete_simple_port(handle, portId);
        portId=-1;
      }
      if(portId>=0)
      {
        ret=QSharedPointer<AlsaPort>(new AlsaPort(m_thisRef.toStrongRef(), handle, ownsHandle, portId, isInput));
        if(!ownsHandle)
          addPortToCache(id, ret);
      }
      else if(ownsHandle)
        snd_seq_close(handle);
    }
  }
  return ret;
}


AlsaPort::AlsaPort(const QSharedPointer<AlsaDevice> &device, snd_seq_t *handle, bool ownsHandle, int portId, bool isInput): m_device(device), m_handle(handle), m_ownsHandle(ownsHandle), m_id(portId), m_isInput(isInput)
{
  m_thread=nullptr;
  m_pipeWrite=-1;
  if(isInput)
  {
    int pipes[2]={-1, -1};
    if(pipe(pipes)>=0)
    {
      qDebug()<<"Got pipes"<<pipes[1]<<">"<<pipes[0];
      m_pipeWrite=pipes[1];
      m_thread=new AlsaThread(m_handle, portId, pipes[0]);
      connect(m_thread, &AlsaThread::midiEvent, this, &Port::midiEvent, Qt::QueuedConnection);
      m_thread->start();
    }
  }
}

//  m_midiCoder=snd_midi_event_new()


AlsaPort::~AlsaPort()
{
  qDebug()<<"Destroyed alsa port"<<m_id;
  if(m_thread)
  {
    m_thread->requestInterruption();
    write(m_pipeWrite, "\0", 1); // Wakes up the reading thread
    m_thread->wait();
    if(m_id>=0)
      snd_seq_delete_simple_port(m_handle, m_id);
    m_thread->deleteLater();
  }
  if(m_ownsHandle && m_handle)
    snd_seq_close(m_handle);
}

bool AlsaPort::sendData(const QByteArray &data)
{

  if(m_id>=0)
  {
    QByteArray dataCopy;
    snd_seq_event_t ev;
    qDebug()<<"Outputting event"<<data<<sizeof(ev.data);
    snd_seq_ev_clear(&ev);
    snd_seq_ev_set_source(&ev, m_id);
    snd_seq_ev_set_subs(&ev);
    snd_seq_ev_set_direct(&ev);
    if(data.size()<=sizeof(ev.data))
    {
      qDebug()<<"Short message";
//      snd_seq_ev_set_fixed(&ev);
//      memcpy(&ev.data, data.data(), data.size());
      snd_midi_event_t* mev;
      snd_midi_event_new(data.size(), &mev);
      snd_midi_event_resize_buffer(mev, data.size());
      snd_midi_event_encode(mev, (unsigned char*) data.data(), data.size(), &ev);

      int err,err2;
      err=snd_seq_event_output(m_handle, &ev);
      err2=snd_seq_drain_output(m_handle);
      qDebug()<<err<<err2;

    }
    else
      qDebug()<<"Unhandled (for now) long messages";
  }
  qDebug()<<"Alsa port send data"<<data;
  return false;
}

AlsaThread::AlsaThread(snd_seq_t *handle, int portId, int pipeRead): m_handle(handle), m_portId(portId), m_pipeRead(pipeRead)
{

}
void AlsaThread::run()
{
  bool ok=true;
  snd_seq_event_t* ev = nullptr;
  snd_midi_event *midEv=nullptr;
  int bufferSize=128;
  unsigned char *buffer=(unsigned char *)alloca(bufferSize);
  if(!buffer)
    ok=false;
  if(ok && snd_midi_event_new(bufferSize, &midEv)!=0)
    ok=false;
  else
  {
    snd_midi_event_init(midEv);
    snd_midi_event_no_status(midEv, 1);
  }
  int pollFdCount = snd_seq_poll_descriptors_count( m_handle, POLLIN );
  struct pollfd* pollFds = nullptr;
  if(ok)
  {
    pollFds=(struct pollfd*)alloca( pollFdCount+1 * sizeof( struct pollfd ));
    snd_seq_poll_descriptors( m_handle, pollFds+1, pollFdCount, POLLIN );
    pollFds[0].fd = m_pipeRead;
    pollFds[0].events = POLLIN;
    pollFdCount++;
  }
  while (ok && !isInterruptionRequested()) {
    if ( snd_seq_event_input_pending( m_handle, 1 ) == 0 )
    {
      poll(pollFds, pollFdCount, -1);
      continue;
    }
    if(snd_seq_event_input(m_handle, &ev )<0)
      continue;
    bool decode=false;
    switch(ev->type)
    {
    case SND_SEQ_EVENT_CLIENT_START:
    case SND_SEQ_EVENT_CLIENT_EXIT:
    case SND_SEQ_EVENT_PORT_SUBSCRIBED:
    case SND_SEQ_EVENT_PORT_UNSUBSCRIBED:
    case SND_SEQ_EVENT_CLIENT_CHANGE:
      break;
    case SND_SEQ_EVENT_PORT_START:
      emit portAdded(ev->data.addr.client, ev->data.addr.port); break;
    case SND_SEQ_EVENT_PORT_EXIT:
      emit portRemoved(ev->data.addr.client, ev->data.addr.port); break;
    case SND_SEQ_EVENT_PORT_CHANGE:
      emit portChanged(ev->data.addr.client, ev->data.addr.port); break;
    default:
      decode=true;
    }
    if(decode)
    {
      int nBytes = snd_midi_event_decode(midEv, buffer, bufferSize, ev);
      if ( nBytes > 0 ) {
        QByteArray data((char *)buffer, nBytes);
        qDebug()<<"Midi event";
        emit midiEvent(data);
      }
      else
        qDebug()<<"Failed to decode"<<ev->type;

//        // The ALSA sequencer has a maximum buffer size for MIDI sysex
//        // events of 256 bytes.  If a device sends sysex messages larger
//        // than this, they are segmented into 256 byte chunks.  So,
//        // we'll watch for this and concatenate sysex chunks into a
//        // single sysex message if necessary.
//        if ( !continueSysex )
//          message.bytes.assign( buffer, &buffer[nBytes] );
//        else
//          message.bytes.insert( message.bytes.end(), buffer, &buffer[nBytes] );

//        continueSysex = ( ( ev->type == SND_SEQ_EVENT_SYSEX ) && ( message.bytes.back() != 0xF7 ) );
//        if ( !continueSysex ) {

//          // Calculate the time stamp:
//          message.timeStamp = 0.0;

//          // Method 1: Use the system time.
//          //(void)gettimeofday(&tv, (struct timezone *)NULL);
//          //time = (tv.tv_sec * 1000000) + tv.tv_usec;

//          // Method 2: Use the ALSA sequencer event time data.
//          // (thanks to Pedro Lopez-Cabanillas!).

//          // Using method from:
//          // https://www.gnu.org/software/libc/manual/html_node/Elapsed-Time.html

//          // Perform the carry for the later subtraction by updating y.
//          // Temp var y is timespec because computation requires signed types,
//          // while snd_seq_real_time_t has unsigned types.
//          snd_seq_real_time_t &x( ev->time.time );
//          struct timespec y;
//          y.tv_nsec = apiData->lastTime.tv_nsec;
//          y.tv_sec = apiData->lastTime.tv_sec;
//          if ( x.tv_nsec < y.tv_nsec ) {
//              int nsec = (y.tv_nsec - (int)x.tv_nsec) / 1000000000 + 1;
//              y.tv_nsec -= 1000000000 * nsec;
//              y.tv_sec += nsec;
//          }
//          if ( x.tv_nsec - y.tv_nsec > 1000000000 ) {
//              int nsec = ((int)x.tv_nsec - y.tv_nsec) / 1000000000;
//              y.tv_nsec += 1000000000 * nsec;
//              y.tv_sec -= nsec;
//          }

//          // Compute the time difference.
//          time = (int)x.tv_sec - y.tv_sec + ((int)x.tv_nsec - y.tv_nsec)*1e-9;

//          apiData->lastTime = ev->time.time;

//          if ( data->firstMessage == true )
//            data->firstMessage = false;
//          else
//            message.timeStamp = time;
//        }
//        else {
//#if defined(__RTMIDI_DEBUG__)
//          std::cerr << "\nMidiInAlsa::alsaMidiHandler: event parsing error or not a MIDI event!\n\n";
//#endif
//        }
//      }
//    }
   }
 }
 if(midEv)
   snd_midi_event_free(midEv);
 qDebug()<<"Alsa thread exit";
}

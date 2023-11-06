#include "QG_Uart.h"


#ifdef UNICODE
#define TP_UART_FILENAME_PREFIX     L"\\\\.\\"
#else
#define TP_UART_FILENAME_PREFIX     "\\\\.\\"
#endif // !UNICODE

struct QG_Uart_Manage
{
  QG_Handle Get_Handle()
  {
    return handle;
  }
  void Set_Handle(QG_Handle hdle)
  {
    handle= hdle;
  }
  void initialize_overlapped(LPOVERLAPPED overlapped)
  {
    ZeroMemory(overlapped, sizeof(OVERLAPPED));
    if (overlapped->hEvent != NULL)
    {
      ResetEvent(overlapped->hEvent);
      overlapped->hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
      return;
    }
    overlapped->hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
  }
  void release_overlapped(LPOVERLAPPED overlapped)
  {
    ResetEvent(overlapped->hEvent);
  }
  DWORD inqueue_;
  DWORD outqueue_;
  QG_Uart_Manage():
        write_overlapped(),
        read_overlapped(),
        wait_overlapped()
  {
    inqueue_ = DEFAULTQUEUESIZE;
    outqueue_ = DEFAULTQUEUESIZE;
    handle = nullptr;
    initialize_overlapped(&write_overlapped);
    initialize_overlapped(&read_overlapped);
    initialize_overlapped(&wait_overlapped);
  }
  ~QG_Uart_Manage()
  {
    release_overlapped(&write_overlapped);
    release_overlapped(&read_overlapped);
    release_overlapped(&wait_overlapped);
  }
  OVERLAPPED write_overlapped;
  OVERLAPPED read_overlapped;
  OVERLAPPED wait_overlapped;
  DCB      other_dcb;
  DWORD    other_mask;
 private:
  QG_Handle handle;
};

bool CheckHandle(QG_Handle handle)
{
  if (handle == INVALID_HANDLE_VALUE|| handle==nullptr)
  {
    return false;
  }
  return true;
}
//Sync
QG_Handle CreateSyncWinfile(std::string portname, QG_Uart::OpenMode mode)
{
#ifdef UNICODE
  std::wstring comname = TP_UART_FILENAME_PREFIX + StringToWString(portname);
#else
  std::string comname = TP_UART_FILENAME_PREFIX + portname;
#endif // !UNICODE

  DWORD dwDesiredAccess_ = GENERIC_READ | GENERIC_WRITE;
  switch (mode)
  {
    case QG_IO_Base::ReadOnly:
      dwDesiredAccess_ = GENERIC_READ;
      break;
    case QG_IO_Base::WriteOnly:
      dwDesiredAccess_ = GENERIC_WRITE;
      break;
    default:
      break;
  }
  QG_Handle handle=
      CreateFile(                   /*unicode*/
                 comname.c_str(),
                 dwDesiredAccess_,
                 0,                          /* no share  */
                 NULL,                       /* no security */
                 OPEN_EXISTING,
                 FILE_ATTRIBUTE_NORMAL,      /* no threads */
                 NULL                        /* no templates */
                 );
  return handle;
}

//Async
QG_Handle CreateAsyncWinfile(std::string portname, QG_Uart::OpenMode mode)
{
#ifdef UNICODE
  std::wstring comname = TP_UART_FILENAME_PREFIX + StringToWString(portname);
#else
  std::string comname = TP_UART_FILENAME_PREFIX + portname;
#endif // !UNICODE
  DWORD dwDesiredAccess_ = GENERIC_READ | GENERIC_WRITE;
  switch (mode)
  {
    case QG_IO_Base::ReadOnly:
      dwDesiredAccess_ = GENERIC_READ;
      break;
    case QG_IO_Base::WriteOnly:
      dwDesiredAccess_ = GENERIC_WRITE;
      break;
    default:
      break;
  }
  QG_Handle handle =
      CreateFile(                   /*unicode*/
                 comname.c_str(),
                 dwDesiredAccess_,
                 0,                          /* no share  */
                 NULL,                       /* no security */
                 OPEN_EXISTING,
                 FILE_FLAG_OVERLAPPED,      
                 NULL                        /* no templates */
                 );
  return handle;
}

///timeout
bool SetTimeOut(QG_Handle handle, COMMTIMEOUTS  port_timeouts)
{
  return  SetCommTimeouts(handle, &port_timeouts);
}
///reset native dcb
bool reset_dcb(QG_Handle handle,LPDCB dcb)
{
  if (!CheckHandle(handle))
  {
    QGERROR("error handle" QG_ENDL );
    return false;
  }
  bool ack = false;
  ack = SetCommState(handle, dcb);
  if (!ack)
  {
    QGERROR("error SetCommState" QG_ENDL );
    DEBUG_CODE; QG_Uart::PrintLastErrorStr();
    return false;
  }
  return ack;
}
///DCB
bool SetDCB(QG_Handle handle, SerialPortInfo * info)
{
  if (!CheckHandle(handle))
  {
    QGERROR("error handle" QG_ENDL );
    return false;
  }
  DCB dcb;
  memset(&dcb, 0, sizeof(DCB));
  bool ack = false;
  ack = GetCommState(handle, &dcb);
  if (!ack)
  {
    DEBUG_CODE; QG_Uart::PrintLastErrorStr();
    return false;
  }

  dcb.DCBlength = sizeof(DCB);
  dcb.BaudRate = (DWORD)info->baudrate;
  dcb.fParity =  TRUE;
  dcb.Parity =   info->parity;
  dcb.ByteSize = info->databits;
  dcb.StopBits = info->stopbits-1;
  switch (info->flowcontrol)
  {
    case FlowControl::NoFlowControl:
      dcb.fDtrControl = DTR_CONTROL_DISABLE;
      dcb.fRtsControl = RTS_CONTROL_DISABLE;
      break;
    case SoftwareControl:
      dcb.fInX = TRUE;
      dcb.fOutX = TRUE;
      break;
    case HardwareControl:
      dcb.fOutxCtsFlow = TRUE;
      dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
      break;
    default:
      break;
  }
  ack = SetCommState(handle, &dcb);
  if (!ack)
  {
    QGERROR("error SetCommState" QG_ENDL );
    DEBUG_CODE; QG_Uart::PrintLastErrorStr();
    return false;
  }
  return ack;
}

///mask
bool SetMask(QG_Handle handle,OVERLAPPED &old)
{
  if (!CheckHandle(handle))
  {
    QGERROR("error handle" QG_ENDL );
    return false;
  }
  BOOL ack=
      SetCommMask(handle,
                  EV_RXCHAR
                      | EV_RXFLAG
                      | EV_TXEMPTY
                      | EV_CTS
                      | EV_DSR
                      | EV_RLSD
                      | EV_BREAK
                      | EV_ERR
                      | EV_RING
                      | EV_PERR
                      | EV_RX80FULL
                  );
  if (!ack)
  {
    DEBUG_CODE; QG_Uart::PrintLastErrorStr();
    return false;
  }
  return true;
}
bool SetMask(QG_Handle handle,DWORD dwEvtMask)
{
  if (!CheckHandle(handle))
  {
    QGERROR("error handle" QG_ENDL);
    return false;
  }
  BOOL ack =
      SetCommMask(handle,
                  dwEvtMask
                  );
  if (!ack)
  {
    DEBUG_CODE; QG_Uart::PrintLastErrorStr();
    return false;
  }
  return true;
}

bool SetMask(QG_Handle handle,QG_IO_Base::OpenMode mode)
{
  const DWORD eventMask = (mode & QG_IO_Base::OpenMode::ReadOnly) ? EV_RXCHAR : 0;
  if (!::SetCommMask(handle, eventMask))
  {
    DEBUG_CODE; QG_Uart::PrintLastErrorStr();
    return false;
  }
  return true;
};

///wait event 
bool WaitEvent(QG_Handle handle , OVERLAPPED& old)
{
  if (!CheckHandle(handle))
  {
    QGERROR("error handle" QG_ENDL );
    return false;
  }
  DWORD dwEvtMask=0;
  //TODO 事件触发
  if (WaitCommEvent(handle, &dwEvtMask, &old))
  {
    SetMask(handle, old);
    if (dwEvtMask & EV_RXCHAR)
    {
      QGDEBUG("EV_RXCHAR" QG_ENDL );
    }
    if (dwEvtMask & EV_RXFLAG)
    {
      QGDEBUG("EV_RXFLAG" QG_ENDL );
    }
    if (dwEvtMask & EV_TXEMPTY)
    {
      QGDEBUG("EV_TXEMPTY" QG_ENDL );
    }
    if (dwEvtMask & EV_CTS)
    {
      QGDEBUG("EV_CTS" QG_ENDL );
    }
    if (dwEvtMask & EV_DSR)
    {
      QGDEBUG("EV_DSR" QG_ENDL );
    }
    if (dwEvtMask & EV_RLSD)
    {
      QGDEBUG("EV_RLSD" QG_ENDL );
    }
    if (dwEvtMask & EV_BREAK)
    {
      QGDEBUG("EV_BREAK" QG_ENDL );
    }
    if (dwEvtMask & EV_ERR)
    {
      QGDEBUG("EV_ERR" QG_ENDL );
    }
    if (dwEvtMask & EV_RING)
    {
      QGDEBUG("EV_RING" QG_ENDL );
    }
    if (dwEvtMask & EV_PERR)
    {
      QGDEBUG("EV_PERR" QG_ENDL );
    }
    if (dwEvtMask & EV_RX80FULL)
    {
      QGDEBUG("EV_RX80FULL" QG_ENDL );
    }

  }
  else
  {
    DWORD dwRet = GetLastError();
    if (ERROR_IO_PENDING == dwRet)
    {
      QGERROR("I/O is pending..." QG_ENDL);
    }
    else
    {
      DEBUG_CODE; QG_Uart::PrintLastErrorStr();
    }
    return false;
  }
  return true;
}

QG_Uart::QG_Uart()
    :QG_Uart_Base{}
{
  ini_Uart_Manage();
}

QG_Uart::QG_Uart(std::string name)
    :QG_Uart_Base{ name }
{
  ini_Uart_Manage();
  serialport_info->port_name = name;
}

QG_Uart::QG_Uart(const SerialPortInfo& info):QG_Uart_Base{ info }
{
  ini_Uart_Manage();
  *serialport_info = info;
}

QG_Uart::~QG_Uart()
{
  close();
  destory_Uart_Manage();
}

bool QG_Uart::open(OpenMode mode)
{
  QG_Handle handle= CreateAsyncWinfile(serialport_info->port_name,mode);
  if (!CheckHandle(handle))
  {
    QGERROR(" open error" QG_ENDL );
    DEBUG_CODE; QG_Uart::PrintLastErrorStr();
    return false;
  }
  uartmanage_->Set_Handle(handle);
  initialize(mode);
  //SetMask(handle, uartmanage_->old);
  //RunWaitEventThread();
  from_handle=false;
  stop_upg(false);
  return true;
}

bool QG_Uart::initialize(QG_Uart::OpenMode mode)
{
  if (!SetDCB(uartmanage_->Get_Handle(), serialport_info))
  {
    close();
    QGERROR(" error SetDCB" QG_ENDL);
    return false;
  }
  if (!set_read_interval_timeout())
  {
    close();
    QGERROR(" error set_read_interval_timeout" QG_ENDL);
    return false;
  }
  if (!set_buffer_size())
  {
    close();
    QGERROR(" error set_buffer_size" QG_ENDL);
    return false;
  }
  return true;
  ///TODO add readready event to monitor rxchar
}

void QG_Uart::RunWaitEventThread()
{
  if (runmask&& !uartmask_td_.joinable())
  {
    runmask = false;
    QGDEBUG("WaitForSingleObject" QG_ENDL);
    WaitForSingleObject(uartmask_td_.native_handle(), DEFAULTWAITEVENTTIME);
  }
  runmask = true;
  QGDEBUG("UartMask joinable and start " QG_ENDL);
  uartmask_td_ = std::thread([=]()
                             {
                               while (runmask && is_open())
                               {
                                 WaitEvent(uartmanage_->Get_Handle(), uartmanage_->wait_overlapped);
                               }
                               runmask = false;
                             });
  uartmask_td_.detach();
}

void QG_Uart::set_serialport_info(const SerialPortInfo& info)
{
  *serialport_info = info;
  if (!CheckHandle(uartmanage_->Get_Handle()))
  {
    return ;
  }
  if (!SetDCB(uartmanage_->Get_Handle(), serialport_info))
  {
    //close();
    QGERROR(" error SetDCB" QG_ENDL);
    return ;
  }
}

void QG_Uart::set_port_name(std::string name)
{
  serialport_info->port_name = name;
}

bool QG_Uart::close()
{
  //[]other handle do not close
  if(from_handle)
  {
    return true;
  }
  ///todo mask
  //runmask = false;
  if (!CheckHandle(uartmanage_->Get_Handle()))
  {
    //QGERROR("CheckHandle " QG_ENDL );
    return false;
  }
  //SetCommMask(uartmanage_->Get_Handle(), 0);
  clear();
  bool ack= CloseHandle(uartmanage_->Get_Handle());
  uartmanage_->Set_Handle( INVALID_HANDLE_VALUE );
  if (!ack)
  {
    QGERROR("close error" QG_ENDL);
    DEBUG_CODE; QG_Uart::PrintLastErrorStr();
  }
  return ack;
}

std::size_t QG_Uart::write(const char* wbuffer, std::size_t size)
{
  if (!CheckHandle(uartmanage_->Get_Handle()))
  {
    QGERROR("error handle" QG_ENDL );
    return RETURN_ERROR_SIZE;
  }
  DWORD written=0;
  bool ack= WriteFile(uartmanage_->Get_Handle(), wbuffer, size, &written, &uartmanage_->write_overlapped);
  DWORD last_error = 0;
  if(!ack)
  {
    if (GetLastError() == ERROR_IO_PENDING)
    {
      while (!GetOverlappedResult(uartmanage_->Get_Handle(), &uartmanage_->write_overlapped, &written, FALSE))
      {
        if (GetLastError() == ERROR_IO_INCOMPLETE)
        {
          continue;
        }
        else
        {
          QGERROR("WriteFile Error " QG_ENDL);
          DEBUG_CODE; QG_Uart::PrintLastErrorStr();
          ClearCommError(uartmanage_->Get_Handle(), &last_error, NULL);
          break;
        }
      }
    }
  }

  return written;
}

std::size_t QG_Uart::read(char* rbuffer, std::size_t size, std::size_t timeout)
{
  if (!CheckHandle(uartmanage_->Get_Handle()))
  {
    QGERROR("CheckHandle error " QG_ENDL);
    return RETURN_ERROR_SIZE;
  }
  COMMTIMEOUTS time{0};
  if( !GetCommTimeouts(uartmanage_->Get_Handle(), &time))
  {
    QGERROR("GetCommTimeouts error " QG_ENDL);
    DEBUG_CODE; QG_Uart::PrintLastErrorStr();
    return RETURN_ERROR_SIZE;
  }
  time.ReadTotalTimeoutConstant = timeout;
  if (!SetCommTimeouts(uartmanage_->Get_Handle(), &time))
  {
    QGERROR("SetCommTimeouts error " QG_ENDL);
    DEBUG_CODE; QG_Uart::PrintLastErrorStr();
    return RETURN_ERROR_SIZE;
  }
  DWORD readbytes=0;
  bool ack = ReadFile(uartmanage_->Get_Handle(), rbuffer, size, &readbytes, &uartmanage_->read_overlapped);
  DWORD last_error = 0;
  if (!ack)
  {
    if (GetLastError() == ERROR_IO_PENDING)
    {
      while (!GetOverlappedResult(uartmanage_->Get_Handle(), &uartmanage_->read_overlapped, &readbytes, FALSE))
      {
        if (GetLastError() == ERROR_IO_INCOMPLETE)
        {
          continue;
        }
        else
        {
          QGERROR("ReadFile Error " QG_ENDL);
          DEBUG_CODE; QG_Uart::PrintLastErrorStr();
          ClearCommError(uartmanage_->Get_Handle(), &last_error, NULL);
          break;
        }
      }
    }
  }
  return readbytes;
}

bool QG_Uart::set_baudrate(uint32_t baudrate)
{
  if (!CheckHandle(uartmanage_->Get_Handle()))
  {
    QGERROR("error handle" QG_ENDL );
    return false;
  }
  serialport_info->baudrate = baudrate;
  bool ack=
      SetDCB(uartmanage_->Get_Handle(), serialport_info);
  if (!ack)
  {
    QGERROR("error  baudrate: %d" QG_ENDL , baudrate);
    DEBUG_CODE; QG_Uart::PrintLastErrorStr();
  }
  return ack;
}

bool QG_Uart::set_parity(Parity parity)
{
  if (!CheckHandle(uartmanage_->Get_Handle()))
  {
    QGERROR("error handle" QG_ENDL );
    return false;
  }
  serialport_info->parity = parity;
  bool ack =
      SetDCB(uartmanage_->Get_Handle(), serialport_info);
  if (!ack)
  {
    QGERROR("error  parity: %d" QG_ENDL , parity);
    DEBUG_CODE; QG_Uart::PrintLastErrorStr();
  }
  return ack;
}

bool QG_Uart::set_databits(DataBits databits)
{
  if (!CheckHandle(uartmanage_->Get_Handle()))
  {
    QGERROR("error handle" QG_ENDL );
    return false;
  }
  serialport_info->databits = databits;
  bool ack =
      SetDCB(uartmanage_->Get_Handle(), serialport_info);
  if (!ack)
  {
    QGERROR("error  databits: %d" QG_ENDL , databits);
    DEBUG_CODE; QG_Uart::PrintLastErrorStr();
  }
  return ack;
}

bool QG_Uart::set_pinout_signals(PinoutSignal pin, bool enable)
{
  if (!CheckHandle(uartmanage_->Get_Handle()))
  {
    QGERROR("error handle" QG_ENDL );
    return false;
  }
  int flag = enable ? TRUE : FALSE;
  DCB dcb;
  memset(&dcb, 0, sizeof(DCB));
  bool ack = false;
  ack = GetCommState(uartmanage_->Get_Handle(), &dcb);
  if (!ack)
  {
    QGERROR("error  " QG_ENDL );
    DEBUG_CODE; QG_Uart::PrintLastErrorStr();
    return ack;
  }
  switch (pin)
  {
    case DataTerminalReadySignal:
      dcb.fDtrControl = flag;
      break;
    case RequestToSendSignal:
      dcb.fRtsControl = flag;
      break;
    default:
      dcb.fDtrControl = flag;
      dcb.fRtsControl = flag;
      break;
  }
  ack=    SetDCB(uartmanage_->Get_Handle(), serialport_info);
  if (!ack)
  {
    QGERROR("%s error set PinoutSignal: %d" QG_ENDL , pin);
  }
  return ack;
}

bool QG_Uart::write_response(WriteResponse writeresponse)
{
  if (!CheckHandle(uartmanage_->Get_Handle()))
  {
    QGERROR("error handle" QG_ENDL);
    return false;
  }
  write(writeresponse.writedata, writeresponse.writesize);
  char* responsedata = new char[writeresponse.responsesize] {0};
  read(responsedata, writeresponse.responsesize, writeresponse.responsetimeout);
  if (!memcmp(responsedata, writeresponse.responsedata, writeresponse.responsesize))
  {
    return true;
  }
  return false;
}

#include <system_error>
void QG_Uart::PrintLastErrorStr( )
{
  DWORD errCode = GetLastError();
  std::string message = std::system_category().message(errCode);
  QG::mprint(QG::ERROR_, "system error %d: %s ", errCode, message.data ());
}

void QG_Uart::stop_upg(bool stop)
{
  advance_stop_flag_.store (stop);
  if(advance_stop_flag_.load ())
  {
    QGINFO ("Early termination of upgrades..." QG_ENDL);
    if (!CheckHandle(uartmanage_->Get_Handle()))
    {
      return ;
    }
    COMMTIMEOUTS time{0};
    if( !GetCommTimeouts(uartmanage_->Get_Handle(), &time))
    {
      return ;
    }
    time.ReadTotalTimeoutConstant = 1000;
    if (!SetCommTimeouts(uartmanage_->Get_Handle(), &time))
    {
      return ;
    }
  }
}

bool QG_Uart::get_stop_flag()
{
  return advance_stop_flag_.load ();
}

void QG_Uart::ini_Uart_Manage()
{
  if (!uartmanage_)
  {
    uartmanage_ = new QG_Uart_Manage;
  }

}

void QG_Uart::destory_Uart_Manage()
{
  if (uartmanage_)
  {
    delete  uartmanage_ ;
    uartmanage_ = nullptr;
  }
}

bool QG_Uart::is_open()
{
  if (!CheckHandle(uartmanage_->Get_Handle()))
  {
    return false;
  }
  return true;
}

QG_Handle QG_Uart::get_native()
{
  return uartmanage_->Get_Handle();
}

bool QG_Uart::set_native(QG_Handle handle)
{
  if (is_open()&&handle!=uartmanage_->Get_Handle ())
  {
    QGINFO ("close handle" QG_ENDL );
    close();
  }

  if (!CheckHandle(handle))
  {
    QGERROR("error handle %X" QG_ENDL,handle );
    return false;
  }
  uartmanage_->Set_Handle(handle);

  error_clear();

  GetCommMask (handle,&uartmanage_->other_mask);
  QGINFO ("native flags %d",uartmanage_->other_mask);
  GetCommState (handle,&uartmanage_->other_dcb);
  //SetMask(handle,QG_IO_Base::OpenMode::NotOpen);//delete other mask
  //SetMask(handle, uartmanage_->old);
  //RunWaitEventThread();
  from_handle=true;
  return true;
}
void QG_Uart::error_clear()
{
  if (!CheckHandle(uartmanage_->Get_Handle ()))
  {
    QGERROR("error handle %X" QG_ENDL,uartmanage_->Get_Handle () );
    return ;
  }
  DWORD dwErrorFlags; //错误标志
  COMSTAT comStat; //通讯状态
  ClearCommError(uartmanage_->Get_Handle (), &dwErrorFlags, &comStat); //清除通讯错误，获得设备当前状态
}

bool QG_Uart::reset()
{
  if(!SetMask (uartmanage_->Get_Handle (),uartmanage_->other_mask))
  {
    return false;
  }
  return reset_dcb (uartmanage_->Get_Handle (),&uartmanage_->other_dcb);
};
bool QG_Uart::set_buffer_size(Direction dir, std::size_t size)
{
  switch (dir)
  {
    case Direction::Input:
      uartmanage_->inqueue_ = size;
      break;
    case Direction::Output:
      uartmanage_->outqueue_ = size;
      break;
    case Direction::AllDirections:
      uartmanage_->inqueue_ = size;
      uartmanage_->outqueue_ = size;
      break;
    default:
      uartmanage_->inqueue_ = size;
      uartmanage_->outqueue_ = size;
      break;
  }
  bool ack= SetupComm(uartmanage_->Get_Handle(), uartmanage_->inqueue_, uartmanage_->outqueue_);
  if (!ack)
  {
    QGERROR("error" QG_ENDL );
  }
  return ack;
}

bool QG_Uart::clear(Direction direction)
{
  DWORD flags;
  switch (direction)
  {
    case Direction::Input:
      flags =  PURGE_TXCLEAR  | PURGE_TXABORT;
      break;
    case Direction::Output:
      flags = PURGE_RXCLEAR | PURGE_RXABORT;
      break;
    case Direction::AllDirections:
      flags = PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT;
      break;
    default:
      flags = PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT;
      break;
  }
  bool ack= PurgeComm(uartmanage_->Get_Handle(), flags);
  if (!ack)
  {
    QGERROR("PurgeComm error" QG_ENDL );
  }
  return ack;
}

bool QG_Uart::flush()
{
  bool ack= FlushFileBuffers(uartmanage_->Get_Handle());
  if (!ack)
  {
    QGERROR("error  " QG_ENDL );
  }
  return ack;
}

bool QG_Uart::set_read_interval_timeout(int timeout)
{
  read_interval_timeout_ = timeout;
  COMMTIMEOUTS commtimeouts;
  commtimeouts.ReadIntervalTimeout = timeout;
  commtimeouts.ReadTotalTimeoutMultiplier = 0;
  commtimeouts.ReadTotalTimeoutConstant = 0;
  commtimeouts.WriteTotalTimeoutConstant = 0;
  commtimeouts.WriteTotalTimeoutMultiplier = 0;
  bool ack=  SetTimeOut(uartmanage_->Get_Handle(), commtimeouts);
  return ack;
}


#include "windows.h" // CreateFile GetTickCount64
#include "tchar.h" // _sntprintf _T

#include <Setupapi.h> //SetupDiGetClassDevs Setup*
#include <initguid.h> //GUID

#pragma comment (lib, "setupapi.lib")

using namespace std;

#ifndef GUID_DEVINTERFACE_COMPORT
DEFINE_GUID(GUID_DEVINTERFACE_COMPORT, 0x86E0D1E0L, 0x8089, 0x11D0, 0x9C, 0xE4, 0x08, 0x00, 0x3E, 0x30, 0x1F, 0x73);
#endif




bool enumDetailsSerialPorts(std::vector<SerialPortInfo>& portInfoList)
{
  // https://docs.microsoft.com/en-us/windows/win32/api/setupapi/nf-setupapi-setupdienumdeviceinfo

  bool bRet = false;
  SerialPortInfo m_serialPortInfo;

  std::string strFriendlyName;
  std::string strPortName;

  HDEVINFO hDevInfo = INVALID_HANDLE_VALUE;

  // Return only devices that are currently present in a system
  // The GUID_DEVINTERFACE_COMPORT device interface class is defined for COM ports. GUID
  // {86E0D1E0-8089-11D0-9CE4-08003E301F73}
  hDevInfo = SetupDiGetClassDevs(&GUID_DEVINTERFACE_COMPORT, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

  if (INVALID_HANDLE_VALUE != hDevInfo)
  {
    SP_DEVINFO_DATA devInfoData;
    // The caller must set DeviceInfoData.cbSize to sizeof(SP_DEVINFO_DATA)
    devInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

    for (DWORD i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &devInfoData); i++)
    {
      // get port name
      TCHAR portName[256];
      HKEY hDevKey = SetupDiOpenDevRegKey(hDevInfo, &devInfoData, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ);
      if (INVALID_HANDLE_VALUE != hDevKey)
      {
        DWORD dwCount = 255; // DEV_NAME_MAX_LEN
        RegQueryValueEx(hDevKey, _T("PortName"), NULL, NULL, (BYTE*)portName, &dwCount);
        RegCloseKey(hDevKey);
      }

      // get friendly name
      TCHAR fname[256];
      SetupDiGetDeviceRegistryProperty(hDevInfo, &devInfoData, SPDRP_FRIENDLYNAME, NULL, (PBYTE)fname,
                                       sizeof(fname), NULL);

#ifdef UNICODE
      strPortName = wstringToString(portName);
      strFriendlyName = wstringToString(fname);
#else
      strPortName = std::string(portName);
      strFriendlyName = std::string(fname);
#endif
      // remove (COMxx)
      strFriendlyName = strFriendlyName.substr(0, strFriendlyName.find(("(COM")));
      m_serialPortInfo.port_name = strPortName;
      m_serialPortInfo.port_description = strFriendlyName;
      portInfoList.push_back(m_serialPortInfo);
    }

    if (ERROR_NO_MORE_ITEMS == GetLastError())
    {
      bRet = true; // no more item
    }
  }
  SetupDiDestroyDeviceInfoList(hDevInfo);
  return bRet;
}

bool QG_Uart::get_serialportinfos(std::vector<SerialPortInfo>& portInfoList)
{
  return enumDetailsSerialPorts(portInfoList);
}

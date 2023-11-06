#include "QG_Uart_Base.h"

QG_Uart_Base::QG_Uart_Base(std::string PortName)
{
    if (!serialport_info)
    {
        serialport_info = new SerialPortInfo;
    }
    serialport_info->port_name = PortName;
}

QG_Uart_Base::QG_Uart_Base(const SerialPortInfo& info)
{
    if (!serialport_info)
    {
        serialport_info = new SerialPortInfo;
    }
    *serialport_info = info;
}

QG_Uart_Base::QG_Uart_Base()
{
    if (!serialport_info)
    {
        serialport_info = new SerialPortInfo;
    }
}

QG_Uart_Base:: ~QG_Uart_Base()
{
    delete serialport_info;
  serialport_info = nullptr;
}


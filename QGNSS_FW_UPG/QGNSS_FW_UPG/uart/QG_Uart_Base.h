#ifndef __UART_QG_UART_BASE_H__
#define __UART_QG_UART_BASE_H__

#include "QG_IO_Base.h"

#include <thread>
#include <atomic>
#include <optional>

enum Parity
{
    NoParity,
    OddParity,
    EvenParity,
    MarkParity,
    SpaceParity,
    UnknownParity
};

enum DataBits
{
    Data4 = 4,
    Data5,
    Data6,
    Data7,
    Data8,
    UnknownDataBits
};

enum StopBits
{
    OneStop=1,
    OneAndHalfStop=3,
    TwoStop=2,
    UnknownStopBits=-1
};

enum FlowControl
{
    NoFlowControl,
    HardwareControl,
    SoftwareControl,
    UnknownFlowControl
};

enum PinoutSignal
{
    DataTerminalReadySignal,          //DTR
    RequestToSendSignal               //RTS
};

enum Direction
{
    Input,
    Output,
    AllDirections
};

 struct SerialPortInfo
{
    std::string   port_name;
    std::string   port_description;
    uint32_t      baudrate=115200;
    Parity        parity= Parity::NoParity;
    DataBits      databits= DataBits::Data8;
    StopBits      stopbits= StopBits::OneStop;
    FlowControl   flowcontrol= FlowControl::NoFlowControl;
    QG_Handle     uart_handle=INVALID_HANDLE_VALUE;
};

struct QG_Uart_Manage;

class QG_PORTEXTERN QG_Uart_Base:public QG_IO_Base
{
public:
    QG_Uart_Base();
    QG_Uart_Base(std::string PortName);
    QG_Uart_Base(const SerialPortInfo& info);

    virtual ~QG_Uart_Base();
    virtual void set_serialport_info(const SerialPortInfo& info)=0;
    virtual void set_port_name(std::string name) = 0;
    virtual bool set_baudrate(uint32_t  baudrate)=  0;
    virtual bool set_parity(Parity      parity) =   0;
    virtual bool set_databits(DataBits  databits) = 0;
    virtual bool set_pinout_signals(PinoutSignal pin, bool)=0;
    virtual bool set_read_interval_timeout(int timeout) = 0;
    virtual bool clear(Direction) = 0;
    virtual bool set_buffer_size(Direction, std::size_t)=0;
    virtual bool is_open()=0;
    virtual void error_clear()=0;
    virtual bool reset()=0;
private:
 
protected:
    SerialPortInfo* serialport_info = nullptr;
    int read_interval_timeout_=200;
    std::thread uartmask_td_;
    std::atomic<bool> runmask = false;
};

#endif

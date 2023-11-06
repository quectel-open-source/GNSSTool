#ifndef __UART_QG_UART__
#define __UART_QG_UART__

#include "QG_Uart_Base.h"

#define DEFAULTQUEUESIZE 6000
#define DEFAULTWAITEVENTTIME 500

class QG_PORTEXTERN QG_Uart :public QG_Uart_Base
{
public:
    QG_Uart();
    QG_Uart(std::string name);
    QG_Uart(const SerialPortInfo& info);
    ~QG_Uart();

    
    virtual bool set_baudrate(uint32_t baudrate) override;
    virtual bool set_parity(Parity parity) override;
    virtual bool set_databits(DataBits databits) override;
    virtual bool set_pinout_signals(PinoutSignal pin, bool) override;

    // Inherited via QG_Uart_Base
    virtual bool open(OpenMode openMode = OpenMode::ReadWrite) override;
    virtual bool close() override;
    virtual std::size_t write(const char* wbuffer, std::size_t size) override;
    virtual std::size_t read(char* rbuffer, std::size_t, std::size_t timeout) override;
    virtual bool set_read_interval_timeout(int timeout=1) override;
    virtual bool flush() override;
    virtual bool clear(Direction=Direction::AllDirections) override;
    virtual bool set_buffer_size(Direction dir= Direction::AllDirections, std::size_t =6000) override;
    virtual QG_Handle get_native() override;
    virtual bool set_native(QG_Handle) override;
    virtual bool is_open() override;
    virtual void set_serialport_info(const SerialPortInfo& info) override;
    virtual void set_port_name(std::string name) override;
    virtual void error_clear() override;
    virtual bool reset() override;
    //upg helpe func
    struct WriteResponse
    {
        char* writedata;
        unsigned int writesize;
        char* responsedata;
        unsigned int responsesize;
        unsigned int responsetimeout;
    };
    bool write_response(WriteResponse writeresponse);
    void stop_upg(bool stop);
    bool get_stop_flag();
    //static
    static void PrintLastErrorStr();
    static  bool get_serialportinfos(std::vector<SerialPortInfo>& portInfoList);
private:
    QG_Uart_Manage* uartmanage_=nullptr;
    void ini_Uart_Manage();
    bool initialize(QG_Uart::OpenMode mode);
    void destory_Uart_Manage();
    void RunWaitEventThread();
    std::atomic_bool advance_stop_flag_ = false;

    ///get handle from other
    bool from_handle=false;
};
#endif // !__UART_QG_UART__

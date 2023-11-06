#ifndef ST8100_DOWNLOAD_H
#define ST8100_DOWNLOAD_H

#include <map>
#include <string>
#include <functional>
#include <atomic>
#include <chrono>
#include "QG_Uart.h"
#include "QG_Common.h"
#include "QG_Print.h"
/// <summary>
/// USART protocol used in the STM32 bootloader
/// </summary>
class ST8100_USART_download :public QG_Uart
{
public:
    ST8100_USART_download();
    ~ST8100_USART_download();
    enum Command:unsigned char
    {
        Get=0x00,                           //Gets the version and the allowed commands supported by the current version of the bootloader.
        GetVersionAndReadProtectionStatus,  //Gets the bootloader version and the Read Protection status of the Flash memory
        GetID,                              //Gets the chip ID.
        ReadMemory=0x11,                    //Reads up to 256 bytes of memory starting from an address specified by the application.
        Go=0x21,                            //Jumps to user application code located in the internal Flash memory or in the SRAM
        WriteMemory=0x31,                   //Writes up to 256 bytes to the RAM or Flash memory starting from an address specified by the application.
        Erase=0x43,                         //Erases from one to all the Flash memory pages
        ExtendedErase=0x44,                 //Erases from one to all the Flash memory pages using two byte addressing mode(available only for v3.0 usart bootloader versions and above).
        WriteProtect=0x63,                  //Enables the write protection for some sectors
        WriteUnprotect=0x73,                //Disables the write protection for all Flash memory sectors
        ReadoutProtect=0x82,                //Enables the read protection
        ReadUnprotect=0x92                  //Disables the read protection
    };
    struct CommandStr
    {                                                ;
     unsigned char Get                               ;
     unsigned char GetVersionAndReadProtectionStatus ;
     unsigned char GetID                             ;
     unsigned char ReadMemory                        ;
     unsigned char Go                                ;
     unsigned char WriteMemory                       ;
     unsigned char Erase                             ;
//     unsigned char ExtendedErase                     ;
     unsigned char WriteProtect                      ;
     unsigned char WriteUnprotect                    ;
     unsigned char ReadoutProtect                    ;
     unsigned char ReadUnprotect                     ;
    };

    /*USART bootloader code sequence
    Once the system memory boot mode is entered and the STM32 microcontroller  has been configured (for more details refer to AN2606) the bootloader 
    code begins to scan the USARTx_RX line pin, waiting to receive the 0x7F data frame: a 
    start bit, 0x7F data bits, even parity bit and a stop bit.
    The duration of this data frame is measured using the Systick timer. The count value of the 
    timer is then used to calculate the corresponding baud rate factor with respect to the current 
    system clock.
    Next, the code initializes the serial interface accordingly. Using this calculated baud rate, an 
    acknowledge byte (0x79) is returned to the host, which signals that the STM32 is ready to 
    receive commands.
    */
    bool  sync(unsigned intervaltime,unsigned int totaltime);

    /*
    The Get command allows the user to get the version of the bootloader and the supported 
    commands. When the bootloader receives the Get command, it transmits the bootloader 
    version and the supported command codes to the host
    */
    bool  get_command( unsigned int time, char &version, CommandStr& cmd);

    /*
    The Get Version & Read Protection Status command is used to get the bootloader version 
    and the read protection status. After receiving the command the bootloader transmits the 
    version, the read protection and number of times it was enabled and disabled to the host
    */
    bool  get_protection_status(unsigned int time, char &status, unsigned char& times);
    
    /*
    The Get ID command is used to get the version of the chip ID (identification). When the 
    bootloader receives the command, it transmits the product ID to the host
    */
    bool  get_chip_id_command(unsigned int time, unsigned short &PID);

    /*
    The Read Memory command is used to read data from any valid memory address (refer to 
    the product datasheets and to AN2606 for more details) in RAM, Flash memory and the 
    information block (system memory or option byte areas).
    When the bootloader receives the Read Memory command, it transmits the ACK byte to the 
    application. After the transmission of the ACK byte, the bootloader waits for an address (four 
    bytes, byte 1 is the address MSB and byte 4 is the LSB) and a checksum byte, then it 
    checks the received address. If the address is valid and the checksum is correct, the 
    bootloader transmits an ACK byte, otherwise it transmits a NACK byte and aborts the 
    command.
    When the address is valid and the checksum is correct, the bootloader waits for the number 
    of bytes to be transmitted ¨C 1 (N bytes) and for its complemented byte (checksum). If the 
    checksum is correct it then transmits the needed data ((N + 1) bytes) to the application, 
    starting from the received address. If the checksum is not correct, it sends a NACK before 
    aborting the command
    */
    bool  read_memory_command(unsigned int time,
                              unsigned int start_address,
                              unsigned char readsize,
                              char* read
    );

    /*
    The Go command is used to execute the downloaded code or any other code by branching 
    to an address specified by the application. When the bootloader receives the Go command, 
    it transmits the ACK byte to the application. After the transmission of the ACK byte, the 
    bootloader waits for an address (four bytes, byte 1 is the address MSB and byte 4 is LSB) 
    and a checksum byte, then it checks the received address. If the address is valid and the 
    checksum is correct, the bootloader transmits an ACK byte, otherwise it transmits a NACK 
    byte and aborts the command
    */
    bool  go_command(unsigned int time, unsigned int execute_address);

    /*
    The Write Memory command is used to write data to any valid memory address (see note 
    below) i.e. RAM, Flash memory, option byte area...
    When the bootloader receives the Write Memory command, it transmits the ACK byte to the 
    application. After the transmission of the ACK byte, the bootloader waits for an address (four 
    bytes, byte 1 is the address MSB and byte 4 is the LSB) and a checksum byte, it then 
    checks the received address. For the option byte area, the start address must be the base 
    address of the option byte area (see note) to avoid writing inopportunely in this area.
    If the received address is valid and the checksum is correct, the bootloader transmits an 
    ACK byte, otherwise it transmits a NACK byte and aborts the command. When the address 
    is valid and the checksum is correct, the bootloader:
    */
    bool  write_memory_command(unsigned int time, 
                               unsigned int startaddress,
                               char *perdata, 
                               unsigned int size);

    /*
    The Erase Memory command allows the host to erase Flash memory pages. When the 
    bootloader receives the Erase Memory command, it transmits the ACK byte to the host. 
    After the transmission of the ACK byte, the bootloader receives one byte (number of pages 
    to be erased), the Flash memory page codes and a checksum byte; if the checksum is 
    correct then bootloader erases the memory and sends an ACK byte to the host, otherwise it 
    sends a NACK byte to the host and the command is aborted
    */
    bool  erase_memory_command(unsigned int time, std::vector< char>pages);
    /*
      The Extended Erase Memory command allows the host to erase Flash memory pages using 
      two bytes addressing mode. When the bootloader receives the Extended Erase Memory 
      command, it transmits the ACK byte to the host. After the transmission of the ACK byte, the 
      bootloader receives two bytes (number of pages to be erased), the Flash memory page 
      codes (each one coded on two bytes, MSB first) and a checksum byte (XOR of the sent 
      bytes); if the checksum is correct, the bootloader erases the memory and sends an ACK 
      byte to the host. Otherwise it sends a NACK byte to the host and the command is aborted
    */
    bool  extended_erase_memory_command(unsigned int time,/*0 number pages others is page number*/ std::vector< short>pages);

    bool write_protect_command(unsigned int time, std::vector< char>sectors);

    bool write_unprotect_command(unsigned int time);


    //helpful fuc
    bool  write_memory_command_hp(unsigned int time,
        unsigned int startaddress,
        char* fwdata,
        unsigned int size, 
        unsigned int pkg_size
    );
private:

    const char* sync_name =           "Readyding  receive commands....";
    const char* Get_version_name =    "Get version....";
    const char* Get_Status_name =     "Get Version & Read Protection Status command....";
    const char* Get_chip_ID_name =    "Get chip ID....";
    const char* Read_Memory_name =    "Read Memory...";
    const char* Go_Command_name =     "Go Command ...";
    const char* Write_Memory_name =   "Write Memory....";
    const char* Erase_name =          "Erase Memory...";
    const char* ExtendedErase_name  = "Extended Erase Memory...";
    const char* Writeprotect_name   = "Write protect...";
    const char* WriteUnprotect_name = "Write Unprotect...";
    
    char SYNC_ = 0x7F;
    char ACK_  = 0x79;
    char NACK_ = 0x1F;
    unsigned char Max_Pages = 0xFF;
   
    constexpr static unsigned short Max_PKG_Size = 256;
    constexpr static unsigned short Send_Command_Size = 2;
};

#endif
/****************************************************************************

**
****************************************************************************/

/*==================================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN(yyyy/mm/dd)    WHO         WHAT, WHERE, WHY
------------        -------     ----------------------------------------------------
2020/06/11          wise.hu     original

===================================================================================*/


#ifndef LC79D_AGNSS_LTO_H
#define LC79D_AGNSS_LTO_H

#ifdef __cplusplus
extern "C" {
#endif

extern unsigned int  crc32_tab[] ;

#define STOP_BUTTON_STRING "&Stop"
#define SEND_BUTTON_STRING  "&Send"
#define CONNECT_BUTTON_STRING  "&Connect"
#define MMC_OUTPUT_BUTTON_STRING  "&Write"
#define NAND_OUTPUT_BUTTON_STRING  "&Program"
#define FLASH_OUTPUT_BUTTON_STRING  "&Program"
#define FILE_OUTPUT_BUTTON_STRING  "&Create"
#define ERASE_OUTPUT_BUTTON_STRING  "&Erase"
#define DUMP_OUTPUT_BUTTON_STRING  "&Dump"

#define FIRMWARE_FILE_EXTENSION_STRING   "LTO file (*.dat)|*.dat|All files  (*.*)|*.*"
#define FIRMWARE_TITLE_STRING   "Firmware file selection..."

#define IDLE_STRING   "Idle"
#define BUSY_STRING  "Busy"
#define INACTIVE_STRING  IDLE_STRING

 #define URI_SEPARATOR_STRING   "%20"
 #define SPACE_STRING           " "
 #define NULL_STRING            ""
 #define NEWLINE_STRING         "\n"
 #define BYTE_STRING            " bytes"
 #define DASH_STRING            "-"
 #define  DONE_STRING           "Done"
 #define   DETECTING_NMEA_BAUD_RATE              "Detecting NMEA baud rate..."
 #define   SYNCHRONIZING_WITH_DEVICE_STRING      "Synchronizing with target device..."
 #define   FWUPGRADE_COMMAND_STRING              "Sending LTO Download command..."
 #define   PREPARING_FOR_FLASH_STRING            "Initializing target device..."
 #define   PROGRAMMING_STRING                    "Programming target device..."
 #define   ERASING_PROGRAM_STRING                "Erasing LTO area..."
 #define   ERASING_NVM_STRING                    "Erasing NVM area..."
 #define   DUMPING_FLASH_STRING                  "Dumping flash device..."
 #define   DOTS_STRING                                    "..."
 #define   DEVICE_ERASED_FLASH_STRING            "Device successfully erased!"
 #define   DEVICE_SUCCESSFULLY_PROGRAMMED        "LTO successfully downloaded!"
 #define   DEVICE_SUCCESSFULLY_DUMPED            "Device successfully dumped!"
 #define   MSGBOX_INFORMATION_STRING             "Information"
 #define   MSGBOX_ERROR_STRING        "Error"
 #define   MSGBOX_WARNING_STRING      "Warning"
 #define   PROCESS_ABORT_STRING       "Process aborted"

 #define  NMEA_FWUPGRADE_COMMAND_STRING       "$PQLTODOWNLOAD\r\n"
 #define  NMEA_FWUPGRADE_RESPOND_STRING       "$PQLTODOWNLOADOK*46"
 #define  NMEA_FWUPGRADE_ERROR_STRING         "$PQLTODOWNLOADERROR"
 #define  NMEA_SW_VERSION_COMMAND_STRING      "$PSTMGETSWVER\r\n"
 #define  NMEA_SW_VERSION_RESPOND_STRING      "$PSTMVER"

 #define  ERROR_STRING                            "Error"
 #define  NMEA_BAUD_RATE_ERROR_STRING             "Error: NMEA baud rate not detected"
 #define  SERIAL_PORT_ERROR_STRING                "Error: No COM ports found"
 #define  USB_PORT_ERROR_STRING                   "Error: ST GNSS USB Receiver not found"
 #define  FWUPGRADE_COMMAND_ERROR_STRING          "Error: Firmware upgrade command is not valid"
 #define  NMEA_PORT_ERROR_STRING                  "Error: Firmware upgrade command failed. Check the NMEA port is valid"
 #define  SYNC_TIMEOUT_ERROR_STRING               "Error: Synchronization with target device has timed out"
 #define  UPGRADE_TIMEOUT_ERROR_STRING            "Error: Upgrade start has timed out"
 #define  CODE_NOT_ALIGNED_STRING                 "Error: Code must be word aligned (4 bytes)"
 #define  INVALID_CODE_SIZE_STRING                "Error: File size cannot be zero"
 #define  TARGET_NOT_RESPOND                      "Error: Communication error with target board or target device not present"
 #define  CHANGE_BAUD_RATE_ERROR_STRING           "Error: Change UART baud rate failed. Try with smaller value."
 #define  DEVICE_INIT_ERROR_STRING                "Error: Device initialization failed"
 #define  SERIAL_FLASH_PROGRAMMING_ERROR_STRING   "Error: Flash programming error"
 #define  SERIAL_FLASH_ERASING_ERROR_STRING       "Flash erase error"
 #define  SERIAL_FLASH_DUMPING_ERROR_STRING       "Error: Flash dump error"
 #define  SERIAL_FLASH_CRC_ERROR_STRING           "Error: CRC check failed"

#define DEVICE_NOT_FLASHED  0x0B
#define DEVICE_FLASHED  0x0A
#define DEVICE_DUMPED   0x0B
#define DEVICE_READY   0xC7
#define DEVICE_START_COMMUNICATION   0xA3
#define FLASHER_READY   0x4A
#define HOST_READY   0x5A
#define HOST_NOT_READY   0x5B
#define DEVICE_ACK   0xCC
#define DEVICE_NACK   0xDD

#define ERROR_CODE      0
#define NO_ERROR_CODE   1

#define BYTES_PER_SECTOR                512
#define FLASHER_BLOCK_SIZE       (16 * 1024)      // 16 KB
#define NVM_START_OFFSET       (1024 * 1024)      // 1 MB
#define NVM_FLASH_ERASE_SIZE    (384 * 1024)      // (320+64) KB


 #define  RETURN_STRING           "\n"
 #define  APPLICATION_NAME_STRING "LC79D LTO Download"
 #define  VERSION_STRING          "1.0"
 #define  SILICON_VERSION         "(Silicon version)"
 #define  FINAL_VERSION           ""
 #define  FPGA_VERSION            "(FPGA version)"
 #define  DEVELOPER_NAME          "Developed by engineer"
 #define  COMPANY_STRING          "2011 (c) Quectel"
 #define  TERMINAL_STRING         "- Terminal"
 #define  CDC_PORT_STRING         "CDC Port"
 #define  USB_RECEIVER_STRING     "GNSS USB Receiver"

 #define  APPLICATION_VERSION            FINAL_VERSION
 #define  APPLICATION_TITLE_STRING       APPLICATION_NAME_STRING  SPACE_STRING  VERSION_STRING  SPACE_STRING  APPLICATION_VERSION
 #define  INFO_APPLICATION_TITLE_STRING  APPLICATION_NAME_STRING  SPACE_STRING  VERSION_STRING  RETURN_STRING APPLICATION_VERSION
 #define TERMINAL_WINDOW_TITLE_STRING    APPLICATION_NAME_STRING  SPACE_STRING  VERSION_STRING  SPACE_STRING  TERMINAL_STRING

 typedef enum 
{
    GenericRead = 0x80000000,
    GenericWrite = 0x40000000,
    GenericExecute = 0x20000000,
    GenericAll = 0x10000000
}EFileAccess;

typedef enum  
{
    None = 0x00000000,
    /// Enables subsequent open operations on an object to request read access.
    /// Otherwise, other processes cannot open the object if they request read access.
    /// If this flag is not specified, but the object has been opened for read access, the function fails.
    Read = 0x00000001,
    /// Enables subsequent open operations on an object to request write access.
    /// Otherwise, other processes cannot open the object if they request write access.
    /// If this flag is not specified, but the object has been opened for write access, the function fails.
    Write = 0x00000002,
    /// Enables subsequent open operations on an object to request delete access.
    /// Otherwise, other processes cannot open the object if they request delete access.
    /// If this flag is not specified, but the object has been opened for delete access, the function fails.
    Delete = 0x00000004
}EFileShare;

typedef enum 
{
    /// Creates a new file. The function fails if a specified file exists.
    NewCreates = 1,
    /// Creates a new file, always.
    /// If a file exists, the function overwrites the file, clears the existing attributes, combines the specified file attributes,
    /// and flags with FILE_ATTRIBUTE_ARCHIVE, but does not set the security descriptor that the SECURITY_ATTRIBUTES structure specifies.
    CreateAlways = 2,
    /// Opens a file. The function fails if the file does not exist.
    OpenExisting = 3,
    /// Opens a file, always.
    /// If a file does not exist, the function creates a file as if dwCreationDisposition is CREATE_NEW.
    OpenAlways = 4,
    /// Opens a file and truncates it so that its size is 0 (zero) bytes. The function fails if the file does not exist.
    /// The calling process must open the file with the GENERIC_WRITE access right.
    TruncateExisting = 5
}ECreationDisposition;
    
typedef enum 
{
    Readonly = 0x00000001,
    Hidden = 0x00000002,
    System = 0x00000004,
    Directory = 0x00000010,
    Archive = 0x00000020,
    Device = 0x00000040,
    Normal = 0x00000080,
    Temporary = 0x00000100,
    SparseFile = 0x00000200,
    ReparsePoint = 0x00000400,
    Compressed = 0x00000800,
    Offline = 0x00001000,
    NotContentIndexed = 0x00002000,
    Encrypted = 0x00004000,
    Write_Through = 0x80000000,
    Overlapped = 0x40000000,
    NoBuffering = 0x20000000,
    RandomAccess = 0x10000000,
    SequentialScan = 0x08000000,
    DeleteOnClose = 0x04000000,
    BackupSemantics = 0x02000000,
    PosixSemantics = 0x01000000,
    OpenReparsePoint = 0x00200000,
    OpenNoRecall = 0x00100000,
    FirstPipeInstance = 0x00080000
}EFileAttributes;

typedef struct
{
    unsigned int     Size;
     unsigned int CRC;
}LTO_Options;

extern unsigned int calculate_ether_crc32(unsigned int crc32val, unsigned int integer);
#ifdef __cplusplus
}
#endif

#endif  /* #ifndef LC79D_AGNSS_LTO_H */


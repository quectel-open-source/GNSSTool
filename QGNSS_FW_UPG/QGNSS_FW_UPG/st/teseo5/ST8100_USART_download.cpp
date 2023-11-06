#include "ST8100_USART_download.h"

ST8100_USART_download::ST8100_USART_download()
{

}

ST8100_USART_download::~ST8100_USART_download()
{

}

bool ST8100_USART_download::sync(unsigned intervaltime, unsigned int totaltime)
{
  QGINFO(sync_name); QGPRINT(QG_ENDL);
  clear();
  auto start = std::chrono::steady_clock::now();
  bool sync_flag = false;
  do
  {
    if (!write_response({ &SYNC_,1,&ACK_,1,intervaltime }))
    {
      int mis = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();
      int progress = QG_get_progress(totaltime,mis , 100);
      QGPROGRESS_FMT(sync_name, 100,"", progress);
    }
    else
    {
      int progress = QG_get_progress(totaltime, totaltime, 100);
      QGPROGRESS_FMT(sync_name, 100,"", progress);
      sync_flag = true;
      break;
    }
  } while (QG_overtime(start, totaltime)&&!get_stop_flag());
  if (!sync_flag)
  {
    QGERROR("sync fail" QG_ENDL);
  }
  return sync_flag;
}

bool ST8100_USART_download::get_command(unsigned int time, char& version, CommandStr& cmd)
{
  QGINFO(Get_version_name); QGPRINT(QG_ENDL);
  char send[Send_Command_Size] {0};
  send[0] = Command::Get;
  send[1] = Command::Get ^ 0xFF;
  if (!write_response({ send,Send_Command_Size,&ACK_,1,time }))
  {
    std::string hexstr;
    str2hex(std::string(send, Send_Command_Size), hexstr);
    QGERROR("[NACK] Command:%s ..." QG_ENDL, hexstr.data());
    return false;
  }
  //[] the number of bytes to follow – 1
  char numberbytes[1]{0};
  auto size = read(numberbytes, 1, time);
  if (!size)
  {
    QGERROR("[Read error] get %d bytes..." QG_ENDL,size);
    return false;
  }
  if ((unsigned char)numberbytes[0]!=sizeof(CommandStr))
  {
    QGERROR("[Read error] number of bytes %d unequal %d  bytes..." QG_ENDL,(int)numberbytes[0], sizeof(CommandStr));
    return false;
  }
  //[] Bootloader version
     size = read(&version, 1, time);
  if (!size)
  {
    QGERROR("[Read error] get %d bytes..." QG_ENDL, size);
    return false;
  }
  //[] Command
  char* cmdc = reinterpret_cast<char*>(&cmd);
  size=read(cmdc, sizeof(CommandStr), time);
  if (!size)
  {
    QGERROR("[Read error] get %d bytes..." QG_ENDL, size);
    return false;
  }
  //[] ack
  char ack[1]{ 0 };
  size= read(ack, 1, time);
  if ((unsigned char)ack[0] != ACK_)
  {
    QGERROR("NACK!" QG_ENDL);
    return false;
  }
  return true;
}

bool ST8100_USART_download::get_protection_status(unsigned int time,  char& status, unsigned char& times)
{
  QGINFO(Get_version_name); QGPRINT(QG_ENDL);
  char send[Send_Command_Size]{ 0 };
  send[0] = Command::GetVersionAndReadProtectionStatus;
  send[1] = Command::GetVersionAndReadProtectionStatus ^ 0xFF;
  if (!write_response({ send,Send_Command_Size,&ACK_,1,time }))
  {
    std::string hexstr;
    str2hex(std::string(send, Send_Command_Size), hexstr);
    QGERROR("[NACK] Command:%s ..." QG_ENDL, hexstr.data());
    return false;
  }
  //[] Bootloader version(0 < Version ≤ 255), example: 0x10 = Version 1.0
  char version[1]{ 0 };
  auto size=read(version, 1, time);
  char versionstr[20];
  int v = version[0] >> 4;
  int vl = version[0] << 4;
  sprintf(versionstr,"%d.%d",v, vl);
  QGINFO("Bootloader version %s" QG_ENDL, versionstr);
  if (!size)
  {
    QGERROR("[Read error] get %d bytes..." QG_ENDL, size);
    return false;
  }
  // read protection
  size=read(&status, 1, time);
  if (!size)
  {
    QGERROR("[Read error] get %d bytes..." QG_ENDL, size);
    return false;
  }
  //number of times it was enabled and disabled to the host.
  char ts = (char)times;
  size = read(&ts, 1, time);
  if (!size)
  {
    QGERROR("[Read error] get %d bytes..." QG_ENDL, size);
    return false;
  }
  times = (unsigned char)ts;
  //[] ack
  char ack[1]{ 0 };
  size = read(ack, 1, time);
  if ((unsigned char)ack[0] != ACK_)
  {
    QGERROR("NACK!" QG_ENDL);
    return false;
  }
  return true;
}

bool ST8100_USART_download::get_chip_id_command(unsigned int time, unsigned short& PID)
{
  QGINFO(Get_chip_ID_name); QGPRINT(QG_ENDL);
  char send[Send_Command_Size]{ 0 };
  send[0] = Command::GetID;
  send[1] = Command::GetID ^ 0xFF;
  if (!write_response({ send,Send_Command_Size,&ACK_,1,time }))
  {
    std::string hexstr;
    str2hex(std::string(send, Send_Command_Size), hexstr);
    QGERROR("[NACK] Command:%s ..." QG_ENDL, hexstr.data());
    return false;
  }
  //[]the number of bytes – 1 (N = 1 for STM32), except for current byte and ACKs.
  char numberbytes{ 0 };
  auto size=read(&numberbytes, 1, time);
  if (!size)
  {
    QGERROR("[Read error] get %d bytes..." QG_ENDL, size);
    return false;
  }
  //PID
  char* pidc = reinterpret_cast<char*>(&PID);
  if (isLittleEndian())
  {
    QG_reverse<unsigned short, char>(PID);
  }
  size = read(pidc, 2, time);
  if (!size)
  {
    QGERROR("[Read error] get %d bytes..." QG_ENDL, size);
    return false;
  }
  //[] ack
  char ack{ 0 };
  size = read(&ack, 1, time);
  if (ack != ACK_)
  {
    QGERROR("NACK!" QG_ENDL);
    return false;
  }
  return true;
}

bool ST8100_USART_download::read_memory_command(unsigned int time,
                                                unsigned int start_address,
                                                unsigned char readsize,
                                                char* readdata
                                                )
{
  QGINFO(Read_Memory_name); QGPRINT(QG_ENDL);
  char send[Send_Command_Size]{ 0 };
  send[0] = Command::ReadMemory;
  send[1] = Command::ReadMemory ^ 0xFF;
  if (!write_response({ send,Send_Command_Size,&ACK_,1,time }))
  {
    std::string hexstr;
    str2hex(std::string(send, Send_Command_Size), hexstr);
    QGERROR("[NACK] Command:%s ..." QG_ENDL, hexstr.data());
    return false;
  }
  //[] Start address byte 3: MSB, byte 6: LSB Checksum: XOR (byte 3, byte 4, byte 5, byte 6
  char* startaddress = reinterpret_cast<char*>(&start_address);
  if (isLittleEndian())
  {
    QG_reverse<unsigned int, char>(start_address);
  }
  char xor_data = QG_XOR_CheckSum(startaddress,sizeof(unsigned int));
  char sendstart[5]{ 0 };
  memcpy(sendstart, startaddress, sizeof(unsigned int));
  memcpy(sendstart+ sizeof(unsigned int), &xor_data, 1);
  if (!write_response({ sendstart,5,&ACK_,1,time }))
  {
    std::string hexstr;
    str2hex(std::string(sendstart, Send_Command_Size), hexstr);
    QGERROR("[NACK] Command:%s ..." QG_ENDL, hexstr.data());
    return false;
  }
  //[] send number of bytes to be read and a checksum
  send[0] = readsize;
  send[1] = readsize ^ 0xFF;
  if (!write_response({ send,Send_Command_Size,&ACK_,1,time }))
  {
    std::string hexstr;
    str2hex(std::string(send, Send_Command_Size), hexstr);
    QGERROR("[NACK] Command:%s ..." QG_ENDL, hexstr.data());
    return false;
  }
  //read memory
  auto size= read(readdata, readsize+1, time);
  if (size!= readsize + 1)
  {
    QGERROR("[Read error] get %d bytes less than %d" QG_ENDL, size, readsize);
    return false;
  }
  return true;
}

bool ST8100_USART_download::go_command(unsigned int time,
                                       unsigned int execute_address)
{
  QGINFO(Go_Command_name); QGPRINT(QG_ENDL);
  char send[Send_Command_Size]{ 0 };
  send[0] = Command::Go;
  send[1] = Command::Go ^ 0xFF;
  if (!write_response({ send,Send_Command_Size,&ACK_,1,time }))
  {
    std::string hexstr;
    str2hex(std::string(send, Send_Command_Size), hexstr);
    QGERROR("[NACK] Command:%s ..." QG_ENDL, hexstr.c_str ());
    return false;
  }
  //[]send start address with checksum
  char* startaddress = reinterpret_cast<char*>(&execute_address);
  if (isLittleEndian())
  {
    QG_reverse<unsigned int, char>(execute_address);
  }
  char xor_data = QG_XOR_CheckSum(startaddress, sizeof(unsigned int));
  char sendstart[5]{ 0 };
  memcpy(sendstart, startaddress, sizeof(unsigned int));
  memcpy(sendstart + sizeof(unsigned int), &xor_data, 1);
  if (!write_response({ sendstart,5,&ACK_,1,time }))
  {
    std::string hexstr;
    str2hex(std::string(send, Send_Command_Size), hexstr);
    QGERROR("[NACK] Command:%s ..." QG_ENDL, hexstr.c_str ());
    return false;
  }
  return true;
}

bool ST8100_USART_download::write_memory_command(unsigned int time,
                                                 unsigned int startaddress_,
                                                 char* data, unsigned int size
                                                 )
{

  if (size > Max_PKG_Size)
  {
    QGERROR("Write size %d big than %d ..." QG_ENDL, size, Max_PKG_Size);
    return false;
  }
  char send[Send_Command_Size]{ 0 };
  send[0] = Command::WriteMemory;
  send[1] = Command::WriteMemory ^ 0xFF;
  if (!write_response({ send,Send_Command_Size,&ACK_,1,time }))
  {
    std::string hexstr;
    str2hex(std::string(send, Send_Command_Size), hexstr);
    QGERROR("[NACK] Command:%s ..." QG_ENDL, hexstr.c_str ());
    return false;
  }
  //[]
    char* startaddress = reinterpret_cast<char*>(&startaddress_);
  if (isLittleEndian())
  {
    QG_reverse<unsigned int, char>(startaddress_);
  }
  char xor_data = QG_XOR_CheckSum(startaddress, sizeof(unsigned int));
  char sendstart[5]{ 0 };
  memcpy(sendstart, startaddress, sizeof(unsigned int));
  memcpy(sendstart + sizeof(unsigned int), &xor_data, 1);
  if (!write_response({ sendstart,5,&ACK_,1,time }))
  {
    std::string hexstr;
    str2hex(std::string(send, Send_Command_Size), hexstr);
    QGERROR("[NACK] Command:%s ..." QG_ENDL, hexstr.c_str ());
    return false;
  }
  //send the  number of bytes to be written(1bytes) the data(N+1)and checksum
  unsigned int sendsize = size;
  char* send_data = new char[sendsize + 2] {0};
  send_data[0] = sendsize-1;
  memcpy(send_data+1, data, sendsize);
  char xorc = QG_XOR_CheckSum(send_data, sendsize+1);
  send_data[sendsize + 1] = xorc;
  //write(send_data, sendsize + 2);
  if (!write_response({ send_data,sendsize + 2,&ACK_,1,time }))
  {
    QGERROR("NACK!" QG_ENDL);
    return false;
  }
  return true;
}

bool ST8100_USART_download::erase_memory_command(unsigned int time,
                                                 std::vector< char> pages)
{
  QGINFO(Erase_name); QGPRINT(QG_ENDL);
  char send[Send_Command_Size]{ 0 };
  send[0] = Command::Erase;
  send[1] = Command::Erase ^ 0xFF;
  if (!write_response({ send,Send_Command_Size,&ACK_,1,time }))
  {
    std::string hexstr;
    str2hex(std::string(send, Send_Command_Size), hexstr);
    QGERROR("[NACK] Command:%s ..." QG_ENDL, hexstr.c_str ());
    return false;
  }
  //
  if (pages.size() == Max_Pages)
  {
    send[0] = 0xFF;
    send[1] = 0x00;
    if (!write_response({ send,Send_Command_Size,&ACK_,1,time }))
    {
      std::string hexstr;
      str2hex(std::string(send, Send_Command_Size), hexstr);
      QGERROR("[NACK] Command:%s ..." QG_ENDL, hexstr.c_str ());
      return false;
    }
    return true;
  }
  char pageSize = pages.size();
  write(&pageSize, 1);
  write(pages.data(), pageSize);
  char xorc = QG_XOR_CheckSum(pages.data(), pageSize);
  if (!write_response({ &xorc,1,&ACK_,1,time }))
  {
    std::string hexstr;
    str2hex(std::string(send, Send_Command_Size), hexstr);
    QGERROR("[NACK] Command:%s ..." QG_ENDL, hexstr.data());
    return false;
  }
  return false;
}

bool ST8100_USART_download::extended_erase_memory_command(unsigned int time,
                                                          std::vector<short> pages)
{
  QGINFO(ExtendedErase_name); QGPRINT(QG_ENDL);
  char send[Send_Command_Size]{ 0 };
  send[0] = Command::ExtendedErase;
  send[1] = Command::ExtendedErase ^ 0xFF;
  if (!write_response({ send,Send_Command_Size,&ACK_,1,time }))
  {
    std::string hexstr;
    str2hex(std::string(send, Send_Command_Size), hexstr);
    QGERROR("[NACK] Command:%s ..." QG_ENDL, hexstr.data());
    return false;
  }
  //
  char xorc = 0;
  short esize = 0;
  switch (pages[0])
  {
    case 0xFFFF:
    {
        xorc = 0x00;
      esize = 0xFFFF;
      break;
    }
    case 0xFFFE:
    {
      xorc = 0x01;
      esize = 0xFFFE;
      break;
    }
    case 0xFFFD:
    {
      xorc = 0x02;
      esize = 0xFFFD;
      break;
    }
    default:
      esize = pages[0];
      break;
  }
  char* sizec = nullptr;
  std::vector<char> erasepages;
  if (isLittleEndian())
  {
    sizec = QG_reverse<short, char>(esize);
  }
  sizec = reinterpret_cast<char*>(&esize);
  erasepages.push_back(sizec[0]);
  erasepages.push_back(sizec[1]);
  for (auto i = pages.begin() + 1; i != pages.end(); i++)
  {
    short p = *i;
    char* page = nullptr;
    if (isLittleEndian())
    {
      page = QG_reverse<short, char>(p);
    }
    else
    {
      page = reinterpret_cast<char*>(&p);
    }
    erasepages.push_back(page[0]);
    erasepages.push_back(page[1]);
  }
  xorc = QG_XOR_CheckSum(erasepages.data() , erasepages.size() );
  erasepages.push_back(xorc);
  if (!write_response({ erasepages.data(),(unsigned int)erasepages.size(),&ACK_,1,time}))
  {
    QGERROR("NACK!" QG_ENDL);
    return false;
  }
  return true;
}

bool ST8100_USART_download::write_protect_command(unsigned int time, std::vector< char>sectors)
{
  QGINFO(Writeprotect_name); QGPRINT(QG_ENDL);
  char send[Send_Command_Size]{ 0 };
  send[0] = Command::WriteProtect;
  send[1] = Command::WriteProtect ^ 0xFF;
  if (!write_response({ send,Send_Command_Size,&ACK_,1,time }))
  {
    std::string hexstr;
    str2hex(std::string(send, Send_Command_Size), hexstr);
    QGERROR("[NACK] Command:%s ..." QG_ENDL, hexstr.data());
    return false;
  }
  //
  char sectors_size = sectors.size();
  write(&sectors_size-1,1);//0<=N<=255
  write(sectors.data(), sectors.size());
  char checksum = QG_XOR_CheckSum(sectors.data(), sectors.size());
  if (!write_response({ &checksum,1,&ACK_,1,time }))
  {
    QGERROR("NACK!" QG_ENDL);
    return false;
  }
  return true;
}

bool ST8100_USART_download::write_unprotect_command(unsigned int time)
{
  QGINFO(WriteUnprotect_name); QGPRINT(QG_ENDL);
  char send[Send_Command_Size]{ 0 };
  send[0] = Command::WriteUnprotect;
  send[1] = Command::WriteUnprotect ^ 0xFF;
  if (!write_response({ send,Send_Command_Size,&ACK_,1,time }))
  {
    std::string hexstr;
    str2hex(std::string(send, Send_Command_Size), hexstr);
    QGERROR("[NACK] Remove the protection for the entire flash memory fail! (cmd:%s)" QG_ENDL, hexstr.c_str ());
    return false;
  }
  //[]
  char ack[1]{ 0 };
  read(ack, 1, time);
  if(ack[0]!=ACK_)
  {
    QGERROR("[NACK] Generate system reset fail!" QG_ENDL);
    return false;
  }
  return true;
}

bool ST8100_USART_download::write_memory_command_hp(unsigned int time,
                                                    unsigned int startaddress,
    char* fwdata,
    unsigned int size,
                                                    unsigned int pkg_size)
{
  QGINFO(Write_Memory_name); QGPRINT(QG_ENDL);
  int total_send_size = 0;
  bool flag = true;
  int per_pkg_size_ = pkg_size;
  do
  {
    auto start = std::chrono::steady_clock::now();
    per_pkg_size_ = pkg_size;
    if (size - total_send_size < pkg_size)
    {
      per_pkg_size_ = size - total_send_size;
    }
    char* send_data = new char[per_pkg_size_] {0};
    memcpy(send_data, fwdata + total_send_size, per_pkg_size_);
    if (!write_memory_command(time, startaddress, send_data, per_pkg_size_))
    {
      flag = false;
      break;
    }
    total_send_size += per_pkg_size_;
    startaddress += per_pkg_size_;
    int mis = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();
    int progress = QG_get_progress(size, total_send_size, 100);
    float rate=((float)per_pkg_size_/(mis/1000.0))/1000;//
    auto rate_s= std::to_string (std::round (rate*10.0)/10);
    char rate_fmt[1024];
    sprintf (rate_fmt,"%d Bytes/%d Bytes    %s KBps",total_send_size,size,rate_s.c_str ());
    QGPROGRESS_FMT(Write_Memory_name, 100,rate_fmt, progress);
  } while (total_send_size< size&& !get_stop_flag());
  if(get_stop_flag()&&total_send_size< size)
  {
    flag=false;
  }
  return flag;
}

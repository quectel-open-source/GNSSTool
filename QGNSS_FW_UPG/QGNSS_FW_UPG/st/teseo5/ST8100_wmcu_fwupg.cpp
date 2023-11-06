#include "ST8100_wmcu_fwupg.h"

ST8100_wmcu_fwupg::ST8100_wmcu_fwupg()
{
}

void ST8100_wmcu_fwupg::default_fw_info( gnss_fw_info& expert_config)
{

  expert_config.destaddr = 0x10000000u;
  expert_config.entrypoint = 0x00000400u;
  expert_config.nvm_offset = 0x000ff000u;
  expert_config.nvm_erase_size = 0x00101000u;
  expert_config.erasenvm = 0x01u;
  expert_config.eraseonly = 0x00u;
  expert_config.programonly = 0x00u;
  memset(expert_config.reversed, 0, ARRAY_SIZE(expert_config.reversed, unsigned char));

}

void ST8100_wmcu_fwupg::default_fw_info( app_fw_info& expert_config)
{
  expert_config.destaddr = 0x00000000u;
  expert_config.erasenvm = 0x01u;
  memset(expert_config.reversed, 0, ARRAY_SIZE(expert_config.reversed, unsigned char));
}

bool ST8100_wmcu_fwupg::connect_module(unsigned int downloadbaudrate)
{
  QGINFO(connect_module_name); QGPRINT(QG_ENDL);
  if (!is_open())
        {
    QGERROR("%s Not Open!" QG_ENDL,serialport_info->port_name.data ());
    return false;
  }
  set_baudrate(downloadbaudrate);
  return true;
}

bool ST8100_wmcu_fwupg::synchronize_module(unsigned int totaltimeout, unsigned int intervaltime)
{
  QGINFO(synchronize_module_name); QGPRINT(QG_ENDL);
  auto  start=std::chrono::steady_clock::now();
  char* sync1 = reinterpret_cast<char*>(&sync_word[0]);
  char* rsp_word1 = reinterpret_cast<char*>(&rsp_word[0]);

  char* sync2 = reinterpret_cast<char*>(&sync_word[1]);
  char* rsp_word2 = reinterpret_cast<char*>(&rsp_word[1]);
  bool sync_success = false;
  clear();
  auto startime = std::chrono::steady_clock::now();
  do
  {

    if (write_response({ sync1, sizeof(unsigned int) ,rsp_word1 ,sizeof(unsigned int),intervaltime }))
    {
      int progress = QG_get_progress(totaltimeout, totaltimeout, 100);
      QGPROGRESS_FMT(synchronize_module_name, 100,"", progress);
      sync_success = true;
      break;
    }
    else
    {
      int mis = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - startime).count();
      int progress = QG_get_progress(totaltimeout,mis , 100);
      QGPROGRESS_FMT(synchronize_module_name, 100,"", progress);
    }
  } while (QG_overtime(start, totaltimeout)&& !get_stop_flag());
  if (!sync_success)
  {
    QGERROR("sync fail: send sync1 %X not response %X !" QG_ENDL, sync_word[0], rsp_word[0]);
    return false;
  }
  if (write_response({ sync2, sizeof(unsigned int) ,rsp_word2 ,sizeof(unsigned int),intervaltime }))
  {
    sync_success = true;
  }
  else
  {
    QGERROR("sync fail: send sync1 %X not response %X !" QG_ENDL, sync_word[1], rsp_word[1]);
    sync_success = false;
  }
  return sync_success;
}

bool ST8100_wmcu_fwupg::query_the_bootloaer_version(ClassID id, unsigned int timeout, msg_pkg_response& pkgresponse)
{
  QGINFO(query_the_bootloaer_version_name); QGPRINT(QG_ENDL);
  //int payload = 0x00000000;
  //char* payloadc = reinterpret_cast<char*>(&payload);
  msg_pkg pkg{ 0xAA,id ,MsgID::send_fw_address,0,nullptr };
  auto data = pkg.data();
  write(data.c_str(), data.size());
  char response[response_size]{ 0 };
  int responsesize= read(response, response_size, timeout);
  if (responsesize != response_size)
  {
    std::string hexstr;
    str2hex(std::string(response, responsesize), hexstr);
    QGERROR("Read size %d less than %d read:%s" QG_ENDL, responsesize, response_size, hexstr.c_str());
    return false;
  }
  constexpr  int status_size = 7;
  char status[status_size]{ 0 };
  memcpy(status, response + 5, status_size);
  pkgresponse = *reinterpret_cast<msg_pkg_response*>(status);
  if(isLittleEndian())
    QG_reverse<unsigned short, char>(pkgresponse.status);//to big end
  if (Response_Status.find(pkgresponse.status) == std::end(Response_Status))
  {
    std::string hexstr;
    str2hex(std::string(response, responsesize), hexstr);
    QGERROR("Response error: %s" QG_ENDL, hexstr.c_str());
    return false;
  }
  else if (pkgresponse.status != 0x0000)
  {
    QGERROR("%s" QG_ENDL, Response_Status[pkgresponse.status]);
    return false;
  }
  return true;
}

bool ST8100_wmcu_fwupg::send_firmware_address(ClassID id, unsigned int timeout)
{
  QGINFO(send_firmware_address_name); QGPRINT(QG_ENDL);
  int payload = 0x00000000;
  char* payloadc = reinterpret_cast<char*>(&payload);
  msg_pkg pkg{ 0xAA,id ,MsgID::send_fw_address,0x0004,payloadc };
  auto data = pkg.data();
  write(data.c_str(), data.size());
  char response[response_size]{ 0 };
  int responsesize = read(response, response_size, timeout);
  if (responsesize != response_size)
  {
    std::string hexstr;
    str2hex(std::string(response, responsesize), hexstr);
    QGERROR("Read size %d less than %d read:%s" QG_ENDL, responsesize, response_size, hexstr.c_str());
    return false;
  }
  constexpr  int status_size = 7;
  char status[status_size]{ 0 };
  memcpy(status, response + 5, status_size);
  auto pkgresponse = *reinterpret_cast<msg_pkg_response*>(status);
  if (isLittleEndian())
    QG_reverse<unsigned short, char>(pkgresponse.status);//to big end
  if (Response_Status.find(pkgresponse.status) == std::end(Response_Status))
  {
    std::string hexstr;
    str2hex(std::string(response, responsesize), hexstr);
    QGERROR("Response error: %s" QG_ENDL, hexstr.c_str());
    return false;
  }
  else if (pkgresponse.status != 0x0000)
  {
    QGERROR("%s" QG_ENDL, Response_Status[pkgresponse.status]);
    return false;
  }
  return true;
}

bool ST8100_wmcu_fwupg::send_Firmware_Information(ClassID id, char* payload,unsigned short payloadsize, unsigned int timeout)
{
  QGINFO(Send_Firmware_Information_name); QGPRINT(QG_ENDL);
  msg_pkg pkg{ 0xAA,id ,MsgID::send_fw_information,payloadsize,payload };
  auto data = pkg.data();
  write(data.c_str(), data.size());
  char response[response_size]{ 0 };
  int responsesize = read(response, response_size, timeout);
  if (responsesize != response_size)
  {
    std::string hexstr;
    str2hex(std::string(response, responsesize), hexstr);
    QGERROR("Read size %d less than %d read:%s" QG_ENDL, responsesize, response_size, hexstr.c_str());
    return false;
  }
  constexpr  int status_size = 4;
  char status[sizeof(msg_pkg_response)]{0};
  memcpy(status, response + 5, status_size);
  auto pkgresponse = *reinterpret_cast<msg_pkg_response*>(status);
  if (isLittleEndian())
    QG_reverse<unsigned short, char>(pkgresponse.status);//to big end
  if (Response_Status.find(pkgresponse.status) == std::end(Response_Status))
  {
    std::string hexstr;
    str2hex(std::string(response, responsesize), hexstr);
    QGERROR("Response error: %s" QG_ENDL, hexstr.c_str());
    return false;
  }
  else if (pkgresponse.status != 0x0000)
  {
    QGERROR("%s" QG_ENDL, Response_Status[pkgresponse.status]);
    return false;
  }
  return true;
}

bool ST8100_wmcu_fwupg::send_Start_Firmware_Upgrade_Command(ClassID id, unsigned int timeout)
{
  QGINFO(Send_Start_Firmware_Upgrade_Command_name); QGPRINT(QG_ENDL);
  msg_pkg pkg{ 0xAA,id ,MsgID::start_fw_upg,0x0000,nullptr };
  auto data = pkg.data();
  write(data.c_str(), data.size());
  char response[response_size]{ 0 };
  int responsesize = read(response, response_size, timeout);
  if (responsesize != response_size)
  {
    std::string hexstr;
    str2hex(std::string(response, responsesize), hexstr);
    QGERROR("Read size %d less than %d read:%s" QG_ENDL, responsesize, response_size, hexstr.c_str());
    return false;
  }
  constexpr  int status_size = 4;
  char status[status_size]{ 0 };
  memcpy(status, response + 5, status_size);
  auto pkgresponse = *reinterpret_cast<msg_pkg_response*>(status);
  if (isLittleEndian())
    QG_reverse<unsigned short, char>(pkgresponse.status);//to big end
  if (Response_Status.find(pkgresponse.status) == std::end(Response_Status))
  {
    std::string hexstr;
    str2hex(std::string(response, responsesize), hexstr);
    QGERROR("Response error: %s" QG_ENDL, hexstr.c_str());
    return false;
  }
  else if (pkgresponse.status != 0x0000)
  {
    QGERROR("%s" QG_ENDL, Response_Status[pkgresponse.status]);
    return false;
  }
  return true;
}

bool ST8100_wmcu_fwupg::send_Firmware(	ClassID id, char* fwdata,
                                      unsigned int fwsize,
                                      unsigned int timeout, unsigned int pkgsize)
{
  QGINFO(Send_Firmware_name); QGPRINT(QG_ENDL);
  auto sendfunc = [=]( std::string &fwpkg) ->bool
  {
    msg_pkg pkg{ 0xAA,id ,MsgID::send_fw_pkg,(unsigned short)fwpkg.size(),fwpkg.data()};
    auto data = pkg.data();
    write(data.c_str(), data.size());
    char response[response_size]{ 0 };
    int responsesize = read(response, response_size, timeout);
    if (responsesize != response_size)
    {
      std::string hexstr;
      str2hex(std::string(response, responsesize), hexstr);
      QGERROR("Read size %d less than %d read:%s" QG_ENDL, responsesize, response_size, hexstr.c_str());
      return false;
    }
    constexpr  int status_size = 4;
    char status[status_size]{ 0 };
    memcpy(status, response + 5, status_size);
    auto pkgresponse = *reinterpret_cast<msg_pkg_response*>(status);
    if (isLittleEndian())
      QG_reverse<unsigned short, char>(pkgresponse.status);//to big end
    if (Response_Status.find(pkgresponse.status) == std::end(Response_Status))
    {
      std::string hexstr;
      str2hex(std::string(response, responsesize), hexstr);
      QGERROR("Response error: %s" QG_ENDL, hexstr.c_str());
      return false;
    }
    else if (pkgresponse.status != 0x0000)
    {
      QGERROR("%s" QG_ENDL, Response_Status[pkgresponse.status]);
      return false;
    }
    return true;
  };
  int totalsendsize = 0;
  int persize = pkgsize;
  bool scs = true;
  int pkg_index = 0;
  while (totalsendsize< fwsize&&!get_stop_flag())
  {
    auto start = std::chrono::steady_clock::now();
    persize = fwsize - totalsendsize < pkgsize ? fwsize - totalsendsize : pkgsize;
    int index = pkg_index;
    char* pkg_index_c = isLittleEndian() ? QG_reverse<int, char>(index) : reinterpret_cast<char*>(&index);
    std::string payload;
    payload.append(pkg_index_c, sizeof(int));
    payload.append(fwdata + totalsendsize, persize);
    if (!sendfunc(payload))
    {
      scs=false;
      break;
    }
    pkg_index++;
    totalsendsize += persize;
    int mis = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();
    int progress = QG_get_progress(fwsize, totalsendsize, 100);
    float rate=((float)persize/(mis/1000.0))/1000;//
    auto rate_s= std::to_string (std::round (rate*10.0)/10);
    char rate_fmt[1024];
    sprintf (rate_fmt,"%d Bytes/%d Bytes    %s KBps",totalsendsize,fwsize,rate_s.c_str ());
    QGPROGRESS_FMT(Send_Firmware_name, 100,rate_fmt, progress);
  }
  if(get_stop_flag()&&totalsendsize< fwsize)
  {

    scs=false;
  }
  return scs;
}

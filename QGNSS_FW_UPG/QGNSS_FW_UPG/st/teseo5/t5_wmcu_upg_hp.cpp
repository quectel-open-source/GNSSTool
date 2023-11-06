#include "t5_wmcu_upg_hp.h"

#define CMDCLASSID			't'
#define CMDENABLEQUIETMODEC 'q'
#define CMDBINFILE			'f'
#define CMDPORTNAME			'c'
#define CMDBAUDRATE			'b'
#define CMDRTSDTR			'l'
#define CMDDESTADDRESS		'd'
#define CMDENTRYPOINT		'e'
#define CMDNVMOFFSET		'o'
#define CMDNVMERASESIZE		's'
#define CMDERASRONLY		'r'
#define CMDPROGRAMONLY		'p'

#define CMDMAXSIZE			2

t5_wmcu_upg::t5_wmcu_upg()
{

}

t5_wmcu_upg::~t5_wmcu_upg()
{

}

bool t5_wmcu_upg::get_fw_infor_payload(ST8100_wmcu_fwupg::ClassID id, std::vector<char> &payload)
{
  firmware_data_.clear();
  if (!params_.file_path)
  {
    return false;
  }
  int fwsize =QG_file_size(params_.file_path.value().data ());

  QG_Read_File(params_.file_path.value().data (), "rb", fwsize, firmware_data_);
  //get firmware crc
  fwsize = firmware_data_.size();
  int fwcrc32 = crc32_eval(0u, &fwsize, sizeof(int));
  fwcrc32 = crc32_eval(fwcrc32, firmware_data_.data(), firmware_data_.size());
  switch (id)
  {
    case ST8100_wmcu_fwupg::ClassID::GNSS:
    case ST8100_wmcu_fwupg::ClassID::SGNSS:
    {
      ST8100_wmcu_fwupg::gnss_fw_info fwinfo;
      t5_upg_.default_fw_info(fwinfo);
      fwinfo.fw_size = firmware_data_.size();
      fwinfo.fw_crc32 = fwcrc32;
      if (params_.destaddr)
      {
        fwinfo.destaddr = params_.destaddr.value();
      }
      if (params_.entrypoint)
      {
        fwinfo.entrypoint = params_.entrypoint.value();
      }
      if (params_.nvm_offset)
      {
        fwinfo.nvm_offset = params_.nvm_offset.value();
      }
      if (params_.nvm_erase_size)
      {
        fwinfo.nvm_erase_size = params_.nvm_erase_size.value();
      }
      if (params_.erasenvm)
      {
        fwinfo.erasenvm = params_.erasenvm.value();
      }
      if (params_.eraseonly)
      {
        fwinfo.eraseonly = params_.eraseonly.value();
      }
      if (params_.programonly)
      {
        fwinfo.programonly = params_.programonly.value();
      }
      char *payloaddata= fwinfo.Get_chr(fwsize, fwcrc32);
      payload.insert(payload.begin(), payloaddata, payloaddata+sizeof(ST8100_wmcu_fwupg::gnss_fw_info));
    }
    break;
    case ST8100_wmcu_fwupg::ClassID::APP:
    case ST8100_wmcu_fwupg::ClassID::SAPP:
    {
      ST8100_wmcu_fwupg::app_fw_info fwinfo;
      t5_upg_.default_fw_info(fwinfo);
      fwinfo.fw_size = firmware_data_.size();
      fwinfo.fw_crc32 = fwcrc32;
      if (params_.destaddr)
      {
        fwinfo.destaddr = params_.destaddr.value();
      }
      if (params_.erasenvm)
      {
        fwinfo.erasenvm = params_.erasenvm.value();
      }
      char* payloaddata = fwinfo.Get_chr(fwsize, fwcrc32);
      payload.insert(payload.begin(), payloaddata, payloaddata + sizeof(ST8100_wmcu_fwupg::app_fw_info));
    }
    break;
  }
  return true;
}

bool t5_wmcu_upg::params_parse(int args, char* arg[])
{
  int arg_index = 0;
  do
  {
    std::string_view argstr{ arg[arg_index] };
    //check size(cmd only two char -x)
    if (argstr.size() != CMDMAXSIZE)
    {
      arg_index++;
      continue;
    }
    char start_c = argstr[0];
    char cmd_c = argstr[1];
    if (start_c == CMDSTARTCHAR)
    {
      arg_index++;
      std::string_view cmd_value{ arg[arg_index] };
      switch (cmd_c)
      {
        case CMDCLASSID:
        {
          if (QG_Str_Compare(cmd_value,"GNSS"))
          {
            params_.classid_ = ST8100_wmcu_fwupg::ClassID::GNSS;
          } else
              if (QG_Str_Compare(cmd_value, "APP"))
          {
            params_.classid_ = ST8100_wmcu_fwupg::ClassID::APP;
          } else
              if (QG_Str_Compare(cmd_value, "SGNSS"))
          {
            params_.classid_ = ST8100_wmcu_fwupg::ClassID::SGNSS;
          } else
              if (QG_Str_Compare(cmd_value, "SAPP"))
          {
            params_.classid_ = ST8100_wmcu_fwupg::ClassID::SAPP;
          }
          else
          {
            QGERROR("class id %s not exist! ", cmd_value.data());
            return false;
          }
          break;
        }
        case CMDENABLEQUIETMODEC:
        {
          if (QG_Str_Compare(cmd_value, "TRUE"))
          {
            params_.quit_mode = true;
          } else
              if (QG_Str_Compare(cmd_value, "FALSE"))
          {
            params_.quit_mode = false;
          }
          else
          {
            QGERROR("quit mode not exist!:  %s  ", cmd_value.data());
            return false;
          }
        }
        break;
        case CMDBINFILE:
        {
          if (!std::filesystem::exists(cmd_value))
          {
            QGERROR("firmware not exist!:  %s  ", cmd_value.data());
            return false;
          }
          params_.file_path = cmd_value.data();
        }
        break;
        case CMDPORTNAME:
        {
          std::vector<SerialPortInfo> portinfos;
          QG_Uart::get_serialportinfos(portinfos);
          auto find= std::find_if(portinfos.cbegin(), portinfos.cend(), [&]( SerialPortInfo info)
                                   {
                                     return info.port_name == cmd_value;
                                   });
          if (find != std::end(portinfos))
          {
            if (!params_.serialportinfo)
            {
              params_.serialportinfo =new  SerialPortInfo();
            }
            params_.serialportinfo->port_name = find->port_name;
            params_.serialportinfo->port_description = find->port_description;
          }
          else
          {
            QGERROR("port name not exist!:  %s  ", cmd_value.data());
            return false;
          }
        }
        break;
        case CMDBAUDRATE:
        {
          uint32_t baudrate = std::stoi(cmd_value.data());
          if (!params_.serialportinfo)
          {
            params_.serialportinfo =new  SerialPortInfo();
          }
          params_.serialportinfo->baudrate = baudrate;
        }
        break;
        case CMDRTSDTR:
          break;
        case CMDDESTADDRESS:
          break;
        case CMDENTRYPOINT:
          break;
        case CMDNVMOFFSET:
          break;
        case CMDNVMERASESIZE:
          break;
        case CMDERASRONLY:
          break;
        case CMDPROGRAMONLY:
          break;
        default:
        {
          QGERROR("cmd %s not exist!!", arg[args]);
        }
        break;
      }
    }
  } while (arg_index < args);
  return true;
}

bool t5_wmcu_upg::set_params(const t5_upg_params& params)
{
  params_ = params;
  return true;
}

bool t5_wmcu_upg::exec()
{
  bool success_=true;
  do{
    if (!params_.serialportinfo)
    {
      QGERROR("serialport not config!");
      success_=false;
      break;
    }
    //uart
    if (params_.serialportinfo->uart_handle!= INVALID_HANDLE_VALUE)
    {
      if (!t5_upg_.set_native(params_.serialportinfo->uart_handle))
      {
        success_=false;
        break;
      }
      t5_upg_.set_serialport_info(*params_.serialportinfo);
    }
    else
    {
      t5_upg_.set_serialport_info(*params_.serialportinfo);
      if (!t5_upg_.is_open ()&&!t5_upg_.open())
      {
        success_=false;
        break;
      }
    }
    t5_upg_.clear ();
    if (!t5_upg_.connect_module(params_.serialportinfo->baudrate))
    {
      success_=false;
      break;
    }
    t5_upg_.clear ();
    if (!t5_upg_.synchronize_module(sync_total_time, sync_interval))
    {
      success_=false;
      break;
    }
    /*
    ST8100_wmcu_fwupg::msg_pkg_response msgreponse;
    if (!t5_upg_.query_the_bootloaer_version(params_->classid_.value(), max_read_timeout, msgreponse))
    {
     return false;
    }
    */
    if (!t5_upg_.send_firmware_address(params_.classid_.value(), max_read_timeout))
    {
      success_=false;
      break;
    }

    std::vector<char> payload;
    if (!get_fw_infor_payload(params_.classid_.value(),
                              payload))
    {
      success_=false;
      break;
    }

    if (!t5_upg_.send_Firmware_Information(params_.classid_.value(),
                                           payload.data(),
                                           payload.size(),
                                           max_read_timeout))
    {
      success_=false;
      break;
    }
    if (!t5_upg_.send_Start_Firmware_Upgrade_Command(params_.classid_.value(), upgcmd_read_timeout))
    {
      success_=false;
      break;
    }
    int fw_size = firmware_data_.size();
    char* fw_data = firmware_data_.data();
    if (!t5_upg_.send_Firmware(params_.classid_.value(), fw_data, fw_size,max_read_timeout,per_pkg_size))
    {
      success_=false;
      break;
    }
    QGINFO("Success..." QG_ENDL);
    break;
  }while (true);
  t5_upg_.close ();
  return success_;
}

void t5_wmcu_upg::stop()
{
  t5_upg_.stop_upg (true);
}


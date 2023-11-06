#include "t5_wmcu_download_hp.h"

t5_wmcu_download_hp::t5_wmcu_download_hp()
{
}

t5_wmcu_download_hp::~t5_wmcu_download_hp()
{
}

bool t5_wmcu_download_hp::params_parse(int args, char* arg[])
{
  return true;
}

bool  t5_wmcu_download_hp::set_params(const t5_dwd_params& params)
{
  params_ = params;
  return true;
}

bool t5_wmcu_download_hp::exec()
{
  bool success_=true;
  do {
    if (!params_.serialportinfo)
    {
      QGERROR("serialport not config!" QG_ENDL);
      success_=false;
      break;
    }
    //uart
    if (params_.serialportinfo->uart_handle!= INVALID_HANDLE_VALUE)
    {
      if (!st_uart_dwd_.set_native(params_.serialportinfo->uart_handle))
      {
        success_=false;
        break;
      }
      st_uart_dwd_.set_serialport_info(*params_.serialportinfo);
    }
    else
    {
      st_uart_dwd_.set_serialport_info(*params_.serialportinfo);
      if (!st_uart_dwd_.is_open ()&&!st_uart_dwd_.open())
      {
        success_=false;
        break;
      }
    }

    if (!params_.file_path)
    {
      QGERROR("firmware path empty!" QG_ENDL);
      success_=false;
      break;
    }
    if (!std::filesystem::exists(params_.file_path.value()))
    {
      QGERROR("firmware path not exists!" QG_ENDL);
      success_=false;
      break;
    }
    st_uart_dwd_.clear();
    //[]bootloader code sequence
    if (!st_uart_dwd_.sync(sync_interval, sync_total_time))
    {
      success_=false;
      break;
    }
    //[]
    ST8100_USART_download::CommandStr cmdstr;
    char version;
    if (!st_uart_dwd_.get_command(max_read_timeout, version, cmdstr))
    {
      success_=false;
      break;
    }
    //[]
    unsigned short chipid;
    if (!st_uart_dwd_.get_chip_id_command(max_read_timeout, chipid))
    {
      success_=false;
      break;
    }
    //[]
    std::string read_memory_;
    read_memory_.resize(params_.read_number_bytes.value());
    if (!st_uart_dwd_.read_memory_command(max_read_timeout,
                                          params_.read_start_address.value(),
                                          params_.read_number_bytes.value(),
                                          read_memory_.data())
        )
    {
      success_=false;
      break;
    }
    //[]
    if (!st_uart_dwd_.write_unprotect_command(max_read_timeout))
    {
      success_=false;
      break;
    }
    //[]
    std::vector<short> pages;
    pages.resize(params_.erease_pages.value() + 1);
    pages[0]=(params_.erease_pages.value() - 1);
    int i = 0;
    std::generate_n(++pages.begin(), params_.erease_pages.value(), [&]() {return i++; });
    if (!st_uart_dwd_.extended_erase_memory_command(max_read_timeout, pages))
    {
      success_=false;
      break;
    }
    //[]
    read_memory_.resize(params_.read_number_bytes.value());
    if (!st_uart_dwd_.read_memory_command(max_read_timeout,
                                          params_.read_start_address.value(),
                                          params_.read_number_bytes.value(),
                                          read_memory_.data())
        )
    {
      success_=false;
      break;
    }
    //get fw
    int fwsize = QG_file_size(params_.file_path.value().data (), "rb");
    QG_Read_File(params_.file_path.value().data (), "rb", fwsize, fwdata_);
    //[] write memory
    if (!st_uart_dwd_.write_memory_command_hp(max_read_timeout,
                                              params_.write_start_address.value(),
                                              fwdata_.data(),
                                              fwsize,
                                              per_pkg_size
                                              ))
    {
      success_=false;
      break;
    }
    QGINFO("Success..."  QG_ENDL);
    break;
  } while (true);
  st_uart_dwd_.close ();
  return success_;
}

void t5_wmcu_download_hp::stop()
{
    st_uart_dwd_.stop_upg (true);
}

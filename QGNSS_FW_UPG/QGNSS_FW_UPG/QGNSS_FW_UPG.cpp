#include "QGNSS_FW_UPG.h"
#include "QG_Uart.h"
#include <cstdio>

using namespace std;

#define TESTUART 1
void testuart(std::string name, int baudrate)
{
	SerialPortInfo info;
	info.baudrate = baudrate;
	info.port_name = name;
	QG_Uart uart(info);
	uart.open();
	constexpr int cachesize = 4096;
	char buf[cachesize+1]{ 0 };
	while (true)
	{
		std::size_t readsize= uart.read(buf, cachesize, 0);
		buf[readsize] = '\0';
		QGINFO("read data %d :\r\n%s" QG_ENDL, readsize, buf);
	}
}
#include "t5_wmcu_upg_hp.h"
#define TEST_T5_UPG 0
void test_t5_upg(std::string name, int baudrate)
{
	t5_wmcu_upg t5upg;
	t5_wmcu_upg::t5_upg_params params;
	params.serialportinfo = new SerialPortInfo;
	SerialPortInfo info;
	info.baudrate = baudrate;
	info.port_name = name;
	*params.serialportinfo = info;
	params.classid_ = ST8100_wmcu_fwupg::ClassID::APP;
	params.file_path = R"(F:\Victor_Project\QGNSS_FlashTool\flashtools\flashTool\FlashTool_exe_v4.1.0\ST\ST\STA8100\LG69T\LG69TAP\Customer\QianXun\Release firmware\LG69TAPNR01A08V01_QX\LG69TAPNR01A08V01_QX_UPG.bin)";
	t5upg.set_params(params);
	t5upg.exec();
}
#include "t5_wmcu_download_hp.h"
#define TEST_T5_DWN 0
void test_t5_dwn(std::string name, int baudrate)
{
	t5_wmcu_download_hp t5dwd;
	t5_wmcu_download_hp::t5_dwd_params params;
	params.serialportinfo = new SerialPortInfo;
	SerialPortInfo info;
	info.baudrate = baudrate;
	info.port_name = name;
	info.parity = EvenParity;
	*params.serialportinfo = info;
	params.file_path = R"(F:\Victor_Project\QGNSS_FlashTool\flashtools\flashTool\FlashTool_exe_v4.1.0\ST\ST\STA8100\LG69T\LG69TAP\Customer\QianXun\Release firmware\LG69TAPNR01A08V01_QX\LG69TAPNR01A08V01_QX_BOOT.bin)";
	t5dwd.set_params(params);
	t5dwd.exec();
}
int main()
{
#if TESTUART //test uart
	testuart("COM5", 460800);
#endif

#if TEST_T5_UPG //
	test_t5_upg("COM5", 460800);
#endif 

#if TEST_T5_DWN
	test_t5_dwn("COM5", 460800);
#endif // TEST_T5_DWN
	int x = 0;
	do
	{
		x = getchar();
		printf("%c", x);
	} while (x != '\n');
	return 0;
}

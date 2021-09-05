// Reference : https://github.com/cheungxiongwei/WMIC

#include "wmic.h"

int main(int argc, char *argv[])
{
	try
	{
		WMIC w;
		w.OperatingSystem();
		w.VideoController();
		w.DiskDrive();
		w.BaseBoard();
		w.BIOS();
		w.PhysicalMemory();
		w.Processor();
		w.NetworkAdapter();
	}
	catch (const WMIC_ERROR& e)
	{
		std::cout << e.what() << std::endl;
	}

	return 0;
}

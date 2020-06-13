#pragma once


#include <cstdint>
#include <string>

#include <WinSock2.h>
#include <Windows.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>


#ifdef STDLLEXPORT
#define ST_DLL __declspec(dllexport)
#else
#define ST_DLL __declspec(dllimport)
#endif
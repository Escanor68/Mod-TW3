#pragma once

// Windows configuration to avoid conflicts
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

// Include Windows headers in correct order
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

// Link with required libraries
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "advapi32.lib")
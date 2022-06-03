// header.h: включаемый файл для стандартных системных включаемых файлов
// или включаемые файлы для конкретного проекта
//

#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // Исключите редко используемые компоненты из заголовков Windows
#define _WINSOCK_DEPRECATED_NO_WARNINGS
// Файлы заголовков Windows
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")
#include <WinSock2.h>
#include <iphlpapi.h>
#include <windows.h>
// Файлы заголовков среды выполнения C
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <vector>

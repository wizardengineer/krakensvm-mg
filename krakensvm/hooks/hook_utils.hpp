/* This file is part of krakensvm-mg by medievalghoul, licensed under the MIT license:
*
* MIT License
*
* Copyright (c) medievalghoul 2021
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/

#pragma once

#include <stdint.h>
#include <utility>
//#include <windef.h>

typedef  struct _KSYSTEM_SERVICE_TABLE
{
  uint64_t* ServiceTableBase;								// The base address of the service function address table   
  uint64_t* ServiceCounterTableBase; 				// The number of times the SSDT function is called 
  uint64_t  NumberOfService; 								// The number of   service functions PULONG 
  char8_t*  ParamTableBase; 						    // The base address of the service function parameter table    
} KSYSTEM_SERVICE_TABLE, * PKSYSTEM_SERVICE_TABLE;

typedef  struct _KSERVICE_TABLE_DESCRIPTOR
{
  KSYSTEM_SERVICE_TABLE NTOSKRNL;           // Ntoskrnl.exe service function   
  KSYSTEM_SERVICE_TABLE Win32k;             // Win32k.sys service function (kernel supports GDI32.dll/User32.dll a)   
  KSYSTEM_SERVICE_TABLE notUsed1;
  KSYSTEM_SERVICE_TABLE notUsed2;
} KSERVICE_TABLE_DESCRIPTOR, * PKSERVICE_TABLE_DESCRIPTOR;

namespace utils
{
  uint64_t get_kernelbase_addr     ();
  auto get_service_descriptor_table() -> PKSERVICE_TABLE_DESCRIPTOR;
};

/* This file is part of mg-hypervisor by medievalghoul, licensed under the MIT license:
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

#include <wdm.h>
#include <ntddk.h>
#include <krakensvm.hpp>

static void driver_unloading(PDRIVER_OBJECT driver_object);

EXTERN_C
NTSTATUS driver_entry(
	_In_ PDRIVER_OBJECT driver_object,
	_In_ PUNICODE_STRING registry_path
)
{
//  PUNICODE_STRING driver_name    ,
//                  dos_device_name;

  registry_path;
	KdPrint(("The Driver Entry \n"));

//  RtlInitUnicodeString(driver_name,     L"\\Device\\KrakenSvm");
//  RtlInitUnicodeString(dos_device_name, L"\\DosDevices\\KrakenSvm");

	driver_object->DriverUnload = driver_unloading;
  svm::svm_enabling();
	return STATUS_SUCCESS;
}

static void driver_unloading(PDRIVER_OBJECT driver_object)
{
  driver_object;

	KdPrint(("driver unloading\n"));
}

// 242, 585

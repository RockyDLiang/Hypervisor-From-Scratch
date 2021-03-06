#include "stdafx.h"
#include <Windows.h>
#include <conio.h>
#include <iostream>  

using namespace std;

string GetCpuid()
{
	char SysType[13]; // Array consisting of 13 single bytes/characters
	string CpuID; // The string that will be used to add all the characters toStarting coding in assembly language 

	_asm
	{
		//Execute CPUID with EAX = 0 to get the CPU producer
		xor eax, eax
		cpuid
		
		//MOV EBX to EAX and get the characters one by one by using shift out right bitwise operation.
		mov eax, ebx
		mov SysType[0], al
		mov SysType[1], ah
		shr eax, 16
		mov SysType[2], al
		mov SysType[3], ah

		//Get the second part the same way but these values are stored in EDX
		mov eax, edx
		mov SysType[4], al
		mov SysType[5], ah
		shr EAX, 16
		mov SysType[6], al
		mov SysType[7], ah

		//Get the third part
		mov eax, ecx
		mov SysType[8], al
		mov SysType[9], ah
		SHR EAX, 16
		mov SysType[10], al
		mov SysType[11], ah
		mov SysType[12], 00
	}

	CpuID.assign(SysType, 12);

	return CpuID;
}


bool VmxSupportDetection()
{

	bool VMX;

	VMX = false;

	__asm {

		xor    eax, eax
		inc    eax
		cpuid
		bt     ecx, 0x5
		jc     VMXSupport

		VMXNotSupport :
		jmp     NopInstr

		VMXSupport :
		mov    VMX, 0x1

		NopInstr :
		nop
	}

	return VMX;
}

void PrintAppearance() {

	printf("\n"


		"    _   _                             _                  _____                      ____                 _       _     \n"
		"   | | | |_   _ _ __   ___ _ ____   _(_)___  ___  _ __  |  ___| __ ___  _ __ ___   / ___|  ___ _ __ __ _| |_ ___| |__  \n"
		"   | |_| | | | | '_ \\ / _ \\ '__\\ \\ / / / __|/ _ \\| '__| | |_ | '__/ _ \\| '_ ` _ \\  \\___ \\ / __| '__/ _` | __/ __| '_ \\ \n"
		"   |  _  | |_| | |_) |  __/ |   \\ V /| \\__ \\ (_) | |    |  _|| | | (_) | | | | | |  ___) | (__| | | (_| | || (__| | | |\n"
		"   |_| |_|\\__, | .__/ \\___|_|    \\_/ |_|___/\\___/|_|    |_|  |_|  \\___/|_| |_| |_| |____/ \\___|_|  \\__,_|\\__\\___|_| |_|\n"
		"          |___/|_|                                                                                                     \n"
		"\n\n");
}

int main()
{
	string CpuID;
	DWORD ErrorNum;
	HANDLE Handle;

	// Print Hypervisor From Scratch Message
	PrintAppearance();

	CpuID = GetCpuid();

	printf("[*] The CPU Vendor is : %s \n", CpuID.c_str());

	if (CpuID == "GenuineIntel")
	{
		printf("[*] The Processor virtualization technology is VT-x. \n");
	}
	else
	{
		printf("[*] This program is not designed to run in a non-VT-x environemnt !\n");
		return 1;
	}

	if (VmxSupportDetection())
	{
		printf("[*] VMX Operation is supported by your processor .\n");
	}
	else
	{
		printf("[*] VMX Operation is not supported by your processor .\n");
		return 1;
	}

	 Handle = CreateFile("\\\\.\\MyHypervisorDevice",
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ |
		FILE_SHARE_WRITE,
		NULL, /// lpSecurityAttirbutes
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL |
		FILE_FLAG_OVERLAPPED,
		NULL); /// lpTemplateFile 

	if (Handle == INVALID_HANDLE_VALUE)
	{
		ErrorNum = GetLastError();
		printf("[*] CreateFile failed : %d\n", ErrorNum);
		return 1;

	}

	printf("\n[*] Press any key to terminate the VMX operation...\n");

	_getch();

	printf("[*] Terminating VMX !\n");

	// Send IRP_MJ_CLOSE to driver to terminate Vmxs
	CloseHandle(Handle);

	return 0;
}


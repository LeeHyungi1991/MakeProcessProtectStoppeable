#include <Windows.h>
#include "WinIoCtl.h"
#include <stdio.h>
#include <Strsafe.h>

#define SIOCTL_TYPE 40000
#define AUTHENTICATION_CODE "KXyBI79u+65gTGNk2ZyamN/IOZBcTLN7dmJux5JekW4="

// The IOCTL function codes from 0x800 to 0xFFF are for customer use.
#define IOCTL_HELLO CTL_CODE(SIOCTL_TYPE, 0x800, METHOD_BUFFERED, FILE_READ_DATA | FILE_WRITE_DATA)
int main()
{
    HANDLE hDevice;
    char* welcome = AUTHENTICATION_CODE;
    DWORD dwBytesRead = 0;
    char ReadBuffer[50] = { 0 };

    hDevice = CreateFile(L"\\\\.\\MyDevice", GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    //printf("Handle : %p\n", hDevice);

    DeviceIoControl(hDevice, IOCTL_HELLO, welcome, strlen(welcome), ReadBuffer, sizeof(ReadBuffer), &dwBytesRead, NULL);
    //printf("Message received from kerneland : %s\n", ReadBuffer);
    //printf("Bytes read : %d\n", dwBytesRead);

    CloseHandle(hDevice);
	return 0;
}
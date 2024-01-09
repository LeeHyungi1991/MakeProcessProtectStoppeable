#include <Windows.h>
#include "WinIoCtl.h"
#include <stdio.h>
#include <Strsafe.h>

// Device Type
#define SIOCTL_TYPE 40000

// 프로세스 보호 드라이버를 종료할 때 쓰이는 base64 인증코드
#define AUTHENTICATION_CODE "KXyBI79u+65gTGNk2ZyamN/IOZBcTLN7dmJux5JekW4="

//I/O Control .
//1번째 파라미터: Device Type : I/O Control 코드가 사용되는 디바이스 장치 유형
//2번째 파라미터: Function : 구체적인 수행 코드를 분류 (0~4095 [0~2047 : 마이크로소프트에서 예약]/[2047(0x800)~4095(0xFFF) : 사용가능])
//3번째 파라미터: Method : 유저 어플리케이션과 디바이스간 통신을 할 때 버퍼를 어떻게 할 것인가를 정의
//4번째 파라미터: Access : 해당하는 I/O Control Code와 같이 사용되는 버퍼의 용도를 정의
#define IOCTL_HELLO CTL_CODE(SIOCTL_TYPE, 0x800, METHOD_BUFFERED, FILE_READ_DATA | FILE_WRITE_DATA)
int __cdecl main(int argc, char* argv[])
{
	const WCHAR deviceSymLinkBuffer[] = L"\\DosDevices\\MyDevice";
	HANDLE hDevice;
	char* code = AUTHENTICATION_CODE; // 인증코드 정규화
	DWORD dwBytesRead = 0; // 커널로부터 받는 메세지의 비트수를 체크하기 위한 4바이트 DWORD 변수
	char ReadBuffer[50] = { 0 }; // 커널로부터 받는 메세지를 담기 위한 버퍼

	// 커널과 통신할 때 쓰이는 I/O 디바이스를 심볼릭 링크로 조회하여 오픈하고 해당 디바이스의 핸들을 리턴
	// 5번째 파라이터에 OPEN_EXISTING를 넘겨서 해당 I/O 디바이스가 존재할때만
	// 정상적인 핸들을 리턴하게 된다.
	// 현재 통신 대상이 되는 드라이버는 로드할 때 "\\DosDevices\\MyDevice"에 해당하는 심볼릭 링크를 생성하게 된다.

	hDevice = CreateFile(deviceSymLinkBuffer, GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	printf("Handle : %p\n", hDevice);
	if (hDevice == INVALID_HANDLE_VALUE)
	{
		return 1;
	}

	// 커널에 인증코드 문자열을 송신함과 동시에 ReadBuffer에 커널로부터 수신된 문자열을 담는다.
	DeviceIoControl(hDevice, IOCTL_HELLO, code, strlen(code), ReadBuffer, sizeof(ReadBuffer), &dwBytesRead, NULL);
	printf("Message received from kerneland : %s\n", ReadBuffer);
	printf("Bytes read : %d\n", dwBytesRead);

	CloseHandle(hDevice); // 이제 필요없어진 핸들은 닫는다.
	return 0;
}
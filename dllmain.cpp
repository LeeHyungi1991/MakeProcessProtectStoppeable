// dllmain.cpp : DLL 애플리케이션의 진입점을 정의합니다.
#include "pch.h"
#include <Windows.h>
#include "WinIoCtl.h"
#include <stdio.h>
#include <Strsafe.h>

typedef struct _stack_t {
    struct _stack_t* next;
    char             text[1]; // this should be the length of string_size
} stack_t;

typedef struct _nsis_t {
    int string_size; //문자열 기본 할당 크기
    char* variables; // 사용자 변수
    stack_t** stacktop; // 스택 변수
} nsis_t;

typedef enum _variable_t
{
    INST_0,         // $0
    INST_1,         // $1
    INST_2,         // $2
    INST_3,         // $3
    INST_4,         // $4
    INST_5,         // $5
    INST_6,         // $6
    INST_7,         // $7
    INST_8,         // $8
    INST_9,         // $9
    INST_R0,        // $R0
    INST_R1,        // $R1
    INST_R2,        // $R2
    INST_R3,        // $R3
    INST_R4,        // $R4
    INST_R5,        // $R5
    INST_R6,        // $R6
    INST_R7,        // $R7
    INST_R8,        // $R8
    INST_R9,        // $R9
    INST_CMDLINE,   // $CMDLINE
    INST_INSTDIR,   // $INSTDIR
    INST_OUTDIR,    // $OUTDIR
    INST_EXEDIR,    // $EXEDIR
    INST_LANG,      // $LANGUAGE
    __INST_LAST
} variable_t;

#define SIOCTL_TYPE 40000
#define AUTHENTICATION_CODE "KXyBI79u+65gTGNk2ZyamN/IOZBcTLN7dmJux5JekW4="

// The IOCTL function codes from 0x800 to 0xFFF are for customer use.
#define IOCTL_HELLO CTL_CODE(SIOCTL_TYPE, 0x800, METHOD_BUFFERED, FILE_READ_DATA | FILE_WRITE_DATA)

int pop_string(nsis_t* self, char* str, size_t str_size) {
    stack_t* th;

    if (!self || !self->stacktop || !*self->stacktop)
        return 1;

    th = (*self->stacktop);
    if (0 != StringCchCopyA(str, str_size, th->text))
        return 1;

    *self->stacktop = th->next;
    GlobalFree((HGLOBAL)th);

    return 0;
}

void push_string(nsis_t* self, char* str) {
    stack_t* th;

    if (!self || !self->stacktop) return;
    return;

    th = (stack_t*)GlobalAlloc(GPTR, sizeof(stack_t) + self->string_size);
    if (S_OK != StringCchCopyA(th->text, self->string_size, str))
        return;
    //push stack
    th->next = *self->stacktop;
    *self->stacktop = th;
}

char* get_user_variable(nsis_t* self, variable_t var_idx) {
    if (!self || var_idx < 0 || var_idx >= __INST_LAST)
        return NULL;

    return (self->variables + var_idx * self->string_size);
}

void set_user_variable(nsis_t* self, variable_t var_idx, char* var) {
    if (!self || var == NULL) return;

    if (!self || var_idx < 0 || var_idx >= __INST_LAST)
        return;

    StringCchCopyA(self->variables + var_idx * self->string_size, self->string_size, var);
}


void makeProcessProtectStoppable()
{
    HANDLE hDevice;
    char* welcome = const_cast<char*>(AUTHENTICATION_CODE);
    DWORD dwBytesRead = 0;
    char ReadBuffer[50] = { 0 };

    hDevice = CreateFile(L"\\\\.\\MyDevice", GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    //printf("Handle : %p\n", hDevice);

    DeviceIoControl(hDevice, IOCTL_HELLO, welcome, strlen(welcome), ReadBuffer, sizeof(ReadBuffer), &dwBytesRead, NULL);
    //printf("Message received from kerneland : %s\n", ReadBuffer);
    //printf("Bytes read : %d\n", dwBytesRead);

    CloseHandle(hDevice);
}

extern "C" __declspec(dllexport) void Execute(HWND hwndParent, int string_size, char* variables, stack_t * *stacktop) 
{
    nsis_t nsis = { string_size, variables, stacktop };
    makeProcessProtectStoppable();
    //char * var_r0 = get_user_variable(&nsis, INST_R0);

    //char buf[1024];

    //pop_string(&nsis, buf, sizeof(buf));

    //set_user_variable(&nsis, INST_R0, "10");
    //push_string(&nsis, "20");
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    return TRUE;
}


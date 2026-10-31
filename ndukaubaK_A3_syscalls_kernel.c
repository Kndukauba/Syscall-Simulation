/*
* Operating Systems
* Date: April 5, 2026
* Description: Simulated Kernel process that sends responses to the User via Connected Named Pipes
*/

#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define PIPE_NAME TEXT("\\\\.\\pipe\\SyscallPipe") 
#define BUFFER_SIZE 512

// Syscall Function Prototypes

void Get_System_Time(char* param, char* response);
void Get_Random_Number(char* param, char* response);
void Square_Number(char* param, char* response);
void toUpperCase(char* param, char* response); 

// Jump Table Execution 

typedef void (*SyscallFunc)(char*, char*);

SyscallFunc jump_table[] = {
NULL,
Get_System_Time,
Get_Random_Number,
Square_Number,
toUpperCase
};

// Struct Defination

struct kernel_function {
	HANDLE hpipe;
	DWORD bytesWritten, bytesRead;
	char buffer[BUFFER_SIZE];
};

// Kernel Function Operation

void kernel_operation(struct kernel_function* Mycall) {

	Mycall->hpipe = CreateNamedPipe(PIPE_NAME, PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, 1, BUFFER_SIZE, BUFFER_SIZE, 0, NULL);

	if (Mycall->hpipe == INVALID_HANDLE_VALUE) {
		printf("[KERNEL] Failed to establish connection %ld\n", GetLastError());
		return;
	}

	if (ConnectNamedPipe(Mycall->hpipe, NULL) != FALSE) {
		if (ReadFile(Mycall->hpipe, Mycall->buffer, BUFFER_SIZE, &Mycall->bytesRead, NULL)) {
			Mycall->buffer[Mycall->bytesRead] = '\0';

			int funcCode = 0;
			char param[BUFFER_SIZE] = { 0 };
			char response[BUFFER_SIZE] = { 0 };
			char finalMessage[BUFFER_SIZE] = { 0 };

			sscanf(Mycall->buffer, "%d %[^\n]", &funcCode, param);
			printf("[KERNEL] Received Request: Code %d, Data: %s\n", funcCode, param);

			if (funcCode >= 1 && funcCode <= 4) {
				jump_table[funcCode](param, response);
				sprintf(finalMessage, "Success: %s", response);
			}
			else {
				sprintf(finalMessage, "Error: Invalid Function Code %d", funcCode);
			}
			WriteFile(Mycall->hpipe, finalMessage, (DWORD)strlen(finalMessage) + 1, &Mycall->bytesWritten, NULL);
		}
	}
	DisconnectNamedPipe(Mycall->hpipe);
	CloseHandle(Mycall->hpipe);
}

// Syscall Function Operations

void Get_System_Time(char* param, char* response) {
	time_t now = time(NULL);
	struct tm* t = localtime(&now);
	strftime(response, 100, "%Y-%m-%d %H:%M:%S", t);
}

void Get_Random_Number(char* param, char* response) {
	srand((unsigned int)time(NULL));
	sprintf(response, "%d", rand() % 100);
}

void Square_Number(char* param, char* response) {
	int val = atoi(param);
	sprintf(response, "%d", val * val);  
}

void toUpperCase(char* param, char* response) {
	int i = 0;
	while (param[i]) {
		response[i] = (char)toupper((unsigned char)param[i]);
		i++;
	}
	response[i] = '\0';
}

// Main Function 

int main() {
	struct kernel_function Mycall;
	printf("[KERNEL] Waiting for Syscall requests...\n");
	while (1) {
		kernel_operation(&Mycall);
	}
	return 0;
}
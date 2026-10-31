/*
* Operating Systems
* Date: April 3, 2026
* Description: Simulated User process that sends request to the Kernel via Named Pipes
*/

#define _CRT_SECURE_NO_WARNINGS
#include <windows.h> 
#include <stdio.h>s
#include <string.h>

#define PIPE_NAME TEXT("\\\\.\\pipe\\SyscallPipe")
#define BUFFER_SIZE 512  

// Struct Defination

struct function {

	HANDLE hPipe;
	DWORD bytesWritten, bytesRead;
	char response[BUFFER_SIZE];
	char request[BUFFER_SIZE];
	char param[BUFFER_SIZE];
	int choice;
};

// User Input Function

void user_input(struct function* Mycall) {

	printf("1. Get the System Time\n");
	printf("2. Get a Random Number\n");
	printf("3. Square a Number\n");
	printf("4. Convert String to UpperCase\n");
	printf("5. Exit\n");
	printf("Select a function code");
	scanf("%d", &Mycall->choice);
	getchar();

	Mycall->param[0] = '\0';

	if (Mycall->choice == 3) {
		printf("Enter a Number");
		fgets(Mycall->param, BUFFER_SIZE, stdin);
	}
	else if (Mycall->choice == 4) {
		printf("Enter a String");
		fgets(Mycall->param, BUFFER_SIZE, stdin); 
	} 
}

// Kernel Connection Function

void syscall(struct function* Mycall) {

	Mycall->hPipe = CreateFile(PIPE_NAME, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

	if (Mycall->hPipe != INVALID_HANDLE_VALUE) {

		sprintf(Mycall->request, "%d %s", Mycall->choice, Mycall->param);

		WriteFile(Mycall->hPipe, Mycall->request, (DWORD)strlen(Mycall->request) + 1, &Mycall->bytesWritten, NULL);

		if (ReadFile(Mycall->hPipe, Mycall->response, BUFFER_SIZE, &Mycall->bytesRead, NULL)) {

			printf("[KERNEL RESPONSE]: %s\n", Mycall->response);
		}

		CloseHandle(Mycall->hPipe);
	}
	else {
		printf("[ERROR] Kernel not found. Ensure the kernel process is running.\n");
	}

}

// Main Function

int main() {

	struct function Mycall;
	while (1) {
		user_input(&Mycall);
		if (Mycall.choice == 5) break;
		syscall(&Mycall);
	}

	return 0;
}




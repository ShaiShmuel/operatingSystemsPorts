#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <stdio.h>
#include<stdlib.h>
#define BUFFER_SIZE 25

int checkPrime(int n);
int main(VOID)
{
	HANDLE ReadHandle, WriteHandle;
	char buffer[BUFFER_SIZE], buffer2[BUFFER_SIZE];
	DWORD read, written;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ReadHandle = GetStdHandle(STD_INPUT_HANDLE);
	WriteHandle = GetStdHandle(STD_OUTPUT_HANDLE);

	GetStartupInfo(&si);
	si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
	si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	si.hStdInput = ReadHandle;

	/* have the child read from the pipe */
	if (ReadFile(ReadHandle, buffer, BUFFER_SIZE, &read, NULL)) {

		sprintf(buffer2, "%d", checkPrime(atoi(buffer)));
		 
	}
	else
		fprintf(stderr, "Child: Error reading from pipe\n");

	//Write to the pipe
	if (!WriteFile(WriteHandle, buffer2, BUFFER_SIZE, &read, NULL))
		fprintf(stderr, "Error writing to pipe\n");

	return 0;

}

int checkPrime(int n) {
	int flag = 0;
	int i = 0;

	for (i = 2; i <= n / 2; ++i) {

		// condition for non-prime
		if (n % i == 0) {
			flag = 1;
			return 0;
			exit(0);
		}


		else {
			if (flag == 0)
				return 1;//is a prime number"
			else
				return 0;//is not a prime number"
		}

		return 0;
	}
}
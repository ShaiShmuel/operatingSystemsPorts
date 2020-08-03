#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <stdio.h>
#include<stdlib.h>
#define BUFFER_SIZE 25

int checkPrime(int n);
int numOfVes;
HANDLE mutex;

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
	mutex = CreateMutex(NULL, FALSE, NULL);
	if (mutex == NULL)
	{
		return 0;
	}

	/* check the num of vessels */
	if (ReadFile(ReadHandle, buffer, BUFFER_SIZE, &read, NULL)) {
		numOfVes = atoi(buffer);
		if (checkPrime(numOfVes) == 0) {
			sprintf(buffer2, "%d", 1);
			fprintf(stderr, "--not prime--");
		}
		else {
			sprintf(buffer2, "%d", 0);
			fprintf(stderr, "--is prime--");
			exit(0);
			fprintf(stderr, "exit 0");

		}
	}
	else
		fprintf(stderr, "Child: Error reading from pipe\n");

	//Write to the pipe
	WaitForSingleObject(mutex, INFINITE);

	if (!WriteFile(WriteHandle, buffer2, BUFFER_SIZE, &written, NULL))
		fprintf(stderr, "Error writing to pipe\n");

	Sleep(500);

	if (!ReleaseMutex(mutex))
		fprintf(stderr, "responePrime::Unexpected error mutex.V()\n");

	int i = 0;

	while (i < numOfVes) {


		if (ReadFile(ReadHandle, buffer, BUFFER_SIZE, &read, NULL)) {
			strcpy(buffer2buffer) ;
			fprintf(stderr,buffer);
		}
		else
			fprintf(stderr, "Eilat: Error reading from pipe in num of ves\n");

		Sleep(500);
		//Write to the pipe
		WaitForSingleObject(mutex, INFINITE);

		if (!WriteFile(WriteHandle, buffer2, BUFFER_SIZE, &written, NULL))
			fprintf(stderr, "Error writing to pipe\n");

		if (!ReleaseMutex(mutex))
			fprintf(stderr,"EilattoSuez::Unexpected error mutex.V()\n");
		//fprintf(stderr,"%d", i);
		i++;
	}


	return 0;

}

int checkPrime(int n) {
	for (int i = 2; i * i <= n; i++) {
		if (n % i == 0) return 0;
	}
	return 1;
}

#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>

#define BUFFER_SIZE 50


int main(int argc, char* argv[])
{

	if (argc <= 1) {
		printf("You didd not feed me arguments, I will die now :( ...");
		exit(1);
	}  //otherwise continue on our merry way....
    int arg1 = atoi(argv[1]);
	if (arg1 > 50 || arg1 < 2) {
		printf("Ships number should be between 2 to 50!!!! ----> ending process");
		exit(1);
	}


	char ProcessName[256];
	HANDLE StdInRead, StdInWrite;
	HANDLE StdOutRead, StdOutWrite;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	//char num[BUFFER_SIZE];
	CHAR buffer[BUFFER_SIZE];
	//printf("Please enter the num of ships: ");
	//fgets(num, 50, stdin);
	//printf("child message ===> %s", num);
	//num[strlen(num) - 1] = '\0';

	DWORD written, read;

	/* set up security attributes so that pipe handles are inherited */
	SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL,TRUE };

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	/* create the pipe */
	if (!CreatePipe(&StdInRead, &StdInWrite, &sa, 0)) {
		fprintf(stderr, "Create Pipe Failed\n");
		return 1;
	}

	/* create second the pipe */
	if (!CreatePipe(&StdOutRead, &StdOutWrite, &sa, 0)) {
		fprintf(stderr, "Create second Pipe Failed\n");
		return 1;
	}

	/* establish the START_INFO structure for the child process */
	GetStartupInfo(&si);
	si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
	si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);

	/* redirect the standard input to the read end of the pipe */
	si.hStdInput = StdInRead;
	si.hStdOutput = StdOutWrite;
	si.dwFlags = STARTF_USESTDHANDLES;

	/* we do not want the child to inherit the write end of the pipe */
	SetHandleInformation(StdInWrite, HANDLE_FLAG_INHERIT, 0);
	strcpy(ProcessName, "C:\\Users\\shais\\OneDrive\\Documents\\מדמח שנה ב סמסטר ב\\הנדסת מערכות תוכנה\\assignments\\ex3\\ReverseCaseChild.exe");

	// Start the child process.
	if (!CreateProcessA(ProcessName,   // No module name (use command line).
		GetCommandLineA, // Command line.
		NULL,             // Process handle not inheritable.
		NULL,             // Thread handle not inheritable.
		TRUE,            // inherite handle.
		0,                // No creation flags.
		NULL,             // Use parent's environment block.
		NULL,             // Use parent's starting directory.
		&si,              // Pointer to STARTUPINFO structure.
		&pi)             // Pointer to PROCESS_INFORMATION structure.
		)
	{
		fprintf(stderr, "Process Creation Failed\n");
		return -1;
	}


	/* Haifa port now sends ships to the pipe(souze) */
	if (!WriteFile(StdInWrite, arg1, BUFFER_SIZE, &written, NULL))
		printf(stderr, "Error writing to pipe-father\n");

	/* wait for the Eilat to exit */
	WaitForSingleObject(pi.hProcess, INFINITE);

	/*Read from Eilat*/
	if (ReadFile(StdOutRead, buffer, BUFFER_SIZE, &read, NULL)) {
		printf("Parent message ==> %s", buffer);

	}
	else {
		fprintf(stderr, "Child: Error reading from pipe\n");
	}



	/* close all handles */
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}


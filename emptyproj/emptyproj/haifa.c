#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>

#define BUFFER_SIZE 50
#define MAX_SLEEP_TIME 3000 //Miliseconds (3 second)
#define MIN_SLEEP_TIME 5 // Miliseconds
#define True 1
#define False 0

HANDLE mutex;
HANDLE* semVessel;
int arg1;
char ProcessName[256];
HANDLE StdInRead, StdInWrite;
HANDLE StdOutRead, StdOutWrite;
STARTUPINFO si;
PROCESS_INFORMATION pi;
//char num[BUFFER_SIZE];
CHAR buffer[BUFFER_SIZE];

DWORD written, read;
int initGlobalData();
void cleanupGlobalData();
int calcSleepTime(); //generic function to randomise a Sleep time between 1 and MAX_SLEEP_TIME msec
void toSuez(int id);
void fromSuez();
DWORD WINAPI Vessel(PVOID); //Thread function for each Philosopher



int main(int argc, char* argv[])
{

	if (argc <= 1) {
		printf("You did not feed me arguments, I will die now :( ...");
		exit(1);
	}  //otherwise continue on our merry way....
	arg1 = atoi(argv[1]);
	if (arg1 >= 50 || arg1 <= 2) {
		printf("Ships number should be between 2 to 50!!!! ----> ending process");
		exit(1);
	}


	DWORD ThreadId;
	HANDLE* vessels = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, arg1);
	int* vesselID = (int*)malloc(arg1 * sizeof(int));
	semVessel = (int*)malloc(arg1 * sizeof(int));
	if ((vessels || vesselID || semVessel) == NULL) {
		printf("memory error!!!");
		return -1;
	}


	if (initGlobalData() == False)
	{
		printf("main::Unexpected Error in Global Semaphore Creation\n");
		return 1;
	}


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
	wcscpy(ProcessName, L"..\\..\\EilatPort\\Debug\\eilatPort.exe");

	// Start the child process.
	if (!CreateProcessW(ProcessName,   // No module name (use command line).
		GetCommandLineW, // Command line.
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
		printf(stderr, "Process Creation Failed\n");
		return -1;
	}
	sprintf(buffer, "%d", arg1);


	/* Haifa port now sends ships to the pipe(souze) */
	if (!WriteFile(StdInWrite, buffer, BUFFER_SIZE, &written, NULL))
		printf(stderr, "Error writing to pipe-father\n");


	//Read response from Eilat 
	if (ReadFile(StdOutRead, buffer, BUFFER_SIZE, &read, NULL)) {
		if (atoi(buffer) == 1) {
			printf("Sorry, you cant send %d ships to eilat port!", arg1);
			/* close all handles */
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
			exit(0);
		}
		else { printf("Lets RoCk!\n"); }

	}
	else {
		printf(stderr, "haifa: Error reading from pipe\n");
	}

	/* wait for the Eilat to exit */

	//Initialise vessels Threads
	for (int i = 0; i < arg1; i++)
	{
		vesselID[i] = i + 1;
		printf("creation of threas %d\n", vesselID[i]);
		vessels[i] = CreateThread(NULL, 0, Vessel, &vesselID[i], 0, &ThreadId);
		if (vessels[i] == NULL)
		{
			printf("main::Unexpected Error in Vessel %d Creation\n", i);
			return 1;
		}
	}

	int i = 0;

	while (i < arg1)
	{
		if (ReadFile(StdOutRead, buffer, sizeof(buffer), &read, NULL))
			printf("vessel that came back %d \n", atoi(buffer));

		else {
			printf(stderr, "Child: Error reading from pipe\n");
		}

		i++;
	}

	WaitForMultipleObjects(arg1, vessels, TRUE, INFINITE);

	printf("All Vessel are Completed!\n");

	WaitForSingleObject(pi.hProcess, INFINITE);


	/* close all handles */
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	//close all global Semaphore Handles
	cleanupGlobalData();
}

DWORD WINAPI Vessel(PVOID Param)
{
	int i;

	int vesselId = (int)Param;

	for (i = 0; i < arg1; i++)
	{

		Sleep(calcSleepTime());

		toSuez(vesselId);
		//printf("%d", vesselId);
		Sleep(calcSleepTime());

	}

	return 0;
}


//generic function to randomise a Sleep time between 1 and MAX_SLEEP_TIME msec
int calcSleepTime()
{
	srand((unsigned int)time(NULL));
	return(rand() % MAX_SLEEP_TIME + MIN_SLEEP_TIME);
}

void toSuez(int id)
{
	WaitForSingleObject(mutex, INFINITE);

	sprintf(buffer, "%d", id);

	if (!WriteFile(StdInWrite, buffer, BUFFER_SIZE, &written, NULL))
		fprintf(stderr, "Error writing to pipe-father\n");

	if (!ReleaseMutex(mutex))
		printf("toSuez::Unexpected error mutex.V()\n");
}

void fromSuez()
{


}

int initGlobalData()
{
	int i;

	mutex = CreateMutex(NULL, FALSE, NULL);
	if (mutex == NULL)
	{
		return False;
	}

	for (i = 0; i < arg1; i++)
	{
		semVessel[i] = CreateSemaphore(NULL, 0, 1, NULL);
		if (semVessel[i] == NULL)
		{
			return False;
		}
	}

	return True;
}

//Close all global semaphore handlers - after all vessels Threads finish.
void cleanupGlobalData()
{
	int i;

	CloseHandle(mutex);

	for (i = 0; i < arg1; i++)
		CloseHandle(semVessel[i]);
}
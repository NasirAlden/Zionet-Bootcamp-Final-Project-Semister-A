#include "headers/listsAndStructs.h"
#include "headers/logs.h"

//// Global Snaps File Header Struct
//extern snapsFileHeader snapsHeaderFileSave;

// Global snapshot id
extern int snapshotID = 0;
// Global Dll id
extern int globalDllId = 1;
// Global Process id
extern int globalProcessId = 1;
// Global process number in every snapshot
extern int processCount = 0;
// Global Used Memory (Workset) - All Snaps
extern SIZE_T globalWorkingSetAvg = 0;  // Average of working set of all snaps
extern SIZE_T allSnapsWorkingSet = 0;  // Working Set Memory usage per snap
extern SIZE_T maxWorkingSet = 0;
extern int processIdUsingMaxMem = 0;

extern DllDictionaryItem* DllDictHead = NULL;
extern DllDictionaryItem* DllDictTail = NULL;

extern ProcessDictionaryItem* ProcessDictHead = NULL;
extern ProcessDictionaryItem* ProcessDictTail = NULL;

extern MySnapshotStruct* snapHead = NULL;
extern MySnapshotStruct* snapTail = NULL;


//*************************  FUNCTIONS  ******************************//

void calculateWorkingSet() {
	log("Calculating working set of processes list..");
	globalWorkingSetAvg = 0;
	allSnapsWorkingSet = 0;
	if (snapHead == NULL) {
		char msg[200] = "trying to calculate working set of Empty List.";
		logError(msg);
		globalWorkingSetAvg = 0;
	}
	SIZE_T sumPerSnap = 0;
	MySnapshotStruct* tmpCurrSnap = snapHead;
	while (tmpCurrSnap != NULL) {
		MyProcessStruct* tmpCurrProcess = tmpCurrSnap->processListHead;
		while (tmpCurrProcess != NULL) {
			if (tmpCurrProcess->WorkingSetSize > maxWorkingSet) {
				maxWorkingSet = tmpCurrProcess->WorkingSetSize;
				processIdUsingMaxMem = tmpCurrProcess->pid;
			}
			sumPerSnap += tmpCurrProcess->WorkingSetSize;
			tmpCurrProcess = tmpCurrProcess->Next;
		}
		allSnapsWorkingSet += sumPerSnap;
		sumPerSnap = 0;
		tmpCurrSnap = tmpCurrSnap->Next;
	}
	if (snapHead != NULL) {
		globalWorkingSetAvg = allSnapsWorkingSet / snapshotID;
	}
	else {
		globalWorkingSetAvg = 0;
	}
}

//Building Dlls unique Dictionary
void AddDllToDict(DllDictionaryItem* dllItem) {
	DllDictionaryItem* curr = DllDictHead;
	/*curr = (DllDictionary*)malloc(sizeof(DllDictionary));
	curr->key = dllItem->key;
	curr->value = dllItem->value;
	curr*/
	if (DllDictHead == NULL) {
		DllDictHead = DllDictTail = dllItem;
		DllDictHead->next = DllDictTail;
		DllDictTail->prev = DllDictHead;
		DllDictHead->prev = NULL;
		DllDictTail->next = NULL;
		DllDictHead->key = globalDllId;
	}
	else {
		while (curr != NULL) {
			if (strcmp(curr->value, dllItem->value) == 0 && strstr(curr->processesIdUsedDll, dllItem->processesIdUsedDll) == NULL) {
				if (globalDllId == 1) {
					strcat(curr->processesIdUsedDll, dllItem->processesIdUsedDll);
				}
				else {
					strcat(curr->processesIdUsedDll, ",");
					strcat(curr->processesIdUsedDll, dllItem->processesIdUsedDll);
				}
				break;
			}
			if (strcmp(curr->value, dllItem->value) == 0) {
				break;
			}
			curr = curr->next;
		}
		if (curr == NULL) {
			curr = dllItem;
			DllDictTail->next = dllItem;
			curr->prev = DllDictTail;
			DllDictTail = curr;
			DllDictTail->next = NULL;
			DllDictTail->key = ++globalDllId;
		}

	}
}

// Adding process to unique process dictionary.
void AddProcessToDict(ProcessDictionaryItem* processItem) {
	ProcessDictionaryItem* curr = ProcessDictHead;
	/*curr = (DllDictionary*)malloc(sizeof(DllDictionary));
	curr->key = dllItem->key;
	curr->value = dllItem->value;
	curr*/
	if (ProcessDictHead == NULL) {
		ProcessDictHead = ProcessDictTail = processItem;
		ProcessDictHead->next = ProcessDictTail;
		ProcessDictTail->prev = ProcessDictHead;
		ProcessDictHead->prev = NULL;
		ProcessDictTail->next = NULL;
		ProcessDictHead->key = globalProcessId;
	}
	else {
		while (curr != NULL) {
			if (strcmp(curr->value, processItem->value) == 0 && strstr(curr->pidList, processItem->pidList) == NULL) {
				if (globalProcessId == 1) {
					strcat(curr->pidList, processItem->pidList);
				}
				else {
					strcat(curr->pidList, ",");
					strcat(curr->pidList, processItem->pidList);
				}
				break;
			}
			if (strcmp(curr->value, processItem->value) == 0) {
				break;
			}
			curr = curr->next;
		}
		if (curr == NULL) {
			curr = processItem;
			ProcessDictTail->next = processItem;
			curr->prev = ProcessDictTail;
			ProcessDictTail = curr;
			ProcessDictTail->next = NULL;
			ProcessDictTail->key = ++globalProcessId;
		}
	}
	/*logEvent("Process Added to Dictionary.");*/
}

//Free Allocated memory for process lists.
void freeProcessList(MyProcessStruct* pHEAD)
{
	MyProcessStruct* currentItem = pHEAD;
	MyProcessStruct* releaseItem = pHEAD;
	while (currentItem != NULL)
	{
		releaseItem = currentItem;
		currentItem = currentItem->Next;
		free(releaseItem);
	}
}


// Change string type from wchar_t to regular char.
char* wchar2string(wchar_t* str)
{
	char mystring[300] = { '\0' };
	for (int i = 0;str[i] != '\0';++i) {
		mystring[i] = str[i];
	}
	return  mystring;
}

//Creating Dlls lists.
MyProcessStruct* CreateDllList(DWORD processID)
{
	HANDLE hProcess;
	PROCESS_MEMORY_COUNTERS pmc;

	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
		PROCESS_VM_READ,
		FALSE, processID);
	if (NULL == hProcess)
		return NULL;

	HMODULE hMods[1024];
	DWORD cbNeeded;
	TCHAR Buffer[MAX_PATH];
	TCHAR Buffer2[MAX_PATH];
	/*char* processName = "";*/
	char processName[MAX_PATH];
	if (GetModuleFileNameEx(hProcess, 0, Buffer, MAX_PATH))
	{
		// At this point, buffer contains the full path to the executable
		size_t numConverted;
		wcstombs_s(&numConverted, processName, MAX_PATH, Buffer, MAX_PATH);
	}
	else
	{
		// You better call GetLastError() here
		logError(strerror(GetLastError()));
		exit(-1);
	}

	MyProcessStruct* MyprocessInfo = NULL;
	MyprocessInfo = (MyProcessStruct*)malloc(sizeof(MyProcessStruct));
	if (!MyprocessInfo) {
		logError(strerror(GetLastError()));
		exit(1);
	}
	strcpy(MyprocessInfo->pName, processName);
	MyprocessInfo->pid = processID;
	MyprocessInfo->usedDllsCount = 0;
	if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc)))
	{
		/*MyprocessInfo = (MyProcessStruct*)malloc(sizeof(MyProcessStruct));*/
		MyprocessInfo->PageFaultCount = pmc.PageFaultCount;
		MyprocessInfo->WorkingSetSize = pmc.WorkingSetSize;
		MyprocessInfo->QuotaPagedPoolUsage = pmc.QuotaPagedPoolUsage;
		MyprocessInfo->QuotaPeakPagedPoolUsage = pmc.QuotaPeakPagedPoolUsage;
		MyprocessInfo->PagefileUsage = pmc.PagefileUsage;

		/*printf("\tPageFaultCount: 0x%08X\n", pmc.PageFaultCount);
		printf("\tWorkingSetSize: 0x%08X\n", pmc.WorkingSetSize);
		printf("\tQuotaPagedPoolUsage: 0x%08X\n",
			pmc.QuotaPagedPoolUsage);
		printf("\tQuotaPeakPagedPoolUsage: 0x%08X\n",
			pmc.QuotaPeakPagedPoolUsage);
		printf("\tPagefileUsage: 0x%08X\n", pmc.PagefileUsage);*/

	}

	// Get Dlls List and turn it into Linked List 

	if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
	{
		printf("\n **************************\n");
		printf("Snap (%d) process (%d) - %s\n", snapshotID, processCount, processName);
		printf("\n **************************\n");
		MyprocessInfo->usedDllsCount = 0;
		MyDllStruct* dllListHead = NULL;
		MyDllStruct* dllListTail = NULL;
		for (int i = 1; i < cbNeeded / sizeof(HMODULE); i++)
		{
			MyDllStruct* currDLL = (MyDllStruct*)malloc(sizeof(MyDllStruct));
			if (!currDLL) {
				logError(strerror(GetLastError()));
				exit(2);
			}
			TCHAR szModName[MAX_PATH];

			// Get the full path to the module's file.
			if (GetModuleFileNameEx(hProcess, hMods[i], Buffer2, MAX_PATH))
			{
				MyprocessInfo->usedDllsCount++;
				// /*char* ModuleName = wchar2string(Buffer2);*/
				char moduleName[MAX_PATH];
				size_t numConverted;
				wcstombs_s(&numConverted, moduleName, MAX_PATH, Buffer2, MAX_PATH);
				printf("%d) %s\n", MyprocessInfo->usedDllsCount, moduleName);
				/*wprintf(L"%s\n", Buffer2);*/
				sprintf(currDLL->name, "%s", moduleName);

				// Adding Dll to Global Dlls Dictionary
				DllDictionaryItem* tmpDictDllItem = NULL;
				tmpDictDllItem = (DllDictionaryItem*)malloc(sizeof(DllDictionaryItem));
				if (!tmpDictDllItem) {
					logError(strerror(GetLastError()));
					exit(3);
				}
				/*tmpDictDllItem->key=globalDllId++;*/
				sprintf(tmpDictDllItem->value, "%s", currDLL->name);
				sprintf(tmpDictDllItem->processesIdUsedDll, "%d", MyprocessInfo->pid);
				AddDllToDict(tmpDictDllItem); //Adding Dll to the unique dlls dictionary.
			}
			if (dllListHead == NULL) {
				dllListHead = dllListTail = currDLL;
				dllListHead->Prev = NULL;
				dllListTail->Prev = dllListHead;
				dllListHead->Next = dllListTail;
				dllListTail->Next = NULL;
			}
			else {
				dllListTail->Next = currDLL;
				currDLL->Prev = dllListTail;
				dllListTail = currDLL;
				dllListTail->Next = NULL;

			}
		}
		MyprocessInfo->DllListHead = dllListHead;
	}

	CloseHandle(hProcess); //closing the process handle to free up the memory.
	if (MyprocessInfo->usedDllsCount > 0) {
		/*logEvent("Dll List was created.");*/
		return MyprocessInfo;
	}
	else {
		return NULL;
	}
}

//Creating processes lists.
MyProcessStruct* CreateProcessList() {
	MyProcessStruct* processListHead = NULL;
	MyProcessStruct* processListTail = NULL;
	// Get Processes
	// Receive all process ID

	DWORD aProcesses[1024], cbNeeded, cProcesses;
	unsigned int i;

	if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
	{
		// Error. Write to log
		logError(strerror(GetLastError()));
	}

	// Calculate how many process identifiers were returned.

	cProcesses = cbNeeded / sizeof(DWORD);
	processCount = 0;

	// For each Process get its Memory Information

	for (i = 0; i < cProcesses; i++)
	{
		MyProcessStruct* currProc = CreateDllList(aProcesses[i]);
		if (currProc != NULL) {    // Adding only processes that have Dlls
			processCount++;
			/*SnapworkingSetSum += currProc->WorkingSetSize;*/
			if (processListHead == NULL) {
				processListHead = processListTail = currProc;
				processListHead->Next = processListTail;
				processListTail->Next = NULL;
				processListHead->Prev = NULL;
				processListTail->Prev = processListHead;

			}
			else if ((unsigned int)currProc != 0xcccccccc) {
				processListTail->Next = currProc;
				currProc->Prev = processListTail;
				processListTail = currProc;
				processListTail->Next = NULL;
			}

			// Adding Process to Global Processes Dictionary
			ProcessDictionaryItem* tmpDictProcessItem = NULL;
			tmpDictProcessItem = (ProcessDictionaryItem*)malloc(sizeof(ProcessDictionaryItem));
			if (!tmpDictProcessItem) {
				logError(strerror(GetLastError()));
				exit(4);
			}
			/*tmpDictProcessItem->key=globalProcessId++;*/
			sprintf(tmpDictProcessItem->value, "%s", currProc->pName);
			sprintf(tmpDictProcessItem->pidList, "%d", currProc->pid);
			AddProcessToDict(tmpDictProcessItem);
		}
	}
	logEvent("Processes List was created.");
	return processListHead;
}


void oneSnapshot() {
	logEvent("Starting One Snapshot for the processes in memory. ");
	snapshotID++;   // Every snapshot this global counter increase by one.
	MySnapshotStruct* snapBuffer = NULL;
	snapBuffer = (MySnapshotStruct*)malloc(sizeof(MySnapshotStruct));
	if (!snapBuffer) {
		logError(strerror(GetLastError()));
		exit(5);
	}
	snapBuffer->snapId = snapshotID;
	sprintf(snapBuffer->sysTime, "%s", getSysTime());
	snapBuffer->processListHead = CreateProcessList();
	snapBuffer->processCount = processCount;
	processCount = 0;

	if (snapHead == NULL) {
		snapHead = snapTail = snapBuffer;
		snapHead->Next = snapTail;
		snapTail->Prev = snapHead;
		snapHead->Prev = NULL;
		snapTail->Next = NULL;
	}
	else {
		snapTail->Next = snapBuffer;
		snapBuffer->Prev = snapTail;
		snapTail = snapBuffer;
		snapTail->Next = NULL;
	}
	//globalWorkingSetAvg += SnapworkingSetSum;  //Adding working set of snap to global Sum (Avg).
	//globalWorkingSetAvg /= snapshotID;  // Calculating WorkingSet usage average per snap
	//SnapworkingSetSum = 0; // Reset variable to use it next snap
}

// Function to take 20 snapshots (one snap every second) or long term unlimited snaps.
void snapshot20(char op) {
	logEvent("Starting 20/Long Snapshots for the processes in memory. ");
	snapshotID++;   // Every snapshot this global counter increase by one.
	int limit = (op == '2') ? 20 : INT_MAX;  // Making function run 20 or unlimited
	int stopFunction = 0;
	int processCount20 = 0;
	MySnapshotStruct* snapBuffer = NULL;
	snapBuffer = (MySnapshotStruct*)malloc(sizeof(MySnapshotStruct));
	if (!snapBuffer) {
		logError(strerror(GetLastError()));
		exit(6);
	}
	snapBuffer->snapId = snapshotID;
	sprintf(snapBuffer->sysTime, "%s", getSysTime());
	MyProcessStruct* tmpHead = NULL;  // Head of the big processes list of the 20 snapshots
	MyProcessStruct* tmpTail = NULL; // Tail of the big processes list of the 20 snapshots
	MyProcessStruct* tmpCurr = NULL; // Temporary head of process list of every snapshot.
	for (int i = 0;i < limit && !stopFunction;++i) {
		tmpCurr = CreateProcessList();  // Create list of processes for each snapshot
		// Start building a long processes list for the whole 20 snapshots
		if (tmpHead == NULL) {  // First snapshot
			tmpHead = tmpCurr;
			while (tmpCurr->Next != NULL) {
				tmpCurr = tmpCurr->Next;
			}
			tmpTail = tmpCurr;
			processCount20 += processCount;
		}
		else {  // From second snapshot and above ..
			MyProcessStruct* tmpCurrFreeLater = tmpCurr;  // Save the head of the temporary list to free later
			while (tmpCurr != NULL) {
				MyProcessStruct* tmpRun = tmpHead;  // check if process exists in previous list
				while (tmpRun != NULL) {  // Run on the previous process list
					//check if two processes equal by name and id. (process exist before)
					if (strcmp(tmpCurr->pName, tmpRun->pName) == 0 && tmpCurr->pid == tmpRun->pid) {
						// if two processes equal then update process info (Aggregation).
						tmpRun->PageFaultCount += tmpCurr->PageFaultCount;
						tmpRun->WorkingSetSize += tmpCurr->WorkingSetSize;
						tmpRun->QuotaPagedPoolUsage += tmpCurr->QuotaPagedPoolUsage;
						tmpRun->QuotaPeakPagedPoolUsage += tmpCurr->QuotaPeakPagedPoolUsage;
						tmpRun->PagefileUsage += tmpCurr->PagefileUsage;

						// Check if there is a new Dll used in the new instance of process 
						MyDllStruct* tmpDllRun = tmpRun->DllListHead;
						MyDllStruct* tmpDllTail = NULL;
						MyDllStruct* tmpDllCurr = tmpCurr->DllListHead;
						while (tmpDllCurr != NULL) {
							tmpDllRun = tmpRun->DllListHead;
							while (tmpDllRun != NULL) {
								if (strcmp(tmpDllCurr->name, tmpDllRun->name) == 0) {
									break;
								}
								tmpDllTail = tmpDllRun;
								tmpDllRun = tmpDllRun->Next;
							}
							if (tmpDllRun == NULL) {
								// Dll not exist in prev instance of process (Add it to Dll List)
								// At this point tmpDllRun is the tail of prev dll list
								// So we can add tmpDllCurr to it ..
								tmpDllTail->Next = tmpDllCurr;
								tmpDllCurr->Prev = tmpDllTail;
								tmpDllTail->Next = NULL;
								//tmpTail = tmpDllCurr;
							}
							tmpDllCurr = tmpDllCurr->Next;
						}
						tmpRun = NULL;
						break;
					}
					tmpRun = tmpRun->Next;
				}
				/*if (strcmp(tmpCurr->pName, tmpRun->pName) == 0) {
					tmpRun = NULL;
				}*/
				if (tmpRun != NULL) { //tmpCurr is not existing in previous process list
					tmpTail->Next = tmpCurr; // Linking the new process to the big list
					tmpCurr->Prev = tmpTail;
					tmpTail = tmpCurr;
					tmpTail->Next = NULL;
					processCount20++;
				}
				tmpCurr = tmpCurr->Next;
			}

			freeProcessList(tmpCurrFreeLater);

			//tmpTail->Next = tmpCurr;
			//tmpCurr->Prev = tmpTail;
			//while (tmpCurr->Next != NULL) {
			//	tmpCurr = tmpCurr->Next;
			//}
			//tmpTail = tmpCurr;
		}

		if (kbhit())   // Check if Function interrupted
		{
			char ch = getch();
			printf("Function Snapshot20 stopped by user. %c was clicked.\n", ch);
			logEvent("Function Snapshot20/Long stopped by user.");
			if (ch == '4')
				stopFunction = 1;
		}


		Sleep(3000);
	}
	processCount = 0;
	snapBuffer->processListHead = tmpHead;
	snapBuffer->processCount = processCount20;
	if (snapHead == NULL) {
		snapHead = snapTail = snapBuffer;
		snapHead->Prev = NULL;
		snapTail->Prev = snapHead;
		snapHead->Next = snapTail;
		snapTail->Next = NULL;
	}
	else {
		snapTail->Next = snapBuffer;
		snapBuffer->Prev = snapTail;
		snapTail = snapBuffer;
		snapTail->Next = NULL;
	}
}




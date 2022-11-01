
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include "headers/listsAndStructs.h"
#include <time.h>


//Global Heads and Tails
extern MySnapshotStruct* buildSnapHead = NULL;
extern MySnapshotStruct* buildSnapTail = NULL;
extern MyProcessStruct* buildProcessHead = NULL;
extern MyProcessStruct* buildProcessTail = NULL;
extern MyDllStruct* buildDllHead = NULL;
extern MyDllStruct* buildDllTail = NULL;

//Global Counters
extern int countSnap = 0;
extern int countProcess = 1;
extern int countDll = 1;

//Global Files Names
extern char saveFileName[40] = { '\0' };

void CreateSnapFileName() {
	time_t t;
	time(&t);
	struct tm* timeInfo;
	timeInfo = localtime(&t);
	char strTime[100];
	sprintf(strTime, "%s", getSysTime());
	sprintf(saveFileName, "%s_%d_%d_%d.bin", "savedSnapFile", timeInfo->tm_year + 1900, timeInfo->tm_mon + 1, timeInfo->tm_mday);
}


// Save all snaps to local binary file , including all processes and dlls info.
void WriteSnapListToFile() {
	CreateSnapFileName();
	FILE* saveFile = fopen(saveFileName, "wb");
	if (!saveFile) {
		logError(strerror(GetLastError()));
		exit(22);
	}

	MySnapshotStruct* tmpSnapRun = snapHead;
	snapsHeaderFileSave.version = SNAPS_FILE_VER;
	snapsHeaderFileSave.snapsCount = snapshotID;
	snapsHeaderFileSave.processCount = globalProcessId;
	snapsHeaderFileSave.dllsCount = globalDllId;
	fwrite(&snapsHeaderFileSave, sizeof(snapsFileHeader), 1, saveFile);

	while (tmpSnapRun != NULL) {
		fwrite(&tmpSnapRun->snapId, sizeof(int), 1, saveFile);
		fwrite(tmpSnapRun->sysTime, sizeof(tmpSnapRun->sysTime), 1, saveFile);
		fwrite(&tmpSnapRun->processCount, sizeof(int), 1, saveFile);
		MyProcessStruct* tmpProcessRun = tmpSnapRun->processListHead;
		while (tmpProcessRun != NULL) {
			fwrite(&tmpProcessRun->pid, sizeof(int), 1, saveFile);
			fwrite(tmpProcessRun->pName, sizeof(tmpProcessRun->pName), 1, saveFile);
			fwrite(&tmpProcessRun->PageFaultCount, sizeof(DWORD), 1, saveFile);
			fwrite(&tmpProcessRun->PagefileUsage, sizeof(SIZE_T), 1, saveFile);
			fwrite(&tmpProcessRun->WorkingSetSize, sizeof(SIZE_T), 1, saveFile);
			fwrite(&tmpProcessRun->QuotaPeakPagedPoolUsage, sizeof(SIZE_T), 1, saveFile);
			fwrite(&tmpProcessRun->QuotaPagedPoolUsage, sizeof(SIZE_T), 1, saveFile);
			fwrite(&tmpProcessRun->usedDllsCount, sizeof(int), 1, saveFile);
			MyDllStruct* tmpDllRun = tmpProcessRun->DllListHead;
			while (tmpDllRun != NULL) {
				fwrite(tmpDllRun->name, sizeof(tmpDllRun->name), 1, saveFile);
				fwrite(tmpDllRun, sizeof(tmpDllRun->Next), 1, saveFile);
				fwrite(tmpDllRun, sizeof(tmpDllRun->Prev), 1, saveFile);
				tmpDllRun = tmpDllRun->Next;
			}
			fwrite(tmpProcessRun, sizeof(tmpProcessRun->Next), 1, saveFile);
			fwrite(tmpProcessRun, sizeof(tmpProcessRun->Prev), 1, saveFile);
			tmpProcessRun = tmpProcessRun->Next;
		}
		fwrite(tmpSnapRun, sizeof(tmpSnapRun->Next), 1, saveFile);
		fwrite(tmpSnapRun, sizeof(tmpSnapRun->Prev), 1, saveFile);

		tmpSnapRun = tmpSnapRun->Next;
	}
	fclose(saveFile);
}

// Build dlls lists when load from local file.
void buildDllList(MyDllStruct* dllBuffer) {
	if (buildDllHead == NULL) {
		buildDllHead = buildDllTail = dllBuffer;
		buildDllHead->Next = dllBuffer;
		buildDllTail->Prev = dllBuffer;
		buildDllHead->Prev = NULL;
		buildDllTail->Next = NULL;
	}
	else {
		buildDllTail->Next = dllBuffer;
		dllBuffer->Prev = buildDllTail;
		buildDllTail = dllBuffer;
		buildDllTail->Next = NULL;
	}
	countDll++;
}

// Build process lists when load from local file.
void buildProcessList(MyProcessStruct* processBuffer) {

	if (buildProcessHead == NULL) {
		buildProcessHead = buildProcessTail = processBuffer;
		buildProcessHead->Next = buildProcessTail;
		buildProcessTail->Prev = buildProcessHead;
		buildProcessHead->Prev = NULL;
		buildProcessTail->Next = NULL;
	}
	else {
		buildProcessTail->Next = processBuffer;
		processBuffer->Prev = buildProcessTail;
		buildProcessTail = processBuffer;
		buildProcessTail->Next = NULL;
	}
	countProcess++;
}

//Build General Snaps Lists that loaded from local binary file.
void BuildSnapListFromFile() {
	printf("Do you want to Reset Collections before link the loaded file(Y=Yes,N=No)? \n");
	char ch = getch();
	if (toupper(ch) == 'Y') {
		snapshotID = 0;
		if (snapHead != NULL) freeAllLists(snapHead);
	}
	countSnap = 0;
	buildSnapHead = buildSnapTail = NULL;
	CreateSnapFileName();
	FILE* loadFile = fopen(saveFileName, "rb");
	if (!loadFile) {
		logError(strerror(GetLastError()));
		exit(23);
	}
	snapsFileHeader snapsHeaderFileLoad;
	fread(&snapsHeaderFileLoad, sizeof(snapsFileHeader), 1, loadFile);
	if (toupper(ch) == 'Y') {
		globalProcessId = 1;
		globalDllId = 1;
	}
	else {
		if (DllDictHead == NULL || ProcessDictHead == NULL) {
			globalProcessId = 1;
			globalDllId = 1;
		}
		//else {
		//	globalProcessId += 1;
		//	globalDllId += 1;
		//}
	}
	MySnapshotStruct* snapBuffer = NULL;
	snapBuffer = (MySnapshotStruct*)malloc(sizeof(MySnapshotStruct));
	if (!snapBuffer) {
		logError(strerror(GetLastError()));
		exit(16);
	}
	while (fread(&snapBuffer->snapId, sizeof(int), 1, loadFile)) {
		snapshotID++;
		printf("\n\nAdding Snap Number: %d\n", ++countSnap);
		printf("**************************************\n");
		fread(snapBuffer->sysTime, sizeof(snapBuffer->sysTime), 1, loadFile);
		fread(&snapBuffer->processCount, sizeof(int), 1, loadFile);
		for (int i = 0;i < snapBuffer->processCount;++i) {
			MyProcessStruct* processBuffer = (MyProcessStruct*)malloc(sizeof(MyProcessStruct));
			if (!processBuffer) {
				logError(strerror(GetLastError()));
				exit(17);
			}
			fread(&processBuffer->pid, sizeof(int), 1, loadFile);
			fread(processBuffer->pName, sizeof(processBuffer->pName), 1, loadFile);
			printf("\nSnap(%d) Process (%d)- %s\n", snapshotID, i + 1, processBuffer->pName);
			printf("***************************\n");
			fread(&processBuffer->PageFaultCount, sizeof(DWORD), 1, loadFile);
			fread(&processBuffer->PagefileUsage, sizeof(SIZE_T), 1, loadFile);
			fread(&processBuffer->WorkingSetSize, sizeof(SIZE_T), 1, loadFile);
			fread(&processBuffer->QuotaPeakPagedPoolUsage, sizeof(SIZE_T), 1, loadFile);
			fread(&processBuffer->QuotaPagedPoolUsage, sizeof(SIZE_T), 1, loadFile);
			fread(&processBuffer->usedDllsCount, sizeof(int), 1, loadFile);
			for (int j = 0;j < processBuffer->usedDllsCount;++j) {
				MyDllStruct* dllBuffer = (MyDllStruct*)malloc(sizeof(MyDllStruct));
				if (!dllBuffer) {
					logError(strerror(GetLastError()));
					exit(18);
				}
				fread(dllBuffer->name, sizeof(dllBuffer->name), 1, loadFile);
				printf("%d) %s\n", j + 1, dllBuffer->name);
				fread(&dllBuffer->Next, sizeof(dllBuffer->Next), 1, loadFile);
				fread(&dllBuffer->Prev, sizeof(dllBuffer->Prev), 1, loadFile);
				buildDllList(dllBuffer);

				// Adding Dll to Global Dlls Dictionary
				DllDictionaryItem* tmpDictDllItem = NULL;
				tmpDictDllItem = (DllDictionaryItem*)malloc(sizeof(DllDictionaryItem));
				if (!tmpDictDllItem) {
					logError(strerror(GetLastError()));
					exit(19);
				}
				/*tmpDictDllItem->key=globalDllId++;*/
				sprintf(tmpDictDllItem->value, "%s", dllBuffer->name);
				sprintf(tmpDictDllItem->processesIdUsedDll, "%d", processBuffer->pid);
				/*sprintf(tmpDictDllItem->processesIdUsedDll, "%s", tmpDictDllItem->processesIdUsedDll);*/
				AddDllToDict(tmpDictDllItem);
			}
			MyDllStruct* localBuildDllHead = buildDllHead;  // Local Dll Head to process
			MyDllStruct* localBuildDllTail = buildDllTail;  // Local Dll Tail to process
			processBuffer->DllListHead = localBuildDllHead;
			buildDllHead = NULL; // Reset Head for the next process dll list
			buildDllTail = NULL; // Reset Tail for the next process dll list
			fread(&processBuffer->Next, sizeof(processBuffer->Next), 1, loadFile);
			fread(&processBuffer->Prev, sizeof(processBuffer->Prev), 1, loadFile);
			buildProcessList(processBuffer);

			// Adding Process to Global Processes Dictionary
			ProcessDictionaryItem* tmpDictProcessItem = NULL;
			tmpDictProcessItem = (ProcessDictionaryItem*)malloc(sizeof(ProcessDictionaryItem));
			if (!tmpDictProcessItem) {
				logError(strerror(GetLastError()));
				exit(20);
			}
			/*tmpDictProcessItem->key=globalProcessId++;*/
			sprintf(tmpDictProcessItem->value, "%s", processBuffer->pName);
			sprintf(tmpDictProcessItem->pidList, "%d", processBuffer->pid);
			AddProcessToDict(tmpDictProcessItem);
		}
		MyProcessStruct* localBuildProcessHead = buildProcessHead;  // Local process Head to snapshot
		MyProcessStruct* localBuildProcessTail = buildProcessTail;  // Local process Tail to snapshot
		snapBuffer->processListHead = localBuildProcessHead;
		buildProcessHead = NULL; // Reset Head for the next snapshot process list
		buildProcessTail = NULL; // Reset Tail for the next snapshot process list
		fread(&snapBuffer->Next, sizeof(snapBuffer->Next), 1, loadFile);
		fread(&snapBuffer->Prev, sizeof(snapBuffer->Prev), 1, loadFile);

		if (buildSnapHead == NULL) {
			buildSnapHead = buildSnapTail = snapBuffer;
			buildSnapHead->Next = buildSnapTail;
			buildSnapTail->Prev = buildSnapHead;
			buildSnapHead->Prev = NULL;
			buildSnapTail->Next = NULL;

		}
		else {
			buildSnapTail->Next = snapBuffer;
			snapBuffer->Prev = buildSnapTail;
			buildSnapTail = snapBuffer;
			buildSnapTail->Next = NULL;
		}
		snapBuffer = (MySnapshotStruct*)malloc(sizeof(MySnapshotStruct));
		if (!snapBuffer) {
			logError(strerror(GetLastError()));
			exit(21);
		}
	}
	if (toupper(ch) == 'Y' || snapHead == NULL) {
		if (buildSnapHead == NULL) {
			printf("You are trying to load empty file!\n");
			return;
		}
		snapHead = buildSnapHead;
		snapTail = buildSnapTail;
		snapHead->Prev = NULL;
		snapTail->Next = NULL;
		snapshotID = countSnap;
	}
	else {
		if (buildSnapHead == NULL) {
			printf("You are trying to load empty file!\n");
			return;
		}
		int i = 1;
		MySnapshotStruct* tmpUpdateSnapsID = buildSnapHead;
		while (tmpUpdateSnapsID != NULL) {
			tmpUpdateSnapsID->snapId = snapshotID - countSnap + i;
			i++;
			tmpUpdateSnapsID = tmpUpdateSnapsID->Next;
		}
		snapTail->Next = buildSnapHead;
		buildSnapHead->Prev = snapTail;
		snapTail = buildSnapTail;
		snapTail->Next = NULL;
	}
}

#include <stdio.h>
#include <stdlib.h>
#include "headers/listsAndStructs.h"

//Free the memory allocated for all snaps.
void freeSnapList(MySnapshotStruct* sHEAD)
{
	MySnapshotStruct* currentItem = sHEAD;
	MySnapshotStruct* releaseItem = sHEAD;
	while (currentItem != NULL)
	{
		releaseItem = currentItem;
		currentItem = currentItem->Next;
		free(releaseItem);
	}
}

// Free allocated memory for dlls lists.
void freeDllList(MyDllStruct* dllHEAD)
{
	MyDllStruct* currentItem = dllHEAD;
	MyDllStruct* releaseItem = dllHEAD;
	while (currentItem != NULL)
	{
		releaseItem = currentItem;
		currentItem = currentItem->Next;
		free(releaseItem);
	}
	snapshotID = 0;
	snapHead = NULL;
	snapTail = NULL;
}

//Free allocated memory for dlls dictionary.
void freeDllDict(DllDictionaryItem* HEAD)
{
	DllDictionaryItem* currentItem = HEAD;
	DllDictionaryItem* releaseItem = HEAD;
	while (currentItem != NULL)
	{
		releaseItem = currentItem;
		currentItem = currentItem->next;
		free(releaseItem);
	}
	DllDictHead = NULL;
	DllDictTail = NULL;
}

//Free allocated memory for process dictionary.
void freeProcessDict(ProcessDictionaryItem* HEAD)
{
	ProcessDictionaryItem* currentItem = HEAD;
	ProcessDictionaryItem* releaseItem = HEAD;
	while (currentItem != NULL)
	{
		releaseItem = currentItem;
		currentItem = currentItem->next;
		free(releaseItem);
	}
	ProcessDictHead = NULL;
	ProcessDictTail = NULL;
}


// Free all the lists at once (Snaps/Processes/Dlls).
void freeAllLists(MySnapshotStruct* HEAD) {
	MySnapshotStruct* currentSnap = HEAD;
	MySnapshotStruct* releaseSnap = HEAD;
	snapshotID = 0;
	globalDllId = 1;
	globalProcessId = 1;
	globalWorkingSetAvg = 0;
	allSnapsWorkingSet = 0;
	maxWorkingSet = 0;
	processIdUsingMaxMem = 0;

	if (HEAD == NULL) {
		puts("List is Empty!");
		return;
	}
	while (currentSnap != NULL)
	{
		releaseSnap = currentSnap;
		currentSnap = currentSnap->Next;
		freeDllList(releaseSnap->processListHead->DllListHead);
		freeProcessList(releaseSnap->processListHead);
		free(releaseSnap);
	}
	freeDllDict(DllDictHead);
	freeProcessDict(ProcessDictHead);
}


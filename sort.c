#pragma once
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <time.h>
#include <windows.h>
#include "headers/listsAndStructs.h"
#include "headers/logs.h"
#pragma warning(disable:4996)


//Bubble sort the process lists of all snaps by the Dlls Count used by processes.
void BubblesortProcessList()
{
	logEvent("Start Sorting the process lists of all snaps (Bubble Sort).");
	MySnapshotStruct* tmpSnap = snapHead;

	while (tmpSnap != NULL) {
		MyProcessStruct* prevProcess, * nextProcess;
		MyProcessStruct* tempProcess;
		MyProcessStruct* snapAltProcessListHead = NULL;
		tmpSnap->processListHead->Prev = NULL;
		MyProcessStruct* HEAD = tmpSnap->processListHead;
		/*HEAD->Prev = NULL;*/
		int swapped = 1;
		while (swapped)
		{
			swapped = 0;  //At the end of itteration if not changed to 1 that mean the list is sorted.
			tempProcess = HEAD;  // Always start to search from the head of list but in this case it is snap so we build temp head from same process struct type.
			while (tempProcess->Next != NULL)
			{
				if (tempProcess->usedDllsCount < tempProcess->Next->usedDllsCount) //need or not sorting
				{
					prevProcess = tempProcess->Prev;
					nextProcess = tempProcess->Next;
					if (prevProcess != NULL) // tempProcess is not at the beginning of the list
					{
						prevProcess->Next = nextProcess;
					}
					else  // tempProcess is at the beginning of the list
					{
						HEAD = nextProcess;
					}
					tempProcess->Next = nextProcess->Next;
					if (nextProcess->Next != NULL)
					{
						nextProcess->Next->Prev = tempProcess;
					}
					tempProcess->Prev = nextProcess;
					nextProcess->Next = tempProcess;
					nextProcess->Prev = prevProcess;
					swapped = 1; //if reaching here that mean the list is not sorted yet.
				}
				else
				{
					tempProcess = tempProcess->Next;  //traversal over the process list
				}
			}
		}
		tmpSnap->processListHead = HEAD;  // Connecting again process list head to the snap struct.
		tmpSnap = tmpSnap->Next;  //traversal over the snaps list
	}
}

#pragma once
#ifndef _PROCESS_H
#define _PROCESS_H

#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <Windows.h>
#include <psapi.h>
#include <wchar.h>
#include <time.h>
#include <conio.h>

#pragma warning(disable:4996)
#define _CRT_SECURE_NO_WARNINGS
#define SNAPS_FILE_VER 1

// ************************  Global Variables and Structs *************************//

typedef struct snapsFileHeader {
	int version;
	int snapsCount;
	int processCount;
	int dllsCount;
	char reserve[100];
}snapsFileHeader;

// Global Snaps File Header Struct
snapsFileHeader snapsHeaderFileSave;

// Global snapshot id
extern int snapshotID;
// Global Dll id
extern int globalDllId;
// Global Process id
extern int globalProcessId;
// Global process number in every snapshot
extern int processCount;
// Global Used Memory (Workset) - All Snaps
extern SIZE_T globalWorkingSetAvg;  // Average of working set of all snaps
extern SIZE_T allSnapsWorkingSet;  // Working Set Memory usage per snap
extern SIZE_T maxWorkingSet;
extern int processIdUsingMaxMem;


// Global Dlls Dictionary struct
typedef struct dllDict {
	int key;    // Dll id
	char value[300];  // Dll Name (Full Path)
	char processesIdUsedDll [10000];
	struct dllDict* next;
	struct dllDict* prev;
} DllDictionaryItem;

// Global Processes Dictionary struct
typedef struct processDict {
	int key;    // process global unique id (Combined by pid and process name)-Bonus :D !!
	char value[300];  // Process Name (Full Path)
	char pidList[10000];
	struct processDict* next;
	struct processDict* prev;
} ProcessDictionaryItem;


extern DllDictionaryItem* DllDictHead;
extern DllDictionaryItem* DllDictTail;

extern ProcessDictionaryItem* ProcessDictHead;
extern ProcessDictionaryItem* ProcessDictTail;

//My Dll Struct
typedef struct Dll {
	char name[200];
	struct Dll* Next;
	struct Dll* Prev;
}MyDllStruct;


//My Process Struct
typedef struct process {
	int pid;
	char pName[200];
	DWORD PageFaultCount;
	SIZE_T PagefileUsage;
	SIZE_T WorkingSetSize;
	SIZE_T QuotaPeakPagedPoolUsage;
	SIZE_T QuotaPagedPoolUsage;
	int usedDllsCount;
	MyDllStruct* DllListHead;
	struct process* Next;
	struct process* Prev;
}MyProcessStruct;


//My Snapshot Struct
typedef struct snapStruct {
	int snapId;
	char sysTime[20];
	int processCount;
	MyProcessStruct* processListHead;
	struct snapStruct* Next;
	struct snapStruct* Prev;
}MySnapshotStruct;

extern MySnapshotStruct* snapHead;
extern MySnapshotStruct* snapTail;



//*************************  FUNCTIONS  ******************************//

void calculateWorkingSet();

void AddDllToDict(DllDictionaryItem*);

void AddProcessToDict(ProcessDictionaryItem*);
	
void freeProcessList(MyProcessStruct*);

char* wchar2string(wchar_t* str);

MyProcessStruct* CreateDllList(DWORD);

MyProcessStruct* CreateProcessList();

void oneSnapshot();
	
void snapshot20(char);
	
#endif // !_PROCESS_H
#pragma once

#ifndef _LISTS_H
#define _LISTS_H


//Global Heads and Tails
extern MySnapshotStruct* buildSnapHead;
extern MySnapshotStruct* buildSnapTail;
extern MyProcessStruct* buildProcessHead;
extern MyProcessStruct* buildProcessTail;
extern MyDllStruct* buildDllHead;
extern MyDllStruct* buildDllTail;

//Global Counters
extern int countSnap;
extern int countProcess;
extern int countDll;

//Global Files Names
extern char saveFileName[40];

void CreateSnapFileName();

void WriteSnapListToFile();

void buildDllList(MyDllStruct*);
	
void buildProcessList(MyProcessStruct*);

void BuildSnapListFromFile();
	
#endif // !_LISTS_H
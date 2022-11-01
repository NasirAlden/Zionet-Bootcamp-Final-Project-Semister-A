#pragma once
#ifndef _LISTS_H
#define _LISTS_H


void freeSnapList(MySnapshotStruct*);

void freeDllList(MyDllStruct*);

void freeDllDict(DllDictionaryItem*);

void freeProcessDict(ProcessDictionaryItem*);

void freeAllLists(MySnapshotStruct*);

#endif // !_LISTS_H
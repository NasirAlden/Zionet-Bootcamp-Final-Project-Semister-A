#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include "headers/listsAndStructs.h"
#include <time.h>
#include "headers/logs.h"

#define TEMPLATE_HOME_FILE "th.html"
#define OUTPUT_HOME_FILE "index.html"
#define STRINGLENGTH1 400

//Global variables to save and load, To be showed at home HTML page .
char globalDlls[10] = { '\0' };
char globalProcesses[10] = { '\0' };
char globalMemoryUse[10] = { '\0' };

// Function to clean (Empty) strings from garbage.
void emptyString(char* str) {
	for (int i = 0;i < STRINGLENGTH1;i++) {
		str[i] = '\0';
	}
}

// Generate unique Dlls List that will be showed at Homepage.
void GenerateDllsHomeList(FILE* f) {
	log("Generating Home Html Page.");
	DllDictionaryItem* tmpDll = DllDictHead;
	char dllRow[STRINGLENGTH1] = { '\0' };
	while (tmpDll != NULL && (tmpDll->key >= 1 && tmpDll->key <= globalDllId)) {
		emptyString(dllRow);
		sprintf(dllRow, "<tr>\n<td>%s</td>\n<td><a href=\"dll.html?dll_ID=%d\">Dll%d</a></td>\n</tr>", tmpDll->value, tmpDll->key, tmpDll->key);
		fprintf(f, "%s", dllRow);
		tmpDll = tmpDll->next;
	}
}

// Generate unique Snaps List that will be showed at Homepage.
void GenerateSnapsHomeList(FILE* f) {
	log("Generating Snaps Page.");
	char snapRow[STRINGLENGTH1] = { '\0' };
	int dllCount, i;
	SIZE_T SnapWorkingSet = 0;
	MySnapshotStruct* tmpSnap = snapHead;
	while (tmpSnap != NULL) {
		dllCount = 0;
		i = tmpSnap->snapId;
		SnapWorkingSet = 0;
		MyProcessStruct* tmpProcess = tmpSnap->processListHead;
		while (tmpProcess != NULL) {
			SnapWorkingSet += tmpProcess->WorkingSetSize;
			MyDllStruct* tmpDll = tmpProcess->DllListHead;
			while (tmpDll != NULL) {
				dllCount++;
				tmpDll = tmpDll->Next;
			}
			tmpProcess = tmpProcess->Next;
		}
		emptyString(snapRow);
		sprintf(snapRow, "<tr>\n<td>%d</td>\n<td><a href = \"Snap.html?snapID=%d\">Snap%d.html</a></td>\n<td>%d</td>\n<td>%d</td>\n<td>%zu</td>\n</tr>\n", i, i, i, tmpSnap->processCount, dllCount, SnapWorkingSet);
		fprintf(f, "%s", snapRow);
		tmpSnap = tmpSnap->Next;
	}
}

// Generate HTML Database of all info about all the snaps including: processes , dlls and memory usage.
void GenerateSnapsDatabase() {
	log("Generating HTML Database of all Snaps->Processes->Dlls info.");
	FILE* snapsHtmlDB = fopen("snapsdb.html", "w+");
	if (!snapsHtmlDB) {
		logError(strerror(GetLastError()));
		exit(7);
	}
	MySnapshotStruct* tmpSnap = snapHead;
	while (tmpSnap != NULL) {
		char line[STRINGLENGTH1] = { '\0' };
		sprintf(line, "<span id=\"snap%d\" class=\"%d\" hidden>Snap List Number : %d AT : %s</span>", tmpSnap->snapId, tmpSnap->processCount, tmpSnap->snapId, tmpSnap->sysTime);
		fputs(line, snapsHtmlDB);
		MyProcessStruct* tmpProcess = tmpSnap->processListHead;
		while (tmpProcess != NULL) {
			emptyString(line);
			sprintf(line, "<p id=\"process-name-%d%d\" hidden>%s</p>", tmpSnap->snapId, tmpProcess->pid, tmpProcess->pName);
			fputs(line, snapsHtmlDB);
			emptyString(line);
			sprintf(line, "<p id=\"process-pid-%d%d\" hidden>%d</p>", tmpSnap->snapId, tmpProcess->pid, tmpProcess->pid);
			fputs(line, snapsHtmlDB);
			emptyString(line);
			sprintf(line, "<p id=\"process-pFault-%d%d\" hidden>%lu</p>", tmpSnap->snapId, tmpProcess->pid, tmpProcess->PageFaultCount);
			fputs(line, snapsHtmlDB);
			emptyString(line);
			sprintf(line, "<p id=\"process-wSize-%d%d\" hidden>%zu</p>", tmpSnap->snapId, tmpProcess->pid, tmpProcess->WorkingSetSize);
			fputs(line, snapsHtmlDB);
			emptyString(line);
			sprintf(line, "<p id=\"process-qPool-%d%d\" hidden>%zu</p>", tmpSnap->snapId, tmpProcess->pid, tmpProcess->QuotaPagedPoolUsage);
			fputs(line, snapsHtmlDB);
			emptyString(line);
			sprintf(line, "<p id=\"process-qPeak-%d%d\" hidden>%zu</p>", tmpSnap->snapId, tmpProcess->pid, tmpProcess->QuotaPeakPagedPoolUsage);
			fputs(line, snapsHtmlDB);
			emptyString(line);
			sprintf(line, "<p id=\"process-pFile-%d%d\" hidden>%zu</p>", tmpSnap->snapId, tmpProcess->pid, tmpProcess->PagefileUsage);
			fputs(line, snapsHtmlDB);
			emptyString(line);
			sprintf(line, "<p id=\"process-dllCount-%d%d\" hidden>%d</p>", tmpSnap->snapId, tmpProcess->pid, tmpProcess->usedDllsCount);
			fputs(line, snapsHtmlDB);

			MyDllStruct* tmpDll = tmpProcess->DllListHead;
			int dllId = 1;
			while (tmpDll != NULL) {
				emptyString(line);
				sprintf(line, "<p id=\"dll-%d%d%d\" hidden>%s</p>", tmpSnap->snapId, tmpProcess->pid, dllId++, tmpDll->name);
				fputs(line, snapsHtmlDB);
				tmpDll = tmpDll->Next;
			}

			tmpProcess = tmpProcess->Next;
		}
		tmpSnap = tmpSnap->Next;
	}
	fclose(snapsHtmlDB); // Always close all opened file before exiting from functions/program.
}

//Generate HTML snaps page from Snaps page template.
void GenerateSnapPage() {
	FILE* srcf = fopen("snaptemplate.html", "r+");
	if (!srcf) {
		logError(strerror(GetLastError()));
		exit(8);
	}
	FILE* destf = fopen("Snap.html", "w");
	if (!destf) {
		logError(strerror(GetLastError()));
		exit(9);
	}
	FILE* snapDatabase = fopen("snapsdb.html", "r+");
	if (!snapDatabase) {
		logError(strerror(GetLastError()));
		exit(10);
	}
	char line[STRINGLENGTH1] = { '\0' };
	while (fgets(line, STRINGLENGTH1, srcf)) {
		if (strstr(line, "[inputSnapsDatabase]") == NULL) {
			fputs(line, destf);
		}
		else {
			while (fgets(line, STRINGLENGTH1, snapDatabase)) {
				fputs(line, destf);
			}
		}
	}
	fclose(srcf);
	fclose(destf);
	fclose(snapDatabase);
}

// Generate Dlls page from Dlls page template.
void GenerateDllPage() {
	FILE* srcf = fopen("dlltemplate.html", "r");
	if (!srcf) {
		logError(strerror(GetLastError()));
		exit(11);
	}
	FILE* destf = fopen("dll.html", "w+");
	if (!destf) {
		logError(strerror(GetLastError()));
		exit(12);
	}
	char line[STRINGLENGTH1] = { '\0' };
	int dllCount = 0;

	while (fgets(line, STRINGLENGTH1, srcf)) {
		if (strstr(line, "[inputDllTitle]") != NULL) {
			sprintf(line, "<h3 id=\"dtitle\">x Processes used [dll name]</h3>\n");
			fputs(line, destf);
		}
		else if (strstr(line, "[inputProcessesUsedDll]") == NULL) {
			fputs(line, destf);
		}
		else {
			DllDictionaryItem* tmpDll = DllDictHead;
			dllCount = 0;
			while (tmpDll != NULL) {
				dllCount++;
				emptyString(line);
				sprintf(line, "<p id=\"dll-%d\" hidden>%s</p>\n", dllCount, tmpDll->processesIdUsedDll);
				fputs(line, destf);
				emptyString(line);
				sprintf(line, "<p id=\"dllname-%d\" hidden>%s</p>\n", dllCount, tmpDll->value);
				fputs(line, destf);
				tmpDll = tmpDll->next;
			}
			ProcessDictionaryItem* tmpProcess = ProcessDictHead;
			int processCount = 0;
			while (tmpProcess != NULL) {
				processCount++;
				emptyString(line);
				sprintf(line, "<p id=\"process-%d\" class=\"%s\" hidden>%s</p>\n", tmpProcess->key, tmpProcess->pidList, tmpProcess->value);
				fputs(line, destf);
				tmpProcess = tmpProcess->next;
			}
		}
		emptyString(line);
	}
	fclose(srcf);
	fclose(destf);
}

// Generate Unique Processes list used during all the snaps.
void GenerateProcessDictionaryFile() {
	FILE* destf = fopen("processDictionary.csv", "w+");
	if (!destf) {
		logError(strerror(GetLastError()));
		exit(13);
	}
	char line[STRINGLENGTH1] = { '\0' };
	ProcessDictionaryItem* tmpProcess = ProcessDictHead;
	int processCount = 0;
	sprintf(line, "%s,%s,%s,%s,%s\n", "Serial PID", "Process Name", "Process ID 1", "Process ID 2", "Process ID 3");
	fputs(line, destf);
	while (tmpProcess != NULL) {
		processCount++;
		emptyString(line);
		sprintf(line, "%d,%s,%s\n", processCount, tmpProcess->value, tmpProcess->pidList);
		fputs(line, destf);
		tmpProcess = tmpProcess->next;
	}
	fclose(destf);

}

// Generate the HTML Home Page.
void GenerateHtml(char* srcTXT, char* destTXT, char* srcFile, char* destFile) {
	FILE* f = fopen(srcFile, "r+");
	if (!f) {
		logError(strerror(GetLastError()));
		exit(14);
	}
	char line[STRINGLENGTH1];
	FILE* newHtml = fopen(destFile, "w+");
	if (!newHtml) {
		logError(strerror(GetLastError()));
		exit(15);
	}

	if (globalDllId == 1) {
		sprintf(globalDlls, "%d", 0);
	}
	else {
		sprintf(globalDlls, "%d", globalDllId);
	}
	if (globalProcessId == 1) {
		sprintf(globalProcesses, "%d", 0);
	}
	else {
		sprintf(globalProcesses, "%d", globalProcessId);
	}

	sprintf(globalMemoryUse, "%d%s", globalWorkingSetAvg, " Bytes");

	while (fgets(line, STRINGLENGTH1, f)) {
		char* token = strstr(line, srcTXT);
		char* closingTag = (token == NULL) ? NULL : strstr(token, "</p>");
		if (token) { //if substring (srcTXT) found in line
			int tokenIndex = token - line;
			line[tokenIndex] = '\0';
			if (strcmp(srcTXT, "[inputSnapRows]") == 0) {
				GenerateSnapsHomeList(newHtml);
			}
			else if (strcmp(srcTXT, "[inputDllRows]") == 0) {
				GenerateDllsHomeList(newHtml);
			}
			else {
				fprintf(newHtml, "%s%s%s", line, destTXT, closingTag);
			}

		}
		else {
			fprintf(newHtml, "%s", line);
		}
		emptyString(line);
	}
	fclose(f);
	fclose(newHtml);
}

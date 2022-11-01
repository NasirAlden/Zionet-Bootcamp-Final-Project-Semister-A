#pragma once
#define TEMPLATE_HOME_FILE "th.html"
#define OUTPUT_HOME_FILE "index.html"
#define STRINGLENGTH1 400

char globalDlls[10];
char globalProcesses[10];
char globalMemoryUse[10];

void emptyString(char*);

void GenerateDllsHomeList(FILE*);

void GenerateSnapsHomeList(FILE*);

void GenerateSnapsDatabase();

void GenerateSnapPage();
	
void GenerateDllPage();
	
void GenerateProcessDictionaryFile();
	
void GenerateHtml(char*, char*, char*, char*);
	
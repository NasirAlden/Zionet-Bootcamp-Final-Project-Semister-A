#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <time.h>
#include "headers/listsAndStructs.h"
#include <Windows.h>
#include "headers/freeLists.h"
#include "headers/files.h"
#include "headers/generateHomeHTML.h"
#include "headers/sort.h"
#pragma warning(disable:4996)

int main()
{
	char op = '\0';
	log("Program Started");

	do
	{
		// Multichoice Main Menu of the program.
		printf("\n***** Windows Processes info System *****\n\n");
		printf("Choose an operation to do:\n");
		printf("1) Take One SnapShot.\n");
		printf("2) Take 20 SnapShots\n");
		printf("3) Start Long SnapShot\n");
		printf("4) End Long SnapShot\n");
		printf("5) Generate HTML Report\n");
		printf("6) Reset Collections\n");
		printf("7) Save in File\n");
		printf("8) Load from File\n");
		printf("Q) Quit.\n");
		scanf(" %c", &op);
		switch (toupper(op)) {
		case '1':
			oneSnapshot();
			break;
		case '2':
			snapshot20(op);
			break;
		case '3':
			snapshot20(op);
			break;
		case '4':
			// interrupt the function Snapshot20 which is also the long snapshot function.
			break;
		case '5': {
			/*tmpFiles = 0;*/
			calculateWorkingSet();
			BubblesortProcessList();
			GenerateHtml("[inputdll]", globalDlls, "HomeTemplate.html", "index.html");
			GenerateHtml("[inputprocess]", globalProcesses, "index.html", "tmp.html");
			GenerateHtml("[inputmemoryavg]", globalMemoryUse, "tmp.html", "tmp1.html");
			GenerateHtml("[inputSnapRows]", "", "tmp1.html", "tmp.html");
			GenerateHtml("[inputDllRows]", "", "tmp.html", "index.html");
			remove("tmp.html");
			remove("tmp1.html");
			GenerateSnapsDatabase();
			GenerateSnapPage();
			GenerateDllPage();
			GenerateProcessDictionaryFile();
			break;}
		case '6':
			freeAllLists(snapHead);
			break;
		case '7':
			WriteSnapListToFile();
			break;
		case '8':
			BuildSnapListFromFile();
			break;
		}
	} while (toupper(op) != 'Q');
	printf("Thanks for using our system , Goodbye!\n");
	log("Program Ended.\n");
	freeAllLists(snapHead);
	return 0;
}
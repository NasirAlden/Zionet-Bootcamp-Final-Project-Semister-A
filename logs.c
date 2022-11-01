#include "headers/logs.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h> 
#include <string.h>

#pragma warning(disable:4996)
#define _CRT_SECURE_NO_WARNINGS

// Get the system time as a string
char* getSysTime() {
	time_t t;
	time(&t);
	struct tm* timeInfo;
	timeInfo = localtime(&t);
	char strTime[100] = "";
	sprintf(strTime, "[%d/%d/%d-%d:%d]", timeInfo->tm_year + 1900,
		timeInfo->tm_mon + 1, timeInfo->tm_mday, timeInfo->tm_hour, timeInfo->tm_min);
	return strTime;
}

//General log to save logs to file - Used by Error and Event log functions.
void log(char message[100])
{
	time_t t;
	time(&t);
	struct tm* timeInfo;
	timeInfo = localtime(&t);
	char strTime[100];
	//sprintf(strTime, "[%d/%d/%d-%d:%d] -", timeInfo->tm_year + 1900,
	//	timeInfo->tm_mon + 1, timeInfo->tm_mday, timeInfo->tm_hour, timeInfo->tm_min);
	sprintf(strTime, "%s", getSysTime());
	char logFileName[25] = { '\0' };
	sprintf(logFileName, "%s_%d_%d_%d.log", "logFile", timeInfo->tm_mday, timeInfo->tm_mon + 1, timeInfo->tm_year + 1900);
	FILE* f = fopen(logFileName, "a");

	strcat(strTime, message);
	strcat(strTime, "\n");
	printf("%s\n", strTime);
	while (fputs(strTime, f) != NULL)
	{

	}
	fclose(f);
}


//log to file all errors occured while compiling the program or due to runtime.
void logError(char* message) {
	char str[100] = "Error!! - ";
	strcat(str, message);
	log(str);
}

//log to file important events
void logEvent(char* message) {
	char str[100] = "[Event] - ";
	strcat(str, message);
	log(str);
}


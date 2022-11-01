#pragma once
#ifndef  _LOGS_H
#define _LOGS_H

#pragma warning(disable:4996)
#define _CRT_SECURE_NO_WARNINGS

// Get the system time as a string
char* getSysTime();

void log(char message[100]);

void logError(char* message);

void logEvent(char* message);

#endif // ! _LOGS_H
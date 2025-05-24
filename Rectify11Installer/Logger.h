#pragma once

#ifndef MY_HEADERLOGGER_H
#define MY_HEADERLOGGER_H

#include "framework.h"
using namespace std;
class Logger {
private:
	wfstream logfile;
	std::mutex mutex;
public:
	void StartLogger(const wchar_t* filename);
	void WriteLine(wstring line);
	void WriteLine(wstring line, int exitCode);
	~Logger();
};

#endif
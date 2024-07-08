#pragma once

#ifndef MY_HEADERLOGGER_H
#define MY_HEADERLOGGER_H

#include <fstream>
#include <string>
using namespace std;
class Logger {
private:
	wfstream logfile;
public:
	void StartLogger(const wchar_t* filename);
	void WriteLine(wstring line);
	void WriteLine(wstring line, int exitCode);
	~Logger();
};

#endif
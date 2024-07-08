#include "Logger.h"
#include "framework.h"
#include <fstream>
#include <string>
#include <codecvt>
#include <locale>
using namespace std;


void Logger::StartLogger(const wchar_t* filename) {
	logfile.open(filename, wfstream::binary | wfstream::out | wfstream::app);
	if (!logfile)
		logfile.open(filename, wfstream::binary | wfstream::trunc | wfstream::out);

	logfile << "\n\n";
	time_t _tm = time(NULL);
	tm curtime;
	localtime_s(&curtime, &_tm);
	char t[256];
	asctime_s(t, &curtime);
	wstring wt = wstring_convert<codecvt_utf8<wchar_t>>().from_bytes(t);
	wt.erase(remove(wt.begin(), wt.end(), L'\n'), wt.cend());
	if (logfile.good())logfile << "================ Logging started at " << wt << L" ================\n" << endl;
}

void Logger::WriteLine(wstring line) {
	if(logfile.good())logfile << line << endl;
}

void Logger::WriteLine(wstring line, int exitCode) {
	logfile << line << L" (exit code: " << to_wstring(exitCode) << L")" << endl;
}

Logger::~Logger() {
	logfile.close();
}
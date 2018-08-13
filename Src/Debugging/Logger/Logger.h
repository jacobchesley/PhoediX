// Copyright 2018 Jacob Chesley.
// See LICENSE.TXT in root of project for license information.

#ifndef LOGGER_H
#define LOGGER_H

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif

#include "wx/datetime.h"
#include "wx/textfile.h"
#include "wx/stdpaths.h"
#include "wx/filename.h"

class Logger {
public:
	
	static void SetApp(wxApp * application);
	static void Log(wxString message, int logLevel);
	static void SetLogLevel(int logLevel);
	static void LogToFile(bool file);
	static void LogToConsole(bool console);
	static void SetLogFileName();

enum LogLevel{
	LOG_FATAL = 0,
	LOG_ERROR = 1,
	LOG_WARNING = 2,
	LOG_MESSAGE = 3,
	LOG_VERBOSE = 4
};

private:
	static void LogToFile(wxString message);
	static void LogToWindow(wxString message);

	wxFile CreateLogFile();
	void StartLogFile();
	void StopLogFile();

	static wxString logName;
	static wxTextFile logFile;
	static bool logToFile;
	static bool logToConsole;
	static int level;
	static wxApp * app;
};

#endif
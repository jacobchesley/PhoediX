// Copyright 2016 Jacob Chesley

#include "Logger.h"

bool Logger::logToFile = false;
bool Logger::logToConsole = false;
int Logger::level = 0;
wxString Logger::logName = "";
wxTextFile Logger::logFile;

wxApp* Logger::app = NULL;

void Logger::SetLogLevel(int logLevel){
	level = logLevel;
}

void Logger::SetApp(wxApp * application){
	app = application;
}

void Logger::LogToFile(bool file){
	logToFile = file;

	if(logToFile){
		if(wxFile::Exists(logName)){
			logFile.Open(logName);
		}
		else{
			logFile.Create(logName);
			logFile.Open(logName);
		}
	}
	else{
		logFile.Close();
	}
}

void Logger::LogToConsole(bool console){
	logToConsole = console;

	#ifdef __WXMSW__
	if(logToConsole){
		AllocConsole();
		HANDLE lStdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		int hConHandle = _open_osfhandle((intptr_t)lStdHandle, _O_TEXT);
		FILE * fp = _fdopen(hConHandle, "w");
		*stdout = *fp;
		setvbuf(stdout, NULL, _IONBF, 0);
	}
	else{
		FreeConsole();
	}
	#endif
}

void Logger::Log(wxString message, int logLevel){
	
	bool fatalError = false;
	bool outputToFile = false;
	bool outputToCommand = false;

	// Check current log level setting
	if(logLevel <= level){
		if(logToFile){outputToFile = true;}
		outputToCommand = true;
	}

	wxString logMessage = "";

	switch(logLevel){
		case Logger::LogLevel::LOG_FATAL:
			fatalError = true;
			logMessage += "FATAL ERROR! - " + message;
			break;

		case Logger::LogLevel::LOG_ERROR:
			logMessage += "ERROR! - " + message;
			break;

		case Logger::LogLevel::LOG_WARNING:
			logMessage += "WARNING - " + message;
			break; 

		case Logger::LogLevel::LOG_MESSAGE:
			logMessage += message;
			break;

		case Logger::LogLevel::LOG_VERBOSE:
			logMessage += message;
			break;
	}

	// FATAL ERROR, always log to window and possibly to file.  Exit
	if(fatalError){ 
		LogToWindow(logMessage);
		if(outputToFile){
			LogToFile(logMessage);
		}
		app->Exit();
		return;
	}

	// Output message to appropriate places
	if(outputToCommand){ LogToWindow(logMessage); }
	if(outputToFile){ LogToFile(logMessage); }
}

void Logger::SetLogFileName(){

	wxDateTime now = wxDateTime::Now();
	wxString month = wxString::Format("%d ", now.GetMonth()) + "_";
	wxString day = wxString::Format("%d ", now.GetDay()) + "_";
	wxString year = wxString::Format("%d ", now.GetYear()) + "_";

	wxString hour = wxString::Format("%d ", now.GetHour()) + "_";
	wxString minute = wxString::Format("%d ", now.GetMinute()) + "_";
	wxString second = wxString::Format("%d ", now.GetSecond()) + "_";

	wxFileName executable = wxFileName(wxStandardPaths::Get().GetExecutablePath());

	logName = executable.GetPathWithSep() +  "PhoediX_Log_" + month + day + year + "_" + hour + minute + second + ".log";
}

void Logger::LogToWindow(wxString message){

	std::cout<< message.ToStdString() << std::endl;
}

void Logger::LogToFile(wxString message){

	logFile.AddLine(message);
	logFile.Write();
}
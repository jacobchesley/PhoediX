#ifndef LOGGER_H
#define LOGGER_H
	
// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx\textfile.h"

class Logger {
public:
	Logger();
	Logger(wxString logFile);
	void SetLogFile(wxString fileName);
	void Log(wxString appendToFile);
	wxString ReadLine(int lineNum);
	int GetNumLines();
private:
	wxString fileName;
};

#endif
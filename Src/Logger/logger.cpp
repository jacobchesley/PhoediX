// Copyright 2016 Jacob Chesley

#include "Logger.h"

Logger::Logger() {

}

Logger::Logger(wxString logFile) {
	fileName = logFile;
}

void Logger::SetLogFile(wxString logFile) {
	fileName = logFile;
}

void Logger::Log(wxString appendToFile) {

	if (!wxFile::Exists(fileName)) {
		wxFile * newFile = new wxFile();
		newFile->Create(fileName);
		delete newFile;
	}
	wxFile file(fileName, wxFile::write_append);
	if (!file.IsOpened()) {
		return;
	}
	file.Write(appendToFile);
	file.Close();
}

wxString Logger::ReadLine(int lineNum) {

	wxTextFile textFile(fileName);
	textFile.Open();
	wxString returnStr = textFile.GetLine(lineNum);
	returnStr += textFile.GetEOL();
	textFile.Close();
	return returnStr;
}

int Logger::GetNumLines() {

	wxTextFile textFile(fileName);
	textFile.Open();
	int numLines = textFile.GetLineCount();
	textFile.Close();
	return numLines;
}
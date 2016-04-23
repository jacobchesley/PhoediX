#include "MainWindow.h"

MainWindow::MainWindow() : wxFrame(NULL, -1, "PhoediX", wxDefaultPosition, wxDefaultSize){
	this->SetSizeProperties();
}

void MainWindow::SetSizeProperties(){

	// Set the size to the screen size, and position at point 0, 0.  Then enable maximize the screen.
	this->SetSize(wxSystemSettings::GetMetric(wxSYS_SCREEN_X), wxSystemSettings::GetMetric(wxSYS_SCREEN_Y));
	this->SetPosition(wxPoint(0, 0));
	this->Maximize(true);
}
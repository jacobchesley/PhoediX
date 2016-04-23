#include "MainWindow.h"

MainWindow::MainWindow() : wxFrame(NULL, -1, "PhoediX", wxDefaultPosition, wxDefaultSize){
	this->SetSizeProperties();

	// Create and add menu bar with menus
	menuBar = new wxMenuBar;

	menuFile = new wxMenu;
	menuView = new wxMenu;
	menuHelp = new wxMenu;

	menuFile->Append(MainWindow::MenuBar::ID_SHOW_LOAD_FILE, _("Open Image"));
	menuFile->AppendSeparator();
	menuFile->Append(MainWindow::MenuBar::ID_EXIT, _("Exit"));

	// Append menus to menu bar
	menuBar->Append(menuFile, _("&File"));
	menuBar->Append(menuView, _("&View"));
	menuBar->Append(menuHelp, _("&Help"));

	// Set the menu bar
	this->SetMenuBar(menuBar);

	// Set the status bar
	this->CreateStatusBar();

	processor = new Processor();

	this->Bind(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainWindow::ShowLoadFile, this, MainWindow::MenuBar::ID_SHOW_LOAD_FILE);
}

void MainWindow::SetSizeProperties(){

	// Set the size to the screen size, and position at point 0, 0.  Then enable maximize the screen.
	this->SetSize(wxSystemSettings::GetMetric(wxSYS_SCREEN_X), wxSystemSettings::GetMetric(wxSYS_SCREEN_Y));
	this->SetPosition(wxPoint(0, 0));
	this->Maximize(true);
}

void MainWindow::ShowLoadFile(wxCommandEvent& WXUNUSED(event)){

	wxFileDialog openFileDialog(this, _("Open Image"), "", "", "Photos (*.jpeg,*.jpg*.png,*.bmp,*.tiff)|*.jpeg;*.jpg;*.png;*.bmp;*.tiff", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	if (openFileDialog.ShowModal() == wxID_CANCEL) {
		return;
	}
	ImageFileLoader::LoadImageFromFile(openFileDialog.GetPath(), processor->GetImage());
}
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
	statusBarText = NULL;
	this->GetStatusBar()->SetBackgroundColour(Colors::BackDarkDarkDarkGrey);
	this->GetStatusBar()->SetForegroundColour(Colors::TextLightGrey);
	this->SetStatusbarText("test");

	processor = new Processor();


	auiManager = new wxAuiManager(this);
	auiManager->GetArtProvider()->SetColor(wxAuiPaneDockArtSetting::wxAUI_DOCKART_BACKGROUND_COLOUR, Colors::BackDarkDarkGrey);

	editList = new EditListPanel(this);
	auiManager->AddPane(editList,  wxRIGHT, "Edit List");
	auiManager->GetPane(editList).MinSize(wxSize(editList->GetSize().GetWidth() + 200, 50));
	
	this->Bind(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainWindow::ShowLoadFile, this, MainWindow::MenuBar::ID_SHOW_LOAD_FILE);
	this->Bind(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainWindow::OnClose, this, MainWindow::MenuBar::ID_EXIT);
	this->Bind(wxEVT_CLOSE_WINDOW, (wxObjectEventFunction)&MainWindow::OnClose, this);

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
	ImageHandler::LoadImageFromFile(openFileDialog.GetPath(), processor->GetImage());
	imagePanel = new ImagePanel(this, processor->GetImage());
	imagePanel->SetBackgroundColour(Colors::BackDarkDarkGrey);
	auiManager->AddPane(imagePanel, wxLEFT);
	auiManager->Update();
}


void MainWindow::SetStatusbarText(wxString text) {
	if (statusBarText != NULL) {
		statusBarText->Destroy();
	}
	statusBarText = new wxStaticText(this->GetStatusBar(), -1, text, wxPoint(10, 5));
	statusBarText->Show();
}

void MainWindow::OnClose(wxCloseEvent& closeEvent) {

	auiManager->UnInit();
	delete auiManager;
	delete processor;
	closeEvent.Skip();
	this->Destroy();
}
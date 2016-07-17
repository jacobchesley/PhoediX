#include "MainWindow.h"

MainWindow::MainWindow() : wxFrame(NULL, -1, "PhoediX", wxDefaultPosition, wxDefaultSize){
	this->SetSizeProperties();
	
	// Create and add menu bar with menus
	menuBar = new wxMenuBar;

	menuFile = new wxMenu;
	menuView = new wxMenu;
	menuHelp = new wxMenu;

	menuFile->Append(MainWindow::MenuBar::ID_SHOW_LOAD_PROJECT, _("Open PhoediX Project"));
	menuFile->Append(MainWindow::MenuBar::ID_SHOW_SAVE_PROJECT, _("Save PhoediX Project"));
	menuFile->AppendSeparator();
	menuFile->Append(MainWindow::MenuBar::ID_SHOW_LOAD_FILE, _("Open Image"));
	menuFile->AppendSeparator();
	menuFile->Append(MainWindow::MenuBar::ID_EXIT, _("Exit"));

	menuView->Append(MainWindow::MenuBar::ID_SHOW_IMAGE, _("Show Image"));
	menuView->Append(MainWindow::MenuBar::ID_SHOW_EDIT_LIST, _("Show Edit list"));
	menuView->Append(MainWindow::MenuBar::ID_SHOW_HISTOGRAMS, _("Show Histograms"));

	// Append menus to menu bar
	menuBar->Append(menuFile, _("&File"));
	menuBar->Append(menuView, _("&View"));
	menuBar->Append(menuHelp, _("&Help"));

	// Set the menu bar
	this->SetMenuBar(menuBar);

	this->SetBackgroundColour(Colors::BackDarkDarkGrey);

	// Set the status bar
	this->CreateStatusBar();
	statusBarText = NULL;
	this->GetStatusBar()->SetBackgroundColour(Colors::BackDarkDarkDarkGrey);
	this->GetStatusBar()->SetForegroundColour(Colors::TextLightGrey);
	this->SetStatusbarText("test");

	processor = new Processor(this);

	auiManager = new wxAuiManager(this);
	auiManager->GetArtProvider()->SetColor(wxAuiPaneDockArtSetting::wxAUI_DOCKART_BACKGROUND_COLOUR, Colors::BackDarkDarkGrey);
	PhoedixAUIManager::SetPhoedixAUIManager(auiManager);

	editList = new EditListPanel(this, processor);
	wxAuiPaneInfo editListInfo = wxAuiPaneInfo();
	editListInfo.Right();
	editListInfo.Caption("Edit List");
	editListInfo.Name("EditList");
	editListInfo.CloseButton(true);
	editListInfo.PinButton(true);
	editListInfo.DestroyOnClose(false);
	auiManager->AddPane(editList, editListInfo);

	imagePanel = new ZoomImagePanel(this, processor->GetImage());
	imagePanel->SetBackgroundColour(Colors::BackDarkDarkGrey);

	wxAuiPaneInfo imageInfo = wxAuiPaneInfo();
	imageInfo.Center();
	imageInfo.CloseButton(true);
	imageInfo.PinButton(true);
	imageInfo.DestroyOnClose(false);
	imageInfo.Name("ImageDisplay");

	auiManager->AddPane(imagePanel, imageInfo);

	histogramDisplay = new HistogramDisplay(this, processor);
	wxAuiPaneInfo histogramPaneInfo = wxAuiPaneInfo();
	histogramPaneInfo.Left();
	histogramPaneInfo.Caption("Histogram");
	histogramPaneInfo.Name("Histogram");
	histogramPaneInfo.CloseButton(true);
	histogramPaneInfo.PinButton(true);
	histogramPaneInfo.DestroyOnClose(false);
	histogramPaneInfo.BestSize(histogramDisplay->GetClientSize());
	auiManager->AddPane(histogramDisplay, histogramPaneInfo);
	auiManager->Update();
	
	this->Bind(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainWindow::ShowLoadProject, this, MainWindow::MenuBar::ID_SHOW_LOAD_PROJECT);
	this->Bind(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainWindow::ShowSaveProject, this, MainWindow::MenuBar::ID_SHOW_SAVE_PROJECT);
	this->Bind(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainWindow::ShowLoadFile, this, MainWindow::MenuBar::ID_SHOW_LOAD_FILE);
	this->Bind(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainWindow::OnClose, this, MainWindow::MenuBar::ID_EXIT);
	this->Bind(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainWindow::ShowImage, this, MainWindow::MenuBar::ID_SHOW_IMAGE);
	this->Bind(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainWindow::ShowEditList, this, MainWindow::MenuBar::ID_SHOW_EDIT_LIST);
	this->Bind(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainWindow::ShowHistograms, this, MainWindow::MenuBar::ID_SHOW_HISTOGRAMS);
	this->Bind(PROCESSOR_MESSAGE_EVENT, (wxObjectEventFunction)&MainWindow::RecieveMessageFromProcessor, this, ID_PROCESSOR_MESSAGE);
	this->Bind(wxEVT_CLOSE_WINDOW, (wxObjectEventFunction)&MainWindow::OnClose, this);
	
	imgPanelThread = new ImagePanelUpdateThread(imagePanel, processor, histogramDisplay);
	imgPanelThread->Run();

	ImageHandler::LoadImageFromwxImage(new wxImage(0,0), processor->GetImage());
	processor->SetOriginalImage(processor->GetImage());
	
}

void MainWindow::SetSizeProperties(){

	// Set the size to the screen size, and position at point 0, 0.  Then enable maximize the screen.
	this->SetSize(wxSystemSettings::GetMetric(wxSYS_SCREEN_X), wxSystemSettings::GetMetric(wxSYS_SCREEN_Y));
	this->SetPosition(wxPoint(0, 0));
	this->Maximize(true);
}

void MainWindow::ShowSaveProject(wxCommandEvent& WXUNUSED(event)) {

	wxFileDialog saveFileDialog(this, _("Save PhoediX Project"), "", "", "PhoediX Project Files (*.phx)|*.phx", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	if (saveFileDialog.ShowModal() == wxID_CANCEL) {
		return;
	}

	editList->AddEditsToProcessor();
	
	session.GetEditList()->SetSessionEditList(processor->GetEditVector());
	session.SetHistogramDisplaySelect(histogramDisplay->GetHistogramDisplay());
	session.SetImageZoomLevel(imagePanel->GetZoom());
	session.SetImageScrollX(imagePanel->GetDragX());
	session.SetImageScrollY(imagePanel->GetDragY());
	session.SetPerspective(PhoedixAUIManager::GetPhoedixAUIManager()->SavePerspective());
	session.SaveSessionToFile(saveFileDialog.GetPath());
}
void MainWindow::ShowLoadProject(wxCommandEvent& WXUNUSED(event)){

	wxFileDialog openFileDialog(this, _("Open PhoediX Project"), "", "", "PhoediX Project Files (*.phx)|*.phx", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	if (openFileDialog.ShowModal() == wxID_CANCEL) {
		return;
	}

	session.LoadSessionFromFile(openFileDialog.GetPath());

	// Set the new image
	delete processor->GetOriginalImage();
	ImageHandler::LoadImageFromFile(session.GetImageFilePath(), processor->GetImage());
	processor->SetOriginalImage(processor->GetImage());
	
	processor->Lock();
	imagePanel->ChangeImage(processor->GetImage());
	imagePanel->Redraw();
	processor->Unlock();
	processor->SetUpdated(true);

	histogramDisplay->SetHistogramDisplay(session.GetHistogramDisplaySelect());
	imagePanel->SetZoom(session.GetImageZoomLevel());

	// Populate the panel with the edits
	wxVector<ProcessorEdit*> editLayers = session.GetEditList()->GetSessionEditList();
	editList->AddEditWindows(editLayers);

	// Load perspective after edits are loaded
	wxString perspect = session.GetPerspective();
	PhoedixAUIManager::GetPhoedixAUIManager()->LoadPerspective(session.GetPerspective(), true);
	PhoedixAUIManager::GetPhoedixAUIManager()->Update();

	imagePanel->SetDrag(session.GetImageScrollX(), session.GetImageScrollY());
}

void MainWindow::ShowLoadFile(wxCommandEvent& WXUNUSED(event)) {

	wxFileDialog openFileDialog(this, _("Open Image"), "", "", "Photos (*.jpeg,*.jpg*.png,*.bmp,*.tiff)|*.jpeg;*.jpg;*.png;*.bmp;*.tiff", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	if (openFileDialog.ShowModal() == wxID_CANCEL) {
		return;
	}

	delete processor->GetOriginalImage();
	ImageHandler::LoadImageFromFile(openFileDialog.GetPath(), processor->GetImage());
	processor->SetOriginalImage(processor->GetImage());
	processor->SetUpdated(true);

	session.SetImageFilePath(openFileDialog.GetPath());
}


void MainWindow::ShowImage(wxCommandEvent& WXUNUSED(event)) {
	auiManager->GetPane(imagePanel).Show();
	auiManager->Update();
}

void MainWindow::ShowEditList(wxCommandEvent& WXUNUSED(event)) {
	auiManager->GetPane(editList).Show();
	auiManager->Update();
}

void MainWindow::ShowHistograms(wxCommandEvent& WXUNUSED(event)) {
	auiManager->GetPane(histogramDisplay).Show();
	auiManager->Update();
}

void MainWindow::SetStatusbarText(wxString text) {
	if (statusBarText != NULL) {
		statusBarText->Destroy();
	}
	statusBarText = new wxStaticText(this->GetStatusBar(), -1, text, wxPoint(10, 5));
	statusBarText->Show();
}

void MainWindow::RecieveMessageFromProcessor(wxCommandEvent& messageEvt) {
	this->SetStatusbarText(messageEvt.GetString());
}

void MainWindow::OnClose(wxCloseEvent& closeEvent) {

	imgPanelThread->StopWatching();
	auiManager->UnInit();
	delete auiManager;
	closeEvent.Skip();
	this->Destroy();

	delete processor;
}

MainWindow::ImagePanelUpdateThread::ImagePanelUpdateThread(ZoomImagePanel * imagePanel, Processor * processor, HistogramDisplay * histogramDisplay) : wxThread(wxTHREAD_DETACHED) {
	imgPanel = imagePanel;
	histogramDisp = histogramDisplay;
	proc = processor;
	continueWatch = true;
}

wxThread::ExitCode MainWindow::ImagePanelUpdateThread::Entry() {

	while (continueWatch) {
		if (proc->GetUpdated() && !proc->GetLocked()) {
			proc->Lock();
			imgPanel->ChangeImage(proc->GetImage());
			imgPanel->Redraw();
			histogramDisp->UpdateHistograms();
			proc->Unlock();
			proc->SetUpdated(false);
		}
		this->Sleep(100);
	}
	return (wxThread::ExitCode)0;
}

void MainWindow::ImagePanelUpdateThread::StopWatching() {
	continueWatch = false;
}
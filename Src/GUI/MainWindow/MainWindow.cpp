// Copyright 2016 Jacob Chesley

#include "MainWindow.h"

MainWindow::MainWindow() : wxFrame(NULL, -1, "PhoediX", wxDefaultPosition, wxDefaultSize){

	Icons icons;
	wxIcon theIcon;
	theIcon.CopyFromBitmap(wxBitmap(icons.pxIcon));
	this->SetIcon(theIcon);

	this->SetSizeProperties();
	
	// Create and add menu bar with menus
	menuBar = new wxMenuBar();

	menuFile = new wxMenu();
	menuCloseProjects = new wxMenu();
	menuView = new wxMenu();
	menuTools = new wxMenu();
	menuWindow = new wxMenu();
	menuHelp = new wxMenu();

	menuFile->Append(MainWindow::MenuBar::ID_NEW_PROJECT, _("New PhoediX Project"));
	menuFile->Append(MainWindow::MenuBar::ID_SHOW_LOAD_PROJECT, _("Open PhoediX Project"));
	menuFile->Append(MainWindow::MenuBar::ID_SHOW_SAVE_PROJECT, _("Save PhoediX Project"));
	menuCloseProjects->Append(MainWindow::MenuBar::ID_CLOSE_CURRENT_PROJECT, _("Close Current Project"));
	menuCloseProjects->Append(MainWindow::MenuBar::ID_CLOSE_ALL_PROJECTS, _("Close All Projects"));
	menuFile->AppendSubMenu(menuCloseProjects, _("Close Project..."));
	menuFile->AppendSeparator();
	menuFile->Append(MainWindow::MenuBar::ID_SHOW_LOAD_FILE, _("Open Image"));
	menuFile->AppendSeparator();
	menuFile->Append(MainWindow::MenuBar::ID_SHOW_EXPORT, _("Export Image"));
	menuFile->AppendSeparator();
	menuFile->Append(MainWindow::MenuBar::ID_SHOW_SETTINGS, _("PhoediX Settings"));
	menuFile->AppendSeparator();
	menuFile->Append(MainWindow::MenuBar::ID_EXIT, _("Exit"));

	menuView->AppendCheckItem(MainWindow::MenuBar::ID_SHOW_IMAGE, _("Show Image"));
	menuView->AppendCheckItem(MainWindow::MenuBar::ID_SHOW_EDIT_LIST, _("Show Edit list"));
	menuView->AppendCheckItem(MainWindow::MenuBar::ID_SHOW_HISTOGRAMS, _("Show Histograms"));

	menuTools->AppendCheckItem(MainWindow::MenuBar::ID_SHOW_SNAPSHOTS, _("Show Snapshots"));
	menuTools->AppendCheckItem(MainWindow::MenuBar::ID_SHOW_PIXEL_PEEP, _("Show Pixel Peeper"));
	menuTools->AppendSeparator();
	menuTools->AppendCheckItem(MainWindow::MenuBar::ID_SHOW_LIBRARY, _("Show Library"));

	menuHelp->Append(MainWindow::MenuBar::ID_ABOUT, _("About PhoediX"));

	// Append menus to menu bar
	menuBar->Append(menuFile, _("&File"));
	menuBar->Append(menuView, _("&View"));
	menuBar->Append(menuTools, _("&Tools"));
	menuBar->Append(menuWindow, _("&Windows"));
	menuBar->Append(menuHelp, _("&Help"));

	// Set the menu bar
	this->SetMenuBar(menuBar);
	
	this->SetBackgroundColour(Colors::BackDarkDarkGrey);

	// Set the status bar
	this->CreateStatusBar();
	statusBarText = NULL;
	this->GetStatusBar()->SetBackgroundColour(Colors::BackDarkDarkDarkGrey);
	this->GetStatusBar()->SetForegroundColour(Colors::TextLightGrey);
	
	processor = new Processor(this);
	processor->SetMultithread(true);
	processor->SetNumThreads(wxThread::GetCPUCount());

	auiManager = new wxAuiManager(this);
	auiManager->GetArtProvider()->SetColor(wxAuiPaneDockArtSetting::wxAUI_DOCKART_BACKGROUND_COLOUR, Colors::BackDarkDarkGrey);
	PhoedixAUIManager::SetPhoedixAUIManager(auiManager);

	histogramDisplay = new HistogramDisplay(this, processor);
	wxAuiPaneInfo histogramPaneInfo = wxAuiPaneInfo();
	histogramPaneInfo.Right();
	histogramPaneInfo.Caption("Histogram");
	histogramPaneInfo.Name("Histogram");
	histogramPaneInfo.CloseButton(true);
	histogramPaneInfo.PinButton(true);
	histogramPaneInfo.DestroyOnClose(false);
	histogramPaneInfo.BestSize(histogramDisplay->GetClientSize());
	histogramPaneInfo.Hide();
	auiManager->AddPane(histogramDisplay, histogramPaneInfo);

	editList = new EditListPanel(this, processor);
	wxAuiPaneInfo editListInfo = wxAuiPaneInfo();
	editListInfo.Right();
	editListInfo.Caption("Edit List");
	editListInfo.Name("EditList");
	editListInfo.CloseButton(true);
	editListInfo.PinButton(true);
	editListInfo.DestroyOnClose(false);
	editListInfo.Hide();
	auiManager->AddPane(editList, editListInfo);

	imagePanel = new ZoomImagePanel(this, processor->GetImage());
	imagePanel->SetBackgroundColour(Colors::BackDarkDarkGrey);

	wxAuiPaneInfo imageInfo = wxAuiPaneInfo();
	imageInfo.Center();
	imageInfo.CloseButton(true);
	imageInfo.PinButton(true);
	imageInfo.MaximizeButton(true);
	imageInfo.DestroyOnClose(false);
	imageInfo.Name("ImageDisplay");
	imageInfo.Hide();
	auiManager->AddPane(imagePanel, imageInfo);

	exportWindow = new ExportWindow(this, processor, editList);
	wxAuiPaneInfo exportPaneInfo = wxAuiPaneInfo();
	exportPaneInfo.Float();
	exportPaneInfo.Caption("Export Image");
	exportPaneInfo.Name("Export");
	exportPaneInfo.CloseButton(true);
	exportPaneInfo.PinButton(true);
	exportPaneInfo.DestroyOnClose(false);
	exportPaneInfo.BestSize(exportWindow->GetClientSize());
	exportPaneInfo.Hide();
	auiManager->AddPane(exportWindow, exportPaneInfo);
	
	settingsWindow = new SettingsWindow(this, processor);
	wxAuiPaneInfo settingsPaneInfo = wxAuiPaneInfo();
	settingsPaneInfo.Float();
	settingsPaneInfo.Caption("PhoediX Settings");
	settingsPaneInfo.Name("Settings");
	settingsPaneInfo.CloseButton(true);
	settingsPaneInfo.PinButton(true);
	settingsPaneInfo.DestroyOnClose(false);
	settingsPaneInfo.BestSize(settingsWindow->GetClientSize());
	settingsPaneInfo.Hide();
	auiManager->AddPane(settingsWindow, settingsPaneInfo);

	libraryWindow = new LibraryWindow(this);
	wxAuiPaneInfo libraryPaneInfo = wxAuiPaneInfo();
	libraryPaneInfo.Center();
	libraryPaneInfo.Caption("Image Library");
	libraryPaneInfo.Name("Library");
	libraryPaneInfo.CloseButton(true);
	libraryPaneInfo.PinButton(true);
	libraryPaneInfo.MaximizeButton(true);
	libraryPaneInfo.DestroyOnClose(false);
	libraryPaneInfo.BestSize(settingsWindow->GetClientSize());
	libraryPaneInfo.Hide();
	auiManager->AddPane(libraryWindow, libraryPaneInfo);

	snapshotWindow = new SnapshotWindow(this, editList, processor);
	wxAuiPaneInfo snapshotPaneInfo = wxAuiPaneInfo();
	snapshotPaneInfo.Float();
	snapshotPaneInfo.Caption("Snapshots");
	snapshotPaneInfo.Name("Snapshots");
	snapshotPaneInfo.CloseButton(true);
	snapshotPaneInfo.PinButton(true);
	snapshotPaneInfo.DestroyOnClose(false);
	snapshotPaneInfo.BestSize(snapshotWindow->GetClientSize());
	snapshotPaneInfo.Hide();
	auiManager->AddPane(snapshotWindow, snapshotPaneInfo);

	pixelPeepWindow = new PixelPeepWindow(this);
	this->Bind(wxEVT_RIGHT_DOWN, (wxObjectEventFunction)&MainWindow::OnImagePanelMouse, this);
	wxAuiPaneInfo pixelPeepPaneInfo = wxAuiPaneInfo();
	pixelPeepPaneInfo.Float();
	pixelPeepPaneInfo.Caption("Pixel Peep");
	pixelPeepPaneInfo.Name("Pixel Peep");
	pixelPeepPaneInfo.CloseButton(true);
	pixelPeepPaneInfo.PinButton(true);
	pixelPeepPaneInfo.DestroyOnClose(false);
	pixelPeepPaneInfo.BestSize(pixelPeepWindow->GetClientSize());
	pixelPeepPaneInfo.Hide();
	auiManager->AddPane(pixelPeepWindow, pixelPeepPaneInfo);

	auiManager->Update();

	aboutWindow = new AboutWindow(this);

	this->Bind(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainWindow::OnNewProject, this, MainWindow::MenuBar::ID_NEW_PROJECT);
	this->Bind(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainWindow::ShowLoadProject, this, MainWindow::MenuBar::ID_SHOW_LOAD_PROJECT);
	this->Bind(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainWindow::ShowSaveProject, this, MainWindow::MenuBar::ID_SHOW_SAVE_PROJECT);
	this->Bind(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainWindow::CloseCurrentProject, this, MainWindow::MenuBar::ID_CLOSE_CURRENT_PROJECT);
	this->Bind(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainWindow::CloseAllProjects, this, MainWindow::MenuBar::ID_CLOSE_ALL_PROJECTS);
	this->Bind(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainWindow::ShowLoadFile, this, MainWindow::MenuBar::ID_SHOW_LOAD_FILE);
	this->Bind(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainWindow::ShowExport, this, MainWindow::MenuBar::ID_SHOW_EXPORT);
	this->Bind(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainWindow::ShowSettings, this, MainWindow::MenuBar::ID_SHOW_SETTINGS);
	this->Bind(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainWindow::OnClose, this, MainWindow::MenuBar::ID_EXIT);
	this->Bind(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainWindow::ShowImage, this, MainWindow::MenuBar::ID_SHOW_IMAGE);
	this->Bind(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainWindow::ShowEditList, this, MainWindow::MenuBar::ID_SHOW_EDIT_LIST);
	this->Bind(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainWindow::ShowHistograms, this, MainWindow::MenuBar::ID_SHOW_HISTOGRAMS);
	this->Bind(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainWindow::ShowSnapshots, this, MainWindow::MenuBar::ID_SHOW_SNAPSHOTS);
	this->Bind(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainWindow::ShowPixelPeep, this, MainWindow::MenuBar::ID_SHOW_PIXEL_PEEP);
	this->Bind(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainWindow::ShowLibrary, this, MainWindow::MenuBar::ID_SHOW_LIBRARY);
	this->Bind(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainWindow::ShowAbout, this, MainWindow::MenuBar::ID_ABOUT);

	this->Bind(PROCESSOR_MESSAGE_EVENT, (wxObjectEventFunction)&MainWindow::RecieveMessageFromProcessor, this, ID_PROCESSOR_MESSAGE);
	this->Bind(PROCESSOR_NUM_EVENT, (wxObjectEventFunction)&MainWindow::RecieveNumFromProcessor, this, ID_PROCESSOR_NUM);
	this->Bind(RELOAD_IMAGE_EVENT, (wxObjectEventFunction)&MainWindow::ReloadImage, this, ID_RELOAD_IMAGE);
	this->Bind(wxEVT_CLOSE_WINDOW, (wxObjectEventFunction)&MainWindow::OnClose, this);
	this->Bind(wxEVT_AUI_PANE_CLOSE, (wxObjectEventFunction)&MainWindow::OnPaneClose, this);
	this->Bind(wxEVT_TIMER, (wxObjectEventFunction)&MainWindow::OnReprocessTimer, this);

	imgPanelThread = new ImagePanelUpdateThread(imagePanel, pixelPeepWindow, processor, histogramDisplay, exportWindow);
	imgPanelThread->Run();

	emptyImage = new wxImage(1, 1);
	ImageHandler::LoadImageFromwxImage(emptyImage, processor->GetImage());
	processor->SetOriginalImage(processor->GetImage());

	this->SetMenuChecks();

	numnUnnamedProjectsOpen = 0;

	reprocessCountdown = new wxTimer(this);

	pixelPeepWindow->UpdatePosition(0, 0);
}

void MainWindow::SetSizeProperties(){

	// Set the size to the screen size, and position at point 0, 0.  Then enable maximize the screen.
	this->SetSize(wxSystemSettings::GetMetric(wxSYS_SCREEN_X), wxSystemSettings::GetMetric(wxSYS_SCREEN_Y));
	this->SetPosition(wxPoint(0, 0));
	this->Maximize(true);
}

void MainWindow::OnNewProject(wxCommandEvent& WXUNUSED(event)) {
	this->CreateNewProject();
}

void MainWindow::CloseCurrentProject(wxCommandEvent& WXUNUSED(event)) {

	imagePanel->ChangeImage(emptyImage);

	// Close current session in UI
	if (allSessions.size() > 0) {
		this->CloseSession(&currentSession);
	}

	// Itterate over all sessions and look for current session
	for (size_t i = 0; i < allSessions.size(); i++) {

		// Remove current session from all session and break once complete
		if (currentSession.GetID() == allSessions.at(i).GetID()) {
			allSessions.at(i).Destroy();
			allSessions.erase(allSessions.begin() + i);
			break;
		}
	}
	
	// Set current session to last opened session (if any sessions are open)
	if (allSessions.size() > 0) {
		currentSession = allSessions.at(allSessions.size() - 1);
		this->OpenSession(&currentSession);
	}
	else{
		currentSession = PhoediXSession();
	}
}

void MainWindow::CloseAllProjects(wxCommandEvent& WXUNUSED(event)) {

	// Close current session in UI
	if (allSessions.size() > 0) {
		this->CloseSession(&currentSession);
	}

	// Remove all sessions from menu window
	for (size_t i = 0; i < allSessions.size(); i++) {
		allSessions.at(i).Destroy();
		if(menuWindow->FindChildItem(allSessions.at(i).GetID())){
			menuWindow->Delete(allSessions.at(i).GetID());
		}
	}

	currentSession.Destroy();

	histogramDisplay->DestroyHistograms();

	// Clear all sessions
	for (size_t i = 0; i < allSessions.size(); i++) {
		allSessions.at(i).Destroy();
	}
	allSessions.clear();	

	currentSession = PhoediXSession();
}

void MainWindow::CreateNewProject(){

	// Save the current session
	this->SaveCurrentSession();

	PhoediXSession newSession;
	this->SetUniqueID(&newSession);
	newSession.SetName("Untitled - " + wxString::Format(wxT("%i"), numnUnnamedProjectsOpen + 1));
	numnUnnamedProjectsOpen += 1;

	menuWindow->AppendCheckItem(newSession.GetID(), _(newSession.GetName()));
	this->Bind(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainWindow::OnOpenWindow, this, newSession.GetID());

	allSessions.push_back(newSession);
	currentSession = newSession;
	this->OpenSession(&currentSession);
}

void MainWindow::OnOpenWindow(wxCommandEvent& evt) {
	
	for (size_t i = 0; i < allSessions.size(); i++) {
		if (allSessions[i].GetID() == evt.GetId() && currentSession.GetID() != allSessions[i].GetID()) {
			this->SaveCurrentSession();
			this->OpenSession(&allSessions[i]);
			currentSession = allSessions[i];
			return;
		}
	}
}

void MainWindow::OpenSession(PhoediXSession * session) {

	processor->KillRawProcessing();
	editList->RemoveAllWindows();
	imagePanel->ChangeImage(emptyImage);
	histogramDisplay->ZeroOutHistograms();

	// Set the new image
	this->OpenImage(session->GetImageFilePath());
	if(session->GetImageScrollWidth() > 0 && session->GetImageScrollHeight() > 0) {
		wxImage * tempImage = new wxImage(session->GetImageScrollWidth(), session->GetImageScrollHeight());
		imagePanel->ChangeImage(tempImage);
		tempImage->Destroy();
		delete tempImage;
	}

	// Check the current session in the window, and uncheck all other sessions
	this->CheckUncheckSession(session->GetID());

	histogramDisplay->SetHistogramDisplay(session->GetHistogramDisplaySelect());
	
	// Populate the panel with the edits
	wxVector<ProcessorEdit*> editLayers = session->GetEditList()->GetSessionEditList();
	editList->AddEditWindows(editLayers);

	// Load snapshots
	snapshotWindow->DeleteAllSnapshots();
	snapshotWindow->AddSnapshots(session->GetSnapshots());

	// Load perspective after edits are loaded
	PhoedixAUIManager::GetPhoedixAUIManager()->LoadPerspective(session->GetPerspective(), true);
	PhoedixAUIManager::GetPhoedixAUIManager()->Update();

	// Set zoom and drag after window has been positioned
	imagePanel->SetZoom(session->GetImageZoomLevel());
	imagePanel->SetDrag(session->GetImageScrollX(), session->GetImageScrollY());

	this->SetMenuChecks();
	reprocessCountdown->Start(500, true);
}

void MainWindow::CloseSession(PhoediXSession * session) {

	imagePanel->SetZoom(100.0f);
	imagePanel->ChangeImage(emptyImage);

	// Check the current session in the window, and uncheck all other sessions
	if (menuWindow->FindItem(session->GetID())) {
		menuWindow->Remove(session->GetID());
	}

	// Remove all edits from panel and snapshots
	editList->RemoveAllWindows();
	snapshotWindow->DeleteAllSnapshots();

	processor->GetOriginalImage()->Destroy();
	processor->GetImage()->Destroy();

	histogramDisplay->ZeroOutHistograms();
	pixelPeepWindow->UpdateImage(NULL);

	reprocessCountdown->Start(500, true);
}


void MainWindow::ShowSaveProject(wxCommandEvent& WXUNUSED(event)) {

	wxFileDialog saveFileDialog(this, _("Save PhoediX Project"), "", "", "PhoediX Project Files (*.phx)|*.phx", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	if (saveFileDialog.ShowModal() == wxID_CANCEL) {
		return;
	}
	
	if(currentSession.GetName().Contains("Untitled -") && !currentSession.GetName().Contains(".phx")){
		numnUnnamedProjectsOpen -= 1;
	}
	this->SaveCurrentSession();
	currentSession.SaveSessionToFile(saveFileDialog.GetPath());
	currentSession.SetName(saveFileDialog.GetFilename());

	// Change name on menu object
	int numWindowMenuItems = menuWindow->GetMenuItems().size();
	for(size_t i = 0; i < numWindowMenuItems; i++){
		if(currentSession.GetID() == menuWindow->GetMenuItems()[i]->GetId()){
			menuWindow->GetMenuItems()[i]->SetItemLabel(currentSession.GetName());
		}
	}
}

void MainWindow::SaveCurrentSession() {

	if(currentSession.GetID() < 0){
		return;
	}
	// Save all infomation about the current session
	editList->AddEditsToProcessor();
	currentSession.SetImageFilePath(processor->GetFilePath());
	currentSession.GetEditList()->SetSessionEditList(processor->GetEditVector());
	currentSession.SetHistogramDisplaySelect(histogramDisplay->GetHistogramDisplay());
	currentSession.SetImageZoomLevel(imagePanel->GetZoom());
	currentSession.SetImageScrollX(imagePanel->GetDragX());
	currentSession.SetImageScrollY(imagePanel->GetDragY());
	if(processor->GetImage() != NULL){
		currentSession.SetImageScrollWidth(processor->GetImage()->GetWidth());
		currentSession.SetImageScrollHeight(processor->GetImage()->GetHeight());
	}
	currentSession.SetPerspective(PhoedixAUIManager::GetPhoedixAUIManager()->SavePerspective());
	currentSession.SetSnapshots(snapshotWindow->GetSnapshots());

	// Replace existing session in all sessions, with updated session
	for(size_t i = 0; i < allSessions.size(); i++){
		if(allSessions[i].GetID() == currentSession.GetID()){
			allSessions[i] = currentSession;
		}
	}

}

void MainWindow::ShowLoadProject(wxCommandEvent& WXUNUSED(event)){

	// Display load phoedix project dialog
	wxFileDialog openFileDialog(this, _("Open PhoediX Project"), "", "", "PhoediX Project Files (*.phx)|*.phx", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	if (openFileDialog.ShowModal() == wxID_CANCEL) {
		return;
	}

	// Save the current session
	this->SaveCurrentSession();

	//this->CloseSession(&currentSession);

	// Load the new session from file
	PhoediXSession session;
	session.LoadSessionFromFile(openFileDialog.GetPath());
	session.SetName(openFileDialog.GetFilename());
	this->SetUniqueID(&session);

	// Append the session to the menu bar
	menuWindow->AppendCheckItem(session.GetID(), _(session.GetName()));
	this->Bind(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainWindow::OnOpenWindow, this, session.GetID());

	// Add new session to all session vector, set current session and open
	allSessions.push_back(session);
	currentSession = session;
	this->OpenSession(&session);
}

void MainWindow::CheckUncheckSession(int sessionID){

	int numWindowMenuItems = menuWindow->GetMenuItems().size();
	for(size_t i = 0; i < numWindowMenuItems; i++){
		if(sessionID == menuWindow->GetMenuItems()[i]->GetId()){
			menuWindow->GetMenuItems()[i]->Check(true);
		}
		else{
			menuWindow->GetMenuItems()[i]->Check(false);
		}
	}
}

void MainWindow::SetUniqueID(PhoediXSession * session) {

	session->GenerateID();
	
	bool cont = true;
	while (cont) {

		cont = false;
		// Go through all sessions, and make sure the new session ID does not match any existing
		for (size_t i = 0; i < allSessions.size(); i++) {

			// If there is a match, generate a new ID and recheck
			if (allSessions[i].GetID() == session->GetID()) {

				session->GenerateID();
				cont = true;
				break;
			}
		}
	}
}

void MainWindow::ShowLoadFile(wxCommandEvent& WXUNUSED(event)) {

	wxFileDialog openFileDialog(this, _("Open Image"), "", "", ImageHandler::imageOpenDialogList, wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	if (openFileDialog.ShowModal() == wxID_CANCEL) {
		return;
	}

	this->OpenImage(openFileDialog.GetPath());	
	this->ShowImageRelatedWindows();
	reprocessCountdown->Start(500, true);
}

void MainWindow::ReloadImage(wxCommandEvent& WXUNUSED(evt)) {
	this->OpenImage(currentSession.GetImageFilePath());
}

void MainWindow::OpenImage(wxString imagePath){

	// Make sure file name is correct and exists
	wxFileName imageFile(imagePath);
	if(!imageFile.IsOk()){ return; }
	if(!imageFile.FileExists()){ return; }

	editList->RemoveRawWindow();

	// Handle Raw Image
	if(ImageHandler::CheckRaw(imagePath)){

		currentSession.SetImageFilePath(imagePath);
		processor->SetFilePath(imagePath);
		processor->SetFileName(imageFile.GetFullName());
		this->SetTitle("PhoediX - " + imageFile.GetFullName());
		exportWindow->RawImageLoaded(true);
		editList->AddRawWindow();
		processor->SetDoFitImage(true);

		this->ShowImageRelatedWindows();
	}

	// Handle JPEG, PNG, BMP and TIFF images
	else{

		exportWindow->RawImageLoaded(false);
		// Verify image is okay
		if(ImageHandler::CheckImage(imagePath)){

			ImageHandler::LoadImageFromFile(imagePath, processor->GetImage());
			processor->SetOriginalImage(processor->GetImage());
			processor->SetUpdated(true);
			processor->SetDoFitImage(true);

			// Write image path to current session
			currentSession.SetImageFilePath(imagePath);
			processor->SetFilePath(imagePath);
			processor->SetFileName(imageFile.GetFullName());
			this->SetTitle("PhoediX - " + imageFile.GetFullName());

			this->ShowImageRelatedWindows();
		}
		else{
			wxMessageDialog imageErrorDialog(this, "Error opening image: " + imagePath, "Error Opening Image", wxICON_ERROR);
			imageErrorDialog.ShowModal();
		}
	}
}

void MainWindow::ShowImageRelatedWindows(){

	if(menuWindow->GetMenuItems().size() < 1){
		auiManager->GetPane(imagePanel).Show();
		auiManager->GetPane(histogramDisplay).Show();
		auiManager->GetPane(editList).Show();
		auiManager->Update();
	}
}

void MainWindow::OnReprocessTimer(wxTimerEvent& WXUNUSED(event)){
	editList->ReprocessImageRaw();
}

void MainWindow::ShowSettings(wxCommandEvent& WXUNUSED(evt)) {
	auiManager->GetPane(settingsWindow).Show();
	auiManager->Update();
}

void MainWindow::ShowImage(wxCommandEvent& evt) {

	// If image menu item is checked, show the image
	if(evt.IsChecked()){
		auiManager->GetPane(imagePanel).Show();
		auiManager->Update();
	}

	// Otherwise, hide the image
	else{
		auiManager->GetPane(imagePanel).Hide();
		auiManager->Update();
	}
}

void MainWindow::ShowEditList(wxCommandEvent& evt) {

	// If edit list menu item is checked, show the edit list
	if(evt.IsChecked()){
		auiManager->GetPane(editList).Show();
		auiManager->Update();
	}

	// Otherwise, hide the edit list
	else{
		auiManager->GetPane(editList).Hide();
		auiManager->Update();
	}
}

void MainWindow::ShowHistograms(wxCommandEvent& evt) {

	// If library menu item is checked, show the edit list
	if(evt.IsChecked()){
		auiManager->GetPane(histogramDisplay).Show();
		auiManager->Update();
	}

	// Otherwise, hide the library
	else{
		auiManager->GetPane(histogramDisplay).Hide();
		auiManager->Update();
	}
}

void MainWindow::ShowSnapshots(wxCommandEvent& evt) {

	// If library menu item is checked, show the edit list
	if(evt.IsChecked()){
		auiManager->GetPane(snapshotWindow).Show();
		auiManager->Update();
	}

	// Otherwise, hide the library
	else{
		auiManager->GetPane(snapshotWindow).Hide();
		auiManager->Update();
	}
}

void MainWindow::ShowPixelPeep(wxCommandEvent& evt) {

	// If library menu item is checked, show the edit list
	if (evt.IsChecked()) {
		auiManager->GetPane(pixelPeepWindow).Show();
		auiManager->Update();
	}

	// Otherwise, hide the library
	else {
		auiManager->GetPane(pixelPeepWindow).Hide();
		auiManager->Update();
	}
}

void MainWindow::ShowLibrary(wxCommandEvent& evt) {

	// If library menu item is checked, show the edit list
	if(evt.IsChecked()){
		auiManager->GetPane(libraryWindow).Show();
		auiManager->Update();
	}

	// Otherwise, hide the library
	else{
		auiManager->GetPane(libraryWindow).Hide();
		auiManager->Update();
	}
}

void MainWindow::ShowAbout(wxCommandEvent& WXUNUSED(evt)){
	aboutWindow->CenterOnParent();
	aboutWindow->Show();
}

void MainWindow::SetMenuChecks(){

	// 1st element in view is image panel
	if(auiManager->GetPane(imagePanel).IsShown()){
		menuView->GetMenuItems()[0]->Check(true);
	}
	else{
		menuView->GetMenuItems()[0]->Check(false);
	}

	// 2nd element in view is edit list
	if(auiManager->GetPane(editList).IsShown()){
		menuView->GetMenuItems()[1]->Check(true);
	}
	else{
		menuView->GetMenuItems()[1]->Check(false);
	}

	// 3rd element in view is histograms
	if(auiManager->GetPane(histogramDisplay).IsShown()){
		menuView->GetMenuItems()[2]->Check(true);
	}
	else{
		menuView->GetMenuItems()[2]->Check(false);
	}

	// 1st element in tools is snapshots
	if(auiManager->GetPane(snapshotWindow).IsShown()){
		menuTools->GetMenuItems()[0]->Check(true);
	}
	else{
		menuTools->GetMenuItems()[0]->Check(false);
	}

	// 2nd element in tools is pixel peep
	if (auiManager->GetPane(pixelPeepWindow).IsShown()) {
		menuTools->GetMenuItems()[1]->Check(true);
	}
	else {
		menuTools->GetMenuItems()[1]->Check(false);
	}

	// 3rd element in tools is separator

	// 4th element in tools is library window
	if(auiManager->GetPane(libraryWindow).IsShown()){
		menuTools->GetMenuItems()[3]->Check(true);
	}
	else{
		menuTools->GetMenuItems()[3]->Check(false);
	}
}

void MainWindow::OnPaneClose(wxAuiManagerEvent& evt){
	evt.GetPane()->Hide();
	this->SetMenuChecks();
}

void MainWindow::OnImagePanelMouse(wxMouseEvent & evt) {

	int x = evt.GetPosition().x;
	int y = evt.GetPosition().y;
	pixelPeepWindow->UpdatePosition(x, y);
}

void MainWindow::ShowExport(wxCommandEvent& WXUNUSED(event)) {
	auiManager->GetPane(exportWindow).Show();
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
	exportWindow->SetMessage(messageEvt.GetString());
}

void MainWindow::RecieveNumFromProcessor(wxCommandEvent& numEvt) {
	exportWindow->SetEditNum(numEvt.GetInt());
}

void MainWindow::OnClose(wxCloseEvent& WXUNUSED(evt)) {

	// Kill current processing and stop watching for new edits
	processor->KillCurrentProcessing();
	imgPanelThread->StopWatching();

	reprocessCountdown->Stop();
	delete reprocessCountdown;
	imagePanel->DestroyTimer();

	int currentSessionID = currentSession.GetID();
	currentSession.Destroy();

	// Remove all sessions from menu window
	for (size_t i = 0; i < allSessions.size(); i++) {

		// Destroy session if it is not current session. (Current session already destroyed)
		if (allSessions.at(i).GetID() != currentSessionID) {
			allSessions.at(i).Destroy();
		}
	}

	// Clear all sessions
	allSessions.clear();

	// Delete histogram display resources
	histogramDisplay->DestroyHistograms();

	// Delete all snapshots
	snapshotWindow->DeleteAllSnapshots();

	auiManager->UnInit();
	delete auiManager;
	
	delete processor;
	delete emptyImage;

	this->Destroy();
}

MainWindow::ImagePanelUpdateThread::ImagePanelUpdateThread(ZoomImagePanel * imagePanel, PixelPeepWindow * pixelPeepWindow, Processor * processor, HistogramDisplay * histogramDisplay, ExportWindow * exportWindow) : wxThread(wxTHREAD_DETACHED) {
	imgPanel = imagePanel;
	histogramDisp = histogramDisplay;
	proc = processor;
	exportWin = exportWindow;
	continueWatch = true;
	pixelPeep = pixelPeepWindow;
}

wxThread::ExitCode MainWindow::ImagePanelUpdateThread::Entry() {
	
	while (continueWatch) {
		if (proc->GetUpdated() && !proc->GetLocked() && !proc->GetLockedRaw()) {
			proc->Lock();
			imgPanel->ChangeImage(proc->GetImage());
			pixelPeep->UpdateImage(proc->GetImage());
			proc->Unlock();
			imgPanel->Redraw();
	
			exportWin->ProcessingComplete();
			proc->SetDoFitImage(false);
			proc->Lock();
			histogramDisp->UpdateHistograms();
			proc->Unlock();
			proc->SetUpdated(false);
		}
		this->Sleep(20);
	}
	
	return (wxThread::ExitCode)0;
}

void MainWindow::ImagePanelUpdateThread::StopWatching() {
	continueWatch = false;
}
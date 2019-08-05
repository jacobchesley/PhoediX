// Copyright 2018 Jacob Chesley.
// See LICENSE.TXT in root of project for license information.

#include "MainWindow.h"
#include "Debugging/Logger/Logger.h"

MainWindow::MainWindow(wxApp * application) : wxFrame(NULL, -1, "PhoediX", wxDefaultPosition, wxDefaultSize){

	app = application;
	Icons icons;
	wxIcon theIcon;
	theIcon.CopyFromBitmap(wxBitmap(icons.pxIcon));
	this->SetIcon(theIcon);

	this->SetSizeProperties();
	this->SetBackgroundColour(Colors::BackDarkDarkGrey);

	// Create and add menu bar with menus
	menuBar = new wxMenuBar();

	menuFile = new wxMenu();
	menuCloseProjects = new wxMenu();
	menuView = new wxMenu();
	menuTools = new wxMenu();
	menuWindow = new wxMenu();
	menuHelp = new wxMenu();

	menuFile->Append(MainWindow::MenuBar::ID_SHOW_LOAD_FILE, _("Open Image\tCtrl+O"));
	menuFile->AppendSeparator();
	menuCloseProjects->Append(MainWindow::MenuBar::ID_CLOSE_CURRENT_PROJECT, _("Close Current Project"));
	menuCloseProjects->Append(MainWindow::MenuBar::ID_CLOSE_ALL_PROJECTS, _("Close All Projects"));
	menuFile->AppendSubMenu(menuCloseProjects, _("Close Project..."));
	menuFile->AppendSeparator();
	menuFile->Append(MainWindow::MenuBar::ID_SHOW_EXPORT, _("Export Image\tCtrl+E"));
	menuFile->AppendSeparator();
	menuFile->Append(MainWindow::MenuBar::ID_SHOW_SETTINGS, _("PhoediX Settings"));
	menuFile->AppendSeparator();
	menuFile->Append(MainWindow::MenuBar::ID_EXIT, _("Exit"));

	menuView->AppendCheckItem(MainWindow::MenuBar::ID_SHOW_IMAGE, _("Show Image\tCtrl+I"));
	menuView->AppendCheckItem(MainWindow::MenuBar::ID_SHOW_EDIT_LIST, _("Show Edit Pipeline\tCtrl+P"));
	menuView->AppendCheckItem(MainWindow::MenuBar::ID_SHOW_HISTOGRAMS, _("Show Histograms\tCtrl+H"));
	menuView->AppendCheckItem(MainWindow::MenuBar::ID_SHOW_IMAGE_INFO, _("Show Image Info"));

	menuTools->AppendCheckItem(MainWindow::MenuBar::ID_ENABLE_FAST_EDIT, _("Fast Edit\tCtrl+F"));
	menuTools->Append(MainWindow::MenuBar::ID_FORCE_REPROCESS, _("Force Reprocess\tCtrl+R"));
	menuTools->AppendSeparator();
	menuTools->AppendCheckItem(MainWindow::MenuBar::ID_SHOW_ORIGINAL, _("Show Original Image"));
	menuTools->AppendCheckItem(MainWindow::MenuBar::ID_SHOW_ORIGINAL_WINDOW, _("Show Original Image (separate window)"));
	menuTools->AppendCheckItem(MainWindow::MenuBar::ID_SHOW_SNAPSHOTS, _("Show Snapshots"));
	menuTools->AppendCheckItem(MainWindow::MenuBar::ID_SHOW_PIXEL_PEEP, _("Show Pixel Peeper"));
	menuTools->AppendCheckItem(MainWindow::MenuBar::ID_SHOW_GUIDELINES, _("Show Image Guidelines Configuration"));
	menuTools->AppendSeparator();
	menuTools->AppendCheckItem(MainWindow::MenuBar::ID_SHOW_LIBRARY, _("Show Library\tCtrl+L"));

	menuHelp->Append(MainWindow::MenuBar::ID_SHOW_SUPPORTED_CAMERAS, _("Supported Cameras (RAW)"));
	menuHelp->Append(MainWindow::MenuBar::ID_ABOUT, _("About PhoediX"));

	// Append menus to menu bar
	menuBar->Append(menuFile, _("&File"));
	menuBar->Append(menuView, _("&View"));
	menuBar->Append(menuTools, _("&Tools"));
	menuBar->Append(menuWindow, _("&Windows"));
	menuBar->Append(menuHelp, _("&Help"));

	// Set the menu bar
	this->SetMenuBar(menuBar);

	// Set the status bar
	this->CreateStatusBar();
	statusBarText = NULL;
	this->GetStatusBar()->SetBackgroundColour(Colors::BackDarkDarkDarkGrey);
	this->GetStatusBar()->SetForegroundColour(Colors::TextLightGrey);
	
	processor = new Processor(this);
	processor->SetMultithread(true);
	processor->SetNumThreads(wxThread::GetCPUCount());

	auiManager = new wxAuiManager(this, wxAUI_MGR_DEFAULT | wxAUI_MGR_LIVE_RESIZE);
	auiManager->GetArtProvider()->SetColor(wxAuiPaneDockArtSetting::wxAUI_DOCKART_BACKGROUND_COLOUR, Colors::BackDarkDarkGrey);
	PhoedixAUIManager::SetPhoedixAUIManager(auiManager);
	PhoedixAUIManager::SetMainWindow(this);

	histogramDisplay = new HistogramDisplay(this, processor);
	wxAuiPaneInfo histogramPaneInfo = wxAuiPaneInfo();
	histogramPaneInfo.Right();
	histogramPaneInfo.Position(0);
	histogramPaneInfo.Caption("Histogram");
	histogramPaneInfo.Name("Histogram");
	histogramPaneInfo.CloseButton(true);
	histogramPaneInfo.PinButton(true);
	histogramPaneInfo.DestroyOnClose(false);
	histogramPaneInfo.BestSize(histogramDisplay->GetClientSize());
	histogramPaneInfo.Hide();
	auiManager->AddPane(histogramDisplay, histogramPaneInfo);

	imagePanel = new ZoomImagePanel(this, processor->GetImage());
	imagePanel->SetBackgroundColour(Colors::BackDarkDarkGrey);
	imagePanel->ShowGuidelinesOption();

	originalImagePanel = new ZoomImagePanel(this, processor->GetImage());
	originalImagePanel->SetBackgroundColour(Colors::BackDarkDarkGrey);

	wxAuiPaneInfo imageOrigInfo = wxAuiPaneInfo();
	imageOrigInfo.Center();
	imageOrigInfo.CloseButton(true);
	imageOrigInfo.PinButton(true);
	imageOrigInfo.MaximizeButton(true);
	imageOrigInfo.DestroyOnClose(false);
	imageOrigInfo.Name("Original Image Display");
	imageOrigInfo.Caption("Original Image");
	imageOrigInfo.Hide();
	auiManager->AddPane(originalImagePanel, imageOrigInfo);

	wxAuiPaneInfo imageInfo = wxAuiPaneInfo();
	imageInfo.Center();
	imageInfo.CloseButton(true);
	imageInfo.PinButton(true);
	imageInfo.MaximizeButton(true);
	imageInfo.DestroyOnClose(false);
	imageInfo.Name("Image Display");
	imageInfo.Caption("Working Image");
	imageInfo.Hide();
	auiManager->AddPane(imagePanel, imageInfo);

	editList = new EditListPanel(this, processor, imagePanel);
	wxAuiPaneInfo editListInfo = wxAuiPaneInfo();
	editListInfo.Right();
	editListInfo.Position(1);
	editListInfo.Caption("Edit Pipeline");
	editListInfo.Name("EditPipeline");
	editListInfo.CloseButton(true);
	editListInfo.PinButton(true);
	editListInfo.DestroyOnClose(false);
	editListInfo.Hide();
	auiManager->AddPane(editList, editListInfo);

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
	
	settingsWindow = new SettingsWindow(this, processor, editList);

	guidelinesWindow = new GuidelinesWindow(this, imagePanel);
	wxAuiPaneInfo guidelinesPaneInfo = wxAuiPaneInfo();
	guidelinesPaneInfo.Float();
	guidelinesPaneInfo.Caption("Image Guidelines Configuration");
	guidelinesPaneInfo.Name("GuidelinesConfig");
	guidelinesPaneInfo.CloseButton(true);
	guidelinesPaneInfo.PinButton(true);
	guidelinesPaneInfo.DestroyOnClose(false);
	guidelinesPaneInfo.BestSize(settingsWindow->GetClientSize());
	guidelinesPaneInfo.Hide();
	auiManager->AddPane(guidelinesWindow, guidelinesPaneInfo);

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
	snapshotPaneInfo.MinSize(snapshotWindow->GetClientSize());
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

	imageInfoPanel = new ExifReadWindow(this);
	wxAuiPaneInfo exifReadPaneInfo = wxAuiPaneInfo();
	exifReadPaneInfo.Float();
	exifReadPaneInfo.Caption("Image Information");
	exifReadPaneInfo.Name("Image Information");
	exifReadPaneInfo.CloseButton(true);
	exifReadPaneInfo.PinButton(true);
	exifReadPaneInfo.DestroyOnClose(false);
	exifReadPaneInfo.FloatingSize(imageInfoPanel->GetMinSize());
	exifReadPaneInfo.Hide();
	auiManager->AddPane(imageInfoPanel, exifReadPaneInfo);

	auiManager->Update();

	aboutWindow = new AboutWindow(this);
	supportedCamerasWindow = new SupportedCamerasWindow(this);

	this->Bind(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainWindow::CloseCurrentProject, this, MainWindow::MenuBar::ID_CLOSE_CURRENT_PROJECT);
	this->Bind(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainWindow::CloseAllProjects, this, MainWindow::MenuBar::ID_CLOSE_ALL_PROJECTS);
	this->Bind(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainWindow::ShowLoadImage, this, MainWindow::MenuBar::ID_SHOW_LOAD_FILE);
	this->Bind(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainWindow::ShowExport, this, MainWindow::MenuBar::ID_SHOW_EXPORT);
	this->Bind(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainWindow::ShowSettings, this, MainWindow::MenuBar::ID_SHOW_SETTINGS);
	this->Bind(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainWindow::OnClose, this, MainWindow::MenuBar::ID_EXIT);
	this->Bind(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainWindow::ShowImage, this, MainWindow::MenuBar::ID_SHOW_IMAGE);
	this->Bind(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainWindow::ShowEditList, this, MainWindow::MenuBar::ID_SHOW_EDIT_LIST);
	this->Bind(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainWindow::ShowHistograms, this, MainWindow::MenuBar::ID_SHOW_HISTOGRAMS);
	this->Bind(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainWindow::OnEnableFastEdit, this, MainWindow::MenuBar::ID_ENABLE_FAST_EDIT);
	this->Bind(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainWindow::OnForceReprocess, this, MainWindow::MenuBar::ID_FORCE_REPROCESS);
	this->Bind(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainWindow::ShowOriginal, this, MainWindow::MenuBar::ID_SHOW_ORIGINAL);
	this->Bind(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainWindow::ShowOriginalWindow, this, MainWindow::MenuBar::ID_SHOW_ORIGINAL_WINDOW);
	this->Bind(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainWindow::ShowSnapshots, this, MainWindow::MenuBar::ID_SHOW_SNAPSHOTS);
	this->Bind(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainWindow::ShowPixelPeep, this, MainWindow::MenuBar::ID_SHOW_PIXEL_PEEP);
	this->Bind(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainWindow::ShowGuidelines, this, MainWindow::MenuBar::ID_SHOW_GUIDELINES);
	this->Bind(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainWindow::ShowLibrary, this, MainWindow::MenuBar::ID_SHOW_LIBRARY);
	this->Bind(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainWindow::ShowImageInfo, this, MainWindow::MenuBar::ID_SHOW_IMAGE_INFO);
	this->Bind(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainWindow::ShowSupportedCameras, this, MainWindow::MenuBar::ID_SHOW_SUPPORTED_CAMERAS);
	this->Bind(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainWindow::ShowAbout, this, MainWindow::MenuBar::ID_ABOUT);

	this->Bind(PROCESSOR_MESSAGE_EVENT, (wxObjectEventFunction)&MainWindow::RecieveMessageFromProcessor, this, ID_PROCESSOR_MESSAGE);
	this->Bind(PROCESSOR_NUM_EVENT, (wxObjectEventFunction)&MainWindow::RecieveNumFromProcessor, this, ID_PROCESSOR_NUM);
	this->Bind(PROCESSOR_RAW_COMPLETE_EVENT, (wxObjectEventFunction)&MainWindow::RecieveRawComplete, this, ID_RAW_COMPLETE);
	this->Bind(RELOAD_IMAGE_EVENT, (wxObjectEventFunction)&MainWindow::ReloadImage, this, ID_RELOAD_IMAGE);
	this->Bind(OPEN_IMAGE_NEW_PROJECT_EVENT, (wxObjectEventFunction)&MainWindow::OnImportImageNewProject, this, ID_OPEN_IMAGE_NEW_PROJECT);
	this->Bind(wxEVT_CLOSE_WINDOW, (wxObjectEventFunction)&MainWindow::OnClose, this);
	this->Bind(wxEVT_AUI_PANE_CLOSE, (wxObjectEventFunction)&MainWindow::OnPaneClose, this);
	this->Bind(wxEVT_TIMER, (wxObjectEventFunction)&MainWindow::ClearStatusbarText, this, EVT_CLEAR_STATUS_TIMER);
	this->Bind(UPDATE_IMAGE_EVENT, (wxObjectEventFunction)&MainWindow::OnUpdateImage, this, ID_UPDATE_IMAGE);
	this->Bind(REPROCESS_IMAGE_EVENT, (wxObjectEventFunction)&MainWindow::OnReprocess, this, ID_REPROCESS_IMAGE);
	this->Bind(SAVE_PROJECT_EVENT, (wxObjectEventFunction)&MainWindow::SaveProject, this, ID_SAVE_PROJECT);
	this->Bind(wxEVT_DROP_FILES, (wxObjectEventFunction)&MainWindow::OnDropFiles, this);

	this->DragAcceptFiles(true);


	emptyImage = new wxImage(0, 0);
	emptyPhxImage = new Image();
	processor->SetOriginalImage(emptyPhxImage);
	settingsWindow->ReadSettings();

	this->SetMenuChecks();

	numnUnnamedProjectsOpen = 0;
	currentSession = NULL;

	clearStatusTimer = new wxTimer(this, EVT_CLEAR_STATUS_TIMER);

	this->EnableDisableMenuItemsNoProject(false);
}

void MainWindow::SetSizeProperties(){

	// Set the size to the screen size, and position at point 0, 0.  Then enable maximize the screen.
	this->SetSize(wxSystemSettings::GetMetric(wxSYS_SCREEN_X), wxSystemSettings::GetMetric(wxSYS_SCREEN_Y));
	this->SetPosition(wxPoint(0, 0));
	this->Maximize(true);
}

void MainWindow::OpenFiles(wxArrayString files) {

	for (size_t i = 0; i < files.size(); i++) {

		// Check if PhoediX project
		if (PhoediXSession::CheckIfSession(files.Item(i))) {

			// Get project name and path
			wxString projectName = wxFileName(files.Item(i)).GetName();
			wxString projectPath = wxFileName(files.Item(i)).GetPath();

			// Get image path from project path and name (image is one directory above)
			wxString imagePath = projectPath + wxFileName::GetPathSeparator() + "..";
			wxString fullImagePath = imagePath + wxFileName::GetPathSeparator() + projectName;
			this->OpenImage(fullImagePath);
		}
		else{
			this->OpenImage(files.Item(i));
		}
	}
}

void MainWindow::OnDropFiles(wxDropFilesEvent& evt){

	wxArrayString files;
	for(int i = 0; i < evt.GetNumberOfFiles(); i++){
		files.Add(evt.GetFiles()[i]);
	}
	this->OpenFiles(files);
}

void MainWindow::CloseCurrentProject(wxCommandEvent& WXUNUSED(event)) {

	this->SaveCurrentSession();
	processor->DestroyAll();
	imagePanel->ChangeImage(emptyImage);

	// Close current session in UI
	if (allSessions.size() > 0) {
		this->CloseSession(currentSession);
	}

	// Itterate over all sessions and look for current session
	for (size_t i = 0; i < allSessions.size(); i++) {

		// Remove current session from all session and break once complete
		if (currentSession->GetID() == allSessions.at(i)->GetID()) {
			allSessions.at(i)->Destroy();
			allSessions.erase(allSessions.begin() + i);
			break;
		}
	}
	
	// Set current session to last opened session (if any sessions are open)
	if (allSessions.size() > 0) {
		currentSession = allSessions.at(allSessions.size() - 1);
		// Open the image, then open the session to go along with image
		this->OpenImage(currentSession->GetImageFilePath(), false);
		this->OpenSession(currentSession);
	}

	// No sessions opened, disable menu items related to sessions
	else{
		currentSession = NULL;
		this->EnableDisableMenuItemsNoProject(false);
		this->SetTitle("PhoediX");
	}
}

void MainWindow::CloseAllProjects(wxCommandEvent& WXUNUSED(event)) {

	// Remove all sessions
	for (size_t i = 0; i < allSessions.size(); i++) {

		// Close and destroy the session
		this->CloseSession(allSessions.at(i));
		allSessions.at(i)->Destroy();
	}

	allSessions.clear();
	currentSession = NULL;
	this->EnableDisableMenuItemsNoProject(false);
	this->SetTitle("PhoediX");
}

void MainWindow::CreateNewProject(wxString projectFile, bool rawProject){

	PhoediXSession * newSession = new PhoediXSession();
	this->SetUniqueID(newSession);

	wxFileName phoedixProject(projectFile);
	newSession->SetName(phoedixProject.GetName());
	newSession->SetProjectPath(projectFile);

	if(rawProject){
		wxVector<ProcessorEdit*>  rawEditList;
		rawEditList.push_back(new ProcessorEdit(ProcessorEdit::EditType::RAW));
		newSession->GetEditList()->SetSessionEditList(rawEditList);
	}

	menuWindow->AppendCheckItem(newSession->GetID(), _(newSession->GetName()));
	this->Bind(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainWindow::OnOpenWindow, this, newSession->GetID());
    
	allSessions.push_back(newSession);
	currentSession = newSession;
	processor->SetDoFitImage(true);
	this->OpenSession(currentSession);	
}

void MainWindow::OpenSession(PhoediXSession * session) {

	processor->KillRawProcessing();
	imagePanel->ChangeImage(emptyImage);
	histogramDisplay->ZeroOutHistograms();
	imagePanel->DeactivateGrid();
	imagePanel->DeactivateGuidelines();
		
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
	if(session->GetEditList() != NULL){

		wxVector<ProcessorEdit*> editLayers = session->GetEditList()->GetSessionEditList();
		editList->AddEditWindows(editLayers);
	}
	// Load snapshots
	snapshotWindow->DeleteAllSnapshots();
	snapshotWindow->AddSnapshots(session->GetSnapshots());

	// Load perspective after edits are loaded
	PhoedixAUIManager::GetPhoedixAUIManager()->LoadPerspective(session->GetPerspective(), true);
	PhoedixAUIManager::GetPhoedixAUIManager()->GetPane(imagePanel).Caption("Working Image");
	PhoedixAUIManager::GetPhoedixAUIManager()->Update();

	// Set zoom and drag after window has been positioned
	imagePanel->SetZoom(session->GetImageZoomLevel());
	imagePanel->SetDrag(session->GetImageScrollX(), session->GetImageScrollY());
	imagePanel->SetFitImage(session->GetFitImage());

	// Set export controls
	exportWindow->SetImageType(session->GetExportImageType());
	exportWindow->SetJPEGQuality(session->GetExportJPEGQuality());
	exportWindow->SetExportFolder(session->GetExportFolder());
	exportWindow->SetExportName(session->GetExportName());

	// Set Guide Colors
	guidelinesWindow->SetGuidelineType(session->GetGuidelineType());
	guidelinesWindow->SetGuideColor(session->GetGuidelineColor());
	guidelinesWindow->SetGridColor1(session->GetCropgridColor1());
	guidelinesWindow->SetGridColor2(session->GetCropgridColor2());
	if (session->GetGuidelinesShown()) { imagePanel->ActivateGuidelines(); }
	else { imagePanel->DeactivateGuidelines(); }

	this->SetMenuChecks();

	// At least one session exists now, enable menu items related to sessions
	this->EnableDisableMenuItemsNoProject(true);

	if (session->GetFastProcess()) {
		menuTools->FindItem(MainWindow::MenuBar::ID_ENABLE_FAST_EDIT)->Check(true);
		processor->EnableFastEdit();
		imagePanel->EnableHalfSize();
	}
	else {
		menuTools->FindItem(MainWindow::MenuBar::ID_ENABLE_FAST_EDIT)->Check(false);
		processor->DisableFastEdit();
		imagePanel->DisableHalfSize();
	}

	editList->SetSession(session);
}

void MainWindow::CloseSession(PhoediXSession * session) {

	imagePanel->SetZoom(100.0f);
	imagePanel->ChangeImage(emptyImage);

	// Check the current session in the window, and uncheck all other sessions
	if (menuWindow->FindItem(session->GetID())) {
		menuWindow->Delete(session->GetID());
	}

	// Remove all edits from panel and snapshots
	if (session->GetID() == currentSession->GetID()) {
		editList->RemoveAllWindows();
		snapshotWindow->DeleteAllSnapshots();

		processor->GetOriginalImage()->Destroy();
		processor->GetImage()->Destroy();

		histogramDisplay->ZeroOutHistograms();
		pixelPeepWindow->UpdateImage(NULL);

	}

	if (allSessions.size() > 0) {
		// At least one session exists now, enable menu items related to sessions
		this->EnableDisableMenuItemsNoProject(true);
	}
	else {
		// No sessions exists now, disable menu items related to sessions
		this->EnableDisableMenuItemsNoProject(false);
	}
}

void MainWindow::OnOpenWindow(wxCommandEvent& evt) {
	
	this->SaveCurrentSession();
	// If the session to open is the same as the one currently opened, keep open
	if (evt.GetId() == currentSession->GetID()) {
		menuWindow->FindChildItem(evt.GetId())->Check();
		return;
	}

	// Find the session to open
	for (size_t i = 0; i < allSessions.size(); i++) {

		if (allSessions[i]->GetID() == evt.GetId() && currentSession->GetID() != allSessions[i]->GetID()) {

			// Open the image, then open the session to go along with image
			this->OpenImage(allSessions[i]->GetImageFilePath(), false);
			this->OpenSession(allSessions[i]);
			currentSession = allSessions[i];
			return;
		}
	}
}

void MainWindow::SaveProject(wxCommandEvent & WXUNUSED(evt)) {

	this->SaveCurrentSession();
}

void MainWindow::SaveCurrentSession() {

	if(currentSession == NULL){
		return;
	}
	// Save all infomation about the current session
	currentSession->SetImageFilePath(processor->GetFilePath());
	currentSession->GetEditList()->SetSessionEditList(processor->GetEditVector());
	currentSession->SetHistogramDisplaySelect(histogramDisplay->GetHistogramDisplay());
	currentSession->SetImageZoomLevel(imagePanel->GetZoom());
	currentSession->SetImageScrollX(imagePanel->GetDragX());
	currentSession->SetImageScrollY(imagePanel->GetDragY());
	currentSession->SetFitImage(imagePanel->GetFitImage());
	if(processor->GetImage() != NULL){
		currentSession->SetImageScrollWidth(processor->GetImage()->GetWidth());
		currentSession->SetImageScrollHeight(processor->GetImage()->GetHeight());
	}
	currentSession->SetPerspective(PhoedixAUIManager::GetPhoedixAUIManager()->SavePerspective());
	currentSession->SetSnapshots(snapshotWindow->GetSnapshots());

	currentSession->SetExportImageType(exportWindow->GetImageType());
	currentSession->SetExportJPEGQuality(exportWindow->GetJPEGQuality());
	currentSession->SetExportFolder(exportWindow->GetExportFolder());
	currentSession->SetExportName(exportWindow->GetExportName());

	currentSession->SetGuidelineType(guidelinesWindow->GetGuidelineType());
	currentSession->SetGuidelineColor(guidelinesWindow->GetGuideColor());
	currentSession->SetCropgridColor1(guidelinesWindow->GetGridColor1());
	currentSession->SetCropgridColor2(guidelinesWindow->GetGridColor2());
	currentSession->SetGuidelinesShown(imagePanel->GetGuidelinesActive());

	// Replace existing session in all sessions, with updated session
	for(size_t i = 0; i < allSessions.size(); i++){
		if(allSessions[i]->GetID() == currentSession->GetID()){
			allSessions[i] = currentSession;
		}
	}

	currentSession->SaveSessionToFile(currentSession->GetProjectPath());
}

void MainWindow::LoadProject(wxString projectPath) {

	// Check for already loaded images in session
	for(int i = 0; i < allSessions.size(); i++){

		// Open existing session (if it is not already the active session)
		if(allSessions[i]->GetProjectPath() == projectPath){

			// Open the image, then open the session to go along with image
			this->OpenImage(allSessions[i]->GetImageFilePath(), false);
			this->OpenSession(allSessions[i]);
			currentSession = allSessions[i];
			return;
		
		}
	}

	// Load the new session from file
	PhoediXSession * session = new PhoediXSession();
	session->LoadSessionFromFile(projectPath);

	wxFileName projectFile(projectPath);
	session->SetName(projectFile.GetName());
	session->SetProjectPath(projectFile.GetFullPath());
	this->SetUniqueID(session);

	// Append the session to the menu bar
	menuWindow->AppendCheckItem(session->GetID(), _(session->GetName()));
	this->Bind(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MainWindow::OnOpenWindow, this, session->GetID());

	// Add new session to all session vector, set current session and open
	allSessions.push_back(session);
	currentSession = session;
	this->OpenSession(currentSession);
}

void MainWindow::OnImportImageNewProject(wxCommandEvent& evt) {

	this->OpenImage(evt.GetString());
}

void MainWindow::CheckUncheckSession(int sessionID){

	size_t numWindowMenuItems = menuWindow->GetMenuItems().size();
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
			if (allSessions[i]->GetID() == session->GetID()) {

				session->GenerateID();
				cont = true;
				break;
			}
		}
	}
}

void MainWindow::ShowLoadImage(wxCommandEvent& WXUNUSED(event)) {
   
	wxFileDialog openFileDialog(this, _("Open Image"), "", "", ImageHandler::imageOpenDialogList, wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	if (openFileDialog.ShowModal() == wxID_CANCEL) {
		return;
	}
	this->OpenImage(openFileDialog.GetPath());	
}

void MainWindow::ReloadImage(wxCommandEvent& WXUNUSED(evt)) {

	if(currentSession == NULL){return;}
	this->OpenImage(currentSession->GetImageFilePath());
}

void MainWindow::OpenImage(wxString imagePath, bool checkForProject){

	if (imagePath == "") { return; }

	processor->GetImage()->Destroy();
	processor->GetOriginalImage()->Destroy();

	// Make sure file name is correct and exists
	wxFileName imageFile(imagePath);
	if(!imageFile.FileExists() || !imageFile.IsOk()){ 
		imagePanel->NoImage();
		processor->SetOriginalImage(emptyPhxImage);
		processor->SetFilePath("");
		this->SetTitle("PhoediX");
		return; 
	}

	imageInfoPanel->ClearExif();

	emptyPhxImage->Destroy();
	if (editList->GetRawWindowOpen()) { editList->RemoveRawWindow(); }
	
	processor->SetFilePath(imagePath);
	processor->SetFileName(imageFile.GetFullName());
	this->SetTitle("PhoediX - " + imageFile.GetFullName());

	bool rawProject = false;

	// Handle Raw Image
	if(ImageHandler::CheckRaw(imagePath)){
		exportWindow->RawImageLoaded(true);
		rawProject = true;

		LibRaw * imageInfoRaw = new LibRaw();

		#ifdef __WXMSW__
			imageInfoRaw->open_file(imagePath.wc_str());
		#else
			imageInfoRaw->open_file(imagePath.c_str());
		#endif
		ImageHandler::ReadExifFromRaw(imageInfoRaw, processor->GetImage());
		imageInfoPanel->AddExif(processor->GetImage());

	}

	// Handle JPEG, PNG, BMP and TIFF images
	else if(ImageHandler::CheckImage(imagePath)){
		processor->Lock();
		ImageHandler::LoadImageFromFile(imagePath, processor->GetImage());
		processor->SetOriginalImage(processor->GetImage());
		originalImagePanel->ChangeImage(processor->GetOriginalImage());
		processor->SetUpdated(true);
		processor->Unlock();

		if (ImageHandler::CheckExif(imagePath)) {
			ImageHandler::ReadExif(imagePath, processor->GetImage());
			imageInfoPanel->AddExif(processor->GetImage());
		}
	}

	else{
		wxMessageDialog imageErrorDialog(this, "Error opening image: " + imagePath, "Error Opening Image", wxICON_ERROR);
		imageErrorDialog.ShowModal();
	}
	this->ShowImageRelatedWindows();

	if(checkForProject){
		wxString pathSep = imageFile.GetPathSeparator();
		wxFileName phoedixProject(imageFile.GetPath() + pathSep+ ".PhoediX" + pathSep + imageFile.GetFullName() + ".phx");
		if(phoedixProject.FileExists() && phoedixProject.IsOk()){
			this->LoadProject(phoedixProject.GetFullPath());
		}
		else{
			this->CreateNewProject(phoedixProject.GetFullPath(), rawProject);
		}
	}

	wxCommandEvent evt(UPDATE_IMAGE_EVENT, ID_UPDATE_IMAGE);
	wxPostEvent(this, evt);

}

void MainWindow::RecieveRawComplete(wxCommandEvent & WXUNUSED(evt)) {
	originalImagePanel->ChangeImage(processor->GetOriginalImage());
	originalImagePanel->Redraw();
}

void MainWindow::ShowImageRelatedWindows(){

	auiManager->GetPane(imagePanel).Show();
	auiManager->GetPane(histogramDisplay).Show();
	auiManager->GetPane(editList).Show();
	auiManager->Update();
}

void MainWindow::OnReprocessTimer(wxTimerEvent& WXUNUSED(event)){
	editList->ReprocessImageRaw();
}

void MainWindow::OnForceReprocess(wxCommandEvent& WXUNUSED(event)) {
	editList->ReprocessImageRaw();
}

void MainWindow::OnReprocess(wxCommandEvent& WXUNUSED(event)){
	if (currentSession == NULL) { return; }
	this->SaveCurrentSession();
	if(currentSession->GetProjectPath() != ""){
		currentSession->SaveSessionToFile(currentSession->GetProjectPath());
	}
}

void MainWindow::ShowSettings(wxCommandEvent& WXUNUSED(evt)) {
	settingsWindow->Show();
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
	this->SaveCurrentSession();
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
	this->SaveCurrentSession();
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
	this->SaveCurrentSession();
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
	this->SaveCurrentSession();
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
	this->SaveCurrentSession();
}

void MainWindow::ShowOriginal(wxCommandEvent& evt) {

	// If Show Original Image menu checked, change working image display to show original image
	if (evt.IsChecked()) {
		processor->Lock();
		if (processor->GetOriginalImage()->GetWidth() < 1 || processor->GetOriginalImage()->GetHeight() < 1) {
			processor->Unlock();
			return;
		}
		imagePanel->ChangeImage(processor->GetOriginalImage());
		pixelPeepWindow->UpdateImage(processor->GetOriginalImage());
		histogramDisplay->UpdateHistograms();
		if (processor->GetDoFitImage()) {
			imagePanel->FitImage();
		}
		processor->Unlock();
		imagePanel->Redraw();

		auiManager->GetPane(imagePanel).Caption("Working Image - ORIGINAL");
		auiManager->Update();
	}

	// Otherwise, show processed image
	else {
		processor->Lock();
		if (processor->GetImage()->GetWidth() < 1 || processor->GetImage()->GetHeight() < 1) {
			processor->Unlock();
			return;
		}
		imagePanel->ChangeImage(processor->GetImage());
		pixelPeepWindow->UpdateImage(processor->GetImage());
		histogramDisplay->UpdateHistograms();
		if (processor->GetDoFitImage()) {
			imagePanel->FitImage();
		}
		processor->Unlock();
		imagePanel->Redraw();
		auiManager->GetPane(imagePanel).Caption("Working Image");
		auiManager->Update();
	}
	this->SaveCurrentSession();
}

void MainWindow::ShowOriginalWindow(wxCommandEvent& evt) {

	// If image menu item is checked, show the image
	if (evt.IsChecked()) {
		auiManager->GetPane(originalImagePanel).Show();
		auiManager->Update();
	}

	// Otherwise, hide the image
	else {
		auiManager->GetPane(originalImagePanel).Hide();
		auiManager->Update();
	}
	this->SaveCurrentSession();
}

bool MainWindow::OriginalImageDispalyed() {
	return menuTools->FindItem(MainWindow::MenuBar::ID_SHOW_ORIGINAL_WINDOW)->IsChecked();
}

void MainWindow::ShowGuidelines(wxCommandEvent& evt) {

	// If library menu item is checked, show the edit list
	if (evt.IsChecked()) {
		auiManager->GetPane(guidelinesWindow).Show();
		auiManager->Update();
	}

	// Otherwise, hide the library
	else {
		auiManager->GetPane(guidelinesWindow).Hide();
		auiManager->Update();
	}
	this->SaveCurrentSession();
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
	this->SaveCurrentSession();
}

void MainWindow::ShowImageInfo(wxCommandEvent& evt) {

	// If image info item is checked, show the image info
	if(evt.IsChecked()){
		auiManager->GetPane(imageInfoPanel).Show();
		auiManager->Update();
	}

	// Otherwise, hide the image info
	else{
		auiManager->GetPane(imageInfoPanel).Hide();
		auiManager->Update();
	}
	this->SaveCurrentSession();
}

void MainWindow::ShowAbout(wxCommandEvent& WXUNUSED(evt)){
	aboutWindow->CenterOnParent();
	aboutWindow->Show();
}

void MainWindow::ShowSupportedCameras(wxCommandEvent& WXUNUSED(evt)) {
	supportedCamerasWindow->CenterOnParent();
	supportedCamerasWindow->Show();
}

void MainWindow::SetMenuChecks(){

	menuView->FindItem(MainWindow::MenuBar::ID_SHOW_IMAGE)->Check(auiManager->GetPane(imagePanel).IsShown());
	menuView->FindItem(MainWindow::MenuBar::ID_SHOW_EDIT_LIST)->Check(auiManager->GetPane(editList).IsShown());
	menuView->FindItem(MainWindow::MenuBar::ID_SHOW_HISTOGRAMS)->Check(auiManager->GetPane(histogramDisplay).IsShown());
	menuView->FindItem(MainWindow::MenuBar::ID_SHOW_IMAGE_INFO)->Check(auiManager->GetPane(imageInfoPanel).IsShown());

	menuTools->FindItem(MainWindow::MenuBar::ID_SHOW_SNAPSHOTS)->Check(auiManager->GetPane(snapshotWindow).IsShown());
	menuTools->FindItem(MainWindow::MenuBar::ID_SHOW_ORIGINAL_WINDOW)->Check(auiManager->GetPane(originalImagePanel).IsShown());
	menuTools->FindItem(MainWindow::MenuBar::ID_SHOW_PIXEL_PEEP)->Check(auiManager->GetPane(pixelPeepWindow).IsShown());
	menuTools->FindItem(MainWindow::MenuBar::ID_SHOW_GUIDELINES)->Check(auiManager->GetPane(guidelinesWindow).IsShown());
	menuTools->FindItem(MainWindow::MenuBar::ID_SHOW_LIBRARY)->Check(auiManager->GetPane(libraryWindow).IsShown());
}

void MainWindow::OnEnableFastEdit(wxCommandEvent& evt) {
	if(evt.IsChecked()){ 
		processor->EnableFastEdit(); 
		imagePanel->EnableHalfSize();
		currentSession->SetFastProcess(true);
	}
	else{ 
		processor->DisableFastEdit(); 
		imagePanel->DisableHalfSize();
		currentSession->SetFastProcess(false);
	}
	editList->ReprocessImageRaw(true);
	this->SaveCurrentSession();
}

void MainWindow::EnableDisableMenuItemsNoProject(bool enable) {

	menuFile->FindItem(MainWindow::MenuBar::ID_SHOW_EXPORT)->Enable(enable);
	menuFile->FindItem(MainWindow::MenuBar::ID_CLOSE_ALL_PROJECTS)->Enable(enable);
	menuFile->FindItem(MainWindow::MenuBar::ID_CLOSE_CURRENT_PROJECT)->Enable(enable);

	menuView->FindItem(MainWindow::MenuBar::ID_SHOW_IMAGE)->Enable(enable);
	menuView->FindItem(MainWindow::MenuBar::ID_SHOW_EDIT_LIST)->Enable(enable);
	menuView->FindItem(MainWindow::MenuBar::ID_SHOW_HISTOGRAMS)->Enable(enable);
	menuView->FindItem(MainWindow::MenuBar::ID_SHOW_IMAGE_INFO)->Enable(enable);

	menuTools->FindItem(MainWindow::MenuBar::ID_FORCE_REPROCESS)->Enable(enable);
	menuTools->FindItem(MainWindow::MenuBar::ID_ENABLE_FAST_EDIT)->Enable(enable);
	menuTools->FindItem(MainWindow::MenuBar::ID_SHOW_ORIGINAL)->Enable(enable);
	menuTools->FindItem(MainWindow::MenuBar::ID_SHOW_ORIGINAL_WINDOW)->Enable(enable);
	menuTools->FindItem(MainWindow::MenuBar::ID_SHOW_SNAPSHOTS)->Enable(enable);
	menuTools->FindItem(MainWindow::MenuBar::ID_SHOW_PIXEL_PEEP)->Enable(enable);
	menuTools->FindItem(MainWindow::MenuBar::ID_SHOW_GUIDELINES)->Enable(enable);

	// Uncheck items and hide panels if needed
	if (!enable) {
		menuView->FindItem(MainWindow::MenuBar::ID_SHOW_IMAGE)->Check(false);

		menuView->FindItem(MainWindow::MenuBar::ID_SHOW_EDIT_LIST)->Check(false);
		menuView->FindItem(MainWindow::MenuBar::ID_SHOW_HISTOGRAMS)->Check(false);
		menuView->FindItem(MainWindow::MenuBar::ID_SHOW_IMAGE_INFO)->Check(false);

		menuTools->FindItem(MainWindow::MenuBar::ID_ENABLE_FAST_EDIT)->Check(false);
		menuTools->FindItem(MainWindow::MenuBar::ID_SHOW_ORIGINAL)->Check(false);
		menuTools->FindItem(MainWindow::MenuBar::ID_SHOW_ORIGINAL_WINDOW)->Check(false);
		menuTools->FindItem(MainWindow::MenuBar::ID_SHOW_SNAPSHOTS)->Check(false);
		menuTools->FindItem(MainWindow::MenuBar::ID_SHOW_PIXEL_PEEP)->Check(false);
		menuTools->FindItem(MainWindow::MenuBar::ID_SHOW_GUIDELINES)->Check(false);

		auiManager->GetPane(imagePanel).Hide();
		auiManager->GetPane(originalImagePanel).Hide();
		auiManager->GetPane(pixelPeepWindow).Hide();
		auiManager->GetPane(snapshotWindow).Hide();
		auiManager->GetPane(guidelinesWindow).Hide();
		auiManager->GetPane(editList).Hide();
		auiManager->GetPane(histogramDisplay).Hide();

		auiManager->Update();
	}
}

void MainWindow::OnPaneClose(wxAuiManagerEvent& evt){
	evt.GetPane()->Hide();
	this->SetMenuChecks();
}

void MainWindow::OnImagePanelMouse(wxMouseEvent & evt) {

	int x = evt.GetPosition().x;
	int y = evt.GetPosition().y;

	bool originalShown = menuTools->FindItem(MainWindow::MenuBar::ID_SHOW_ORIGINAL)->IsChecked();
	// Modified image pixel peep
	if(evt.GetId() == imagePanel->GetId() && !originalShown){

		processor->Lock();
		pixelPeepWindow->UpdateImage(processor->GetImage());
		pixelPeepWindow->UpdatePosition(x, y);
		processor->Unlock();
	}

	// Original image pixel peep
	else if(evt.GetId() == originalImagePanel->GetId() || originalShown){

		processor->Lock();
		pixelPeepWindow->UpdateImage(processor->GetOriginalImage());
		pixelPeepWindow->UpdatePosition(x, y);
		processor->Unlock();
	}
	else{}
}

void MainWindow::ShowExport(wxCommandEvent& WXUNUSED(event)) {
	auiManager->GetPane(exportWindow).Show();
	auiManager->GetPane(exportWindow).Center();
	auiManager->Update();
}

void MainWindow::SetStatusbarText(wxString text) {
	if (statusBarText != NULL) {
		statusBarText->Destroy();
	}
	statusBarText = new wxStaticText(this->GetStatusBar(), -1, text, wxPoint(10, 5));
	statusBarText->Show();
}

void MainWindow::ClearStatusbarText(wxTimerEvent& WXUNUSED(evt)){
	this->SetStatusbarText("");
}

void MainWindow::RecieveMessageFromProcessor(wxCommandEvent& messageEvt) {
	this->SetStatusbarText(messageEvt.GetString());
	exportWindow->SetMessage(messageEvt.GetString());
}

void MainWindow::RecieveNumFromProcessor(wxCommandEvent& numEvt) {
	exportWindow->SetEditNum(numEvt.GetInt());
}

void MainWindow::OnUpdateImage(wxCommandEvent& WXUNUSED(event)) {

	// Do not update image display if original image is checked
	if (this->OriginalImageDispalyed()) { 
		processor->SetDoFitImage(false);
		processor->SetUpdated(false);
		return;
	}

	processor->Lock();
	if (processor->GetImage()->GetWidth() < 1 || processor->GetImage()->GetHeight() < 1) {
		processor->Unlock();
		return;
	}
	ChangeImageThread * changeImage = new ChangeImageThread(imagePanel, processor, processor->GetDoFitImage());
	changeImage->Run();
	pixelPeepWindow->UpdateImage(processor->GetImage());
	histogramDisplay->UpdateHistograms();
	if (processor->GetDoFitImage()) {

		currentSession->SetImageZoomLevel(imagePanel->GetZoom());
		this->SaveCurrentSession();
	}
	processor->Unlock();
	imagePanel->Redraw();
	
	exportWindow->ProcessingComplete();
}

bool MainWindow::CheckSessionNeedsSaved(PhoediXSession * session) {

	PhoediXSession sessionOnDisk;

	if (session->GetProjectPath() == "") { return true; }
	sessionOnDisk.LoadSessionFromFile(session->GetProjectPath() + wxFileName::GetPathSeparator() + session->GetName());
	sessionOnDisk.SetName(session->GetName());
	sessionOnDisk.SetProjectPath(session->GetProjectPath());
	return !PhoediXSession::CompareSessions(session, &sessionOnDisk);
}

void MainWindow::OnClose(wxCloseEvent& WXUNUSED(evt)) {

	// Kill current raw processing and wait until raw thread is stopped
	processor->KillRawProcessing();
	while (processor->GetLockedRaw()) { app->Yield(); }

	// Lock raw permanently to prevent processor from attempting to reprocess raw
	processor->LockRaw();

	// Kill current processing and wait until thread is stopped
	processor->KillCurrentProcessing();
	while (processor->GetLocked()) { app->Yield(); }

	// Lock processor permanently to prevent any further processing
	processor->Lock();

	imagePanel->DestroyTimer();
	originalImagePanel->DestroyTimer();

	// Remove all sessions from menu window
	for (size_t i = 0; i < allSessions.size(); i++) {

		// Destroy the session
		delete allSessions.at(i);
	}

	allSessions.clear();

	// Delete histogram display resources
	histogramDisplay->DestroyHistograms();

	// Delete all snapshots
	snapshotWindow->DeleteAllSnapshots();

	settingsWindow->Cleanup();
	auiManager->UnInit();
	delete auiManager;
	
	delete processor;
	delete emptyImage;
	delete emptyPhxImage;
	delete clearStatusTimer;

	this->Destroy();
}

MainWindow::ChangeImageThread::ChangeImageThread(ZoomImagePanel * imagePanel, Processor * processor, bool fitImage) : wxThread(wxTHREAD_DETACHED){
	imgPanel = imagePanel;
	proc = processor;
	fit = fitImage;
}

wxThread::ExitCode MainWindow::ChangeImageThread::Entry(){
	imgPanel->ChangeImage(proc->GetImage());
	if (fit) { imgPanel->FitImage(); }
	proc->SetDoFitImage(false);
	return (wxThread::ExitCode)0;
}

// Copyright 2016 Jacob Chesley

#include "LibraryWindow.h"


LibraryWindow::LibraryWindow(wxWindow * parent) : wxScrolledWindow(parent){

	this->SetBackgroundColour(parent->GetBackgroundColour());
	
	mainLayout = new wxBoxSizer(wxVERTICAL);
	imagesLayout = new wxWrapSizer();
	this->SetSizer(mainLayout);

	toolbarLayout = new wxWrapSizer();

	showDirectoriesButton = new wxButton(this, LibraryWindow::MenuBar::ID_SHOW_DIRECTORY_LIST, "Import Directories", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	showDirectoriesButton->SetForegroundColour(Colors::TextLightGrey);
	showDirectoriesButton->SetBackgroundColour(Colors::BackGrey);
	showDirectoriesButton->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	importButton = new wxButton(this, LibraryWindow::MenuBar::ID_IMPORT, "Populate Library", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	importButton->SetForegroundColour(Colors::TextLightGrey);
	importButton->SetBackgroundColour(Colors::BackGrey);
	importButton->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	copyButton = new wxButton(this, LibraryWindow::MenuBar::ID_COPY_TO, "Copy Selected", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	copyButton->SetForegroundColour(Colors::TextLightGrey);
	copyButton->SetBackgroundColour(Colors::BackGrey);
	copyButton->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	moveButton = new wxButton(this, LibraryWindow::MenuBar::ID_MOVE_TO, "Move Selected", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	moveButton->SetForegroundColour(Colors::TextLightGrey);
	moveButton->SetBackgroundColour(Colors::BackGrey);
	moveButton->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	clearButton = new wxButton(this, -1, "Clear Library", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	clearButton->SetForegroundColour(Colors::TextLightGrey);
	clearButton->SetBackgroundColour(Colors::BackGrey);
	clearButton->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
	clearButton->Connect(wxEVT_ENTER_WINDOW, wxMouseEventHandler(LibraryWindow::OnHoverClearButton), NULL, this);

	toolbarLayout->Add(showDirectoriesButton);
	toolbarLayout->AddSpacer(15);
	toolbarLayout->Add(importButton);
	toolbarLayout->AddSpacer(15);
	toolbarLayout->Add(copyButton);
	toolbarLayout->AddSpacer(15);
	toolbarLayout->Add(moveButton);
	toolbarLayout->AddSpacer(15);
	toolbarLayout->Add(clearButton);

	this->GetSizer()->Add(toolbarLayout);
	this->GetSizer()->Add(imagesLayout);

	directorySelections = new DirectorySelections(this);
	wxAuiPaneInfo directoryPaneInfo = wxAuiPaneInfo();
	directoryPaneInfo.Hide();
	directoryPaneInfo.Float();
	directoryPaneInfo.Caption("Library Directory Selections");
	directoryPaneInfo.Name("Library Directories");
	directoryPaneInfo.CloseButton(true);
	directoryPaneInfo.PinButton(true);
	directoryPaneInfo.DestroyOnClose(false);
	directoryPaneInfo.BestSize(directorySelections->GetClientSize());
	PhoedixAUIManager::GetPhoedixAUIManager()->AddPane(directorySelections, directoryPaneInfo);
	PhoedixAUIManager::GetPhoedixAUIManager()->Update();

	this->SetScrollRate(5, 5);
	
	this->Bind(wxEVT_BUTTON, (wxObjectEventFunction)&LibraryWindow::OnShowDirectories, this, LibraryWindow::MenuBar::ID_SHOW_DIRECTORY_LIST);
	this->Bind(wxEVT_BUTTON, (wxObjectEventFunction)&LibraryWindow::OnImport, this, LibraryWindow::MenuBar::ID_IMPORT);
	this->Bind(wxEVT_BUTTON, (wxObjectEventFunction)&LibraryWindow::OnCopy, this, LibraryWindow::MenuBar::ID_COPY_TO);
	this->Bind(wxEVT_BUTTON, (wxObjectEventFunction)&LibraryWindow::OnMove, this, LibraryWindow::MenuBar::ID_MOVE_TO);
	this->Bind(wxEVT_SIZE, (wxObjectEventFunction)&LibraryWindow::OnResize, this);
	this->Bind(ADD_LIB_IMAGE_EVENT, (wxObjectEventFunction)&LibraryWindow::OnAddImage, this);
}

void LibraryWindow::OnShowDirectories(wxCommandEvent& WXUNUSED(evt)){
	PhoedixAUIManager::GetPhoedixAUIManager()->GetPane(directorySelections).Show();
	PhoedixAUIManager::GetPhoedixAUIManager()->Update();
}

void LibraryWindow::OnResize(wxSizeEvent& WXUNUSED(evt)) {
	this->FitInside();
}

void LibraryWindow::OnImport(wxCommandEvent& WXUNUSED(evt)){

	LoadImagesThread * testImgThread = new LoadImagesThread(this);
	testImgThread->Run();
}

void LibraryWindow::OnCopy(wxCommandEvent& WXUNUSED(evt)){

	// Browse for directory to look for images
	wxDirDialog openDirDialog(this, "Copy Images To...");
	if (openDirDialog.ShowModal() == wxID_CANCEL) {
		return;
	}

	CopyImagesThread * copyThread = new CopyImagesThread(this, this->GetSelectedFileNames(), openDirDialog.GetPath(), false);
	copyThread->Run();
}

void LibraryWindow::OnMove(wxCommandEvent& WXUNUSED(evt)){

	// Browse for directory to look for images
	wxDirDialog openDirDialog(this, "Move Images To...");
	if (openDirDialog.ShowModal() == wxID_CANCEL) {
		return;
	}

	CopyImagesThread * moveThread = new CopyImagesThread(this, this->GetSelectedFileNames(), openDirDialog.GetPath(), true);
	moveThread->Run();
}

void LibraryWindow::OnHoverClearButton(wxMouseEvent& WXUNUSED(evt)) {

	// Display a popup menu of options
	wxMenu popupMenu;
	popupMenu.Append(LibraryWindow::MenuBar::ID_CLEAR_UNSELECTED, "Clear Unselected Images");
	popupMenu.Append(LibraryWindow::MenuBar::ID_CLEAR_SELECTED, "Clear Selected Images");
	popupMenu.Append(LibraryWindow::MenuBar::ID_CLEAR_ALL, "Clear All Images");

	popupMenu.Connect(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(LibraryWindow::OnPopupMenuClick), NULL, this);
	this->PopupMenu(&popupMenu);
}

void LibraryWindow::OnPopupMenuClick(wxCommandEvent& inEvt) {

	int id = inEvt.GetId();

	switch (id) {

		// Clear unselected
		case LibraryWindow::MenuBar::ID_CLEAR_UNSELECTED: {
			this->ClearUnselected();
			break;
		}

		// Clear selected
		case LibraryWindow::MenuBar::ID_CLEAR_SELECTED: {
			this->ClearSelected();
			break;
		}

		// Clear all
		case LibraryWindow::MenuBar::ID_CLEAR_ALL: {
			this->ClearAll();
			break;
		}
	}
}

void LibraryWindow::ClearAll(){

	imagesLayout->Clear(true);
	libraryImages.clear();
	includedImagePaths.clear();
	this->Layout();
	this->FitInside();
}

void LibraryWindow::ClearSelected() {

	wxVector<LibraryImage*> libraryImagesTokeep;

	// Iterate over all library images
	for (size_t i = 0; i < libraryImages.size(); i++) {

		// Detach and destroy all unslected imaged
		if (libraryImages.at(i)->GetSelected()) {
			imagesLayout->Detach(libraryImages.at(i));
			libraryImages.at(i)->Destroy();
		}

		// Add selected images to vector of library images to keep
		else {
			libraryImagesTokeep.push_back(libraryImages.at(i));
		}
	}
	
	// Clear library image vector
	libraryImages.clear();
	includedImagePaths.clear();

	// Add library images to keep
	for (size_t i = 0; i < libraryImagesTokeep.size(); i++) {
		libraryImages.push_back(libraryImagesTokeep.at(i));
	}

	this->Layout();
	this->FitInside();
}

void LibraryWindow::ClearUnselected() {

	wxVector<LibraryImage*> libraryImagesTokeep;

	// Iterate over all library images
	for (size_t i = 0; i < libraryImages.size(); i++) {

		// Detach and destroy all unslected imaged
		if (!libraryImages.at(i)->GetSelected()) {
			imagesLayout->Detach(libraryImages.at(i));
			libraryImages.at(i)->Destroy();
		}

		// Add selected images to vector of library images to keep
		else {
			libraryImagesTokeep.push_back(libraryImages.at(i));
		}
	}

	// Clear library image vector
	libraryImages.clear();
	includedImagePaths.clear();

	// Add library images to keep
	for (size_t i = 0; i < libraryImagesTokeep.size(); i++) {
		libraryImages.push_back(libraryImagesTokeep.at(i));
	}

	this->Layout();
	this->FitInside();
}

void LibraryWindow::OnAddImage(AddLibraryImageEvent & evt){

	// If the image is not currently in the images display
	if(!this->CheckIfImageInDisplay(evt.GetFilePath())){

		// Add the image to the vector of displayed image paths, then add image to display
		includedImagePaths.push_back(evt.GetFilePath());
		this->AddLibraryImage(evt.GetImage(), evt.GetFileName(), evt.GetFilePath());
	}
}

bool LibraryWindow::CheckIfImageInDisplay(wxString imagePath){
	for(size_t i = 0; i < includedImagePaths.size(); i++){
		if(includedImagePaths.at(i).CmpNoCase(imagePath) == 0){
			return true;
		}
	}
	return false;
}
void LibraryWindow::AddLibraryImage(wxImage * newImage, wxString fileName, wxString filePath) {
	
	LibraryImage * newLibImage = new LibraryImage(this, newImage, fileName, filePath);
	newImage->Destroy();
	delete newImage;
	imagesLayout->Add(newLibImage);
	imagesLayout->AddSpacer(50);
	libraryImages.push_back(newLibImage);	
	this->Layout();
	this->FitInside();
}

wxVector<wxString> LibraryWindow::GetSelectedFileNames() {
	
	wxVector<wxString> fileNames;

	for(size_t i = 0; i < libraryImages.size(); i++){
		if(libraryImages.at(i)->GetSelected()){
			fileNames.push_back(libraryImages.at(i)->GetPath());
		}
	}

	return fileNames;
}

LibraryWindow::LoadImagesThread::LoadImagesThread(LibraryWindow * parent) : wxThread(wxTHREAD_DETACHED){
	par = parent;
}

wxThread::ExitCode LibraryWindow::LoadImagesThread::Entry(){

	int maxSize = 250;

	LibRaw rawProc;

	// Get all directories to check
	wxVector<wxString> allDirectories = par->directorySelections->GetDirectoryList();

	// Go through all directories
	for (size_t dir = 0; dir < allDirectories.size(); dir++) {

		// Check directory, and continue if it does not exist
		if (!wxDir::Exists(allDirectories[dir])) {
			continue;
		}

		// Get all file names
		wxArrayString allFileNames;
		wxDir::GetAllFiles(allDirectories[dir], &allFileNames);

		// Go through all files, check if each are an image
		for (size_t file = 0; file < allFileNames.size(); file++) {

			wxString fileName = allFileNames[file];
			wxImage * displayImage = NULL;

			// We have a raw image we can load in
			if (ImageHandler::CheckRaw(fileName)) {
				#ifdef __WXMSW__
					rawProc.open_file(fileName.wc_str());
				#else
					rawProc.open_file(fileName.c_str());
				#endif
				rawProc.unpack();
				rawProc.imgdata.params.half_size = 1;
				rawProc.imgdata.params.use_camera_wb = 1;
				rawProc.imgdata.params.use_auto_wb = 0;

				int rawError = LIBRAW_SUCCESS;

				rawProc.dcraw_process();
				libraw_processed_image_t * rawImg = rawProc.dcraw_make_mem_image(&rawError);

				if (rawError == LIBRAW_SUCCESS) {

					displayImage = new wxImage(rawImg->width, rawImg->height);
					ImageHandler::CopyImageFromRaw(rawImg, displayImage);
				}
				rawProc.dcraw_clear_mem(rawImg);
				rawProc.recycle();
			}

			// We have an image we can load in
			else if (ImageHandler::CheckImage(fileName)) {
				displayImage = new wxImage(fileName);
			}
			else {
				continue;
			}

			if (displayImage != NULL) {
				// Scale image based on width > height
				if (displayImage->GetHeight() < displayImage->GetWidth()) {
					double aspect = (double)displayImage->GetHeight() / (double)displayImage->GetWidth();
					displayImage->Rescale(maxSize, maxSize * aspect, wxIMAGE_QUALITY_HIGH);

				}
				// Scale image based on height > width
				else if (displayImage->GetHeight() > displayImage->GetWidth()) {
					double aspect = (double)displayImage->GetWidth() / (double)displayImage->GetHeight();
					displayImage->Rescale(maxSize * aspect, maxSize, wxIMAGE_QUALITY_HIGH);
				}

				// Image has same width and height, set to max size
				else {
					displayImage->Rescale(maxSize, maxSize, wxIMAGE_QUALITY_HIGH);
				}
				
				// Send the image to the parent to be added to the window
				wxFileName tempFile(fileName);

				AddLibraryImageEvent libImageEvent(ADD_LIB_IMAGE_EVENT, 0, displayImage, tempFile.GetFullName(), fileName);
				wxPostEvent(par, libImageEvent);

			}
		}
	}
	rawProc.recycle();
	return 0;
}

LibraryWindow::CopyImagesThread::CopyImagesThread(LibraryWindow * parent, wxVector<wxString> filesToCopy, wxString destination, bool deleteAfterCopy) : wxThread(wxTHREAD_DETACHED){
	par = parent;
	toCopy = filesToCopy;
	doDelete = deleteAfterCopy;
	destFolder = destination;
}

wxThread::ExitCode LibraryWindow::CopyImagesThread::Entry(){

	// Dont perform copy or move if destination directory does not exist
	if(!wxDirExists(destFolder)){ return (wxThread::ExitCode)0; }

	wxString progressTitle = "";
	if(doDelete){ progressTitle = "Move Files Progress"; }
	else{ progressTitle = "Copy Files Progress"; }

	wxProgressDialog * progressDialog = new wxProgressDialog(progressTitle, "", toCopy.size());
	
	wxString progressMessage = "";

	for(size_t i = 0; i < toCopy.size(); i++){
		wxString fileToCopy = toCopy.at(i);

		// Verify file exists to copy
		if(wxFileExists(fileToCopy)){

			// Create file name to create new file string of destination
			wxFileName fileName(fileToCopy);
			wxString newFilePath = destFolder + fileName.GetPathSeparator() + fileName.GetFullName();

			// Update progress dialog message with appropriate prefix
			if(doDelete){ progressMessage = "Moving file " + fileName.GetFullName(); }
			else{ progressMessage = "Copying file " + fileName.GetFullName(); }

			// Update progress
			progressDialog->Update(i, progressMessage);

			// Copy (and delete file if it is being moved)
			if(wxCopyFile(fileToCopy, newFilePath)){
				if(doDelete){
					wxRemoveFile(fileToCopy);
				}
			}
		}
		else{
			// Update progress dialog with blank message if file does not exist
			progressDialog->Update(i);
		}
	}

	progressDialog->Destroy();
	return (wxThread::ExitCode)0;
}
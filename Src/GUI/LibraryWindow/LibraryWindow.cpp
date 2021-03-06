// Copyright 2018 Jacob Chesley.
// See LICENSE.TXT in root of project for license information.

#include "LibraryWindow.h"

wxDEFINE_EVENT(POPULATION_STARTED_EVENT, wxCommandEvent);
wxDEFINE_EVENT(POPULATION_COMPLETE_EVENT, wxCommandEvent);

LibraryWindow::LibraryWindow(wxWindow * parent) : wxScrolledWindow(parent){

	parWindow = parent;
	populationStarted = false;
	populationCanceled = false;

	this->SetBackgroundColour(parent->GetBackgroundColour());
	
	mainLayout = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(mainLayout);

	toolbarLayout = new wxBoxSizer(wxHORIZONTAL);

	showDirectoriesButton = new PhoediXButton(this, LibraryWindow::MenuBar::ID_SHOW_DIRECTORY_LIST, "Import Directories");
	showDirectoriesButton->SetForegroundColour(Colors::TextLightGrey);
	showDirectoriesButton->SetBackgroundColour(Colors::BackGrey);
	showDirectoriesButton->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	importButton = new PhoediXButton(this, LibraryWindow::MenuBar::ID_IMPORT, "Populate Library");
	importButton->SetForegroundColour(Colors::TextLightGrey);
	importButton->SetBackgroundColour(Colors::BackGrey);
	importButton->SetDisableBackgroundColour(Colors::BackGrey);
	importButton->SetDisableForegroundColour(Colors::TextGrey);
	importButton->Disable();
	importButton->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	copyButton = new PhoediXButton(this, LibraryWindow::MenuBar::ID_COPY_TO, "Copy Selected");
	copyButton->SetForegroundColour(Colors::TextLightGrey);
	copyButton->SetBackgroundColour(Colors::BackGrey);
	copyButton->SetDisableBackgroundColour(Colors::BackGrey);
	copyButton->SetDisableForegroundColour(Colors::TextGrey);
	copyButton->Disable();
	copyButton->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	moveButton = new PhoediXButton(this, LibraryWindow::MenuBar::ID_MOVE_TO, "Move Selected");
	moveButton->SetForegroundColour(Colors::TextLightGrey);
	moveButton->SetBackgroundColour(Colors::BackGrey);
	moveButton->SetDisableBackgroundColour(Colors::BackGrey);
	moveButton->SetDisableForegroundColour(Colors::TextGrey);
	moveButton->Disable();
	moveButton->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	clearButton = new PhoediXButton(this, -1, "Clear Library");
	clearButton->SetForegroundColour(Colors::TextLightGrey);
	clearButton->SetBackgroundColour(Colors::BackGrey);
	clearButton->SetDisableBackgroundColour(Colors::BackGrey);
	clearButton->SetDisableForegroundColour(Colors::TextGrey);
	clearButton->Disable();
	clearButton->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	clearButton->Connect(wxEVT_ENTER_WINDOW, wxMouseEventHandler(LibraryWindow::OnHoverClearButton), NULL, this);

	imageScroll = new ImageWindowScroll(this);
	imageScroll->SetBackgroundColour(this->GetBackgroundColour());

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
	this->GetSizer()->Add(imageScroll, 1, wxEXPAND);

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

	this->Bind(wxEVT_BUTTON, (wxObjectEventFunction)&LibraryWindow::OnShowDirectories, this, LibraryWindow::MenuBar::ID_SHOW_DIRECTORY_LIST);
	this->Bind(wxEVT_BUTTON, (wxObjectEventFunction)&LibraryWindow::OnImport, this, LibraryWindow::MenuBar::ID_IMPORT);
	this->Bind(wxEVT_BUTTON, (wxObjectEventFunction)&LibraryWindow::OnCopy, this, LibraryWindow::MenuBar::ID_COPY_TO);
	this->Bind(wxEVT_BUTTON, (wxObjectEventFunction)&LibraryWindow::OnMove, this, LibraryWindow::MenuBar::ID_MOVE_TO);
	this->Bind(wxEVT_SIZE, (wxObjectEventFunction)&LibraryWindow::OnResize, this);
	this->Bind(ADD_LIB_IMAGE_EVENT, (wxObjectEventFunction)&LibraryWindow::OnAddImage, this);
	this->Bind(POPULATION_STARTED_EVENT, (wxObjectEventFunction)&LibraryWindow::OnPopulationStart, this);
	this->Bind(POPULATION_COMPLETE_EVENT, (wxObjectEventFunction)&LibraryWindow::OnPopulationComplete, this);
	this->Bind(DIR_EXISTS, (wxObjectEventFunction)&LibraryWindow::OnDirExists, this);
	this->Bind(NO_DIR_EXISTS, (wxObjectEventFunction)&LibraryWindow::OnNoDirExists, this);
	this->Bind(LIBRARY_IMAGE_CHECK_EVENT, (wxObjectEventFunction)&LibraryWindow::OnLibraryImageCheck, this);

	totalNumberFiles = -1;
	numberFilesAdded = -1;
}

void LibraryWindow::OnShowDirectories(wxCommandEvent& WXUNUSED(evt)){
	PhoedixAUIManager::GetPhoedixAUIManager()->GetPane(directorySelections).Show();
	PhoedixAUIManager::GetPhoedixAUIManager()->Update();
}

void LibraryWindow::OnResize(wxSizeEvent& WXUNUSED(evt)) {

	imageScroll->SetVirtualSize(imageScroll->GetSize());
	this->FitInside();
	this->Layout();
}

void LibraryWindow::OnDirExists(wxSizeEvent& WXUNUSED(evt)) {
	importButton->Enable();
}

void LibraryWindow::OnNoDirExists(wxSizeEvent& WXUNUSED(evt)) {
	importButton->Disable();
}

void LibraryWindow::EnableDisableMoveCopy() {

	bool anySelected = false;
	// Iterate over all library images
	for (size_t i = 0; i < libraryImages.size(); i++) {

		// Find any selected images
		if (libraryImages.at(i)->GetSelected()) {
			anySelected = true;
		}
	}

	// Enable copy/move
	if (anySelected) {
		moveButton->Enable();
		copyButton->Enable();
	}

	// Disable copy/move
	else {
		moveButton->Disable();
		copyButton->Disable();
	}
}

void LibraryWindow::OnLibraryImageCheck(wxCommandEvent& WXUNUSED(evt)) {
	this->EnableDisableMoveCopy();
}

void LibraryWindow::OnImport(wxCommandEvent& WXUNUSED(evt)){

	if (!populationStarted) {
		testImgThread = new LoadImagesThread(this);
		testImgThread->Run();
		populationCanceled = false;
	}
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

	if (clearButton->GetEnabled() == false) { return; }

	// Display a popup menu of options
	wxMenu popupMenu;
	
	if (populationStarted) {
		popupMenu.Append(LibraryWindow::MenuBar::ID_CANCEL_PROCESS, "Cancel Population");
	}
	else {
		popupMenu.Append(LibraryWindow::MenuBar::ID_CLEAR_UNSELECTED, "Clear Unselected Images");
		popupMenu.Append(LibraryWindow::MenuBar::ID_CLEAR_SELECTED, "Clear Selected Images");
		popupMenu.Append(LibraryWindow::MenuBar::ID_CLEAR_ALL, "Clear All Images");
	}
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

		// Cancel Processing
		case LibraryWindow::MenuBar::ID_CANCEL_PROCESS: {
			if (testImgThread != NULL) {
				populationCanceled = true;
				testImgThread->Cancel();
			}
			break;
		}
	}
}

void LibraryWindow::ClearAll(){

	// Iterate over all library images
	for (size_t i = 0; i < libraryImages.size(); i++) {
		imageScroll->GetSizer()->Detach(libraryImages.at(i));
		libraryImages.at(i)->Destroy();
	}
	imageScroll->GetSizer()->Clear(true);
	libraryImages.clear();
	includedImagePaths.clear();
	imagePaths.Clear();

	this->Layout();
	this->FitInside();

	if (libraryImages.size() > 0) { clearButton->Enable(); }
	else { clearButton->Disable(); }
	this->EnableDisableMoveCopy();
}

void LibraryWindow::ClearSelected() {

	wxVector<LibraryImage*> libraryImagesTokeep;

	// Iterate over all library images
	for (size_t i = 0; i < libraryImages.size(); i++) {

		// Detach and destroy all unslected imaged
		if (libraryImages.at(i)->GetSelected()) {
			imageScroll->GetSizer()->Detach(libraryImages.at(i));
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
	imagePaths.Clear();

	// Add library images to keep
	for (size_t i = 0; i < libraryImagesTokeep.size(); i++) {
		libraryImages.push_back(libraryImagesTokeep.at(i));
		includedImagePaths.push_back(libraryImagesTokeep.at(i)->GetPath());
		imagePaths.Add(libraryImagesTokeep.at(i)->GetPath());
	}

	this->Layout();
	this->FitInside();

	if (libraryImages.size() > 0) { clearButton->Enable(); }
	else { clearButton->Disable(); }
	this->EnableDisableMoveCopy();
}

void LibraryWindow::ClearUnselected() {

	wxVector<LibraryImage*> libraryImagesTokeep;

	// Iterate over all library images
	for (size_t i = 0; i < libraryImages.size(); i++) {

		// Detach and destroy all unslected imaged
		if (!libraryImages.at(i)->GetSelected()) {
			imageScroll->GetSizer()->Detach(libraryImages.at(i));
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
	imagePaths.clear();

	// Add library images to keep
	for (size_t i = 0; i < libraryImagesTokeep.size(); i++) {
		libraryImages.push_back(libraryImagesTokeep.at(i));
		includedImagePaths.push_back(libraryImagesTokeep.at(i)->GetPath());
		imagePaths.Add(libraryImagesTokeep.at(i)->GetPath());
	}

	this->Layout();
	this->FitInside();

	if (libraryImages.size() > 0) { clearButton->Enable(); }
	else { clearButton->Disable(); }
	this->EnableDisableMoveCopy();
}

void LibraryWindow::OnAddImage(AddLibraryImageEvent & evt){

	// If the image is not currently in the images display
	if(!this->CheckIfImageInDisplay(evt.GetFilePath())){

		// Add the image to the vector of displayed image paths, then add image to display
		includedImagePaths.push_back(evt.GetFilePath());
		this->AddLibraryImage(evt.GetImage(), evt.GetFileName(), evt.GetFilePath());
	}
}

void LibraryWindow::OnPopulationStart(wxSizeEvent & WXUNUSED(evt)) {
	populationStarted = true;

	// Get all directories to check
	wxVector<wxString> allDirectories = directorySelections->GetDirectoryList();

	int totalFiles = 0;
	// Go through all directories
	for (size_t dir = 0; dir < allDirectories.size(); dir++) {

		wxString dirName = allDirectories[dir];
		// Check directory, and continue if it does not exist
		if (!wxDir::Exists(allDirectories[dir])) { continue; }

		// Get all file names
		wxArrayString allFileNames;
		wxDir::GetAllFiles(allDirectories[dir], &allFileNames, wxEmptyString, wxDIR_FILES);
		
		// Add to total number of files
		totalFiles += allFileNames.size();
	}
	totalNumberFiles = totalFiles;
	numberFilesAdded = 0;
}

void LibraryWindow::OnPopulationComplete(wxSizeEvent & WXUNUSED(evt)) {
	populationStarted = false;
	populationCanceled = false;

	totalNumberFiles = -1;
	numberFilesAdded = -1;

	wxCommandEvent evt(PROCESSOR_MESSAGE_EVENT, ID_PROCESSOR_MESSAGE);
	evt.SetString("");
	wxPostEvent(parWindow, evt);
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
	
	if (populationCanceled) {

		// Clean up image and return
		newImage->Destroy();
		delete newImage;
		newImage = NULL;
		return;
	}

	locker.Enter();
	LibraryImage * newLibImage = new LibraryImage(imageScroll, this, newImage, fileName, filePath);
	newLibImage->SetMinSize(this->GetLibraryImageSize());
	newImage->Destroy();
	delete newImage;
	newImage = NULL;

	int idx = imagePaths.Add(fileName);
	imageScroll->GetSizer()->Insert(idx, newLibImage);
	libraryImages.insert(libraryImages.begin() + idx, newLibImage);

	this->Layout();
	this->FitInside();

	if (libraryImages.size() > 0) { clearButton->Enable(); }
	else { clearButton->Disable(); }

	numberFilesAdded += 1;
	// Send message to display in parent status on progress of loading
	if (totalNumberFiles > -1 && numberFilesAdded > -1) {

		wxCommandEvent evt(PROCESSOR_MESSAGE_EVENT, ID_PROCESSOR_MESSAGE);
		wxString curFileStr;
		curFileStr << numberFilesAdded;
		wxString numFileStr;
		numFileStr << totalNumberFiles;

		wxString fullImageNumStr = "Loading " + fileName + " (" + curFileStr + "/" + numFileStr + ")";
		evt.SetString(fullImageNumStr);
		wxPostEvent(parWindow, evt);
	}

	locker.Leave();
}

wxSize LibraryWindow::GetLibraryImageSize() {

	int imageSize = 250;
	int buffer = 30;

	return wxSize(imageSize + buffer, imageSize + buffer);
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

LibraryWindow::ImageWindowScroll::ImageWindowScroll(wxWindow * parent) : wxScrolledWindow(parent) {

	imagesLayout = new wxWrapSizer();
	this->SetSizer(imagesLayout);
	this->SetScrollRate(5, 5);
}

LibraryWindow::LoadImagesThread::LoadImagesThread(LibraryWindow * parent) : wxThread(wxTHREAD_DETACHED){
	par = parent;
	canceled = false;
}

void LibraryWindow::LoadImagesThread::Cancel() {
	canceled = true;
	for (int i = 0; i < subThreads.size(); i++) {
		if (subThreads.at(i) != NULL) {
			subThreads.at(i)->Cancel();
		}
	}
}

wxThread::ExitCode LibraryWindow::LoadImagesThread::Entry(){

	wxCommandEvent start(POPULATION_STARTED_EVENT, ID_POPULATION_STARTED);
	wxPostEvent(par, start);

	int totalThreads = PhoedixSettings::GetNumThreads();
	wxArrayString allFiles;

	// Get all directories to check
	wxVector<wxString> allDirectories = par->directorySelections->GetDirectoryList();

	// Go through all directories
	for (size_t dir = 0; dir < allDirectories.size(); dir++) {

		if (canceled) { break; }

		wxString dirName = allDirectories[dir];
		// Check directory, and continue if it does not exist
		if (!wxDir::Exists(allDirectories[dir])) {continue; }

		// Get all file names
		wxArrayString allFileNames;
		wxDir::GetAllFiles(allDirectories[dir], &allFileNames, wxEmptyString, wxDIR_FILES);

		// Add file names to list of files
		for(size_t file = 0; file < allFileNames.size(); file++){
			allFiles.Add(allFileNames[file]);
		}
	}

	int numFilesPerThread = ceil(allFiles.size()/ totalThreads);
	int remainderFiles = allFiles.size() % totalThreads;
	int usedRemainder = 0;
	wxMutex mutexLock;
	wxCondition wait(mutexLock);
	int threadComplete = 0;

	for(int thread = 0; thread < totalThreads; thread++){
		
		// Find indexes of first and last files, based on number of threads
		int startFileIdx = (thread * numFilesPerThread) + usedRemainder;
		int endFileIdx = ((thread + 1) * numFilesPerThread) + usedRemainder;
		if(remainderFiles > 0 && thread < remainderFiles){
			endFileIdx = (thread + 1) * numFilesPerThread + usedRemainder;
			usedRemainder += 1;
		}
		wxArrayString subFiles;

		// Populate array of files
		for(int subFile = startFileIdx; subFile < endFileIdx; subFile++){
			wxString fileName = allFiles[subFile];
			subFiles.Add(allFiles[subFile]);
		}

		LoadSubsetImagesThread * imageThread = new LoadSubsetImagesThread(par, subFiles, &mutexLock, &wait, totalThreads, &threadComplete);
		subThreads.push_back(imageThread);
		imageThread->Run();
	}

	// Wait for all worker threads to complete
	wait.Wait();

	wxCommandEvent finish(POPULATION_COMPLETE_EVENT, ID_POPULATION_COMPLETE);
	wxPostEvent(par, finish);

	return 0;
}

LibraryWindow::LoadSubsetImagesThread::LoadSubsetImagesThread(LibraryWindow * parent, wxArrayString imagesToLoad, wxMutex * mutLockIn, wxCondition * condition, int numThreads, int * threadsComplete) : wxThread(wxTHREAD_DETACHED){
	par = parent;
	toLoad = imagesToLoad;
	canceled = false;
	mutLock = mutLockIn;
	cond = condition;
	threads = numThreads;
	complete = threadsComplete;
}

void LibraryWindow::LoadSubsetImagesThread::Cancel() {
	canceled = true;
}

wxThread::ExitCode LibraryWindow::LoadSubsetImagesThread::Entry(){
	
	LibRaw * rawProcessor = new LibRaw();
	libraw_processed_image_t * rawImage = NULL;
	int rawError = 0;

	wxImage * displayImage = NULL;
	int maxSize = 250;

	for (int file = 0; file < toLoad.size(); file++) {

		if (canceled) {
			break; 
		}

		// Get filename from vector and check if already displayed in library window.  Skip if already displayed
		wxString fileName = toLoad[file];
		if (par->CheckIfImageInDisplay(fileName)) { continue; }

		// We have a raw image we can load in
		if (ImageHandler::CheckRaw(fileName)) {
			#ifdef __WXMSW__
				rawProcessor->open_file(fileName.wc_str());
			#else
				rawProcessor->open_file(fileName.c_str());
			#endif

			bool useThumbnail = !PhoedixSettings::GetLibraryImageUseRaw();
			int flip= 0;

			// Use embedded thumbnail of raw image
			if(useThumbnail){
				rawProcessor->unpack_thumb();
				flip = rawProcessor->imgdata.sizes.flip;
				rawImage = rawProcessor->dcraw_make_mem_thumb(&rawError);
			}

			// Create display image from raw image data
			else{
				rawProcessor->unpack();
				rawProcessor->imgdata.params.half_size = 1;
				rawProcessor->imgdata.params.use_camera_wb = 1;
				rawProcessor->imgdata.params.use_auto_wb = 0;

				rawProcessor->dcraw_process();
				rawImage = rawProcessor->dcraw_make_mem_image(&rawError);

			}

			if (rawError == LIBRAW_SUCCESS) {

				// Copy raw image to wxImage for display
				wxImage * tempImage = new wxImage(rawImage->width, rawImage->height);
				ImageHandler::CopyImageFromRaw(rawImage, tempImage);
				if (flip == 3) { displayImage = new wxImage(tempImage->Rotate180()); }
				else if (flip == 5) { displayImage = new wxImage(tempImage->Rotate90(false)); }
				else if (flip == 6) { displayImage = new wxImage(tempImage->Rotate90(true)); }
				else { displayImage = new wxImage(*tempImage); }
				delete tempImage;
			}

			// Free raw image and processor memory
			rawProcessor->dcraw_clear_mem(rawImage);
			rawProcessor->recycle();
		}

		// We have an image we can load in
		else if (ImageHandler::CheckImage(fileName)) {
			displayImage = new wxImage(fileName);
		}
		else {
			displayImage = NULL;
			continue;
		}

		// Image is loaded at this point!

		// Resize the image to the maximum size for library display
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
				
			if(!displayImage->IsOk()){
				delete displayImage;
				displayImage = NULL;
				continue;
			}
			// Send the image to the parent to be added to the window
			wxFileName tempFile(fileName);

			AddLibraryImageEvent libImageEvent(ADD_LIB_IMAGE_EVENT, 0, displayImage, tempFile.GetFullName(), fileName);
			wxPostEvent(par, libImageEvent);
		}
	}
	
	mutLock->Lock();
	*complete += 1;
	mutLock->Unlock();

	// All worker threads have finished, signal condition to continue
	if (*complete == threads) {
		cond->Broadcast();
	}

	delete rawProcessor;

	return (wxThread::ExitCode)0;
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
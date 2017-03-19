// Copyright 2016 Jacob Chesley

#include "LibraryWindow.h"

wxDEFINE_EVENT(POPULATION_STARTED_EVENT, wxCommandEvent);
wxDEFINE_EVENT(POPULATION_COMPLETE_EVENT, wxCommandEvent);

LibraryWindow::LibraryWindow(wxWindow * parent) : wxScrolledWindow(parent){

	populationStarted = false;
	populationCanceled = false;

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
	this->Bind(POPULATION_STARTED_EVENT, (wxObjectEventFunction)&LibraryWindow::OnPopulationStart, this);
	this->Bind(POPULATION_COMPLETE_EVENT, (wxObjectEventFunction)&LibraryWindow::OnPopulationComplete, this);
}

void LibraryWindow::OnShowDirectories(wxCommandEvent& WXUNUSED(evt)){
	PhoedixAUIManager::GetPhoedixAUIManager()->GetPane(directorySelections).Show();
	PhoedixAUIManager::GetPhoedixAUIManager()->Update();
}

void LibraryWindow::OnResize(wxSizeEvent& WXUNUSED(evt)) {
	this->FitInside();
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
		imagesLayout->Detach(libraryImages.at(i));
		libraryImages.at(i)->Destroy();
	}
	imagesLayout->Clear(true);
	libraryImages.clear();
	includedImagePaths.clear();
	imagePaths.Clear();
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
	imagePaths.Clear();

	// Add library images to keep
	for (size_t i = 0; i < libraryImagesTokeep.size(); i++) {
		libraryImages.push_back(libraryImagesTokeep.at(i));
		includedImagePaths.push_back(libraryImagesTokeep.at(i)->GetPath());
		imagePaths.Add(libraryImagesTokeep.at(i)->GetPath());
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
	imagePaths.clear();

	// Add library images to keep
	for (size_t i = 0; i < libraryImagesTokeep.size(); i++) {
		libraryImages.push_back(libraryImagesTokeep.at(i));
		includedImagePaths.push_back(libraryImagesTokeep.at(i)->GetPath());
		imagePaths.Add(libraryImagesTokeep.at(i)->GetPath());
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

void LibraryWindow::OnPopulationStart(wxSizeEvent & WXUNUSED(evt)) {
	populationStarted = true;
}

void LibraryWindow::OnPopulationComplete(wxSizeEvent & WXUNUSED(evt)) {
	populationStarted = false;
	populationCanceled = false;
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
		return;
	}
	locker.Enter();
	LibraryImage * newLibImage = new LibraryImage(this, newImage, fileName, filePath);
	newImage->Destroy();
	delete newImage;
	newImage = NULL;

	int idx = imagePaths.Add(fileName);
	imagesLayout->Insert((idx * 3), new wxPanel(this, -1, wxDefaultPosition, wxSize(25, 25)));
	imagesLayout->Insert((idx * 3) + 1, newLibImage);
	imagesLayout->Insert((idx * 3) + 2, new wxPanel(this, -1, wxDefaultPosition, wxSize(25, 25)));
	libraryImages.insert(libraryImages.begin() + idx, newLibImage);

	this->Layout();
	this->FitInside();
	locker.Leave();
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
	canceled = false;
}

void LibraryWindow::LibraryWindow::LoadImagesThread::Cancel() {
	canceled = true;
}

wxThread::ExitCode LibraryWindow::LoadImagesThread::Entry(){

	wxCommandEvent start(POPULATION_STARTED_EVENT, ID_POPULATION_STARTED);
	wxPostEvent(par, start);

	int totalThreads = PhoedixSettings::GetNumThreads();
	int maxSize = 250;

	LibRaw * rawProcs = new LibRaw[totalThreads];
	int * rawErrors = new int[totalThreads];
	libraw_processed_image_t ** rawImgs = new libraw_processed_image_t *[totalThreads];
	wxImage ** displayImage = new wxImage*[totalThreads];

	for (int i = 0; i < totalThreads; i++) {
		rawErrors[i] = LIBRAW_SUCCESS;
		rawImgs[i] = NULL;
		displayImage[i] = NULL;
	}

	// Get all directories to check
	wxVector<wxString> allDirectories = par->directorySelections->GetDirectoryList();

	// Go through all directories
	for (size_t dir = 0; dir < allDirectories.size(); dir++) {

		if (canceled) { break; }
		// Check directory, and continue if it does not exist
		if (!wxDir::Exists(allDirectories[dir])) {
			continue;
		}

		// Get all file names
		wxArrayString allFileNames;
		wxDir::GetAllFiles(allDirectories[dir], &allFileNames);

		// Go through all files, check if each are an image
		
		omp_set_dynamic(0);     // Explicitly disable dynamic teams
		omp_set_num_threads(totalThreads); // Use 4 threads for all consecutive parallel regions
		#pragma omp parallel for
		for (int file = 0; file < allFileNames.size(); file++) {

			if (canceled) { break; }

			wxString fileName = allFileNames[file];

			if (par->CheckIfImageInDisplay(fileName)) { continue; }
			
			// We have a raw image we can load in
			if (ImageHandler::CheckRaw(fileName)) {
				#ifdef __WXMSW__
					rawProcs[omp_get_thread_num()].open_file(fileName.wc_str());
				#else
					rawProcs[omp_get_thread_num()].open_file(fileName.c_str());
				#endif
				rawProcs[omp_get_thread_num()].unpack();
				rawProcs[omp_get_thread_num()].imgdata.params.half_size = 1;
				rawProcs[omp_get_thread_num()].imgdata.params.use_camera_wb = 1;
				rawProcs[omp_get_thread_num()].imgdata.params.use_auto_wb = 0;

				rawProcs[omp_get_thread_num()].dcraw_process();
				rawImgs[omp_get_thread_num()] = rawProcs[omp_get_thread_num()].dcraw_make_mem_image(&rawErrors[omp_get_thread_num()]);

				if (rawErrors[omp_get_thread_num()] == LIBRAW_SUCCESS) {

					displayImage[omp_get_thread_num()] = NULL;
					displayImage[omp_get_thread_num()] = new wxImage(rawImgs[omp_get_thread_num()]->width, rawImgs[omp_get_thread_num()]->height);
					ImageHandler::CopyImageFromRaw(rawImgs[omp_get_thread_num()], displayImage[omp_get_thread_num()]);
				}
				rawProcs[omp_get_thread_num()].dcraw_clear_mem(rawImgs[omp_get_thread_num()]);
				rawProcs[omp_get_thread_num()].recycle();
			}

			// We have an image we can load in
			else if (ImageHandler::CheckImage(fileName)) {
				displayImage[omp_get_thread_num()] = new wxImage(fileName);
			}
			else {
				continue;
			}

			if (displayImage[omp_get_thread_num()] != NULL) {
				// Scale image based on width > height
				if (displayImage[omp_get_thread_num()]->GetHeight() < displayImage[omp_get_thread_num()]->GetWidth()) {
					double aspect = (double)displayImage[omp_get_thread_num()]->GetHeight() / (double)displayImage[omp_get_thread_num()]->GetWidth();
					displayImage[omp_get_thread_num()]->Rescale(maxSize, maxSize * aspect, wxIMAGE_QUALITY_HIGH);

				}
				// Scale image based on height > width
				else if (displayImage[omp_get_thread_num()]->GetHeight() > displayImage[omp_get_thread_num()]->GetWidth()) {
					double aspect = (double)displayImage[omp_get_thread_num()]->GetWidth() / (double)displayImage[omp_get_thread_num()]->GetHeight();
					displayImage[omp_get_thread_num()]->Rescale(maxSize * aspect, maxSize, wxIMAGE_QUALITY_HIGH);
				}

				// Image has same width and height, set to max size
				else {
					displayImage[omp_get_thread_num()]->Rescale(maxSize, maxSize, wxIMAGE_QUALITY_HIGH);
				}
				
				// Send the image to the parent to be added to the window
				wxFileName tempFile(fileName);

				AddLibraryImageEvent libImageEvent(ADD_LIB_IMAGE_EVENT, 0, displayImage[omp_get_thread_num()], tempFile.GetFullName(), fileName);
				wxPostEvent(par, libImageEvent);

			}
		}
	}

	delete[] rawProcs;
	delete[] rawErrors;
	delete[] rawImgs;
	delete[] displayImage;

	wxCommandEvent finish(POPULATION_COMPLETE_EVENT, ID_POPULATION_COMPLETE);
	wxPostEvent(par, finish);

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
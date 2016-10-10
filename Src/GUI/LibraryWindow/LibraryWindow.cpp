#include "LibraryWindow.h"


LibraryWindow::LibraryWindow(wxWindow * parent) : wxScrolledWindow(parent){

	this->SetBackgroundColour(parent->GetBackgroundColour());
	
	mainLayout = new wxBoxSizer(wxVERTICAL);
	imagesLayout = new wxWrapSizer();
	this->SetSizer(mainLayout);

	toolbarLayout = new wxBoxSizer(wxHORIZONTAL);

	showDirectoriesButton = new wxButton(this, LibraryWindow::MenuBar::ID_SHOW_DIRECTORY_LIST, "Import Directories", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	showDirectoriesButton->SetForegroundColour(Colors::TextLightGrey);
	showDirectoriesButton->SetBackgroundColour(Colors::BackGrey);
	showDirectoriesButton->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	importButton = new wxButton(this, LibraryWindow::MenuBar::ID_IMPORT, "Populate Library", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	importButton->SetForegroundColour(Colors::TextLightGrey);
	importButton->SetBackgroundColour(Colors::BackGrey);
	importButton->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	toolbarLayout->Add(showDirectoriesButton);
	toolbarLayout->Add(importButton);
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
	this->Bind(wxEVT_SIZE, (wxObjectEventFunction)&LibraryWindow::OnResize, this);
	this->Bind(wxEVT_SCROLLWIN_THUMBRELEASE, (wxObjectEventFunction)&LibraryWindow::OnScroll, this);
}

void LibraryWindow::OnShowDirectories(wxCommandEvent& WXUNUSED(evt)){
	PhoedixAUIManager::GetPhoedixAUIManager()->GetPane(directorySelections).Show();
	PhoedixAUIManager::GetPhoedixAUIManager()->Update();
}

void LibraryWindow::OnResize(wxSizeEvent& evt) {
	this->SetVirtualSize(this->GetSize());
	this->FitInside();
	this->Layout();
	imagesLayout->Layout();

	evt.Skip(true);
}

void LibraryWindow::OnScroll(wxScrollEvent& evt) {
	this->FitInside();
	this->Layout();
	imagesLayout->Layout();

	evt.Skip(false);
}

void LibraryWindow::OnImport(wxCommandEvent& WXUNUSED(evt)){

	LoadImagesThread * testImgThread = new LoadImagesThread(this);
	testImgThread->Run();

}

void LibraryWindow::AddLibraryImage(LibraryImage * newLibraryImage) {
	imagesLayout->Add(newLibraryImage);
	imagesLayout->AddSpacer(50);
	libraryImages.push_back(newLibraryImage);
	this->SetVirtualSize(this->GetBestVirtualSize());
	this->FitInside();
	this->GetSizer()->Layout();
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

				rawProc.recycle();
				rawProc.open_file(fileName.wc_str());
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
				LibraryImage * newLibraryImage = new LibraryImage(par, displayImage);
				delete displayImage;
				par->AddLibraryImage(newLibraryImage);

			}
		}
	}
	return 0;
}
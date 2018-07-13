// Copyright 2016 Jacob Chesley

#include "LibraryImage.h"

wxDEFINE_EVENT(OPEN_IMAGE_NEW_PROJECT_EVENT, wxCommandEvent);
wxDEFINE_EVENT(LIBRARY_IMAGE_CHECK_EVENT, wxCommandEvent);
wxDEFINE_EVENT(ADD_LIB_IMAGE_EVENT, AddLibraryImageEvent);

LibraryImage::LibraryImage(wxWindow * parent, wxWindow * libParent, wxImage * image, wxString fileName, wxString filePath) : wxPanel(parent) {
	
	this->SetBackgroundColour(parent->GetBackgroundColour());;

	path = filePath;

	mainLayout = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(mainLayout);

	subLayout = new wxBoxSizer(wxHORIZONTAL);

	imageDisplay = new WXImagePanel(this, image, true, true);
	imageDisplay->Connect(wxEVT_LEFT_DCLICK, wxMouseEventHandler(LibraryImage::OnLeftDoubleClick), NULL, this);
	imageDisplay->Connect(wxEVT_RIGHT_DOWN, wxMouseEventHandler(LibraryImage::OnRightClick), NULL, this);

	nameDisplay = new wxTextCtrl(this, -1, fileName, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE | wxTE_READONLY);
	selectBox = new wxCheckBox(this, -1, "");

	nameDisplay->SetBackgroundColour(this->GetBackgroundColour());
	nameDisplay->SetForegroundColour(Colors::TextLightGrey);

	subLayout->Add(nameDisplay, 1, wxEXPAND |wxRIGHT);
	subLayout->Add(selectBox);

	this->GetSizer()->Add(imageDisplay);
	this->GetSizer()->Add(subLayout, 0, wxEXPAND);
	this->GetSizer()->AddSpacer(50);
	
	this->GetSizer()->Layout();

	this->Bind(wxEVT_LEFT_DCLICK, (wxMouseEventFunction)&LibraryImage::OnLeftDoubleClick, this);
	this->Bind(wxEVT_RIGHT_DOWN, (wxMouseEventFunction)&LibraryImage::OnRightClick, this);
	this->Bind(wxEVT_CHECKBOX, (wxObjectEventFunction)&LibraryImage::OnCheck, this);

	libraryParent = libParent;
}

void LibraryImage::OnLeftDoubleClick(wxMouseEvent& WXUNUSED(evt)){
	
	wxImage * displayImage = NULL;

	// We have a raw image we can load in
	if (ImageHandler::CheckRaw(path)) {

		LibRaw rawProc;

		rawProc.recycle();
		#ifdef __WXMSW__
			rawProc.open_file(path.wc_str());
		#else
			rawProc.open_file(path.c_str());
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

			ZoomImageFrame * displayFrame = new ZoomImageFrame(this, this->GetName(), displayImage);
			displayFrame->Show();
		}
		rawProc.dcraw_clear_mem(rawImg);
	}

	// We have an image we can load in
	else if (ImageHandler::CheckImage(path)) {
		displayImage = new wxImage(path);

		ZoomImageFrame * displayFrame = new ZoomImageFrame(this, this->GetName(), displayImage);
		displayFrame->Show();
	}

	//  No vailid image
	else{
		return;
	}

	if(displayImage != NULL){
		delete displayImage;
	}
}

void LibraryImage::OnRightClick(wxMouseEvent& WXUNUSED(evt)) {

	// Display a popup menu of options
	wxMenu popupMenu;
	popupMenu.Append(LibraryImage::PopupMenuActions::OPEN_IN_NEW_PROJECT, "Open Image in New Project");
	popupMenu.Append(LibraryImage::PopupMenuActions::VIEW_IMAGE_DETAILS, "View Image Details");

	popupMenu.Connect(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(LibraryImage::OnPopupMenuClick), NULL, this);
	this->PopupMenu(&popupMenu);
}

void LibraryImage::OnPopupMenuClick(wxCommandEvent& inEvt) {

	int id = inEvt.GetId();

	switch (id) {

		// Send event to main window to open image in new project
		case LibraryImage::PopupMenuActions::OPEN_IN_NEW_PROJECT: {
			wxCommandEvent evt(OPEN_IMAGE_NEW_PROJECT_EVENT, ID_OPEN_IMAGE_NEW_PROJECT);
			evt.SetString(this->GetPath());
			wxPostEvent(PhoedixAUIManager::GetMainWindow(), evt);
		}
	}
}

void LibraryImage::OnCheck(wxCommandEvent& WXUNUSED(evt)) {
	wxCommandEvent evt(LIBRARY_IMAGE_CHECK_EVENT, ID_LIB_IMAGE_CHECK_EVT);
	wxPostEvent(libraryParent, evt);
}

void LibraryImage::ChangeImage(wxImage * newImage) {
	img = newImage;
}

wxImage * LibraryImage::GetImage() {
	return img;
}

void LibraryImage::SetPath(wxString inPath) {
	path = inPath;
}

wxString LibraryImage::GetPath() {
	return path;
}

void LibraryImage::SetName(wxString name) {
	nameDisplay->SetLabelText(name);
}

wxString LibraryImage::GetName() {
	return nameDisplay->GetLabelText();

}

bool LibraryImage::GetSelected() {
	return selectBox->GetValue();
}

AddLibraryImageEvent::AddLibraryImageEvent(wxEventType eventType, int winid, wxImage * img, wxString fileName, wxString filePath) : wxEvent(winid, eventType){
	libImage = img;
	name = fileName;
	path = filePath;
}

wxEvent * AddLibraryImageEvent::Clone() const {
	 return new AddLibraryImageEvent(*this);
}

wxImage * AddLibraryImageEvent::GetImage() const {
	return libImage;
}

wxString AddLibraryImageEvent::GetFileName() const {
	return name;
}

wxString AddLibraryImageEvent::GetFilePath() const {
	return path;
}

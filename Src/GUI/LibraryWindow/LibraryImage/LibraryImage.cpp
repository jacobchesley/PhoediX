#include "LibraryImage.h"

LibraryImage::LibraryImage(wxWindow * parent, wxImage * image, wxString fileName, wxString filePath) : wxPanel(parent) {
	
	this->SetBackgroundColour(Colors::BackDarkDarkGrey);;

	path = filePath;

	mainLayout = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(mainLayout);

	subLayout = new wxBoxSizer(wxHORIZONTAL);

	imageDisplay = new WXImagePanel(this, image, true, true);
	imageDisplay->Connect(wxEVT_LEFT_DCLICK, wxMouseEventHandler(LibraryImage::OnLeftDoubleClick), NULL, this);

	nameDisplay = new wxStaticText(this, -1, fileName);
	selectBox = new wxCheckBox(this, -1, "");

	nameDisplay->SetForegroundColour(Colors::TextLightGrey);

	subLayout->Add(nameDisplay);
	subLayout->AddStretchSpacer();
	subLayout->Add(selectBox);

	this->GetSizer()->Add(imageDisplay);
	this->GetSizer()->Add(subLayout, 0, wxEXPAND);
	this->GetSizer()->AddSpacer(50);
	
	this->GetSizer()->Layout();

	this->Bind(wxEVT_LEFT_DCLICK, (wxMouseEventFunction)&LibraryImage::OnLeftDoubleClick, this);
}

void LibraryImage::OnLeftDoubleClick(wxMouseEvent& WXUNUSED(evt)){
	
	wxImage * displayImage = NULL;

	// We have a raw image we can load in
	if (ImageHandler::CheckRaw(path)) {

		LibRaw rawProc;

		rawProc.recycle();
		rawProc.open_file(path.wc_str());
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

wxDEFINE_EVENT(ADD_LIB_IMAGE_EVENT, AddLibraryImageEvent);
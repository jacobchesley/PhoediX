#include "LibraryImage.h"

LibraryImage::LibraryImage(wxWindow * parent, wxImage * image) : wxPanel(parent) {
	mainLayout = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(mainLayout);

	subLayout = new wxBoxSizer(wxHORIZONTAL);

	imageDisplay = new WXImagePanel(this, image, true, true);
	nameDisplay = new wxStaticText(this, -1, "");
	selectBox = new wxCheckBox(this, -1, "");

	subLayout->Add(nameDisplay);
	subLayout->Add(selectBox);
	this->GetSizer()->Add(imageDisplay);
	this->GetSizer()->Add(subLayout);
	this->GetSizer()->AddSpacer(50);

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
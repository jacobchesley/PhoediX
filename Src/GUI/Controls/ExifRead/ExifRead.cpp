#include "ExifRead.h"

ExifRead::ExifRead(wxWindow * parent) : wxPanel(parent) {

	this->SetBackgroundColour(parent->GetBackgroundColour());
	sizer = new wxGridSizer(2);
	this->SetSizer(sizer);
}

void ExifRead::AddExifData(Image * image) {
	this->AddExifData(*image->GetExifMap());
}

void ExifRead::AddExifData(std::map<size_t, void*> exifData) {

	// Add all exif data
	for (std::map<size_t, void*>::const_iterator it = exifData.begin(); it != exifData.end(); it++) {
		this->AddExifRow(it->first, it->second);
	}
}

void ExifRead::AddExifRow(size_t tag, void * data) {

	wxString labelString = Image::exifTags[tag];
	wxString valueString = "";

	// Convert data formats to strings
	int format = Image::exifFormats[tag];

	// short/int/Long to string
	if (format == 1 || format == 3 || format == 4 ||
		format == 6 || format == 8 || format == 9) {
		valueString << (long)data;
	}

	// Unsigned Rational to string
	else if (format == 5) {
		UnsignedRational * uRational = (UnsignedRational*)data;
		valueString << uRational->numerator;
		valueString += "/";
		valueString << uRational->denominator;
	}

	// Signed Rational to string
	else if (format == 10) {
		SignedRational * rational = (SignedRational*)data;
		valueString << rational->numerator;
		valueString += "/";
		valueString << rational->denominator;
	}

	// String to string
	else if (format == 2) {
		valueString = *(wxString*)data;
	}

	// Undefined
	else{}

	// Create text labels
	wxStaticText * label = new wxStaticText(this, -1, labelString);
	wxTextCtrl * value = new wxTextCtrl(this, -1, valueString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
	label->SetForegroundColour(Colors::TextLightGrey);
	value->SetForegroundColour(Colors::TextLightGrey);
	value->SetBackgroundColour(this->GetBackgroundColour());

	labels.push_back(label);
	values.push_back(value);
	formats.push_back(format);

	sizer->Add(label);
	sizer->Add(value);
}

ExifReadFrame::ExifReadFrame(wxWindow * parent, wxString name, Image * image) : wxFrame(parent, -1, name) {

	Icons icons;
	wxIcon theIcon;
	theIcon.CopyFromBitmap(wxBitmap(icons.pxIcon));
	this->SetIcon(theIcon);

	this->SetBackgroundColour(parent->GetBackgroundColour());
	mainSizer = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(mainSizer);

	scrollWin = new wxScrolledWindow(this);
	scrollSizer = new wxBoxSizer(wxVERTICAL);
	scrollWin->SetSizer(scrollSizer);
	scrollWin->SetBackgroundColour(this->GetBackgroundColour());

	exifTable = new ExifRead(scrollWin);
	exifTable->AddExifData(image);
	scrollWin->GetSizer()->Add(exifTable, 0, wxEXPAND);
	scrollWin->SetVirtualSize(exifTable->GetSize());
	scrollWin->SetScrollbars(1, 1, exifTable->GetSize().GetWidth(), exifTable->GetSize().GetHeight());

	this->GetSizer()->Add(scrollWin, 1, wxEXPAND);
	this->GetSizer()->Layout();
}
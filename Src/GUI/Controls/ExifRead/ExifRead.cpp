// Copyright 2018 Jacob Chesley.
// See LICENSE.TXT in root of project for license information.

#include "ExifRead.h"

ExifRead::ExifRead(wxWindow * parent) : wxPanel(parent) {

	this->SetBackgroundColour(parent->GetBackgroundColour());
	sizer = new wxGridSizer(2);
	this->SetSizer(sizer);

	this->AddNoExifMessage();
}

void ExifRead::AddExifData(Image * image) {

	sizer->Clear(true);
	this->AddExifData(*image->GetExifMap());

	if (labels.size() == 0) { this->AddNoExifMessage(); }
}

void ExifRead::AddExifData(std::map<size_t, void*> exifData) {

	sizer->Clear(true);
	// Add all exif data
	for (std::map<size_t, void*>::const_iterator it = exifData.begin(); it != exifData.end(); it++) {
		this->AddExifRow(it->first, it->second);
	}

	if (labels.size() == 0) { this->AddNoExifMessage(); }
}

void ExifRead::AddExifRow(size_t tag, void * data) {

	if (data == NULL) { return; }
	wxString labelString = Image::exifTags[tag];
	wxString valueString = "";

	// Convert data formats to strings
	int format = Image::exifFormats[tag];

	// short/int/Long to string
	if (format == 1 || format == 3 || format == 4 ||
		format == 6 || format == 8 || format == 9) {
		valueString << *(int*)data;
		delete data;
		data = NULL;
	}

	// Unsigned Rational to string
	else if (format == 5 && !Image::exifIsGPSCoordinate(tag)) {
		UnsignedRational * uRational = (UnsignedRational*)data;
		valueString << uRational->numerator;
		valueString += "/";
		valueString << uRational->denominator;

		delete uRational;
		data = NULL;
	}

	// GPS Coordinate to string
	else if (format == 5 && Image::exifIsGPSCoordinate(tag)) {

		wxVector<UnsignedRational*>* gpsCoordList = (wxVector<UnsignedRational*>*)data;
		for (size_t i = 0; i < gpsCoordList->size(); i++) {

			UnsignedRational * uRational = gpsCoordList->at(i);
			if (uRational->denominator != 0) {
				double valueDouble = (double)uRational->numerator / (double)uRational->denominator;
				wxString valStr = "";
				valStr << valueDouble;
				valueString += valStr;
				if (i < gpsCoordList->size() - 1) {
					valueString += ",";
				}
			}
			delete uRational;
			uRational = NULL;
		}
		gpsCoordList->clear();
		delete gpsCoordList;
		gpsCoordList = NULL;
	}

	// Signed Rational to string
	else if (format == 10) {
		SignedRational * rational = (SignedRational*)data;
		valueString << rational->numerator;
		valueString += "/";
		valueString << rational->denominator;

		delete rational;
		rational = NULL;
	}

	// String to string
	else if (format == 2) {
		valueString = *(wxString*)data;
		delete (wxString*)data;
		data = NULL;
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
	tags.push_back(tag);

	sizer->Add(label);
	sizer->Add(value);
}

void ExifRead::AddNoExifMessage() {

	noExifMessage = new wxStaticText(this, -1, "No EXIF or RAW information available.");
	noExifMessage->SetForegroundColour(Colors::TextLightGrey);
	noExifMessage->SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
	sizer->Add(noExifMessage);
}

void ExifRead::ClearExif(){
	sizer->Clear(true);
	labels.clear();
	values.clear();
	tags.clear();
	this->AddNoExifMessage();
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

ExifReadWindow::ExifReadWindow(wxWindow * parent) : wxWindow(parent, -1) {

	Icons icons;
	wxIcon theIcon;
	theIcon.CopyFromBitmap(wxBitmap(icons.pxIcon));

	this->SetBackgroundColour(parent->GetBackgroundColour());
	mainSizer = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(mainSizer);

	scrollWin = new wxScrolledWindow(this);
	scrollSizer = new wxBoxSizer(wxVERTICAL);
	scrollWin->SetSizer(scrollSizer);
	scrollWin->SetBackgroundColour(this->GetBackgroundColour());

	exifTable = new ExifRead(scrollWin);
	scrollWin->GetSizer()->Add(exifTable, 0, wxEXPAND);
	scrollWin->SetVirtualSize(exifTable->GetSize());
	scrollWin->SetScrollbars(1, 1, exifTable->GetSize().GetWidth(), exifTable->GetSize().GetHeight());

	this->GetSizer()->Add(scrollWin, 1, wxEXPAND);
	this->GetSizer()->Layout();
}

void ExifReadWindow::ClearExif(){
	exifTable->ClearExif();
}

void ExifReadWindow::AddExif(Image * img){
	exifTable->AddExifData(img);
}
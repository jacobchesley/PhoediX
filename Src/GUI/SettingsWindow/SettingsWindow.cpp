// Copyright 2016 Jacob Chesley

#include "SettingsWindow.h"

wxDEFINE_EVENT(RELOAD_IMAGE_EVENT, wxCommandEvent);

SettingsWindow::SettingsWindow(wxWindow * parent, Processor * processor, EditListPanel * editLst) : wxScrolledWindow(parent) {

	this->SetBackgroundColour(parent->GetBackgroundColour());

	mainSizer = new wxBoxSizer(wxVERTICAL);

	// 2 Columns, 15 pixel vertical gap, 15 pixel horizontal gap
	gridSizer = new wxFlexGridSizer(2, 15, 15);

	settingsLabel = new wxStaticText(this, -1, "PhoediX Settings");
	settingsLabel->SetForegroundColour(Colors::TextWhite);
	settingsLabel->SetFont(wxFont(13, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));

	// Color Bit Depht Setting
	colorDepthLabel = new wxStaticText(this, -1, "Color Bit Depth");
	colorDepthLabel->SetForegroundColour(Colors::TextLightGrey);
	colorDepth = new wxComboBox(this, -1);
	colorDepth->SetBackgroundColour(Colors::BackDarkDarkGrey);
	colorDepth->SetForegroundColour(Colors::TextLightGrey);
	colorDepth->Append("8 Bit");
	colorDepth->Append("16 Bit");

	if(processor->GetImage()->GetColorDepth() == 8){ colorDepth->SetSelection(0); }
	else{ colorDepth->SetSelection(1); }
	lastColorDepth = colorDepth->GetSelection();

	// Color Space Setting
	colorSpaceLabel = new wxStaticText(this, -1, "Color Space");
	colorSpaceLabel->SetForegroundColour(Colors::TextLightGrey);
	colorSpace = new wxComboBox(this, -1);
	colorSpace->SetBackgroundColour(Colors::BackDarkDarkGrey);
	colorSpace->SetForegroundColour(Colors::TextLightGrey);
	colorSpace->Append("sRGB");
	colorSpace->Append("Adobe RGB");
	colorSpace->Append("Wide Gamut RGB");
	colorSpace->Append("Pro Photo RGB");
	colorSpace->SetSelection(0);

	// Number of Threads Setting
	numThreadsLabel = new wxStaticText(this, -1, "Number of Threads");
	numThreadsLabel->SetForegroundColour(Colors::TextLightGrey);
	int maxThreads = wxThread::GetCPUCount();
	numThreads = new wxSlider(this, -1, maxThreads, 1, maxThreads, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL | wxSL_LABELS);
	numThreads->SetForegroundColour(Colors::TextLightGrey);
	
	buttonSizer = new wxBoxSizer(wxHORIZONTAL);
	applySettingsButton = new wxButton(this, SettingsWindow::ID_APPLY_SETTINGS, "Apply Settings", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	applySettingsButton->SetForegroundColour(Colors::TextLightGrey);
	applySettingsButton->SetBackgroundColour(Colors::BackGrey);
	applySettingsButton->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	cancelButton = new wxButton(this, SettingsWindow::ID_CANCEL, "Cancel", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	cancelButton->SetForegroundColour(Colors::TextGrey);
	cancelButton->SetBackgroundColour(Colors::BackGrey);
	cancelButton->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	gridSizer->Add(colorDepthLabel);
	gridSizer->Add(colorDepth);
	gridSizer->Add(colorSpaceLabel);
	gridSizer->Add(colorSpace);
	gridSizer->Add(numThreadsLabel);
	gridSizer->Add(numThreads);

	buttonSizer->Add(applySettingsButton);
	buttonSizer->AddSpacer(20);
	buttonSizer->Add(cancelButton);

	mainSizer->Add(settingsLabel);
	mainSizer->AddSpacer(10);
	mainSizer->Add(gridSizer);
	mainSizer->AddSpacer(15);
	mainSizer->Add(buttonSizer, 0, wxALIGN_RIGHT);

	this->SetSizer(mainSizer);
	this->FitInside();
	this->SetScrollRate(5, 5);

	this->SetClientSize(this->GetVirtualSize());

	proc = processor;
	editList = editLst;
	parWindow = parent;
	blankMessageTimer = new wxTimer(this);

	this->Bind(wxEVT_BUTTON, (wxObjectEventFunction)&SettingsWindow::OnApply, this, SettingsWindow::ID_APPLY_SETTINGS);
	this->Bind(wxEVT_BUTTON, (wxObjectEventFunction)&SettingsWindow::OnCancel, this, SettingsWindow::ID_CANCEL);
	this->Bind(wxEVT_TIMER, (wxObjectEventFunction)&SettingsWindow::SendBlankMessageTimer, this);
}

void SettingsWindow::OnApply(wxCommandEvent& WXUNUSED(evt)) {
	this->ApplySettings(true);
}

void SettingsWindow::ApplySettings(bool ShowMessage){

	if (colorDepth->GetSelection() == 0 && lastColorDepth != colorDepth->GetSelection()) {
		proc->GetOriginalImage()->Disable16Bit();
		proc->GetImage()->Disable16Bit();
		lastColorDepth = colorDepth->GetSelection();
		PhoedixSettings::SetBitDepth(8);

		wxCommandEvent reloadEvent(RELOAD_IMAGE_EVENT, ID_RELOAD_IMAGE);
		wxPostEvent(parWindow, reloadEvent);

	}
	else if (colorDepth->GetSelection() == 1 && lastColorDepth != colorDepth->GetSelection()) {
		proc->GetOriginalImage()->Enable16Bit();
		proc->GetImage()->Enable16Bit();
		lastColorDepth = colorDepth->GetSelection();
		PhoedixSettings::SetBitDepth(16);

		wxCommandEvent reloadEvent(RELOAD_IMAGE_EVENT, ID_RELOAD_IMAGE);
		wxPostEvent(parWindow, reloadEvent);
	}
	else{}

	if (numThreads->GetValue() > 1) {
		proc->SetMultithread(true);
		proc->SetNumThreads(numThreads->GetValue());
		PhoedixSettings::SetNumThreads(numThreads->GetValue());
	}
	else {
		proc->SetMultithread(false);
		proc->SetNumThreads(1);
		PhoedixSettings::SetNumThreads(1);
	}

	// Set Color Space
	if (colorSpace->GetSelection() == 0) { proc->SetColorSpace(ColorSpaceENUM::sRGB); PhoedixSettings::SetColorSpace(ColorSpaceENUM::sRGB); }
	if (colorSpace->GetSelection() == 1) { proc->SetColorSpace(ColorSpaceENUM::ADOBE_RGB); PhoedixSettings::SetColorSpace(ColorSpaceENUM::ADOBE_RGB); }
	if (colorSpace->GetSelection() == 2) { proc->SetColorSpace(ColorSpaceENUM::WIDE_GAMUT_RGB); PhoedixSettings::SetColorSpace(ColorSpaceENUM::WIDE_GAMUT_RGB); }
	if (colorSpace->GetSelection() == 3) { proc->SetColorSpace(ColorSpaceENUM::PROPHOTO_RGB); PhoedixSettings::SetColorSpace(ColorSpaceENUM::PROPHOTO_RGB); }

	lastSettings.bitDepth = colorDepth->GetSelection();
	lastSettings.colorSpace = colorSpace->GetSelection();
	lastSettings.numThreads = numThreads->GetValue();

	this->WriteSettings();
	editList->ReprocessImageRaw();

	// Send applied settings message to parent to display in info bar
	if (ShowMessage) {
		wxCommandEvent evt(PROCESSOR_MESSAGE_EVENT, ID_PROCESSOR_MESSAGE);
		evt.SetString("Settings Applied");
		wxPostEvent(parWindow, evt);
		blankMessageTimer->Start(1000, true);
	}
}

void SettingsWindow::ReadSettings() {

	wxString exectuablePath = wxStandardPaths::Get().GetExecutablePath();
	wxFileName directory = wxFileName(exectuablePath);
	wxString executableDir = directory.GetPath();
	wxString settingsFilePath = executableDir + wxFileName::GetPathSeparator() + "settings.ini";

	// if settings file does not exist, write settings
	if (!wxFile::Exists(settingsFilePath)) {
		this->WriteSettings();
	}

	wxTextFile settingsFile(settingsFilePath);
	
	if (settingsFile.Open()) {
		for (size_t i = 0; i < settingsFile.GetLineCount(); i++) {

			// Get line in settings file
			wxString setting = settingsFile.GetLine(i);

			// Split string at = sign
			wxStringTokenizer tokenizer(setting, "=");

			// 2 tokens, key and value
			if (tokenizer.CountTokens() == 2) {

				wxString key = tokenizer.GetNextToken();
				wxString value = tokenizer.GetNextToken();

				if (key == "BIT_DEPTH") {
					if (value == "8") { colorDepth->SetSelection(0); }
					if (value == "16") { colorDepth->SetSelection(1); }
				}

				if (key == "COLOR_SPACE") {
					if (value == "sRGB") { colorSpace->SetSelection(0); }
					if (value == "ADOBE_RGB") { colorSpace->SetSelection(1); }
					if (value == "WIDE_GAMUT_RGB") { colorSpace->SetSelection(2); }
					if (value == "PRO_PHOTO_RGB") { colorSpace->SetSelection(3); }
				}

				if (key == "NUM_THREADS") {

					int numThreadVal = wxAtoi(value);
					int maxThreads = wxThread::GetCPUCount();
					if (numThreadVal < 1) { numThreadVal = 1; }
					if (numThreadVal > maxThreads) { numThreadVal = maxThreads; }
					numThreads->SetValue(numThreadVal);
				}
			}
		}
	}
	this->ApplySettings(false);
}

void SettingsWindow::WriteSettings() {

	wxString exectuablePath = wxStandardPaths::Get().GetExecutablePath();
	wxFileName directory = wxFileName(exectuablePath);
	wxString executableDir = directory.GetPath();
	wxString settingsFilePath = executableDir + wxFileName::GetPathSeparator() + "settings.ini";

	wxTextFile settingsFile(settingsFilePath);

	// Settings file already exists, overwrite
	if (wxFile::Exists(settingsFilePath)) {
		settingsFile.Clear();
		this->WriteLines(&settingsFile);
	}

	// Create new settings file
	else {
		settingsFile.Create(settingsFilePath);
		this->WriteLines(&settingsFile);
	}
}

void SettingsWindow::WriteLines(wxTextFile * file) {

	// Color Bit Depth
	wxString bitDepthSettingsStr = "BIT_DEPTH=";
	if (colorDepth->GetSelection() == 0) { bitDepthSettingsStr += "8"; }
	if (colorDepth->GetSelection() == 1) { bitDepthSettingsStr += "16"; }
	file->AddLine(bitDepthSettingsStr);

	// Color Space
	wxString colorSpaceSettingsStr = "COLOR_SPACE=";
	if (colorSpace->GetSelection() == 0) { colorSpaceSettingsStr += "sRGB"; }
	if (colorSpace->GetSelection() == 1) { colorSpaceSettingsStr += "ADOBE_RGB"; }
	if (colorSpace->GetSelection() == 2) { colorSpaceSettingsStr += "WIDE_GAMUT_RGB"; }
	if (colorSpace->GetSelection() == 3) { colorSpaceSettingsStr += "PRO_PHOTO_RGB"; }
	file->AddLine(colorSpaceSettingsStr);

	// Number of Threads
	wxString numThreadsSettingsStr = "NUM_THREADS=";
	numThreadsSettingsStr += wxString::Format(wxT("%i"), numThreads->GetValue());
	file->AddLine(numThreadsSettingsStr);

	file->Write();
}

void SettingsWindow::SendBlankMessageTimer(wxTimerEvent& WXUNUSED(event)) {
	wxCommandEvent evt(PROCESSOR_MESSAGE_EVENT, ID_PROCESSOR_MESSAGE);
	evt.SetString("");
	wxPostEvent(parWindow, evt);
}

void SettingsWindow::OnCancel(wxCommandEvent& WXUNUSED(evt)) {

	colorDepth->SetSelection(lastSettings.bitDepth);
	colorSpace->SetSelection(lastSettings.colorSpace);
	numThreads->SetValue(lastSettings.numThreads);
}
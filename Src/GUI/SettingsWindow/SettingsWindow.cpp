// Copyright 2018 Jacob Chesley.
// See LICENSE.TXT in root of project for license information.

#include "SettingsWindow.h"

wxDEFINE_EVENT(RELOAD_IMAGE_EVENT, wxCommandEvent);

SettingsWindow::SettingsWindow(wxWindow * parent, Processor * processor, EditListPanel * editLst) : wxFrame(parent, -1, "PhoediX Settings") {

	this->SetWindowStyle(wxDEFAULT_FRAME_STYLE | wxSTAY_ON_TOP);
	this->CenterOnParent();

	Icons icons;
	wxIcon theIcon;
	theIcon.CopyFromBitmap(wxBitmap(icons.pxIcon));
	this->SetIcon(theIcon);

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
	colorDepth = new PhoediXComboBox(this, -1);
	colorDepth->SetBackgroundColour(Colors::BackDarkDarkGrey);
	colorDepth->SetForegroundColour(Colors::TextLightGrey);
	colorDepth->Append("8 Bit");
	colorDepth->Append("16 Bit");
	colorDepth->SetSelection(1);

	if(processor->GetImage()->GetColorDepth() == 8){ colorDepth->SetSelection(0); }
	else{ colorDepth->SetSelection(1); }
	lastColorDepth = colorDepth->GetSelection();

	// Color Space Setting
	colorSpaceLabel = new wxStaticText(this, -1, "Color Space");
	colorSpaceLabel->SetForegroundColour(Colors::TextLightGrey);
	colorSpace = new PhoediXComboBox(this, -1);
	colorSpace->SetBackgroundColour(Colors::BackDarkDarkGrey);
	colorSpace->SetForegroundColour(Colors::TextLightGrey);
	colorSpace->Append("sRGB");
	colorSpace->Append("Adobe RGB");
	colorSpace->Append("Wide Gamut RGB");
	colorSpace->Append("Pro Photo RGB");
	colorSpace->SetSelection(0);

	libraryImagePreviewLabel = new wxStaticText(this, -1, "Library Image Preview");
	libraryImagePreviewLabel->SetForegroundColour(Colors::TextLightGrey);
	libraryImagePreview = new PhoediXComboBox(this, -1);
	libraryImagePreview->SetBackgroundColour(Colors::BackDarkDarkGrey);
	libraryImagePreview->SetForegroundColour(Colors::TextLightGrey);
	libraryImagePreview->Append("Use Embedded Thumbnail");
	libraryImagePreview->Append("Use RAW Image");
	libraryImagePreview->SetSelection(0);

	// Number of Threads Setting
	numThreadsLabel = new wxStaticText(this, -1, "Number of Threads");
	numThreadsLabel->SetForegroundColour(Colors::TextLightGrey);
	int maxThreads = wxThread::GetCPUCount();
	numThreads = new DoubleSlider(this, (double)maxThreads, 1.0, (double)maxThreads, (double)maxThreads - 1.0, 0);
	numThreads->SetForegroundColour(Colors::TextLightGrey);
	numThreads->SetBackgroundColour(this->GetBackgroundColour());

	buttonSizer = new wxBoxSizer(wxHORIZONTAL);

	okSettingsButton = new PhoediXButton(this, SettingsWindow::ID_OK_SETTINGS, "OK");
	okSettingsButton->SetForegroundColour(Colors::TextLightGrey);
	okSettingsButton->SetBackgroundColour(Colors::BackGrey);
	okSettingsButton->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	cancelButton = new PhoediXButton(this, SettingsWindow::ID_CANCEL, "Cancel");
	cancelButton->SetForegroundColour(Colors::TextLightGrey);
	cancelButton->SetBackgroundColour(Colors::BackGrey);
	cancelButton->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	applySettingsButton = new PhoediXButton(this, SettingsWindow::ID_APPLY_SETTINGS, "Apply Settings");
	applySettingsButton->SetForegroundColour(Colors::TextLightGrey);
	applySettingsButton->SetBackgroundColour(Colors::BackGrey);
	applySettingsButton->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
	
	gridSizer->Add(colorDepthLabel);
	gridSizer->Add(colorDepth);
	gridSizer->Add(colorSpaceLabel);
	gridSizer->Add(colorSpace);
	gridSizer->Add(libraryImagePreviewLabel);
	gridSizer->Add(libraryImagePreview);
	gridSizer->Add(numThreadsLabel);
	gridSizer->Add(numThreads);
	
	buttonSizer->Add(okSettingsButton);
	buttonSizer->AddSpacer(20);
	buttonSizer->Add(cancelButton);
	buttonSizer->AddSpacer(20);
	buttonSizer->Add(applySettingsButton);

	mainSizer->Add(settingsLabel);
	mainSizer->AddSpacer(10);
	mainSizer->Add(gridSizer);
	mainSizer->AddSpacer(15);
	mainSizer->Add(buttonSizer, 0, wxALIGN_RIGHT);

	this->SetSizer(mainSizer);
	this->FitInside();

	this->SetClientSize(this->GetVirtualSize());

	proc = processor;
	editList = editLst;
	parWindow = parent;
	blankMessageTimer = new wxTimer(this);

	this->Bind(wxEVT_BUTTON, (wxObjectEventFunction)&SettingsWindow::OnApply, this, SettingsWindow::ID_APPLY_SETTINGS);
	this->Bind(wxEVT_BUTTON, (wxObjectEventFunction)&SettingsWindow::OnOkay, this, SettingsWindow::ID_OK_SETTINGS);
	this->Bind(wxEVT_BUTTON, (wxObjectEventFunction)&SettingsWindow::OnCancel, this, SettingsWindow::ID_CANCEL);
	this->Bind(wxEVT_CLOSE_WINDOW, (wxObjectEventFunction)&SettingsWindow::OnClose, this);
	this->Bind(wxEVT_TIMER, (wxObjectEventFunction)&SettingsWindow::SendBlankMessageTimer, this);
}

void SettingsWindow::OnApply(wxCommandEvent& WXUNUSED(evt)) {
	this->ApplySettings(true, false);
}

void SettingsWindow::OnOkay(wxCommandEvent& WXUNUSED(evt)) {
	this->ApplySettings(true, true);
	this->Hide();
}

void SettingsWindow::ApplySettings(bool ShowMessage, bool overwriteLast){

	if (colorDepth->GetSelection() == 0 && lastColorDepth != colorDepth->GetSelection()) {
		proc->GetOriginalImage()->Disable16Bit();
		proc->GetImage()->Disable16Bit();
		lastColorDepth = colorDepth->GetSelection();
		PhoedixSettings::SetBitDepth(8);

	}
	else if (colorDepth->GetSelection() == 1 && lastColorDepth != colorDepth->GetSelection()) {
		proc->GetOriginalImage()->Enable16Bit();
		proc->GetImage()->Enable16Bit();
		lastColorDepth = colorDepth->GetSelection();
		PhoedixSettings::SetBitDepth(16);
	}
	else{}

	if (numThreads->GetValue() > 1.0) {
		proc->SetMultithread(true);
		proc->SetNumThreads((int)numThreads->GetValue());
		PhoedixSettings::SetNumThreads((int)numThreads->GetValue());
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

	// Library Preview
	if (libraryImagePreview->GetSelection() == 0) { PhoedixSettings::SetLibraryImageUseRaw(false); }
	else{ PhoedixSettings::SetLibraryImageUseRaw(true); }

	if (overwriteLast) {
		lastSettings.bitDepth = colorDepth->GetSelection();
		lastSettings.colorSpace = colorSpace->GetSelection();
		lastSettings.numThreads = numThreads->GetValue();
		lastSettings.libraryPreview = libraryImagePreview->GetSelection();
		this->WriteSettings();
	}

	this->ReprocessIfNeeded();

	// Send applied settings message to parent to display in info bar
	if (ShowMessage) {

		wxCommandEvent evt(PROCESSOR_MESSAGE_EVENT, ID_PROCESSOR_MESSAGE);
		if(overwriteLast){ evt.SetString("Settings Saved"); }
		else { evt.SetString("Settings Applied"); }
		wxPostEvent(parWindow, evt);
		blankMessageTimer->Start(2000, true);
	}
}

void SettingsWindow::ReadSettings() {

	wxString settingsFilePath = this->GetSettingsFile();

	// if settings file does not exist, write settings
	if (!wxFile::Exists(settingsFilePath)) {
		colorDepth->SetSelection(1);
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
					numThreads->SetValue((double)numThreadVal);
				}

				if (key == "LIBRARY_PREVIEW") {
					if (value == "USE_EMBEDDED_THUMBNAIL") { libraryImagePreview->SetSelection(0); }
					if (value == "USE_RAW_IMAGE") { libraryImagePreview->SetSelection(1); }
				}
			}
		}
	}
	this->ApplySettings(false);
}

void SettingsWindow::WriteSettings() {

	wxString settingsFilePath = this->GetSettingsFile();

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
	settingsFile.Close();
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

	// Library Preview
	wxString libraryPreviewSettingsStr = "LIBRARY_PREVIEW=";
	if (libraryImagePreview->GetSelection() == 0) { libraryPreviewSettingsStr += "USE_EMBEDDED_THUMBNAIL"; }
	if (libraryImagePreview->GetSelection() == 1) { libraryPreviewSettingsStr += "USE_RAW_IMAGE"; }
	file->AddLine(libraryPreviewSettingsStr);

	// Number of Threads
	wxString numThreadsSettingsStr = "NUM_THREADS=";
	numThreadsSettingsStr += wxString::Format(wxT("%i"),(int) numThreads->GetValue());
	file->AddLine(numThreadsSettingsStr);

	file->Write();
}

wxString SettingsWindow::GetSettingsFile() {

	#if defined(__WXGTK__)
		wxString appDir = ".PhoediX";
	#else
		wxString appDir = "PhoediX";
	#endif


	wxString configDirectory = wxStandardPaths::Get().GetUserConfigDir() + wxFileName::GetPathSeparator() + appDir;
	wxString settingsFilePath = configDirectory + wxFileName::GetPathSeparator() + "settings.ini";

	if (!wxDir::Exists(configDirectory)) {
		wxMkDir(configDirectory, wxS_DIR_DEFAULT);
	}
	return settingsFilePath;
}

void SettingsWindow::SendBlankMessageTimer(wxTimerEvent& WXUNUSED(event)) {
	wxCommandEvent evt(PROCESSOR_MESSAGE_EVENT, ID_PROCESSOR_MESSAGE);
	evt.SetString("");
	wxPostEvent(parWindow, evt);
}

void SettingsWindow::SendMessageToParent(wxString message, int displayTime) {
	wxCommandEvent evt(PROCESSOR_MESSAGE_EVENT, ID_PROCESSOR_MESSAGE);
	evt.SetString(message);
	wxPostEvent(parWindow, evt);
	blankMessageTimer->Start(displayTime, true);
}

void SettingsWindow::ReprocessIfNeeded() {

	wxCommandEvent reloadEvent(RELOAD_IMAGE_EVENT, ID_RELOAD_IMAGE);
	wxPostEvent(parWindow, reloadEvent);

}

void SettingsWindow::OnCancel(wxCommandEvent& WXUNUSED(evt)) {

	this->Hide();
	this->SendMessageToParent("Settings Reverted", 2000);
	this->ReprocessIfNeeded();

	colorDepth->SetSelection(lastSettings.bitDepth);
	colorSpace->SetSelection(lastSettings.colorSpace);
	libraryImagePreview->SetSelection(lastSettings.libraryPreview);
	numThreads->SetValue(lastSettings.numThreads);
}

void SettingsWindow::Cleanup() {
	blankMessageTimer->Stop();
	delete blankMessageTimer;
}

void SettingsWindow::OnClose(wxCloseEvent& WXUNUSED(evt)) {

	this->Hide();
	this->SendMessageToParent("Settings Reverted", 2000);
	this->ReprocessIfNeeded();

	colorDepth->SetSelection(lastSettings.bitDepth);
	colorSpace->SetSelection(lastSettings.colorSpace);
	libraryImagePreview->SetSelection(lastSettings.libraryPreview);
	numThreads->SetValue(lastSettings.numThreads);
	
}
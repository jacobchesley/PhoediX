#include "SettingsWindow.h"

wxDEFINE_EVENT(RELOAD_IMAGE_EVENT, wxCommandEvent);

SettingsWindow::SettingsWindow(wxWindow * parent, Processor * processor) : wxScrolledWindow(parent) {

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

	if(processor->GetImage()->GetColorDepth() == 8){
		colorDepth->SetSelection(0);
		lastColorDepth = colorDepth->GetSelection();
	}
	else{
		colorDepth->SetSelection(1);
		lastColorDepth = colorDepth->GetSelection();
	}

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
	parWindow = parent;

	this->Bind(wxEVT_BUTTON, (wxObjectEventFunction)&SettingsWindow::OnApply, this, SettingsWindow::ID_APPLY_SETTINGS);
}

void SettingsWindow::OnApply(wxCommandEvent& WXUNUSED(evt)) {
	this->ApplySettings();
}

void SettingsWindow::ApplySettings(){

	if (colorDepth->GetSelection() == 0 && lastColorDepth != colorDepth->GetSelection()) {
		proc->GetOriginalImage()->Disable16Bit();
		proc->GetImage()->Disable16Bit();
		lastColorDepth = colorDepth->GetSelection();

		wxCommandEvent reloadEvent(RELOAD_IMAGE_EVENT, ID_RELOAD_IMAGE);
		wxPostEvent(parWindow, reloadEvent);

	}
	else if (colorDepth->GetSelection() == 1 && lastColorDepth != colorDepth->GetSelection()) {
		proc->GetOriginalImage()->Enable16Bit();
		proc->GetImage()->Enable16Bit();
		lastColorDepth = colorDepth->GetSelection();

		wxCommandEvent reloadEvent(RELOAD_IMAGE_EVENT, ID_RELOAD_IMAGE);
		wxPostEvent(parWindow, reloadEvent);
	}
	else{}

	if (numThreads->GetValue() > 1) {
		proc->SetMultithread(true);
		proc->SetNumThreads(numThreads->GetValue());
	}
	else {
		proc->SetMultithread(false);
		proc->SetNumThreads(1);
	}
}
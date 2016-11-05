// Copyright 2016 Jacob Chesley

#include "CollapsiblePane.h"

CollapsiblePane::CollapsiblePane(wxWindow * parent, wxString name) : wxPanel(parent) {

	parentWindow = parent;
	this->SetBackgroundColour(parent->GetBackgroundColour());

	mainSizer = new wxBoxSizer(wxVERTICAL);
	indentAndWindowSizer = new wxBoxSizer(wxHORIZONTAL);

	this->SetSizer(mainSizer);

	collapseButton = new wxButton(this, CollapsiblePane::Button::COLLAPSE, name, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);

	wxCoord textWidth = 0;
	wxCoord textHeight = 0;
	collapseButton->GetTextExtent(name, &textWidth, &textHeight);
	collapseButton->SetMinSize(wxSize(textWidth, textHeight));

	this->GetSizer()->Add(collapseButton, 0, wxALIGN_LEFT);
	this->GetSizer()->AddSpacer(8);
	this->GetSizer()->Add(indentAndWindowSizer, 1, wxEXPAND);

	this->Bind(wxEVT_BUTTON, (wxObjectEventFunction)&CollapsiblePane::OnCollapse, this, CollapsiblePane::Button::COLLAPSE);
	attachedWindow = NULL;

	isDisplayed = parent->IsShown();
}

void CollapsiblePane::OnCollapse(wxCommandEvent& WXUNUSED(event)) {

	// Hide window if it is already displayed
	if (isDisplayed) {
		this->Collapse();
	}

	// Show window if it is hidden
	else {
		this->Open();
	}
}

void CollapsiblePane::Collapse() {

	// Hide window if it is already displayed
	if (isDisplayed) {
		isDisplayed = false;
		if (attachedWindow != NULL) {
			attachedWindow->Hide();
		}
	}
	this->Layout();
	this->Fit();

	parentWindow->Layout();
	parentWindow->FitInside();
}

void CollapsiblePane::Open() {

	// Show window if it is hidden
	if(!isDisplayed){
		isDisplayed = true;
		if (attachedWindow != NULL) {
			attachedWindow->Show();
		}
	}
	this->Layout();
	this->Fit();

	parentWindow->Layout();
	parentWindow->FitInside();
}

void CollapsiblePane::SetTextBackgroundColour(wxColour color) {
	collapseButton->SetBackgroundColour(color);
}

void CollapsiblePane::SetTextForegroundColour(wxColour color) {
	collapseButton->SetForegroundColour(color);
}

void CollapsiblePane::SetTextFont(wxFont font) {

	// Set button font
	collapseButton->SetFont(font);

	// Size button to fit text exactly
	wxCoord textWidth = 0;
	wxCoord textHeight = 0;
	collapseButton->GetTextExtent(collapseButton->GetLabel(), &textWidth, &textHeight);
	collapseButton->SetMinSize(wxSize(textWidth, textHeight));
}

void CollapsiblePane::AttachWindow(wxWindow * attach) {
	indentAndWindowSizer->AddSpacer(20);
	indentAndWindowSizer->Add(attach, 1, wxEXPAND);
	attachedWindow = attach;
	
}
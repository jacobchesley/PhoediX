// Copyright 2018 Jacob Chesley.
// See LICENSE.TXT in root of project for license information.

#include "PhoediXButton.h"

PhoediXButton::PhoediXButton(wxWindow * parent, wxWindowID id, const wxString& label) : wxPanel(parent, id) {
	
	sizer = new wxBoxSizer(wxVERTICAL);
	outerSizer = new wxBoxSizer(wxHORIZONTAL);

	text = new wxStaticText(this, -1, label);
	
	sizer->AddSpacer(8);
	sizer->Add(text);
	sizer->AddSpacer(8);

	outerSizer->AddSpacer(15);
	outerSizer->Add(sizer);
	outerSizer->AddSpacer(15);

	this->SetSizer(outerSizer);

	this->Bind(wxEVT_LEFT_DOWN, (wxObjectEventFunction)&PhoediXButton::LeftDown, this);
	text->Bind(wxEVT_LEFT_DOWN, (wxObjectEventFunction)&PhoediXButton::LeftDown, this);

	this->Bind(wxEVT_RIGHT_DOWN, (wxObjectEventFunction)&PhoediXButton::RightDown, this);
	text->Bind(wxEVT_RIGHT_DOWN, (wxObjectEventFunction)&PhoediXButton::RightDown, this);
	disabled = false;
}

void PhoediXButton::SetBackgroundColour(wxColor newColor) {
	text->SetBackgroundColour(newColor);
	wxPanel::SetBackgroundColour(newColor);
	enableBackground = newColor;
	text->Update();
	text->Refresh();
}

void PhoediXButton::SetForegroundColour(wxColor newColor) {
	text->SetForegroundColour(newColor);
	enableForeground = newColor;
	text->Update();
	text->Refresh();
}

void PhoediXButton::SetDisableBackgroundColour(wxColor disableColor) {
	disableBackground = disableColor;
}

void PhoediXButton::SetDisableForegroundColour(wxColor disableColor) {
	disableForeground = disableColor;
}

void PhoediXButton::Disable() {
	disabled = true;
	text->SetBackgroundColour(disableBackground);
	text->SetForegroundColour(disableForeground);
	wxPanel::SetBackgroundColour(disableBackground);
	text->Update();
	text->Refresh();
}

void PhoediXButton::Enable() {
	disabled = false;
	text->SetBackgroundColour(enableBackground);
	text->SetForegroundColour(enableForeground);
	wxPanel::SetBackgroundColour(enableBackground);
	text->Update();
	text->Refresh();
}

bool PhoediXButton::GetEnabled() {
	return !disabled;
}

wxString PhoediXButton::GetLabel() {
	return text->GetLabel();
}

void PhoediXButton::SetLabel(wxString label) {
	text->SetLabel(label);
}

void PhoediXButton::SetBorder(int xBorder, int yBorder) {
	sizer->Clear();
	outerSizer->Clear();

	sizer = new wxBoxSizer(wxVERTICAL);
	outerSizer = new wxBoxSizer(wxHORIZONTAL);

	sizer->AddSpacer(yBorder);
	sizer->Add(text);
	sizer->AddSpacer(yBorder);

	outerSizer->AddSpacer(xBorder);
	outerSizer->Add(sizer);
	outerSizer->AddSpacer(xBorder);

	this->SetSizer(outerSizer);
	this->Layout();
}

bool PhoediXButton::SetFont(const wxFont& font) {
	return text->SetFont(font);	
}

void PhoediXButton::LeftDown(wxCommandEvent& WXUNUSED(clickEvent)){

	if (!disabled) {
		wxCommandEvent buttonEvt(wxEVT_BUTTON, this->GetId());
		wxPostEvent(this->GetParent(), buttonEvt);
	}
}

void PhoediXButton::RightDown(wxCommandEvent& WXUNUSED(clickEvent)){

	if (!disabled) {
		wxCommandEvent buttonEvt(wxEVT_RIGHT_DOWN, this->GetId());
		wxPostEvent(this->GetParent(), buttonEvt);
	}
}
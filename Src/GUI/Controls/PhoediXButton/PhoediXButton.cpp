// Copyright 2016 Jacob Chesley

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

}

void PhoediXButton::SetBackgroundColour(wxColor newColor) {
	text->SetBackgroundColour(newColor);
	wxPanel::SetBackgroundColour(newColor);
	text->Update();
	text->Refresh();
}

void PhoediXButton::SetForegroundColour(wxColor newColor) {
	text->SetForegroundColour(newColor);
	text->Update();
	text->Refresh();
}

bool PhoediXButton::SetFont(const wxFont& font) {
	return text->SetFont(font);	
}

void PhoediXButton::LeftDown(wxCommandEvent& WXUNUSED(clickEvent)){
	wxCommandEvent buttonEvt(wxEVT_BUTTON, this->GetId());
	wxPostEvent(this->GetParent(), buttonEvt);
}

void PhoediXButton::RightDown(wxCommandEvent& WXUNUSED(clickEvent)){
	wxCommandEvent buttonEvt(wxEVT_RIGHT_DOWN, this->GetId());
	wxPostEvent(this->GetParent(), buttonEvt);
}
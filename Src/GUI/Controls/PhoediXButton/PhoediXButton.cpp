// Copyright 2018 Jacob Chesley.
// See LICENSE.TXT in root of project for license information.

#include "PhoediXButton.h"

PhoediXButton::PhoediXButton(wxWindow * parent, wxWindowID id, const wxString& label) : wxPanel(parent, id) {
	
	sizer = new wxBoxSizer(wxVERTICAL);
	outerSizer = new wxBoxSizer(wxHORIZONTAL);

	text = new wxStaticText(this, -1, label, wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_HORIZONTAL);
	text->SetMinSize(text->GetTextExtent(text->GetLabel() + "  ") + wxSize(0, 5));

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

	this->Bind(wxEVT_ENTER_WINDOW, (wxMouseEventFunction)& PhoediXButton::OnMouseEnter, this);
	text->Connect(wxEVT_ENTER_WINDOW, wxMouseEventHandler(PhoediXButton::OnMouseEnter), NULL, this);

	this->Bind(wxEVT_LEAVE_WINDOW, (wxMouseEventFunction)& PhoediXButton::OnMouseLeave, this);
	text->Connect(wxEVT_LEAVE_WINDOW, wxMouseEventHandler(PhoediXButton::OnMouseLeave), NULL, this);

	disabled = false;
	doHighlight = true;
}

void PhoediXButton::SetBackgroundColour(wxColor newColor) {
	text->SetBackgroundColour(newColor);
	wxPanel::SetBackgroundColour(newColor);
	enableBackground = newColor;
	text->Update();
	text->Refresh();
	this->Update();
	this->Refresh();
}

void PhoediXButton::SetForegroundColour(wxColor newColor) {
	text->SetForegroundColour(newColor);
	enableForeground = newColor;
	text->Update();
	text->Refresh();
	this->Update();
	this->Refresh();
}

void PhoediXButton::SetDisableBackgroundColour(wxColor disableColor) {
	disableBackground = disableColor;
}

void PhoediXButton::SetDisableForegroundColour(wxColor disableColor) {
	disableForeground = disableColor;
}

void PhoediXButton::HighlightWhenEnabled(bool highlight) {
	doHighlight = highlight;

	if (doHighlight) {
		this->Bind(wxEVT_ENTER_WINDOW, (wxMouseEventFunction)& PhoediXButton::OnMouseEnter, this);
		text->Connect(wxEVT_ENTER_WINDOW, wxMouseEventHandler(PhoediXButton::OnMouseEnter), NULL, this);

		this->Bind(wxEVT_LEAVE_WINDOW, (wxMouseEventFunction)& PhoediXButton::OnMouseLeave, this);
		text->Connect(wxEVT_LEAVE_WINDOW, wxMouseEventHandler(PhoediXButton::OnMouseLeave), NULL, this);
	}
	else {
		this->Unbind(wxEVT_ENTER_WINDOW, (wxMouseEventFunction)& PhoediXButton::OnMouseEnter, this);
		text->Disconnect(wxEVT_ENTER_WINDOW, wxMouseEventHandler(PhoediXButton::OnMouseEnter), NULL, this);

		this->Unbind(wxEVT_LEAVE_WINDOW, (wxMouseEventFunction)& PhoediXButton::OnMouseLeave, this);
		text->Disconnect(wxEVT_LEAVE_WINDOW, wxMouseEventHandler(PhoediXButton::OnMouseLeave), NULL, this);
	}
}

void PhoediXButton::Disable() {
	disabled = true;
	text->SetBackgroundColour(disableBackground);
	text->SetForegroundColour(disableForeground);
	wxPanel::SetBackgroundColour(disableBackground);
	text->Update();
	text->Refresh();
	this->Update();
	this->Refresh();
}

void PhoediXButton::Enable() {
	disabled = false;
	text->SetBackgroundColour(enableBackground);
	text->SetForegroundColour(enableForeground);
	wxPanel::SetBackgroundColour(enableBackground);
	text->Update();
	text->Refresh();
	this->Update();
	this->Refresh();
}

bool PhoediXButton::GetEnabled() {
	return !disabled;
}

wxString PhoediXButton::GetLabel() {
	return text->GetLabel();
}

void PhoediXButton::SetLabel(wxString label) {
	text->SetLabel(label);
    text->SetMinSize(text->GetTextExtent(text->GetLabel() + "  ") + wxSize(0, 5));
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
	bool setFontRet = text->SetFont(font);	
	text->SetMinSize(text->GetTextExtent(text->GetLabel() + "  ") + wxSize(0, 5));
	return setFontRet;
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

void PhoediXButton::OnMouseEnter(wxMouseEvent& mouseEvent) {

	if (doHighlight && !disabled) {
		wxColor oldColor = text->GetBackgroundColour();

		int newRed = oldColor.Red() + 20;
		newRed = newRed > 255 ? 255 : newRed;

		int newGreen = oldColor.Green() + 20;
		newGreen = newGreen > 255 ? 255 : newGreen;

		int newBlue = oldColor.Blue() + 20;
		newBlue = newBlue > 255 ? 255 : newBlue;

		wxColor highlightColor = wxColor(newRed, newGreen, newBlue);

		this->SetBackgroundColour(highlightColor);
	}
	mouseEvent.Skip(true);
}

void PhoediXButton::OnMouseLeave(wxMouseEvent& mouseEvent) {

	if (doHighlight && !disabled) {
		wxColor oldColor = text->GetBackgroundColour();

		int newRed = oldColor.Red() - 20;
		newRed = newRed < 0 ? 0 : newRed;

		int newGreen = oldColor.Green() - 20;
		newGreen = newGreen < 0 ? 0 : newGreen;

		int newBlue = oldColor.Blue() - 20;
		newBlue = newBlue < 0 ? 0 : newBlue;

		wxColor highlightColor = wxColor(newRed, newGreen, newBlue);

		this->SetBackgroundColour(highlightColor);
	}
	mouseEvent.Skip(true);
}

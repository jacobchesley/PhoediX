#include "EditListItem.h"

wxDEFINE_EVENT(EDIT_UP_EVENT, wxCommandEvent);
wxDEFINE_EVENT(EDIT_DOWN_EVENT, wxCommandEvent);
wxDEFINE_EVENT(EDIT_DELETE_EVENT, wxCommandEvent);

EditListItem::EditListItem(wxWindow * parent, wxString title, int Sequence) : wxPanel(parent) {

	parWindow = parent;

	sizer = new wxBoxSizer(wxHORIZONTAL);
	this->SetSizer(sizer);
	this->SetBackgroundColour(Colors::BackGrey);

	textSizer = new wxBoxSizer(wxVERTICAL);
	titleText = new wxStaticText(this, - 1, title);
	titleText->SetForegroundColour(Colors::TextWhite);
	titleText->SetFont(wxFont(11, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
	textSizer->AddSpacer(5);
	textSizer->Add(titleText, 0, wxALIGN_CENTER, 1);
	textSizer->AddSpacer(5);

	this->GetSizer()->Add(textSizer, 1, wxEXPAND);

	upDownButtonSizer = new wxBoxSizer(wxVERTICAL);
	upButton = new wxButton(this, EditListItem::Buttons::UP_BUTTON, "UP");
	downButton = new wxButton(this, EditListItem::Buttons::DOWN_BUTTON, "DO");
	deleteButton = new wxButton(this, EditListItem::Buttons::DELETE_BUTTON, "X");

	upDownButtonSizer->Add(upButton, 0);
	upDownButtonSizer->AddSpacer(5);
	upDownButtonSizer->Add(downButton, 0);

	this->GetSizer()->Add(upDownButtonSizer, 0, wxEXPAND);
	this->GetSizer()->Add(deleteButton, 0, wxEXPAND);

	this->Bind(wxEVT_BUTTON, (wxObjectEventFunction)&EditListItem::OnUp, this, EditListItem::Buttons::UP_BUTTON);
	this->Bind(wxEVT_BUTTON, (wxObjectEventFunction)&EditListItem::OnDown, this, EditListItem::Buttons::DOWN_BUTTON);
	this->Bind(wxEVT_BUTTON, (wxObjectEventFunction)&EditListItem::OnDelete, this, EditListItem::Buttons::DELETE_BUTTON);

	seq = Sequence;
}

void EditListItem::OnUp(wxCommandEvent& WXUNUSED(event)) {
	wxCommandEvent evt(EDIT_UP_EVENT, ID_EDIT_UP);
	evt.SetInt(seq);
	wxPostEvent(parWindow, evt);
}

void EditListItem::OnDown(wxCommandEvent& WXUNUSED(event)) {
	wxCommandEvent evt(EDIT_DOWN_EVENT, ID_EDIT_DOWN);
	evt.SetInt(seq);
	wxPostEvent(parWindow, evt);
}

void EditListItem::OnDelete(wxCommandEvent& WXUNUSED(event)) {
	wxCommandEvent evt(EDIT_DELETE_EVENT, ID_EDIT_DELETE);
	evt.SetInt(seq);
	wxPostEvent(parWindow, evt);
}

wxString EditListItem::GetTitle() {
	return titleText->GetLabel();
}

void EditListItem::SetTitle(wxString title) {
	return titleText->SetLabel(title);
}

int EditListItem::GetSequence() {
	return seq;
}

void EditListItem::SetSequence(size_t sequence) {
	seq = sequence;
}
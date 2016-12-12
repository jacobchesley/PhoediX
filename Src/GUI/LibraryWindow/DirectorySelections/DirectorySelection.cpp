// Copyright 2016 Jacob Chesley

#include "DirectorySelections.h"

wxDEFINE_EVENT(DELETE_DIR_EVENT, wxCommandEvent);

DirectorySelections::DirectoryDisplayItem::DirectoryDisplayItem(wxWindow * parent) : wxPanel(parent) {

	this->SetBackgroundColour(parent->GetBackgroundColour());

	mainLayout = new wxBoxSizer(wxHORIZONTAL);
	this->SetSizer(mainLayout);

	directoryText = new wxTextCtrl(this, -1, "");
	directoryText->SetForegroundColour(Colors::TextLightGrey);
	directoryText->SetBackgroundColour(Colors::BackDarkGrey);

	directoryButton = new wxButton(this, DirectorySelections::DirectoryDisplayItem::ID_SHOW_DIRECTORY, "Browse", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	directoryButton->SetForegroundColour(Colors::TextLightGrey);
	directoryButton->SetBackgroundColour(Colors::BackDarkGrey);
	directoryButton->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	deleteButton = new wxButton(this, DirectorySelections::DirectoryDisplayItem::ID_DELETE, "", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	deleteButton->SetBackgroundColour(this->GetBackgroundColour());

	Icons icons;
	deleteButton->SetBitmap(icons.DeleteButton.Rescale(22, 22, wxIMAGE_QUALITY_HIGH));

	this->GetSizer()->Add(directoryText, 1, wxEXPAND);
	this->GetSizer()->AddSpacer(15);
	this->GetSizer()->Add(directoryButton);
	this->GetSizer()->Add(deleteButton);

	this->Bind(wxEVT_BUTTON, (wxObjectEventFunction)&DirectorySelections::DirectoryDisplayItem::OnShowDirectory, this, DirectorySelections::DirectoryDisplayItem::ID_SHOW_DIRECTORY);
	this->Bind(wxEVT_BUTTON, (wxObjectEventFunction)&DirectorySelections::DirectoryDisplayItem::OnDelete, this, DirectorySelections::DirectoryDisplayItem::ID_DELETE);

	parWindow = parent;
	seq = -1;
}


void DirectorySelections::DirectoryDisplayItem::OnShowDirectory(wxCommandEvent& WXUNUSED(evt)) {

	// Browse for directory to look for images
	wxDirDialog openDirDialog(this, "Image Directory");
	if (openDirDialog.ShowModal() == wxID_CANCEL) {
		return;
	}
	directoryText->SetValue(openDirDialog.GetPath());
}

void DirectorySelections::DirectoryDisplayItem::OnDelete(wxCommandEvent& WXUNUSED(evt)) {

	wxCommandEvent evt(DELETE_DIR_EVENT, ID_DELETE_EVT);
	evt.SetInt(seq);
	wxPostEvent(parWindow, evt);
}

void DirectorySelections::DirectoryDisplayItem::SetSequence(int sequence) {
	seq = sequence;
}

int DirectorySelections::DirectoryDisplayItem::GetSequence() {
	return seq;
}

wxString DirectorySelections::DirectoryDisplayItem::GetDirectoryName() {
	return directoryText->GetValue();
}

DirectorySelections::DirectorySelections(wxWindow * parent) : wxScrolledWindow(parent) {

	this->SetBackgroundColour(parent->GetBackgroundColour());
	mainLayout = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(mainLayout);

	addDirectory = new wxButton(this, DirectorySelections::ID_ADD_DIRECTORY, "Add Directory", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	addDirectory->SetForegroundColour(Colors::TextLightGrey);
	addDirectory->SetBackgroundColour(Colors::BackGrey);
	addDirectory->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	this->Bind(wxEVT_BUTTON, (wxObjectEventFunction)&DirectorySelections::OnAddDirectory, this, DirectorySelections::ID_ADD_DIRECTORY);
	this->Bind(DELETE_DIR_EVENT, (wxObjectEventFunction)&DirectorySelections::OnDeleteDir, this, ID_DELETE_EVT);
	this->RedrawPanel();

	this->AddDirectoryDisplayItem();
	this->SetSize(this->GetBestVirtualSize());

}

void DirectorySelections::AddDirectoryDisplayItem() {

	// Add the edit to the sizer, and push onto the editList
	DirectoryDisplayItem * newDir = new DirectoryDisplayItem(this);
	directoryItems.push_back(newDir);
	this->RedrawPanel();
}

void DirectorySelections::OnAddDirectory(wxCommandEvent& WXUNUSED(evt)) {
	this->AddDirectoryDisplayItem();
}

void DirectorySelections::OnDeleteDir(wxCommandEvent& evt) {
	size_t dirNum = (size_t)evt.GetInt();

	// Remove all edit items from the sizer, but do not destroy them
	this->GetSizer()->Clear();

	// Cant delete directory item that doesnt exist
	if (dirNum >= directoryItems.size()) {
		return;
	}
	// Remove the edit window from AUI Manager if it exists
	if (directoryItems[dirNum] != NULL) {
		directoryItems[dirNum]->Destroy();
		directoryItems.erase(directoryItems.begin() + dirNum);
	}

	this->RedrawPanel();
}

void DirectorySelections::RedrawPanel() {

	// Remove all items from sizer
	this->GetSizer()->Clear();

	// Add all directory items
	for (size_t i = 0; i < directoryItems.size(); i++) {
		directoryItems[i]->SetSequence(i);
		this->GetSizer()->Add(directoryItems[i], 0, wxALL | wxEXPAND);
		this->GetSizer()->AddSpacer(12);
	}

	// Add button at bottom
	this->GetSizer()->AddSpacer(10);
	this->GetSizer()->Add(addDirectory, 0, wxCENTER);

	this->FitInside();
}

wxVector<wxString> DirectorySelections::GetDirectoryList() {
	wxVector<wxString> returnVec;

	for (size_t i = 0; i < directoryItems.size(); i++) {
		returnVec.push_back(directoryItems[i]->GetDirectoryName());
	}

	return returnVec;
}
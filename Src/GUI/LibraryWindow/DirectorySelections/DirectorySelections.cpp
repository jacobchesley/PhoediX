// Copyright 2016 Jacob Chesley

#include "DirectorySelections.h"

wxDEFINE_EVENT(DELETE_DIR_EVENT, wxCommandEvent);
wxDEFINE_EVENT(DIR_EXISTS, wxCommandEvent);
wxDEFINE_EVENT(NO_DIR_EXISTS, wxCommandEvent);

DirectorySelections::DirectoryDisplayItem::DirectoryDisplayItem(DirectorySelections * parent) : wxPanel(parent) {

	this->SetBackgroundColour(parent->GetBackgroundColour());

	mainLayout = new wxBoxSizer(wxHORIZONTAL);
	this->SetSizer(mainLayout);

	directoryText = new wxTextCtrl(this, -1, "");
	directoryText->SetForegroundColour(Colors::TextLightGrey);
	directoryText->SetBackgroundColour(Colors::BackDarkGrey);

	directoryButton = new PhoediXButton(this, DirectorySelections::DirectoryDisplayItem::ID_SHOW_DIRECTORY, "Browse");
	directoryButton->SetForegroundColour(Colors::TextLightGrey);
	directoryButton->SetBackgroundColour(Colors::BackDarkGrey);
	directoryButton->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	subDirectoriesCheck = new wxCheckBox(this, -1, "");
	subDirectoriesCheck->SetForegroundColour(Colors::TextLightGrey);
	subDirectoriesCheck->SetBackgroundColour(this->GetBackgroundColour());

	subDirectoriesLabel = new wxStaticText(this, -1, "Sub Directories");
	subDirectoriesLabel->SetForegroundColour(Colors::TextLightGrey);
	subDirectoriesLabel->SetBackgroundColour(this->GetBackgroundColour());

	deleteButton = new wxButton(this, DirectorySelections::DirectoryDisplayItem::ID_DELETE, "", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	deleteButton->SetBackgroundColour(this->GetBackgroundColour());

	Icons icons;
	deleteButton->SetBitmap(icons.DeleteButton.Rescale(22, 22, wxIMAGE_QUALITY_HIGH));

	this->GetSizer()->Add(directoryText, 1, wxEXPAND);
	this->GetSizer()->AddSpacer(15);
	this->GetSizer()->Add(directoryButton);
	this->GetSizer()->AddSpacer(15);
	this->GetSizer()->Add(subDirectoriesCheck);
	this->GetSizer()->Add(subDirectoriesLabel);
	this->GetSizer()->AddSpacer(15);
	this->GetSizer()->Add(deleteButton);

	this->Bind(wxEVT_BUTTON, (wxObjectEventFunction)&DirectorySelections::DirectoryDisplayItem::OnShowDirectory, this, DirectorySelections::DirectoryDisplayItem::ID_SHOW_DIRECTORY);
	this->Bind(wxEVT_BUTTON, (wxObjectEventFunction)&DirectorySelections::DirectoryDisplayItem::OnDelete, this, DirectorySelections::DirectoryDisplayItem::ID_DELETE);
	this->Bind(wxEVT_TEXT, (wxObjectEventFunction)&DirectorySelections::DirectoryDisplayItem::OnText, this);

	dirSelects = parent;
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
	wxPostEvent(dirSelects, evt);
}

void DirectorySelections::DirectoryDisplayItem::OnText(wxCommandEvent& WXUNUSED(evt)) {
	
	// This item has a valid directory.  No need to check more.
	if (wxDir::Exists(directoryText->GetValue())) {
		dirSelects->SendDirectoriesExist();
		return;
	}

	else {
		wxVector<wxString> allDirs = dirSelects->GetDirectoryList();

		// Go through all directories in list
		for (size_t i = 0; i < allDirs.size(); i++) {

			// Found a valid directory.
			if (wxDir::Exists(allDirs[i])) {
				dirSelects->SendDirectoriesExist();
				return;
			}
		}
		dirSelects->SendNoDirectoriesExist();
	}
}

void DirectorySelections::DirectoryDisplayItem::SetSequence(int sequence) {
	seq = sequence;
}

int DirectorySelections::DirectoryDisplayItem::GetSequence() {
	return seq;
}

wxVector<wxString> DirectorySelections::DirectoryDisplayItem::GetDirectoriesNames() {

	wxVector<wxString> returnList;
	returnList.push_back(directoryText->GetValue());

	wxString dirPrefix = directoryText->GetValue();
	if (!wxFileName::IsPathSeparator(dirPrefix[dirPrefix.size() - 1])){
		dirPrefix += wxFileName::GetPathSeparator();
	}

	// Add all sub directories to vector if needed
	if (subDirectoriesCheck->IsChecked()) {
		
		wxDir directory(directoryText->GetValue());
		wxString dirName;
		bool cont = directory.GetFirst(&dirName, "", wxDIR_DIRS);
		while (cont) {
			cont = directory.GetNext(&dirName);
			returnList.push_back(dirPrefix + dirName);
		}
	}

	return returnList;
}

wxString DirectorySelections::DirectoryDisplayItem::GetDirectoryName() {

	return directoryText->GetValue();
}

DirectorySelections::DirectorySelections(wxWindow * parent) : wxScrolledWindow(parent) {

	this->SetBackgroundColour(parent->GetBackgroundColour());
	mainLayout = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(mainLayout);

	addDirectory = new PhoediXButton(this, DirectorySelections::ID_ADD_DIRECTORY, "Add Directory");
	addDirectory->SetForegroundColour(Colors::TextLightGrey);
	addDirectory->SetBackgroundColour(Colors::BackGrey);
	addDirectory->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	this->Bind(wxEVT_BUTTON, (wxObjectEventFunction)&DirectorySelections::OnAddDirectory, this, DirectorySelections::ID_ADD_DIRECTORY);
	this->Bind(DELETE_DIR_EVENT, (wxObjectEventFunction)&DirectorySelections::OnDeleteDir, this, ID_DELETE_EVT);
	this->RedrawPanel();

	this->AddDirectoryDisplayItem();
	this->SetSize(this->GetBestVirtualSize());
	this->FitInside();
	this->SetScrollRate(5, 5);
	par = parent;
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

		wxVector<wxString> subDirs = directoryItems[i]->GetDirectoriesNames();
		for (size_t j = 0; j < subDirs.size(); j++) {
			returnVec.push_back(subDirs[j]);
		}
	}

	return returnVec;
}

wxVector<wxString> DirectorySelections::GetTopDirectoryList() {
	wxVector<wxString> returnVec;

	for (size_t i = 0; i < directoryItems.size(); i++) {
		returnVec.push_back(directoryItems[i]->GetDirectoryName());
	}

	return returnVec;
}

void DirectorySelections::SendDirectoriesExist() {

	wxCommandEvent evt(DIR_EXISTS, ID_DIR_EXISTS);
	wxPostEvent(par, evt);
}

void DirectorySelections::SendNoDirectoriesExist() {

	wxCommandEvent evt(NO_DIR_EXISTS, ID_NO_DIR_EXISTS);
	wxPostEvent(par, evt);
}
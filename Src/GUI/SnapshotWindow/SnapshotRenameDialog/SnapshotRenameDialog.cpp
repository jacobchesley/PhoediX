// Copyright 2016 Jacob Chesley

#include "SnapshotRenameDialog.h"

SnapshotRenameDialog::SnapshotRenameDialog(wxWindow * parent, wxString oldSnapshotName) : wxDialog(parent, -1, "Rename Snapshot"){

	this->SetBackgroundColour(parent->GetBackgroundColour());
	mainSizer = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(mainSizer);

	message = new wxStaticText(this, -1, "New name for snapshot");
	message->SetForegroundColour(Colors::TextLightGrey);

	newName = new wxTextCtrl(this, -1, oldSnapshotName);
	newName->SetBackgroundColour(this->GetBackgroundColour());
	newName->SetForegroundColour(Colors::TextLightGrey);

	buttonSizer = new wxBoxSizer(wxHORIZONTAL);

	cancelButton = new wxButton(this, wxID_CANCEL, "Cancel", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	cancelButton->SetForegroundColour(Colors::TextLightGrey);
	cancelButton->SetBackgroundColour(Colors::BackGrey);

	okayButton = new wxButton(this, wxID_OK, "OK", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	okayButton->SetForegroundColour(Colors::TextLightGrey);
	okayButton->SetBackgroundColour(Colors::BackGrey);

	buttonSizer->Add(cancelButton);
	buttonSizer->AddSpacer(10);
	buttonSizer->Add(okayButton);

	mainSizer->Add(message, 0, wxALIGN_CENTER);
	mainSizer->AddSpacer(20);
	mainSizer->Add(newName, 0, wxALIGN_CENTER | wxEXPAND);
	mainSizer->AddSpacer(20);
	mainSizer->Add(buttonSizer, 0, wxALIGN_RIGHT);
}

wxString SnapshotRenameDialog::GetNewName(){
	return newName->GetValue();
}
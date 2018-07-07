// Copyright 2016 Jacob Chesley

#include "ImportImageDialog.h"

ImportImageDialog::ImportImageDialog(wxWindow * parent) : wxDialog(parent, -1, "Import Image To...") {

	this->SetBackgroundColour(parent->GetBackgroundColour());
	mainSizer = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(mainSizer);

	message = new wxStaticText(this, -1, "Import image to...");
	message->SetForegroundColour(Colors::TextLightGrey);

	buttonSizer = new wxBoxSizer(wxHORIZONTAL);

	currentButton = new PhoediXButton(this, ImportImageDialog::ID_IMPORT_CURRENT_PROJECT, "Current Project");
	currentButton->SetForegroundColour(Colors::TextLightGrey);
	currentButton->SetBackgroundColour(Colors::BackGrey);

	newButton = new PhoediXButton(this, ImportImageDialog::ID_IMPORT_NEW_PROJECT, "New Project");
	newButton->SetForegroundColour(Colors::TextLightGrey);
	newButton->SetBackgroundColour(Colors::BackGrey);

	buttonSizer->Add(currentButton);
	buttonSizer->AddSpacer(10);
	buttonSizer->Add(newButton);

	mainSizer->AddSpacer(10);
	mainSizer->Add(message, 0, wxALIGN_CENTER);
	mainSizer->AddSpacer(20);
	mainSizer->Add(buttonSizer, 0, wxALIGN_CENTER);

	this->Bind(wxEVT_BUTTON, (wxObjectEventFunction) &ImportImageDialog::OnButton, this);
	this->SetSize(this->GetClientSize());
}

void ImportImageDialog::OnButton(wxCommandEvent & evt) {
	
	if (evt.GetId() == ImportImageDialog::ID_IMPORT_CURRENT_PROJECT) {
		this->EndModal(ImportImageDialog::ID_IMPORT_CURRENT_PROJECT);
		return;
	}
	else if (evt.GetId() == ImportImageDialog::ID_IMPORT_NEW_PROJECT) {
		this->EndModal(ImportImageDialog::ID_IMPORT_NEW_PROJECT);
		return;
	}
	else {
		return;
	}
}
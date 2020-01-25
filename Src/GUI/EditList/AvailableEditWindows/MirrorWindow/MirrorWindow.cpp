// Copyright 2018 Jacob Chesley.
// See LICENSE.TXT in root of project for license information.

#include "MirrorWindow.h"

MirrorWindow::MirrorWindow(wxWindow * parent, wxString editName, Processor * processor) : EditWindow(parent, editName, processor) {

	this->SetBackgroundColour(parent->GetBackgroundColour());

	mainSizer = new wxBoxSizer(wxVERTICAL);

	// 2 Columns, 15 pixel vertical gap, 5 pixel horizontal gap
	gridSizer = new wxFlexGridSizer(2, 15, 5);

	editLabel = new wxStaticText(this, -1, editName);
	editLabel->SetForegroundColour(Colors::TextWhite);
	editLabel->SetFont(wxFont(13, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));

	mirrorMethodLabel = new wxStaticText(this, -1, "Rotation Type");

	mirrorMethodLabel->SetForegroundColour(Colors::TextLightGrey);

	mirrorMethod = new PhoediXComboBox(this, -1);
	mirrorMethod->AppendString("No Mirror");
	mirrorMethod->AppendString("Mirror Horizontal");
	mirrorMethod->AppendString("Mirror Vertical");
	mirrorMethod->SetSelection(0);

	mirrorMethod->SetBackgroundColour(this->GetBackgroundColour());
	mirrorMethod->SetForegroundColour(Colors::TextLightGrey);

	gridSizer->Add(mirrorMethodLabel);
	gridSizer->Add(mirrorMethod);

	mainSizer->Add(editLabel);
	mainSizer->AddSpacer(10);
	mainSizer->Add(gridSizer);

	proc = processor;
	parWindow = parent;

	this->SetSizer(mainSizer);
	this->FitInside();
	this->SetScrollRate(5, 5);

	this->Bind(wxEVT_COMBOBOX, (wxObjectEventFunction)&MirrorWindow::OnUpdate, this);

	this->SetClientSize(this->GetVirtualSize());

	wxCommandEvent comboEvt(wxEVT_COMBOBOX, 0);
	wxPostEvent(this, comboEvt);
}

void MirrorWindow::SetParamsAndFlags(ProcessorEdit * edit) {

	if(edit == NULL){ return; }

	// Choose method based on edit loaded
	if (edit->GetEditType() == ProcessorEdit::EditType::MIRROR_HORIZONTAL ||
		edit->GetEditType() == ProcessorEdit::EditType::MIRROR_VERTICAL ||
		edit->GetEditType() == ProcessorEdit::EditType::MIRROR_NONE) {

		if (edit->CheckForFlag(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_MIRROR_SELECT)) { mirrorMethod->SetSelection(edit->GetFlag(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_MIRROR_SELECT)); }
	}
}

ProcessorEdit * MirrorWindow::GetParamsAndFlags(){

	ProcessorEdit * mirrorEdit;
	int mirrorSelection = mirrorMethod->GetSelection();

	if (mirrorSelection == 2) {
		mirrorEdit = new ProcessorEdit(ProcessorEdit::EditType::MIRROR_VERTICAL);
	}
	else if (mirrorSelection == 1) {
		mirrorEdit = new ProcessorEdit(ProcessorEdit::EditType::MIRROR_HORIZONTAL);
	}

	else{
		mirrorEdit = new ProcessorEdit(ProcessorEdit::EditType::MIRROR_NONE);
	}

	mirrorEdit->AddFlag(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_MIRROR_SELECT, mirrorSelection);

	// Set enabled / disabled
	mirrorEdit->SetDisabled(isDisabled);

	return mirrorEdit;
}

bool MirrorWindow::CheckCopiedParamsAndFlags(){

	ProcessorEdit * edit = proc->GetEditForCopyPaste();
	if(edit == NULL){ return false; }

	if (edit->GetEditType() == ProcessorEdit::EditType::MIRROR_HORIZONTAL ||
		edit->GetEditType() == ProcessorEdit::EditType::MIRROR_VERTICAL ||
		edit->GetEditType() == ProcessorEdit::EditType::MIRROR_NONE) {
		return true;
		
	}
	return false;
}
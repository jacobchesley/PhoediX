// Copyright 2016 Jacob Chesley

#include "BlurWindow.h"

BlurWindow::BlurWindow(wxWindow * parent, wxString editName, Processor * processor) : EditWindow(parent, editName, processor) {

	this->SetBackgroundColour(parent->GetBackgroundColour());

	mainSizer = new wxBoxSizer(wxVERTICAL);

	// 2 Columns, 15 pixel vertical gap, 5 pixel horizontal gap
	gridSizer = new wxFlexGridSizer(2, 15, 5);

	editLabel = new wxStaticText(this, -1, editName);
	editLabel->SetForegroundColour(Colors::TextWhite);
	editLabel->SetFont(wxFont(13, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));

	blurDirectionLabel = new wxStaticText(this, -1, "Blur Direction");
	blurSizeLabel = new wxStaticText(this, -1, "Blur Size");
	numPassesLabel = new wxStaticText(this, -1, "Number of Passes");

	blurDirectionLabel->SetForegroundColour(Colors::TextLightGrey);
	blurSizeLabel ->SetForegroundColour(Colors::TextLightGrey);
	numPassesLabel ->SetForegroundColour(Colors::TextLightGrey);

	blurDirection = new wxComboBox(this, -1);
	blurDirection->AppendString("Box Blur");
	blurDirection->AppendString("Horizontal");
	blurDirection->AppendString("Vertical");

	blurDirection->SetSelection(0);

	blurSizeSlider = new DoubleSlider(this, 0.0, 0.0, 5.0, 100000, 3);
	blurSizeSlider->SetValuePosition(DoubleSlider::VALUE_INLINE_RIGHT);

	numPassesSlider = new DoubleSlider(this, 3.0, 1.0, 10, 10, 0);
	numPassesSlider->SetValuePosition(DoubleSlider::VALUE_INLINE_RIGHT);

	blurDirection->SetBackgroundColour(this->GetBackgroundColour());
	blurDirection->SetForegroundColour(Colors::TextLightGrey);
	blurSizeSlider->SetForegroundColour(Colors::TextLightGrey);
	blurSizeSlider->SetBackgroundColour(parent->GetBackgroundColour());
	numPassesSlider->SetForegroundColour(Colors::TextLightGrey);
	numPassesSlider->SetBackgroundColour(parent->GetBackgroundColour());

	gridSizer->Add(blurDirectionLabel);
	gridSizer->Add(blurDirection);
	gridSizer->Add(blurSizeLabel);
	gridSizer->Add(blurSizeSlider);
	gridSizer->Add(numPassesLabel);
	gridSizer->Add(numPassesSlider);

	mainSizer->Add(editLabel);
	mainSizer->AddSpacer(10);
	mainSizer->Add(gridSizer);

	proc = processor;
	parWindow = parent;

	this->Bind(wxEVT_SCROLL_THUMBRELEASE, (wxObjectEventFunction)&BlurWindow::OnUpdate, this);
	this->Bind(wxEVT_TEXT_ENTER, (wxObjectEventFunction)&BlurWindow::OnUpdate, this);
	this->Bind(wxEVT_COMBOBOX, (wxObjectEventFunction)&BlurWindow::OnCombo, this);

	this->SetSizer(mainSizer);
	this->FitInside();
	this->SetScrollRate(5, 5);

	this->SetClientSize(this->GetVirtualSize());

	wxCommandEvent comboEvt(wxEVT_COMBOBOX, 0);
	wxPostEvent(this, comboEvt);

	this->StartWatchdog();
}

void BlurWindow::OnCombo(wxCommandEvent& WXUNUSED(event)) {
	this->SetUpdated(true);
}

void BlurWindow::SetParamsAndFlags(ProcessorEdit * edit) {

	if(edit->GetEditType() == ProcessorEdit::EditType::BLUR || 
		edit->GetEditType() == ProcessorEdit::EditType::VERTICAL_BLUR ||
		edit->GetEditType() == ProcessorEdit::EditType::HORIZONTAL_BLUR){

			if(edit->GetParamsSize() != 2){ return; }
	}

	if(edit->GetEditType() == ProcessorEdit::EditType::BLUR){ blurDirection->SetSelection(0); }
	if(edit->GetEditType() == ProcessorEdit::EditType::HORIZONTAL_BLUR){ blurDirection->SetSelection(1); }
	if(edit->GetEditType() == ProcessorEdit::EditType::VERTICAL_BLUR){ blurDirection->SetSelection(2); }

	blurSizeSlider->SetValue(edit->GetParam(0) * 100.0);
	numPassesSlider->SetValue(edit->GetParam(1));
}

ProcessorEdit * BlurWindow::GetParamsAndFlags(){

	int blurSelection = blurDirection->GetSelection();

	if (blurSelection == 0) {

		ProcessorEdit * blurEdit = new ProcessorEdit(ProcessorEdit::EditType::BLUR);
		blurEdit->AddParam(blurSizeSlider->GetValue() / 100.0);
		blurEdit->AddParam(wxRound(numPassesSlider->GetValue()));
		blurEdit->SetDisabled(isDisabled);

		return blurEdit;
	}

	else if(blurSelection == 1){

		ProcessorEdit * blurEdit = new ProcessorEdit(ProcessorEdit::EditType::HORIZONTAL_BLUR);
		blurEdit->AddParam(blurSizeSlider->GetValue() / 100.0);
		blurEdit->AddParam(wxRound(numPassesSlider->GetValue()));
		blurEdit->SetDisabled(isDisabled);

		return blurEdit;
	}

	else if(blurSelection == 2){

		ProcessorEdit * blurEdit = new ProcessorEdit(ProcessorEdit::EditType::VERTICAL_BLUR);
		blurEdit->AddParam(blurSizeSlider->GetValue() / 100.0);
		blurEdit->AddParam(wxRound(numPassesSlider->GetValue()));
		blurEdit->SetDisabled(isDisabled);

		int numPasses = numPassesSlider->GetValue();
		return blurEdit;
	}
	return NULL;
}

bool BlurWindow::CheckCopiedParamsAndFlags(){

	ProcessorEdit * edit = proc->GetEditForCopyPaste();
	if(edit == NULL){ return false; }

	if(edit->GetEditType() == ProcessorEdit::EditType::BLUR || 
		edit->GetEditType() == ProcessorEdit::EditType::VERTICAL_BLUR ||
		edit->GetEditType() == ProcessorEdit::EditType::HORIZONTAL_BLUR){

			if(edit->GetParamsSize() != 2){ return false; }
			else{ return true; }
	}
	return false;
}

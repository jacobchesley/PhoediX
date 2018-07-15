// Copyright 2016 Jacob Chesley

#include "AdjustBrightnessWindow.h"

AdjustBrightnessWindow::AdjustBrightnessWindow(wxWindow * parent, wxString editName, Processor * processor) : EditWindow(parent, editName, processor) {

	this->SetBackgroundColour(parent->GetBackgroundColour());

	mainSizer = new wxBoxSizer(wxVERTICAL);

	// 2 Columns, 15 pixel vertical gap, 5 pixel horizontal gap
	gridSizer = new wxFlexGridSizer(2, 15, 5);

	editLabel = new wxStaticText(this, -1, editName);
	editLabel->SetForegroundColour(Colors::TextWhite);
	editLabel->SetFont(wxFont(13, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));

	brightLabel = new wxStaticText(this, -1, "Brightness Amount");
	detailsLabel = new wxStaticText(this, -1, "Preserve Details in");
	toneLabel = new wxStaticText(this, -1, "Tonal Preservation");
	preservationLabel = new wxStaticText(this, -1, "Preservation Amount");
	brightLabel->SetForegroundColour(Colors::TextLightGrey);
	detailsLabel->SetForegroundColour(Colors::TextLightGrey);
	toneLabel->SetForegroundColour(Colors::TextLightGrey);
	preservationLabel->SetForegroundColour(Colors::TextLightGrey);

	brightSlider = new DoubleSlider(this, 0.0, -1.0, 1.0, 100000, 3);
	detailsSelect = new PhoediXComboBox(this, -1);
	detailsSelect->AppendString("Shadows and Highlights");
	detailsSelect->AppendString("Shadows Only");
	detailsSelect->AppendString("Highlights Only");
	detailsSelect->SetSelection(0);
	toneSlider = new DoubleSlider(this, 0.5, 0.0, 1.0, 100000, 3);
	preservationSlider = new DoubleSlider(this, 0.0, 0.0, 1.0, 100000, 3);
	
	brightSlider->SetValuePosition(DoubleSlider::VALUE_INLINE_RIGHT);
	toneSlider->SetValuePosition(DoubleSlider::VALUE_INLINE_RIGHT);
	preservationSlider->SetValuePosition(DoubleSlider::VALUE_INLINE_RIGHT);

	brightSlider->SetForegroundColour(Colors::TextLightGrey);
	brightSlider->SetBackgroundColour(parent->GetBackgroundColour());
	detailsSelect->SetForegroundColour(Colors::TextLightGrey);
	detailsSelect->SetBackgroundColour(parent->GetBackgroundColour());
	toneSlider->SetForegroundColour(Colors::TextLightGrey);
	toneSlider->SetBackgroundColour(parent->GetBackgroundColour());
	preservationSlider->SetForegroundColour(Colors::TextLightGrey);
	preservationSlider->SetBackgroundColour(parent->GetBackgroundColour());

	gridSizer->Add(brightLabel);
	gridSizer->Add(brightSlider);
	gridSizer->Add(detailsLabel);
	gridSizer->Add(detailsSelect);
	gridSizer->Add(toneLabel);
	gridSizer->Add(toneSlider);
	gridSizer->Add(preservationLabel);
	gridSizer->Add(preservationSlider);

	mainSizer->Add(editLabel);
	mainSizer->AddSpacer(10);
	mainSizer->Add(gridSizer);

	proc = processor;
	parWindow = parent;

	this->Bind(wxEVT_SCROLL_THUMBRELEASE, (wxObjectEventFunction)&AdjustBrightnessWindow::OnUpdate, this);
	this->Bind(wxEVT_TEXT_ENTER, (wxObjectEventFunction)&AdjustBrightnessWindow::OnUpdate, this);
	this->Bind(wxEVT_COMBOBOX, (wxObjectEventFunction)&AdjustBrightnessWindow::OnUpdate, this);

	this->SetSizer(mainSizer);
	this->FitInside();
	this->SetScrollRate(5, 5);

	this->SetClientSize(this->GetVirtualSize());
	this->StartWatchdog();
}

void AdjustBrightnessWindow::SetParamsAndFlags(ProcessorEdit * edit){

	if(edit == NULL){ return;}

	// Populate sliders based on edit loaded
	if (edit->GetEditType() == ProcessorEdit::EditType::ADJUST_BRIGHTNESS) {

		if (edit->CheckForParameter(PHOEDIX_PARAMETER_BRIGHTNESS)) { brightSlider->SetValue(edit->GetParam(PHOEDIX_PARAMETER_BRIGHTNESS)); }
		if (edit->CheckForParameter(PHOEDIX_PARAMETER_PRESERVATION)) { preservationSlider->SetValue(edit->GetParam(PHOEDIX_PARAMETER_PRESERVATION)); }
		if (edit->CheckForParameter(PHOEDIX_PARAMETER_TONE)) { toneSlider->SetValue(edit->GetParam(PHOEDIX_PARAMETER_TONE)); }

		if (edit->CheckForFlag(PHOEDIX_FLAG_PRESERVATION_TYPE)) {
			if (edit->GetFlag(PHOEDIX_FLAG_PRESERVATION_TYPE) == Processor::BrightnessPreservation::SHADOWS_AND_HIGHLIGHTS) { detailsSelect->SetSelection(0); }
			else if (edit->GetFlag(PHOEDIX_FLAG_PRESERVATION_TYPE) == Processor::BrightnessPreservation::SHADOWS) { detailsSelect->SetSelection(1); }
			else if (edit->GetFlag(PHOEDIX_FLAG_PRESERVATION_TYPE) == Processor::BrightnessPreservation::HIGHLIGHTS) { detailsSelect->SetSelection(2); }
		}
	}
}

ProcessorEdit * AdjustBrightnessWindow::GetParamsAndFlags(){

	// Add params
	ProcessorEdit * brightEdit = new ProcessorEdit(ProcessorEdit::EditType::ADJUST_BRIGHTNESS);
	brightEdit->AddParam(PHOEDIX_PARAMETER_BRIGHTNESS, brightSlider->GetValue());
	brightEdit->AddParam(PHOEDIX_PARAMETER_PRESERVATION, preservationSlider->GetValue());
	brightEdit->AddParam(PHOEDIX_PARAMETER_TONE, toneSlider->GetValue());
	
	// Add Flag
	if(detailsSelect->GetSelection() == 0){ brightEdit->AddFlag(PHOEDIX_FLAG_PRESERVATION_TYPE, Processor::BrightnessPreservation::SHADOWS_AND_HIGHLIGHTS); }
	else if(detailsSelect->GetSelection() == 1){ brightEdit->AddFlag(PHOEDIX_FLAG_PRESERVATION_TYPE, Processor::BrightnessPreservation::SHADOWS); }
	else{ brightEdit->AddFlag(PHOEDIX_FLAG_PRESERVATION_TYPE, Processor::BrightnessPreservation::HIGHLIGHTS); }

	// Set enabled / disabled
	brightEdit->SetDisabled(isDisabled);

	return brightEdit;
}

bool AdjustBrightnessWindow::CheckCopiedParamsAndFlags(){

	ProcessorEdit * edit = proc->GetEditForCopyPaste();
	if(edit == NULL){ return false;}

	if (edit->GetEditType() == ProcessorEdit::EditType::ADJUST_BRIGHTNESS) {
		return true;
	}
	return false;
}
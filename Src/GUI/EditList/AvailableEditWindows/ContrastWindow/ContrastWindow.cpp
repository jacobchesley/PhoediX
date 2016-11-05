// Copyright 2016 Jacob Chesley

#include "ContrastWindow.h"

ContrastWindow::ContrastWindow(wxWindow * parent, wxString editName, Processor * processor) : EditWindow(parent, editName, processor) {

	this->SetBackgroundColour(parent->GetBackgroundColour());

	mainSizer = new wxBoxSizer(wxVERTICAL);

	// 2 Columns, 15 pixel vertical gap, 5 pixel horizontal gap
	gridSizer = new wxFlexGridSizer(2, 15, 5);

	editLabel = new wxStaticText(this, - 1, editName);
	editLabel->SetForegroundColour(Colors::TextWhite);
	editLabel->SetFont(wxFont(13, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));

	allContrastLabel = new wxStaticText(this, -1, "All Contrast");
	redContrastLabel = new wxStaticText(this, -1, "Red Contrast");
	greenContrastLabel = new wxStaticText(this, -1, "Green Contrast");
	blueContrastLabel = new wxStaticText(this, -1, "Blue Contrast");
	allContrastCenterLabel = new wxStaticText(this, -1, "All Contrast Center");
	redContrastCenterLabel = new wxStaticText(this, -1, "Red Contrast Center");
	greenContrastCenterLabel = new wxStaticText(this, -1, "Green Contrast Center");
	blueContrastCenterLabel = new wxStaticText(this, -1, "Blue Contrast Center");
	curveSelectionLabel = new wxStaticText(this, -1, "Use S Curve");

	allContrastLabel->SetForegroundColour(Colors::TextLightGrey);
	redContrastLabel->SetForegroundColour(Colors::TextLightGrey);
	greenContrastLabel->SetForegroundColour(Colors::TextLightGrey);
	blueContrastLabel->SetForegroundColour(Colors::TextLightGrey);
	allContrastCenterLabel->SetForegroundColour(Colors::TextLightGrey);
	redContrastCenterLabel->SetForegroundColour(Colors::TextLightGrey);
	greenContrastCenterLabel->SetForegroundColour(Colors::TextLightGrey);
	blueContrastCenterLabel->SetForegroundColour(Colors::TextLightGrey);
	curveSelectionLabel->SetForegroundColour(Colors::TextLightGrey);

	allContrastSlider = new DoubleSlider(this, 1.0, 0.0, 5.0, 100000);
	redContrastSlider = new DoubleSlider(this, 1.0, 0.0, 5.0, 100000);
	greenContrastSlider = new DoubleSlider(this, 1.0, 0.0, 5.0, 100000);
	blueContrastSlider = new DoubleSlider(this, 1.0, 0.0, 5.0, 100000);
	allContrastCenterSlider = new DoubleSlider(this, 0.5, 0.0, 1.0, 100000);
	redContrastCenterSlider = new DoubleSlider(this, 0.5, 0.0, 1.0, 100000);
	greenContrastCenterSlider = new DoubleSlider(this, 0.5, 0.0, 1.0, 100000);
	blueContrastCenterSlider = new DoubleSlider(this, 0.5, 0.0, 1.0, 100000);
	curveSelection = new wxCheckBox(this, -1, "");

	allContrastSlider->SetValuePosition(DoubleSlider::VALUE_INLINE_RIGHT);
	redContrastSlider->SetValuePosition(DoubleSlider::VALUE_INLINE_RIGHT);
	greenContrastSlider->SetValuePosition(DoubleSlider::VALUE_INLINE_RIGHT);
	blueContrastSlider->SetValuePosition(DoubleSlider::VALUE_INLINE_RIGHT);
	allContrastCenterSlider->SetValuePosition(DoubleSlider::VALUE_INLINE_RIGHT);
	redContrastCenterSlider->SetValuePosition(DoubleSlider::VALUE_INLINE_RIGHT);
	greenContrastCenterSlider->SetValuePosition(DoubleSlider::VALUE_INLINE_RIGHT);
	blueContrastCenterSlider->SetValuePosition(DoubleSlider::VALUE_INLINE_RIGHT);

	allContrastSlider->SetForegroundColour(Colors::TextLightGrey);
	allContrastSlider->SetBackgroundColour(parent->GetBackgroundColour());
	redContrastSlider->SetForegroundColour(Colors::TextLightGrey);
	redContrastSlider->SetBackgroundColour(parent->GetBackgroundColour());
	greenContrastSlider->SetForegroundColour(Colors::TextLightGrey);
	greenContrastSlider->SetBackgroundColour(parent->GetBackgroundColour());
	blueContrastSlider->SetForegroundColour(Colors::TextLightGrey);
	blueContrastSlider->SetBackgroundColour(parent->GetBackgroundColour());
	allContrastCenterSlider->SetForegroundColour(Colors::TextLightGrey);
	allContrastCenterSlider->SetBackgroundColour(parent->GetBackgroundColour());
	redContrastCenterSlider->SetForegroundColour(Colors::TextLightGrey);
	redContrastCenterSlider->SetBackgroundColour(parent->GetBackgroundColour());
	greenContrastCenterSlider->SetForegroundColour(Colors::TextLightGrey);
	greenContrastCenterSlider->SetBackgroundColour(parent->GetBackgroundColour());
	blueContrastCenterSlider->SetForegroundColour(Colors::TextLightGrey);
	blueContrastCenterSlider->SetBackgroundColour(parent->GetBackgroundColour());
	curveSelection->SetForegroundColour(Colors::TextLightGrey);

	gridSizer->Add(allContrastLabel);
	gridSizer->Add(allContrastSlider);
	gridSizer->Add(allContrastCenterLabel);
	gridSizer->Add(allContrastCenterSlider);
	gridSizer->AddSpacer(10);
	gridSizer->AddSpacer(10);
	gridSizer->Add(redContrastLabel);
	gridSizer->Add(redContrastSlider);
	gridSizer->Add(redContrastCenterLabel);
	gridSizer->Add(redContrastCenterSlider);
	gridSizer->AddSpacer(10);
	gridSizer->AddSpacer(10);
	gridSizer->Add(greenContrastLabel);
	gridSizer->Add(greenContrastSlider);
	gridSizer->Add(greenContrastCenterLabel);
	gridSizer->Add(greenContrastCenterSlider);
	gridSizer->AddSpacer(10);
	gridSizer->AddSpacer(10);
	gridSizer->Add(blueContrastLabel);
	gridSizer->Add(blueContrastSlider);
	gridSizer->Add(blueContrastCenterLabel);
	gridSizer->Add(blueContrastCenterSlider);
	gridSizer->Add(curveSelectionLabel);
	gridSizer->Add(curveSelection);

	mainSizer->Add(editLabel);
	mainSizer->AddSpacer(10);
	mainSizer->Add(gridSizer);

	proc = processor;
	parWindow = parent;

	this->Bind(wxEVT_SCROLL_CHANGED, (wxObjectEventFunction)&ContrastWindow::OnUpdate, this);
	this->Bind(wxEVT_TEXT_ENTER, (wxObjectEventFunction)&ContrastWindow::OnUpdate, this);

	this->SetSizer(mainSizer);
	this->FitInside();
	this->SetScrollRate(5, 5);

	this->SetClientSize(this->GetVirtualSize());

	this->StartWatchdog();
}

void ContrastWindow::SetParamsAndFlags(ProcessorEdit * edit) {
	
	if(edit == NULL){ return;}

	// Populate sliders based on edit loaded
	if (edit->GetParamsSize() == 8 && edit->GetEditType() == ProcessorEdit::EditType::ADJUST_CONTRAST) {
		allContrastSlider->SetValue(edit->GetParam(0));
		redContrastSlider->SetValue(edit->GetParam(1));
		greenContrastSlider->SetValue(edit->GetParam(2));
		blueContrastSlider->SetValue(edit->GetParam(3));
		allContrastCenterSlider->SetValue(edit->GetParam(4));
		redContrastCenterSlider->SetValue(edit->GetParam(5));
		greenContrastCenterSlider->SetValue(edit->GetParam(6));
		blueContrastCenterSlider->SetValue(edit->GetParam(7));
		curveSelection->SetValue(0);

	}
	if (edit->GetParamsSize() == 8 && edit->GetEditType() == ProcessorEdit::EditType::ADJUST_CONTRAST_CURVE) {
		allContrastSlider->SetValue(edit->GetParam(0));
		redContrastSlider->SetValue(edit->GetParam(1));
		greenContrastSlider->SetValue(edit->GetParam(2));
		blueContrastSlider->SetValue(edit->GetParam(3));
		allContrastCenterSlider->SetValue(edit->GetParam(4));
		redContrastCenterSlider->SetValue(edit->GetParam(5));
		greenContrastCenterSlider->SetValue(edit->GetParam(6));
		blueContrastCenterSlider->SetValue(edit->GetParam(7));
		curveSelection->SetValue(1);
	}
}

ProcessorEdit * ContrastWindow::GetParamsAndFlags(){

	if (!curveSelection->GetValue()) {
		ProcessorEdit * contrastEdit = new ProcessorEdit(ProcessorEdit::EditType::ADJUST_CONTRAST);
		contrastEdit->AddParam(allContrastSlider->GetValue());
		contrastEdit->AddParam(redContrastSlider->GetValue());
		contrastEdit->AddParam(greenContrastSlider->GetValue());
		contrastEdit->AddParam(blueContrastSlider->GetValue());
		contrastEdit->AddParam(allContrastCenterSlider->GetValue());
		contrastEdit->AddParam(redContrastCenterSlider->GetValue());
		contrastEdit->AddParam(greenContrastCenterSlider->GetValue());
		contrastEdit->AddParam(blueContrastCenterSlider->GetValue());

		// Set enabled / disabled
		contrastEdit->SetDisabled(isDisabled);

		return contrastEdit;
	}
	else {
		ProcessorEdit * contrastEdit = new ProcessorEdit(ProcessorEdit::EditType::ADJUST_CONTRAST_CURVE);
		contrastEdit->AddParam(allContrastSlider->GetValue());
		contrastEdit->AddParam(redContrastSlider->GetValue());
		contrastEdit->AddParam(greenContrastSlider->GetValue());
		contrastEdit->AddParam(blueContrastSlider->GetValue());
		contrastEdit->AddParam(allContrastCenterSlider->GetValue());
		contrastEdit->AddParam(redContrastCenterSlider->GetValue());
		contrastEdit->AddParam(greenContrastCenterSlider->GetValue());
		contrastEdit->AddParam(blueContrastCenterSlider->GetValue());

		// Set enabled / disabled
		contrastEdit->SetDisabled(isDisabled);

		return contrastEdit;
	}
}

bool ContrastWindow::CheckCopiedParamsAndFlags(){

	ProcessorEdit * edit = proc->GetEditForCopyPaste();
	if(edit == NULL){ return false;}

	if (edit->GetParamsSize() == 8 && edit->GetEditType() == ProcessorEdit::EditType::ADJUST_CONTRAST){
		return true;
	}

	if(edit->GetParamsSize() == 8 && edit->GetEditType() == ProcessorEdit::EditType::ADJUST_CONTRAST_CURVE) {
		return true;
	}
	return false;
}
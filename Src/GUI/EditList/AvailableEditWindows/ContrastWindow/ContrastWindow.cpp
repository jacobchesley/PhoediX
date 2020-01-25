// Copyright 2018 Jacob Chesley.
// See LICENSE.TXT in root of project for license information.

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

	allContrastSlider = new DoubleSlider(this, 1.0, 0.0, 3.0, 100000);
	redContrastSlider = new DoubleSlider(this, 1.0, 0.0, 3.0, 100000);
	greenContrastSlider = new DoubleSlider(this, 1.0, 0.0, 3.0, 100000);
	blueContrastSlider = new DoubleSlider(this, 1.0, 0.0, 3.0, 100000);
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

	this->Bind(wxEVT_SCROLL_THUMBRELEASE, (wxObjectEventFunction)&ContrastWindow::OnUpdate, this);
	this->Bind(wxEVT_TEXT_ENTER, (wxObjectEventFunction)&ContrastWindow::OnUpdate, this);
	this->Bind(wxEVT_CHECKBOX, (wxObjectEventFunction)&ContrastWindow::OnUpdate, this);

	this->SetSizer(mainSizer);
	this->FitInside();
	this->SetScrollRate(5, 5);

	this->SetClientSize(this->GetVirtualSize());
}

void ContrastWindow::SetParamsAndFlags(ProcessorEdit * edit) {
	
	if(edit == NULL){ return;}

	// Populate sliders based on edit loaded
	if (edit->GetEditType() == ProcessorEdit::EditType::ADJUST_CONTRAST) {
		curveSelection->SetValue(0);
	}
	if (edit->GetEditType() == ProcessorEdit::EditType::ADJUST_CONTRAST_CURVE) {
		curveSelection->SetValue(1);
	}

	if (edit->CheckForParameter(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_ALL_CONTRAST)) { allContrastSlider->SetValue(edit->GetParam(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_ALL_CONTRAST)); }
	if (edit->CheckForParameter(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_RED_CONTRAST)) { redContrastSlider->SetValue(edit->GetParam(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_RED_CONTRAST)); }
	if (edit->CheckForParameter(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_GREEN_CONTRAST)) { greenContrastSlider->SetValue(edit->GetParam(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_GREEN_CONTRAST)); }
	if (edit->CheckForParameter(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_BLUE_CONTRAST)) { blueContrastSlider->SetValue(edit->GetParam(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_BLUE_CONTRAST)); }
	if (edit->CheckForParameter(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_ALL_CONTRAST_CENTER)) { allContrastCenterSlider->SetValue(edit->GetParam(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_ALL_CONTRAST_CENTER)); }
	if (edit->CheckForParameter(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_RED_CONTRAST_CENTER)) { redContrastCenterSlider->SetValue(edit->GetParam(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_RED_CONTRAST_CENTER)); }
	if (edit->CheckForParameter(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_GREEN_CONTRAST_CENTER)) { greenContrastCenterSlider->SetValue(edit->GetParam(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_GREEN_CONTRAST_CENTER)); }
	if (edit->CheckForParameter(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_BLUE_CONTRAST_CENTER)) { blueContrastCenterSlider->SetValue(edit->GetParam(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_BLUE_CONTRAST_CENTER)); }
}

ProcessorEdit * ContrastWindow::GetParamsAndFlags(){

	ProcessorEdit * contrastEdit;
	
	if (!curveSelection->GetValue()) {
		contrastEdit = new ProcessorEdit(ProcessorEdit::EditType::ADJUST_CONTRAST);
	}
	else {
		contrastEdit = new ProcessorEdit(ProcessorEdit::EditType::ADJUST_CONTRAST_CURVE);
	}
	contrastEdit->AddParam(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_ALL_CONTRAST, allContrastSlider->GetValue());
	contrastEdit->AddParam(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_RED_CONTRAST, redContrastSlider->GetValue());
	contrastEdit->AddParam(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_GREEN_CONTRAST, greenContrastSlider->GetValue());
	contrastEdit->AddParam(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_BLUE_CONTRAST, blueContrastSlider->GetValue());
	contrastEdit->AddParam(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_ALL_CONTRAST_CENTER, allContrastCenterSlider->GetValue());
	contrastEdit->AddParam(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_RED_CONTRAST_CENTER, redContrastCenterSlider->GetValue());
	contrastEdit->AddParam(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_GREEN_CONTRAST_CENTER, greenContrastCenterSlider->GetValue());
	contrastEdit->AddParam(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_BLUE_CONTRAST_CENTER, blueContrastCenterSlider->GetValue());

	// Set enabled / disabled
	contrastEdit->SetDisabled(isDisabled);

	return contrastEdit;
	
}

bool ContrastWindow::CheckCopiedParamsAndFlags(){

	ProcessorEdit * edit = proc->GetEditForCopyPaste();
	if(edit == NULL){ return false;}

	if (edit->GetEditType() == ProcessorEdit::EditType::ADJUST_CONTRAST ||
		edit->GetEditType() == ProcessorEdit::EditType::ADJUST_CONTRAST_CURVE){
		return true;
	}
	return false;
}
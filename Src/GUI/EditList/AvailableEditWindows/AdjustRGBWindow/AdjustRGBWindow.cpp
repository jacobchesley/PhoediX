// Copyright 2018 Jacob Chesley.
// See LICENSE.TXT in root of project for license information.

#include "AdjustRGBWindow.h"

AdjustRGBWindow::AdjustRGBWindow(wxWindow * parent, wxString editName, Processor * processor) : EditWindow(parent, editName, processor) {

	this->SetBackgroundColour(parent->GetBackgroundColour());

	mainSizer = new wxBoxSizer(wxVERTICAL);

	// 2 Columns, 15 pixel vertical gap, 5 pixel horizontal gap
	gridSizer = new wxFlexGridSizer(2, 15, 5);

	editLabel = new wxStaticText(this, -1, editName);
	editLabel->SetForegroundColour(Colors::TextWhite);
	editLabel->SetFont(wxFont(13, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));

	allBrightLabel = new wxStaticText(this, -1, "All Channels");
	redBrightLabel = new wxStaticText(this, -1, "Red ");
	greenBrightLabel = new wxStaticText(this, -1, "Green");
	blueBrightLabel = new wxStaticText(this, -1, "Blue");
	allBrightLabel->SetForegroundColour(Colors::TextLightGrey);
	redBrightLabel->SetForegroundColour(Colors::TextLightGrey);
	greenBrightLabel->SetForegroundColour(Colors::TextLightGrey);
	blueBrightLabel->SetForegroundColour(Colors::TextLightGrey);

	allBrightSlider = new DoubleSlider(this, 0.0, -1.0, 1.0, 100000);
	redBrightSlider = new DoubleSlider(this, 0.0, -1.0, 1.0, 100000);
	greenBrightSlider = new DoubleSlider(this, 0.0, -1.0, 1.0, 100000);
	blueBrightSlider = new DoubleSlider(this, 0.0, -1.0, 1.0, 100000);

	allBrightSlider->SetValuePosition(DoubleSlider::VALUE_INLINE_RIGHT);
	redBrightSlider->SetValuePosition(DoubleSlider::VALUE_INLINE_RIGHT);
	greenBrightSlider->SetValuePosition(DoubleSlider::VALUE_INLINE_RIGHT);
	blueBrightSlider->SetValuePosition(DoubleSlider::VALUE_INLINE_RIGHT);

	allBrightSlider->SetForegroundColour(Colors::TextLightGrey);
	allBrightSlider->SetBackgroundColour(parent->GetBackgroundColour());
	redBrightSlider->SetForegroundColour(Colors::TextLightGrey);
	redBrightSlider->SetBackgroundColour(parent->GetBackgroundColour());
	greenBrightSlider->SetForegroundColour(Colors::TextLightGrey);
	greenBrightSlider->SetBackgroundColour(parent->GetBackgroundColour());
	blueBrightSlider->SetForegroundColour(Colors::TextLightGrey);
	blueBrightSlider->SetBackgroundColour(parent->GetBackgroundColour());

	gridSizer->Add(allBrightLabel);
	gridSizer->Add(allBrightSlider);
	gridSizer->Add(redBrightLabel);
	gridSizer->Add(redBrightSlider);
	gridSizer->Add(greenBrightLabel);
	gridSizer->Add(greenBrightSlider);
	gridSizer->Add(blueBrightLabel);
	gridSizer->Add(blueBrightSlider);

	mainSizer->Add(editLabel);
	mainSizer->AddSpacer(10);
	mainSizer->Add(gridSizer);

	proc = processor;
	parWindow = parent;

	this->Bind(wxEVT_SCROLL_THUMBRELEASE, (wxObjectEventFunction)&AdjustRGBWindow::OnUpdate, this);
	this->Bind(wxEVT_TEXT_ENTER, (wxObjectEventFunction)&AdjustRGBWindow::OnUpdate, this);

	this->SetSizer(mainSizer);
	this->FitInside();
	this->SetScrollRate(5, 5);

	this->SetClientSize(this->GetVirtualSize());
}

void AdjustRGBWindow::SetParamsAndFlags(ProcessorEdit * edit){

	if(edit == NULL){ return; }
	// Populate sliders based on edit loaded
	if (edit->GetEditType() == ProcessorEdit::EditType::ADJUST_RGB) {
		if (edit->CheckForParameter(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_ALL)) { allBrightSlider->SetValue(edit->GetParam(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_ALL)); }
		if (edit->CheckForParameter(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_RED)) { redBrightSlider->SetValue(edit->GetParam(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_RED)); }
		if (edit->CheckForParameter(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_GREEN)) { greenBrightSlider->SetValue(edit->GetParam(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_GREEN)); }
		if (edit->CheckForParameter(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_BLUE)) { blueBrightSlider->SetValue(edit->GetParam(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_BLUE)); }
	}
}

ProcessorEdit * AdjustRGBWindow::GetParamsAndFlags(){

	ProcessorEdit * AdjustRgbEdit = new ProcessorEdit(ProcessorEdit::EditType::ADJUST_RGB);
	AdjustRgbEdit->AddParam(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_ALL, allBrightSlider->GetValue());
	AdjustRgbEdit->AddParam(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_RED, redBrightSlider->GetValue());
	AdjustRgbEdit->AddParam(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_GREEN, greenBrightSlider->GetValue());
	AdjustRgbEdit->AddParam(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_BLUE, blueBrightSlider->GetValue());

	AdjustRgbEdit->SetDisabled(isDisabled);

	return AdjustRgbEdit;
}

bool AdjustRGBWindow::CheckCopiedParamsAndFlags(){

	ProcessorEdit * edit = proc->GetEditForCopyPaste();
	if(edit == NULL){ return false; }

	if (edit->GetEditType() == ProcessorEdit::EditType::ADJUST_RGB) {
		return true;
	}
	return false;
}
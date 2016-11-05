// Copyright 2016 Jacob Chesley

#include "AdjustHSLWindow.h"

AdjustHSLWindow::AdjustHSLWindow(wxWindow * parent, wxString editName, Processor * processor) : EditWindow(parent, editName, processor) {

	this->SetBackgroundColour(parent->GetBackgroundColour());

	mainSizer = new wxBoxSizer(wxVERTICAL);

	// 2 Columns, 15 pixel vertical gap, 5 pixel horizontal gap
	gridSizer = new wxFlexGridSizer(2, 15, 5);

	editLabel = new wxStaticText(this, -1, editName);
	editLabel->SetForegroundColour(Colors::TextWhite);
	editLabel->SetFont(wxFont(13, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));

	hueShiftLabel = new wxStaticText(this, -1, "Hue Shift");
	saturationScaleLabel = new wxStaticText(this, -1, "Saturation Scale");
	luminaceScaleLabel = new wxStaticText(this, -1, "Luminace Scale");
	hueShiftLabel->SetForegroundColour(Colors::TextLightGrey);
	saturationScaleLabel->SetForegroundColour(Colors::TextLightGrey);
	luminaceScaleLabel->SetForegroundColour(Colors::TextLightGrey);

	hueShiftSlider = new DoubleSlider(this, 0.0, 0.0, 360.0, 100000);
	saturationScaleSlider = new DoubleSlider(this, 1.0, 0.0, 3.0, 100000);
	luminaceScaleSlider = new DoubleSlider(this, 1.0, 0.0, 3.0, 100000);

	hueShiftSlider->SetValuePosition(DoubleSlider::VALUE_INLINE_RIGHT);
	saturationScaleSlider->SetValuePosition(DoubleSlider::VALUE_INLINE_RIGHT);
	luminaceScaleSlider->SetValuePosition(DoubleSlider::VALUE_INLINE_RIGHT);

	hueShiftSlider->SetForegroundColour(Colors::TextLightGrey);
	hueShiftSlider->SetBackgroundColour(parent->GetBackgroundColour());
	saturationScaleSlider->SetForegroundColour(Colors::TextLightGrey);
	saturationScaleSlider->SetBackgroundColour(parent->GetBackgroundColour());
	luminaceScaleSlider->SetForegroundColour(Colors::TextLightGrey);
	luminaceScaleSlider->SetBackgroundColour(parent->GetBackgroundColour());

	gridSizer->Add(hueShiftLabel);
	gridSizer->Add(hueShiftSlider);
	gridSizer->Add(saturationScaleLabel);
	gridSizer->Add(saturationScaleSlider);
	gridSizer->Add(luminaceScaleLabel);
	gridSizer->Add(luminaceScaleSlider);

	mainSizer->Add(editLabel);
	mainSizer->AddSpacer(10);
	mainSizer->Add(gridSizer);

	proc = processor;
	parWindow = parent;

	this->Bind(wxEVT_SCROLL_CHANGED, (wxObjectEventFunction)&AdjustHSLWindow::OnUpdate, this);
	this->Bind(wxEVT_TEXT_ENTER, (wxObjectEventFunction)&AdjustHSLWindow::OnUpdate, this);

	this->SetSizer(mainSizer);
	this->FitInside();
	this->SetScrollRate(5, 5);

	this->SetClientSize(this->GetVirtualSize());
	this->StartWatchdog();
}

void AdjustHSLWindow::SetParamsAndFlags(ProcessorEdit * edit){

	// Populate sliders based on edit loaded
	if(edit == NULL){ return;}

	if (edit->GetParamsSize() == 3 && edit->GetEditType() == ProcessorEdit::EditType::ADJUST_HSL) {
		hueShiftSlider->SetValue(edit->GetParam(0));
		saturationScaleSlider->SetValue(edit->GetParam(1));
		luminaceScaleSlider->SetValue(edit->GetParam(2));
	}
}

ProcessorEdit * AdjustHSLWindow::GetParamsAndFlags(){

	ProcessorEdit * hslEdit = new ProcessorEdit(ProcessorEdit::EditType::ADJUST_HSL);
	hslEdit->AddParam(hueShiftSlider->GetValue());
	hslEdit->AddParam(saturationScaleSlider->GetValue());
	hslEdit->AddParam(luminaceScaleSlider->GetValue());

	// Set enabled / disabled
	hslEdit->SetDisabled(isDisabled);

	return hslEdit;
}

bool AdjustHSLWindow::CheckCopiedParamsAndFlags(){

	ProcessorEdit * edit = proc->GetEditForCopyPaste();
	if(edit == NULL){ return false;}

	if (edit->GetParamsSize() == 3 && edit->GetEditType() == ProcessorEdit::EditType::ADJUST_HSL) {
		return true;
	}
	return false;
}
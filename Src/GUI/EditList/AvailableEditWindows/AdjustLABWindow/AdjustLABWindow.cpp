// Copyright 2016 Jacob Chesley

#include "AdjustLABWindow.h"

AdjustLABWindow::AdjustLABWindow(wxWindow * parent, wxString editName, Processor * processor) : EditWindow(parent, editName, processor) {

	this->SetBackgroundColour(parent->GetBackgroundColour());

	mainSizer = new wxBoxSizer(wxVERTICAL);

	// 2 Columns, 15 pixel vertical gap, 5 pixel horizontal gap
	gridSizer = new wxFlexGridSizer(2, 15, 5);

	editLabel = new wxStaticText(this, -1, editName);
	editLabel->SetForegroundColour(Colors::TextWhite);
	editLabel->SetFont(wxFont(13, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));

	luminaceScaleLabel = new wxStaticText(this, -1, "Luminace Scale");
	aLabel = new wxStaticText(this, -1, "A Shift");
	bLabel = new wxStaticText(this, -1, "B Shift");

	redLabel = new wxStaticText(this, -1, "Red");
	greenLabel = new wxStaticText(this, -1, "Green");
	blueLabel = new wxStaticText(this, -1, "Blue");

	luminaceScaleLabel->SetForegroundColour(Colors::TextLightGrey);
	aLabel->SetForegroundColour(Colors::TextLightGrey);
	bLabel->SetForegroundColour(Colors::TextLightGrey);
	redLabel->SetForegroundColour(Colors::TextLightGrey);
	greenLabel->SetForegroundColour(Colors::TextLightGrey);
	blueLabel->SetForegroundColour(Colors::TextLightGrey);

	lSlider = new DoubleSlider(this, 1.0, 0.0, 1.0, 100000);
	aSlider = new DoubleSlider(this, 0.0, -128.0, 128.0, 100000);
	bSlider = new DoubleSlider(this, 0.0, -128.0, 128.0, 100000);
	
	rScaleSlider = new DoubleSlider(this, 1.0, 0.0, 1.0, 100000);
	gScaleSlider = new DoubleSlider(this, 1.0, 0.0, 1.0, 100000);
	bScaleSlider = new DoubleSlider(this, 1.0, 0.0, 1.0, 100000);

	lSlider->SetValuePosition(DoubleSlider::VALUE_INLINE_RIGHT);
	aSlider->SetValuePosition(DoubleSlider::VALUE_INLINE_RIGHT);
	bSlider->SetValuePosition(DoubleSlider::VALUE_INLINE_RIGHT);
	rScaleSlider->SetValuePosition(DoubleSlider::VALUE_INLINE_RIGHT);;
	gScaleSlider->SetValuePosition(DoubleSlider::VALUE_INLINE_RIGHT);
	bScaleSlider->SetValuePosition(DoubleSlider::VALUE_INLINE_RIGHT);

	lSlider->SetForegroundColour(Colors::TextLightGrey);
	lSlider->SetBackgroundColour(parent->GetBackgroundColour());
	aSlider->SetForegroundColour(Colors::TextLightGrey);
	aSlider->SetBackgroundColour(parent->GetBackgroundColour());
	bSlider->SetForegroundColour(Colors::TextLightGrey);
	bSlider->SetBackgroundColour(parent->GetBackgroundColour());
	rScaleSlider->SetForegroundColour(Colors::TextLightGrey);
	rScaleSlider->SetBackgroundColour(parent->GetBackgroundColour());
	gScaleSlider->SetForegroundColour(Colors::TextLightGrey);
	gScaleSlider->SetBackgroundColour(parent->GetBackgroundColour());
	bScaleSlider->SetForegroundColour(Colors::TextLightGrey);
	bScaleSlider->SetBackgroundColour(parent->GetBackgroundColour());

	gridSizer->Add(luminaceScaleLabel);
	gridSizer->Add(lSlider);
	gridSizer->Add(aLabel);
	gridSizer->Add(aSlider);
	gridSizer->Add(bLabel);
	gridSizer->Add(bSlider);
	gridSizer->AddSpacer(10);
	gridSizer->AddSpacer(10);
	gridSizer->Add(redLabel);
	gridSizer->Add(rScaleSlider);
	gridSizer->Add(greenLabel);
	gridSizer->Add(gScaleSlider);
	gridSizer->Add(blueLabel);
	gridSizer->Add(bScaleSlider);

	mainSizer->Add(editLabel);
	mainSizer->AddSpacer(10);
	mainSizer->Add(gridSizer);

	proc = processor;
	parWindow = parent;

	this->Bind(wxEVT_SCROLL_THUMBRELEASE, (wxObjectEventFunction)&AdjustLABWindow::OnUpdate, this);
	this->Bind(wxEVT_TEXT_ENTER, (wxObjectEventFunction)&AdjustLABWindow::OnUpdate, this);

	this->SetSizer(mainSizer);
	this->FitInside();
	this->SetScrollRate(5, 5);

	this->SetClientSize(this->GetVirtualSize());
	this->StartWatchdog();
}

void AdjustLABWindow::SetParamsAndFlags(ProcessorEdit * edit){

	// Populate sliders based on edit loaded
	if(edit == NULL){ return;}

	if (edit->GetEditType() == ProcessorEdit::EditType::ADJUST_LAB) {
		if (edit->CheckForParameter(PHOEDIX_PARAMETER_LUMINACE)) { lSlider->SetValue(edit->GetParam(PHOEDIX_PARAMETER_LUMINACE)); }
		if (edit->CheckForParameter(PHOEDIX_PARAMETER_A)) { aSlider->SetValue(edit->GetParam(PHOEDIX_PARAMETER_A)); }
		if (edit->CheckForParameter(PHOEDIX_PARAMETER_B)) { bSlider->SetValue(edit->GetParam(PHOEDIX_PARAMETER_B)); }
		if (edit->CheckForParameter(PHOEDIX_PARAMETER_RED_SCALE)) { rScaleSlider->SetValue(edit->GetParam(PHOEDIX_PARAMETER_RED_SCALE)); }
		if (edit->CheckForParameter(PHOEDIX_PARAMETER_GREEN_SCALE)) { bScaleSlider->SetValue(edit->GetParam(PHOEDIX_PARAMETER_GREEN_SCALE)); }
		if (edit->CheckForParameter(PHOEDIX_PARAMETER_BLUE_SCALE)) { gScaleSlider->SetValue(edit->GetParam(PHOEDIX_PARAMETER_BLUE_SCALE)); }
	}
}

ProcessorEdit * AdjustLABWindow::GetParamsAndFlags(){

	ProcessorEdit * LABEdit = new ProcessorEdit(ProcessorEdit::EditType::ADJUST_LAB);
	LABEdit->AddParam(PHOEDIX_PARAMETER_LUMINACE, lSlider->GetValue());
	LABEdit->AddParam(PHOEDIX_PARAMETER_A, aSlider->GetValue());
	LABEdit->AddParam(PHOEDIX_PARAMETER_B, bSlider->GetValue());
	LABEdit->AddParam(PHOEDIX_PARAMETER_RED_SCALE, rScaleSlider->GetValue());
	LABEdit->AddParam(PHOEDIX_PARAMETER_GREEN_SCALE, gScaleSlider->GetValue());
	LABEdit->AddParam(PHOEDIX_PARAMETER_BLUE_SCALE, bScaleSlider->GetValue());

	// Set enabled / disabled
	LABEdit->SetDisabled(isDisabled);

	return LABEdit;
}

bool AdjustLABWindow::CheckCopiedParamsAndFlags(){

	ProcessorEdit * edit = proc->GetEditForCopyPaste();
	if(edit == NULL){ return false;}

	if (edit->GetParamsSize() == 3 && edit->GetEditType() == ProcessorEdit::EditType::ADJUST_LAB) {
		return true;
	}
	return false;
}
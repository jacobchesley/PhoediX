// Copyright 2018 Jacob Chesley.
// See LICENSE.TXT in root of project for license information.

#include "RotationWindow.h"

RotationWindow::RotationWindow(wxWindow * parent, wxString editName, Processor * processor) : EditWindow(parent, editName, processor) {

	this->SetBackgroundColour(parent->GetBackgroundColour());

	mainSizer = new wxBoxSizer(wxVERTICAL);

	// 2 Columns, 15 pixel vertical gap, 5 pixel horizontal gap
	gridSizer = new wxFlexGridSizer(2, 15, 5);

	editLabel = new wxStaticText(this, -1, editName);
	editLabel->SetForegroundColour(Colors::TextWhite);
	editLabel->SetFont(wxFont(13, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));

	rotationMethodLabel = new wxStaticText(this, -1, "Rotation Type");
	rotationInterpolationLabel = new wxStaticText(this, -1, "Interpolation Method");
	rotationCropLabel = new wxStaticText(this, -1, "Cropping");
	customRotationLabel = new wxStaticText(this, -1, "Custom Angle");

	rotationMethodLabel->SetForegroundColour(Colors::TextLightGrey);
	rotationInterpolationLabel->SetForegroundColour(Colors::TextLightGrey);
	rotationCropLabel->SetForegroundColour(Colors::TextLightGrey);
	customRotationLabel->SetForegroundColour(Colors::TextLightGrey);

	rotationMethod = new PhoediXComboBox(this, -1);
	rotationMethod->AppendString("No Rotation");
	rotationMethod->AppendString("90 Deg. Clockwise");
	rotationMethod->AppendString("180 Deg.");
	rotationMethod->AppendString("270 Deg. Clockwise");
	rotationMethod->AppendString("Custom Angle");
	rotationMethod->SetSelection(0);

	customRotationInterpolation = new PhoediXComboBox(this, -1);
	customRotationInterpolation->AppendString("Nearest Neightbor");
	customRotationInterpolation->AppendString("Bilinear");
	customRotationInterpolation->AppendString("Bicubic");
	customRotationInterpolation->SetSelection(2);

	customRotationCrop = new PhoediXComboBox(this, -1);
	customRotationCrop->AppendString("Keep Size");
	customRotationCrop->AppendString("Fit");
	customRotationCrop->AppendString("Expand");
	customRotationCrop->SetSelection(0);

	customRotationSlider = new DoubleSlider(this, 0.0, -180.0, 180.0, 100000);

	customRotationSlider->SetValuePosition(DoubleSlider::VALUE_INLINE_RIGHT);

	rotationMethod->SetBackgroundColour(this->GetBackgroundColour());
	rotationMethod->SetForegroundColour(Colors::TextLightGrey);
	customRotationInterpolation->SetBackgroundColour(this->GetBackgroundColour());
	customRotationInterpolation->SetForegroundColour(Colors::TextLightGrey);
	customRotationCrop->SetBackgroundColour(this->GetBackgroundColour());
	customRotationCrop->SetForegroundColour(Colors::TextLightGrey);
	customRotationSlider->SetForegroundColour(Colors::TextLightGrey);
	customRotationSlider->SetBackgroundColour(parent->GetBackgroundColour());

	gridSizer->Add(rotationMethodLabel);
	gridSizer->Add(rotationMethod);
	gridSizer->Add(rotationInterpolationLabel);
	gridSizer->Add(customRotationInterpolation);
	gridSizer->Add(rotationCropLabel);
	gridSizer->Add(customRotationCrop);
	gridSizer->Add(customRotationLabel);
	gridSizer->Add(customRotationSlider);

	mainSizer->Add(editLabel);
	mainSizer->AddSpacer(10);
	mainSizer->Add(gridSizer);

	proc = processor;
	parWindow = parent;

	this->Bind(wxEVT_SCROLL_THUMBRELEASE, (wxObjectEventFunction)&RotationWindow::OnUpdate, this);
	this->Bind(wxEVT_TEXT_ENTER, (wxObjectEventFunction)&RotationWindow::OnUpdate, this);
	this->Bind(wxEVT_COMBOBOX, (wxObjectEventFunction)&RotationWindow::OnCombo, this);

	this->SetSizer(mainSizer);
	this->FitInside();
	this->SetScrollRate(5, 5);

	this->SetClientSize(this->GetVirtualSize());

	wxCommandEvent comboEvt(wxEVT_COMBOBOX, 0);
	wxPostEvent(this, comboEvt);
}

void RotationWindow::OnCombo(wxCommandEvent& WXUNUSED(event)) {

	if(rotationMethod->GetSelection() == 4){
		rotationInterpolationLabel->Show();
		customRotationInterpolation->Show();
		customRotationLabel->Show();
		customRotationSlider->Show();
		rotationCropLabel->Show();
		customRotationCrop->Show();
	}
	else{
		rotationInterpolationLabel->Hide();
		customRotationInterpolation->Hide();
		customRotationLabel->Hide();
		customRotationSlider->Hide();
		rotationCropLabel->Hide();
		customRotationCrop->Hide();
	}
	this->Refresh();
	this->Update();
	this->SetUpdated(true);
}

void RotationWindow::SetParamsAndFlags(ProcessorEdit * edit) {

	// Choose method based on edit loaded
	if (edit->GetEditType() == ProcessorEdit::EditType::ROTATE_NONE ||
		edit->GetEditType() == ProcessorEdit::EditType::ROTATE_90_CW ||
		edit->GetEditType() == ProcessorEdit::EditType::ROTATE_180 ||
		edit->GetEditType() == ProcessorEdit::EditType::ROTATE_270_CW) {

		if(edit->CheckForFlag(PHOEDIX_FLAG_ROTATE_TYPE)) { rotationMethod->SetSelection(edit->GetFlag(PHOEDIX_FLAG_ROTATE_TYPE)); }
	}

	// Populate sliders based on edit loaded
	if (edit->GetEditType() == ProcessorEdit::EditType::ROTATE_CUSTOM_NEAREST ||
		edit->GetEditType() == ProcessorEdit::EditType::ROTATE_CUSTOM_BILINEAR ||
		edit->GetEditType() == ProcessorEdit::EditType::ROTATE_CUSTOM_BICUBIC){

		if(edit->CheckForFlag(PHOEDIX_FLAG_ROTATE_TYPE)) { rotationMethod->SetSelection(edit->GetFlag(PHOEDIX_FLAG_ROTATE_TYPE)); }
		if(edit->CheckForFlag(PHOEDIX_FLAG_INTERPOLATION)) { customRotationInterpolation->SetSelection(edit->GetFlag(PHOEDIX_FLAG_INTERPOLATION)); }
		int cropMethod = Processor::RotationCropping::KEEP_SIZE;
		if(edit->CheckForFlag(PHOEDIX_FLAG_ROTATE_CROP)) { cropMethod = edit->GetFlag(PHOEDIX_FLAG_ROTATE_CROP); }

		if (cropMethod == Processor::RotationCropping::KEEP_SIZE) { customRotationCrop->SetSelection(0); }
		if (cropMethod == Processor::RotationCropping::FIT) { customRotationCrop->SetSelection(1); }
		if (cropMethod == Processor::RotationCropping::EXPAND) { customRotationCrop->SetSelection(2); }

		if(edit->CheckForParameter(PHOEDIX_PARAMETER_ROTATE_ANGLE)) { customRotationSlider->SetValue(edit->GetParam(PHOEDIX_PARAMETER_ROTATE_ANGLE)); }
	}
}

ProcessorEdit * RotationWindow::GetParamsAndFlags(){

	int rotationSelection = rotationMethod->GetSelection();

	if (rotationSelection == 0) {
	
		ProcessorEdit * rotateEdit = new ProcessorEdit(ProcessorEdit::EditType::ROTATE_NONE);
		rotateEdit->AddFlag(PHOEDIX_FLAG_ROTATE_TYPE, rotationSelection);

		// Set enabled / disabled
		rotateEdit->SetDisabled(isDisabled);

		return rotateEdit;
	}

	else if (rotationSelection == 1) {
		ProcessorEdit * rotateEdit = new ProcessorEdit(ProcessorEdit::EditType::ROTATE_90_CW);

		rotateEdit->AddFlag(PHOEDIX_FLAG_ROTATE_TYPE, rotationSelection);

		// Set enabled / disabled
		rotateEdit->SetDisabled(isDisabled);

		return rotateEdit;
	}

	else if (rotationSelection == 2) {
		ProcessorEdit * rotateEdit = new ProcessorEdit(ProcessorEdit::EditType::ROTATE_180);

		rotateEdit->AddFlag(PHOEDIX_FLAG_ROTATE_TYPE, rotationSelection);

		// Set enabled / disabled
		rotateEdit->SetDisabled(isDisabled);

		return rotateEdit;
	}

	else if (rotationSelection == 3) {
		ProcessorEdit * rotateEdit = new ProcessorEdit(ProcessorEdit::EditType::ROTATE_270_CW);

		rotateEdit->AddFlag(PHOEDIX_FLAG_ROTATE_TYPE, rotationSelection);

		// Set enabled / disabled
		rotateEdit->SetDisabled(isDisabled);

		return rotateEdit;
	}

	else if (rotationSelection == 4) {

		int crop = Processor::RotationCropping::KEEP_SIZE;

		if(customRotationCrop->GetSelection() == 1){
			crop = Processor::RotationCropping::FIT;
		}
		if(customRotationCrop->GetSelection() == 2){
			crop = Processor::RotationCropping::EXPAND;
		}
		if (customRotationInterpolation->GetSelection() == 0) {
			ProcessorEdit * rotateEdit = new ProcessorEdit(ProcessorEdit::EditType::ROTATE_CUSTOM_NEAREST);
			rotateEdit->AddParam(PHOEDIX_PARAMETER_ROTATE_ANGLE, customRotationSlider->GetValue());
			rotateEdit->AddFlag(PHOEDIX_FLAG_ROTATE_TYPE, rotationSelection);
			rotateEdit->AddFlag(PHOEDIX_FLAG_INTERPOLATION, 0);
			rotateEdit->AddFlag(PHOEDIX_FLAG_ROTATE_CROP, crop);

			// Set enabled / disabled
			rotateEdit->SetDisabled(isDisabled);

			return rotateEdit;
		}
		else if (customRotationInterpolation->GetSelection() == 1) {
			ProcessorEdit * rotateEdit = new ProcessorEdit(ProcessorEdit::EditType::ROTATE_CUSTOM_BILINEAR);
			rotateEdit->AddParam(PHOEDIX_PARAMETER_ROTATE_ANGLE, customRotationSlider->GetValue());
			rotateEdit->AddFlag(PHOEDIX_FLAG_ROTATE_TYPE, rotationSelection);
			rotateEdit->AddFlag(PHOEDIX_FLAG_INTERPOLATION, 1);
			rotateEdit->AddFlag(PHOEDIX_FLAG_ROTATE_CROP, crop);

			// Set enabled / disabled
			rotateEdit->SetDisabled(isDisabled);

			return rotateEdit;
		}
		else if (customRotationInterpolation->GetSelection() == 2) {
			ProcessorEdit * rotateEdit = new ProcessorEdit(ProcessorEdit::EditType::ROTATE_CUSTOM_BICUBIC);
			rotateEdit->AddParam(PHOEDIX_PARAMETER_ROTATE_ANGLE, customRotationSlider->GetValue());
			rotateEdit->AddFlag(PHOEDIX_FLAG_ROTATE_TYPE, rotationSelection);
			rotateEdit->AddFlag(PHOEDIX_FLAG_INTERPOLATION, 2);
			rotateEdit->AddFlag(PHOEDIX_FLAG_ROTATE_CROP, crop);

			// Set enabled / disabled
			rotateEdit->SetDisabled(isDisabled);

			return rotateEdit;
		}
	}
	return NULL;
}

bool RotationWindow::CheckCopiedParamsAndFlags(){

	ProcessorEdit * edit = proc->GetEditForCopyPaste();
	if(edit == NULL){ return false; }

	// Choose method based on edit loaded
	if (edit->GetEditType() == ProcessorEdit::EditType::ROTATE_NONE ||
		edit->GetEditType() == ProcessorEdit::EditType::ROTATE_90_CW ||
		edit->GetEditType() == ProcessorEdit::EditType::ROTATE_180 ||
		edit->GetEditType() == ProcessorEdit::EditType::ROTATE_270_CW|| 
		edit->GetEditType() == ProcessorEdit::EditType::ROTATE_CUSTOM_NEAREST ||
		edit->GetEditType() == ProcessorEdit::EditType::ROTATE_CUSTOM_BILINEAR ||
		edit->GetEditType() == ProcessorEdit::EditType::ROTATE_CUSTOM_BICUBIC) {

		return true;
	}

	return false;
}

#include "RotationWindow.h"

RotationWindow::RotationWindow(wxWindow * parent, wxString editName, Processor * processor) : EditWindow(parent, editName) {

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

	rotationMethod = new wxComboBox(this, -1);
	rotationMethod->AppendString("No Rotation");
	rotationMethod->AppendString("90 Deg. Clockwise");
	rotationMethod->AppendString("180 Deg.");
	rotationMethod->AppendString("270 Deg. Clockwise");
	rotationMethod->AppendString("Custom Angle");
	rotationMethod->SetSelection(0);

	customRotationInterpolation = new wxComboBox(this, -1);
	customRotationInterpolation->AppendString("Nearest Neightbor");
	customRotationInterpolation->AppendString("Bilinear");
	customRotationInterpolation->AppendString("Bicubic");
	customRotationInterpolation->SetSelection(2);

	customRotationCrop = new wxComboBox(this, -1);
	customRotationCrop->AppendString("Keep Size");
	customRotationCrop->AppendString("Fit");
	customRotationCrop->AppendString("Expand");
	customRotationCrop->SetSelection(0);

	customRotationSlider = new DoubleSlider(this, 0.0, -180.0, 180.0, 100000);

	customRotationSlider->SetValuePosition(DoubleSlider::VALUE_INLINE_RIGHT);

	processButton = new wxButton(this, EditWindow::ID_PROCESS_EDITS, "Process Edits", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	processButton->SetForegroundColour(Colors::TextLightGrey);
	processButton->SetBackgroundColour(Colors::BackGrey);
	processButton->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

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
	mainSizer->AddSpacer(15);
	mainSizer->Add(processButton, 0, wxALIGN_LEFT);

	proc = processor;
	parWindow = parent;

	this->Bind(wxEVT_SCROLL_CHANGED, (wxObjectEventFunction)&RotationWindow::OnUpdate, this);
	this->Bind(wxEVT_TEXT_ENTER, (wxObjectEventFunction)&RotationWindow::OnUpdate, this);
	this->Bind(wxEVT_BUTTON, (wxObjectEventFunction)&RotationWindow::Process, this, EditWindow::ID_PROCESS_EDITS);
	this->Bind(wxEVT_COMBOBOX, (wxObjectEventFunction)&RotationWindow::OnCombo, this);

	this->SetSizer(mainSizer);
	this->FitInside();
	this->SetScrollRate(5, 5);

	this->SetClientSize(this->GetVirtualSize());

	wxCommandEvent comboEvt(wxEVT_COMBOBOX, 0);
	wxPostEvent(this, comboEvt);

	this->StartWatchdog();
}

void RotationWindow::Process(wxCommandEvent& WXUNUSED(event)) {

	wxCommandEvent evt(REPROCESS_IMAGE_EVENT, ID_REPROCESS_IMAGE);
	wxPostEvent(parWindow, evt);
	this->SetUpdated(false);
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

void RotationWindow::AddEditToProcessor() {
	
	int rotationSelection = rotationMethod->GetSelection();

	if (rotationSelection == 0) {
	
		ProcessorEdit * rotateEdit = new ProcessorEdit(ProcessorEdit::EditType::ROTATE_NONE);
		rotateEdit->AddFlag(rotationSelection);

		// Set enabled / disabled
		rotateEdit->SetDisabled(isDisabled);

		proc->AddEdit(rotateEdit);
	}

	else if (rotationSelection == 1) {
		ProcessorEdit * rotateEdit = new ProcessorEdit(ProcessorEdit::EditType::ROTATE_90_CW);
		rotateEdit->AddFlag(rotationSelection);

		// Set enabled / disabled
		rotateEdit->SetDisabled(isDisabled);

		proc->AddEdit(rotateEdit);
	}

	else if (rotationSelection == 2) {
		ProcessorEdit * rotateEdit = new ProcessorEdit(ProcessorEdit::EditType::ROTATE_180);
		rotateEdit->AddFlag(rotationSelection);

		// Set enabled / disabled
		rotateEdit->SetDisabled(isDisabled);

		proc->AddEdit(rotateEdit);
	}

	else if (rotationSelection == 3) {
		ProcessorEdit * rotateEdit = new ProcessorEdit(ProcessorEdit::EditType::ROTATE_270_CW);
		rotateEdit->AddFlag(rotationSelection);

		// Set enabled / disabled
		rotateEdit->SetDisabled(isDisabled);

		proc->AddEdit(rotateEdit);
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
			rotateEdit->AddParam(customRotationSlider->GetValue());
			rotateEdit->AddFlag(rotationSelection);
			rotateEdit->AddFlag(0);
			rotateEdit->AddFlag(crop);

			// Set enabled / disabled
			rotateEdit->SetDisabled(isDisabled);

			proc->AddEdit(rotateEdit);
		}
		else if (customRotationInterpolation->GetSelection() == 1) {
			ProcessorEdit * rotateEdit = new ProcessorEdit(ProcessorEdit::EditType::ROTATE_CUSTOM_BILINEAR);
			rotateEdit->AddParam(customRotationSlider->GetValue());
			rotateEdit->AddFlag(rotationSelection);
			rotateEdit->AddFlag(1);
			rotateEdit->AddFlag(crop);

			// Set enabled / disabled
			rotateEdit->SetDisabled(isDisabled);

			proc->AddEdit(rotateEdit);
		}
		else if (customRotationInterpolation->GetSelection() == 2) {
			ProcessorEdit * rotateEdit = new ProcessorEdit(ProcessorEdit::EditType::ROTATE_CUSTOM_BICUBIC);
			rotateEdit->AddParam(customRotationSlider->GetValue());
			rotateEdit->AddFlag(rotationSelection);
			rotateEdit->AddFlag(2);
			rotateEdit->AddFlag(crop);

			// Set enabled / disabled
			rotateEdit->SetDisabled(isDisabled);

			proc->AddEdit(rotateEdit);
		}
	}
}

void RotationWindow::SetParamsAndFlags(ProcessorEdit * edit) {

	// Choose method based on edit loaded
	if (edit->GetFlagsSize() == 1 && (
		edit->GetEditType() == ProcessorEdit::EditType::ROTATE_NONE ||
		edit->GetEditType() == ProcessorEdit::EditType::ROTATE_90_CW ||
		edit->GetEditType() == ProcessorEdit::EditType::ROTATE_180 ||
		edit->GetEditType() == ProcessorEdit::EditType::ROTATE_270_CW)) {

		rotationMethod->SetSelection(edit->GetFlag(0));
	}

	// Populate sliders based on edit loaded
	if (edit->GetFlagsSize() == 3 && edit->GetParamsSize() == 1 && (
		edit->GetEditType() == ProcessorEdit::EditType::ROTATE_CUSTOM_NEAREST ||
		edit->GetEditType() == ProcessorEdit::EditType::ROTATE_CUSTOM_BILINEAR ||
		edit->GetEditType() == ProcessorEdit::EditType::ROTATE_CUSTOM_BICUBIC)){

		rotationMethod->SetSelection(edit->GetFlag(0));
		customRotationInterpolation->SetSelection(edit->GetFlag(1));

		int cropMethod = edit->GetFlag(2);
		if (cropMethod == Processor::RotationCropping::KEEP_SIZE) { customRotationCrop->SetSelection(0); }
		if (cropMethod == Processor::RotationCropping::FIT) { customRotationCrop->SetSelection(1); }
		if (cropMethod == Processor::RotationCropping::EXPAND) { customRotationCrop->SetSelection(2); }

		customRotationSlider->SetValue(edit->GetParam(0));
	}
	this->SetUpdated(true);
}
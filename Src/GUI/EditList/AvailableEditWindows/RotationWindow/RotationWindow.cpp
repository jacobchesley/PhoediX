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
	customRotationLabel = new wxStaticText(this, -1, "Custom Angle");

	rotationMethodLabel->SetForegroundColour(Colors::TextLightGrey);
	customRotationLabel->SetForegroundColour(Colors::TextLightGrey);

	rotationMethod = new wxComboBox(this, -1);
	rotationMethod->AppendString("No Rotation");
	rotationMethod->AppendString("90 Deg. Clockwise");
	rotationMethod->AppendString("180 Deg.");
	rotationMethod->AppendString("270 Deg. Clockwise");
	rotationMethod->AppendString("Custom Angle");
	rotationMethod->SetSelection(0);

	wxArrayString interpolationMethods = wxArrayString();
	interpolationMethods.push_back("Nearest Pixel");
	interpolationMethods.push_back("Bilinear");
	interpolationMethods.push_back("bicubic");
	customRotationInterpolation = new wxRadioBox(this, -1, "Interpolation Method", wxDefaultPosition, wxDefaultSize, interpolationMethods);
	customRotationSlider = new DoubleSlider(this, 0.0, -180.0, 180.0, 100000);

	customRotationSlider->SetValuePosition(DoubleSlider::VALUE_INLINE_RIGHT);

	processButton = new wxButton(this, EditWindow::ID_PROCESS_EDITS, "Process Edits");

	rotationMethod->SetBackgroundColour(this->GetBackgroundColour());
	rotationMethod->SetForegroundColour(Colors::TextLightGrey);
	customRotationInterpolation->SetBackgroundColour(this->GetBackgroundColour());
	customRotationInterpolation->SetForegroundColour(Colors::TextLightGrey);
	customRotationSlider->SetForegroundColour(Colors::TextLightGrey);
	customRotationSlider->SetBackgroundColour(parent->GetBackgroundColour());

	gridSizer->Add(rotationMethodLabel);
	gridSizer->Add(rotationMethod);
	gridSizer->Add(0, 0);
	gridSizer->Add(customRotationInterpolation);
	gridSizer->Add(customRotationLabel);
	gridSizer->Add(customRotationSlider);

	mainSizer->Add(editLabel);
	mainSizer->AddSpacer(10);
	mainSizer->Add(gridSizer);
	mainSizer->AddSpacer(15);
	mainSizer->Add(processButton, 0, wxALIGN_LEFT);

	proc = processor;
	parWindow = parent;

	//this->Bind(wxEVT_SCROLL_CHANGED, (wxObjectEventFunction)&ConvertGreyscaleWindow::Process, this);
	//this->Bind(wxEVT_TEXT_ENTER, (wxObjectEventFunction)&ConvertGreyscaleWindow::Process, this);
	//this->Bind(wxEVT_TEXT, (wxObjectEventFunction)&ConvertGreyscaleWindow::Process, this);
	this->Bind(wxEVT_BUTTON, (wxObjectEventFunction)&RotationWindow::Process, this, EditWindow::ID_PROCESS_EDITS);

	this->SetSizer(mainSizer);
	this->FitInside();
	this->SetScrollRate(5, 5);

	this->SetClientSize(this->GetVirtualSize());
}

void RotationWindow::Process(wxCommandEvent& WXUNUSED(event)) {

	wxCommandEvent evt(REPROCESS_IMAGE_EVENT, ID_REPROCESS_IMAGE);
	wxPostEvent(parWindow, evt);
}

void RotationWindow::AddEditToProcessor() {
	
	int rotationSelection = rotationMethod->GetSelection();

	if (rotationSelection == 0) {}

	else if (rotationSelection == 1) {
		ProcessorEdit * rotateEdit = new ProcessorEdit(ProcessorEdit::EditType::ROTATE_90_CW);
		proc->AddEdit(rotateEdit);
	}

	else if (rotationSelection == 2) {
		ProcessorEdit * rotateEdit = new ProcessorEdit(ProcessorEdit::EditType::ROTATE_180);
		proc->AddEdit(rotateEdit);
	}

	else if (rotationSelection == 3) {
		ProcessorEdit * rotateEdit = new ProcessorEdit(ProcessorEdit::EditType::ROTATE_270_CW);
		proc->AddEdit(rotateEdit);
	}

	else if (rotationSelection == 4) {

		if (customRotationInterpolation->GetSelection() == 0) {
			ProcessorEdit * rotateEdit = new ProcessorEdit(ProcessorEdit::EditType::ROTATE_CUSTOM_NEAREST);
			rotateEdit->AddParam(customRotationSlider->GetValue());
			proc->AddEdit(rotateEdit);
		}
		else if (customRotationInterpolation->GetSelection() == 1) {
			ProcessorEdit * rotateEdit = new ProcessorEdit(ProcessorEdit::EditType::ROTATE_CUSTOM_BILINEAR);
			rotateEdit->AddParam(customRotationSlider->GetValue());
			proc->AddEdit(rotateEdit);
		}
		else if (customRotationInterpolation->GetSelection() == 2) {
			ProcessorEdit * rotateEdit = new ProcessorEdit(ProcessorEdit::EditType::ROTATE_CUSTOM_BICUBIC);
			rotateEdit->AddParam(customRotationSlider->GetValue());
			proc->AddEdit(rotateEdit);
		}
	}
}
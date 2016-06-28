#include "ConvertGreyscaleWindow.h"

ConvertGreyscaleWindow::ConvertGreyscaleWindow(wxWindow * parent, wxString editName, Processor * processor) : EditWindow(parent, editName) {

	this->SetBackgroundColour(parent->GetBackgroundColour());

	mainSizer = new wxBoxSizer(wxVERTICAL);

	// 2 Columns, 15 pixel vertical gap, 5 pixel horizontal gap
	gridSizer = new wxFlexGridSizer(2, 15, 5);

	editLabel = new wxStaticText(this, -1, editName);
	editLabel->SetForegroundColour(Colors::TextWhite);
	editLabel->SetFont(wxFont(13, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));

	greyscaleMethodLabel = new wxStaticText(this, -1, "Greyscale Conversion Method");
	redBrightLabel = new wxStaticText(this, -1, "Red Multiplier");
	greenBrightLabel = new wxStaticText(this, -1, "Green Multiplier");
	blueBrightLabel = new wxStaticText(this, -1, "Blue Multiplier");
	greyscaleMethodLabel->SetForegroundColour(Colors::TextLightGrey);
	redBrightLabel->SetForegroundColour(Colors::TextLightGrey);
	greenBrightLabel->SetForegroundColour(Colors::TextLightGrey);
	blueBrightLabel->SetForegroundColour(Colors::TextLightGrey);

	greyscaleMethod = new wxComboBox(this, -1);
	greyscaleMethod->AppendString("Human Eyesight RGB Scalars");
	greyscaleMethod->AppendString("Average RGB Values");
	greyscaleMethod->AppendString("Custom RGB Scalars");
	greyscaleMethod->SetSelection(0);
	redBrightSlider = new DoubleSlider(this, (1.0 / 3.0), 0.0, 5.0, 100000);
	greenBrightSlider = new DoubleSlider(this, (1.0 / 3.0), 0.0, 5.0, 100000);
	blueBrightSlider = new DoubleSlider(this, (1.0 / 3.0), 0.0, 5.0, 100000);

	redBrightSlider->SetValuePosition(DoubleSlider::VALUE_INLINE_RIGHT);
	greenBrightSlider->SetValuePosition(DoubleSlider::VALUE_INLINE_RIGHT);
	blueBrightSlider->SetValuePosition(DoubleSlider::VALUE_INLINE_RIGHT);

	processButton = new wxButton(this, EditWindow::ID_PROCESS_EDITS, "Process Edits");

	greyscaleMethod->SetBackgroundColour(this->GetBackgroundColour());
	greyscaleMethod->SetForegroundColour(Colors::TextLightGrey);
	redBrightSlider->SetForegroundColour(Colors::TextLightGrey);
	redBrightSlider->SetBackgroundColour(parent->GetBackgroundColour());
	greenBrightSlider->SetForegroundColour(Colors::TextLightGrey);
	greenBrightSlider->SetBackgroundColour(parent->GetBackgroundColour());
	blueBrightSlider->SetForegroundColour(Colors::TextLightGrey);
	blueBrightSlider->SetBackgroundColour(parent->GetBackgroundColour());

	gridSizer->Add(greyscaleMethodLabel);
	gridSizer->Add(greyscaleMethod);
	gridSizer->Add(redBrightLabel);
	gridSizer->Add(redBrightSlider);
	gridSizer->Add(greenBrightLabel);
	gridSizer->Add(greenBrightSlider);
	gridSizer->Add(blueBrightLabel);
	gridSizer->Add(blueBrightSlider);

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
	this->Bind(wxEVT_BUTTON, (wxObjectEventFunction)&ConvertGreyscaleWindow::Process, this, EditWindow::ID_PROCESS_EDITS);
	this->Bind(wxEVT_COMBOBOX, (wxObjectEventFunction)&ConvertGreyscaleWindow::OnCombo, this);

	this->SetSizer(mainSizer);
	this->FitInside();
	this->SetScrollRate(5, 5);

	this->SetClientSize(this->GetVirtualSize());

	wxCommandEvent comboEvt(wxEVT_COMBOBOX, 0);
	wxPostEvent(this, comboEvt);
}

void ConvertGreyscaleWindow::Process(wxCommandEvent& WXUNUSED(event)) {

	wxCommandEvent evt(REPROCESS_IMAGE_EVENT, ID_REPROCESS_IMAGE);
	wxPostEvent(parWindow, evt);
}

void ConvertGreyscaleWindow::OnCombo(wxCommandEvent& WXUNUSED(event)) {

	if (greyscaleMethod->GetSelection() == 2) {
		redBrightLabel->Show();
		redBrightSlider->Show();
		greenBrightLabel->Show();
		greenBrightSlider->Show();
		blueBrightLabel->Show();
		blueBrightSlider->Show();
	}

	else {
		redBrightLabel->Hide();
		redBrightSlider->Hide();
		greenBrightLabel->Hide();
		greenBrightSlider->Hide();
		blueBrightLabel->Hide();
		blueBrightSlider->Hide();
	}
}

void ConvertGreyscaleWindow::AddEditToProcessor() {
	
	int greyscaleSelection = greyscaleMethod->GetSelection();

	if (greyscaleSelection == 0) {
		ProcessorEdit * greyEdit = new ProcessorEdit(ProcessorEdit::EditType::CONVERT_GREYSCALE_EYE);
		greyEdit->AddFlag(greyscaleSelection);
		proc->AddEdit(greyEdit);
	}

	else if (greyscaleSelection == 1) {
		ProcessorEdit * greyEdit = new ProcessorEdit(ProcessorEdit::EditType::CONVERT_GREYSCALE_AVG);
		greyEdit->AddFlag(greyscaleSelection);
		proc->AddEdit(greyEdit);
	}

	else if (greyscaleSelection == 2) {
		ProcessorEdit * greyEdit = new ProcessorEdit(ProcessorEdit::EditType::CONVERT_GREYSCALE_CUSTOM);
		greyEdit->AddParam(redBrightSlider->GetValue());
		greyEdit->AddParam(greenBrightSlider->GetValue());
		greyEdit->AddParam(blueBrightSlider->GetValue());

		greyEdit->AddFlag(greyscaleSelection);
		proc->AddEdit(greyEdit);
	}
}

void ConvertGreyscaleWindow::SetParamsAndFlags(ProcessorEdit * edit) {
	
	// Choose method based on edit loaded
	if (edit->GetFlagsSize() == 1 && (
		edit->GetEditType() == ProcessorEdit::EditType::CONVERT_GREYSCALE_AVG ||
		edit->GetEditType() == ProcessorEdit::EditType::CONVERT_GREYSCALE_EYE || 
		edit->GetEditType() == ProcessorEdit::EditType::CONVERT_GREYSCALE_CUSTOM)) {

		greyscaleMethod->SetSelection(edit->GetFlag(0));
	}

	// Populate sliders based on edit loaded
	if (edit->GetParamsSize() == 3 && edit->GetEditType() == ProcessorEdit::EditType::CONVERT_GREYSCALE_CUSTOM){
		redBrightSlider->SetValue(edit->GetParam(0));
		greenBrightSlider->SetValue(edit->GetParam(1));
		blueBrightSlider->SetValue(edit->GetParam(2));
	}
}
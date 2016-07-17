#include "ScaleBrightnessWindow.h"

ScaleBrightnessWindow::ScaleBrightnessWindow(wxWindow * parent, wxString editName, Processor * processor) : EditWindow(parent, editName) {

	this->SetBackgroundColour(parent->GetBackgroundColour());

	mainSizer = new wxBoxSizer(wxVERTICAL);

	// 2 Columns, 15 pixel vertical gap, 5 pixel horizontal gap
	gridSizer = new wxFlexGridSizer(2, 15, 5);

	editLabel = new wxStaticText(this, -1, editName);
	editLabel->SetForegroundColour(Colors::TextWhite);
	editLabel->SetFont(wxFont(13, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));

	allBrightLabel = new wxStaticText(this, -1, "All Brightness");
	redBrightLabel = new wxStaticText(this, -1, "Red Brightness");
	greenBrightLabel = new wxStaticText(this, -1, "Green Brightness");
	blueBrightLabel = new wxStaticText(this, -1, "Blue Brightness");
	allBrightLabel->SetForegroundColour(Colors::TextLightGrey);
	redBrightLabel->SetForegroundColour(Colors::TextLightGrey);
	greenBrightLabel->SetForegroundColour(Colors::TextLightGrey);
	blueBrightLabel->SetForegroundColour(Colors::TextLightGrey);

	allBrightSlider = new DoubleSlider(this, 1.0, 0.0, 5.0, 100000);
	redBrightSlider = new DoubleSlider(this, 1.0, 0.0, 5.0, 100000);
	greenBrightSlider = new DoubleSlider(this, 1.0, 0.0, 5.0, 100000);
	blueBrightSlider = new DoubleSlider(this, 1.0, 0.0, 5.0, 100000);

	allBrightSlider->SetValuePosition(DoubleSlider::VALUE_INLINE_RIGHT);
	redBrightSlider->SetValuePosition(DoubleSlider::VALUE_INLINE_RIGHT);
	greenBrightSlider->SetValuePosition(DoubleSlider::VALUE_INLINE_RIGHT);
	blueBrightSlider->SetValuePosition(DoubleSlider::VALUE_INLINE_RIGHT);

	processButton = new wxButton(this, EditWindow::ID_PROCESS_EDITS, "Process Edits");

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
	mainSizer->AddSpacer(15);
	mainSizer->Add(processButton, 0, wxALIGN_LEFT);

	proc = processor;
	parWindow = parent;

	//this->Bind(wxEVT_SCROLL_CHANGED, (wxObjectEventFunction)&ScaleBrightnessWindow::Process, this);
	//this->Bind(wxEVT_TEXT_ENTER, (wxObjectEventFunction)&ScaleBrightnessWindow::Process, this);
	//this->Bind(wxEVT_TEXT, (wxObjectEventFunction)&ScaleBrightnessWindow::Process, this);
	this->Bind(wxEVT_BUTTON, (wxObjectEventFunction)&ScaleBrightnessWindow::Process, this, EditWindow::ID_PROCESS_EDITS);

	this->SetSizer(mainSizer);
	this->FitInside();
	this->SetScrollRate(5, 5);

	this->SetClientSize(this->GetVirtualSize());
}

void ScaleBrightnessWindow::Process(wxCommandEvent& WXUNUSED(event)) {

	wxCommandEvent evt(REPROCESS_IMAGE_EVENT, ID_REPROCESS_IMAGE);
	wxPostEvent(parWindow, evt);
}

void ScaleBrightnessWindow::AddEditToProcessor() {

	ProcessorEdit * BrightEdit = new ProcessorEdit(ProcessorEdit::EditType::SCALE_BRIGHTNESS);
	BrightEdit->AddParam(allBrightSlider->GetValue());
	BrightEdit->AddParam(redBrightSlider->GetValue());
	BrightEdit->AddParam(greenBrightSlider->GetValue());
	BrightEdit->AddParam(blueBrightSlider->GetValue());

	proc->AddEdit(BrightEdit);
}

void ScaleBrightnessWindow::SetParamsAndFlags(ProcessorEdit * edit){

	// Populate sliders based on edit loaded
	if (edit->GetParamsSize() == 4 && edit->GetEditType() == ProcessorEdit::EditType::SHIFT_BRIGHTNESS) {
		allBrightSlider->SetValue(edit->GetParam(0));
		redBrightSlider->SetValue(edit->GetParam(1));
		greenBrightSlider->SetValue(edit->GetParam(2));
		blueBrightSlider->SetValue(edit->GetParam(3));
	}
}
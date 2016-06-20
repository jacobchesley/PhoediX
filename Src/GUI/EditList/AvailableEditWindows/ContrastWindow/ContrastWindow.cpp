#include "ContrastWindow.h"

ContrastWindow::ContrastWindow(wxWindow * parent, wxString editName, Processor * processor) : EditWindow(parent, editName) {

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
	allContrastLabel->SetForegroundColour(Colors::TextLightGrey);
	redContrastLabel->SetForegroundColour(Colors::TextLightGrey);
	greenContrastLabel->SetForegroundColour(Colors::TextLightGrey);
	blueContrastLabel->SetForegroundColour(Colors::TextLightGrey);

	allContrastSlider = new DoubleSlider(this, 1.0, 0.0, 5.0, 100000);
	redContrastSlider = new DoubleSlider(this, 1.0, 0.0, 5.0, 100000);
	greenContrastSlider = new DoubleSlider(this, 1.0, 0.0, 5.0, 100000);
	blueContrastSlider = new DoubleSlider(this, 1.0, 0.0, 5.0, 100000);

	allContrastSlider->SetValuePosition(DoubleSlider::VALUE_INLINE_RIGHT);
	redContrastSlider->SetValuePosition(DoubleSlider::VALUE_INLINE_RIGHT);
	greenContrastSlider->SetValuePosition(DoubleSlider::VALUE_INLINE_RIGHT);
	blueContrastSlider->SetValuePosition(DoubleSlider::VALUE_INLINE_RIGHT);

	processButton = new wxButton(this, EditWindow::ID_PROCESS_EDITS, "Process Edits");

	allContrastSlider->SetForegroundColour(Colors::TextLightGrey);
	allContrastSlider->SetBackgroundColour(parent->GetBackgroundColour());
	redContrastSlider->SetForegroundColour(Colors::TextLightGrey);
	redContrastSlider->SetBackgroundColour(parent->GetBackgroundColour());
	greenContrastSlider->SetForegroundColour(Colors::TextLightGrey);
	greenContrastSlider->SetBackgroundColour(parent->GetBackgroundColour());
	blueContrastSlider->SetForegroundColour(Colors::TextLightGrey);
	blueContrastSlider->SetBackgroundColour(parent->GetBackgroundColour());

	gridSizer->Add(allContrastLabel);
	gridSizer->Add(allContrastSlider);
	gridSizer->Add(redContrastLabel);
	gridSizer->Add(redContrastSlider);
	gridSizer->Add(greenContrastLabel);
	gridSizer->Add(greenContrastSlider);
	gridSizer->Add(blueContrastLabel);
	gridSizer->Add(blueContrastSlider);

	mainSizer->Add(editLabel);
	mainSizer->AddSpacer(10);
	mainSizer->Add(gridSizer);
	mainSizer->AddSpacer(15);
	mainSizer->Add(processButton, 0, wxALIGN_LEFT);

	proc = processor;
	parWindow = parent;

	//this->Bind(wxEVT_SCROLL_CHANGED, (wxObjectEventFunction)&ContrastWindow::Process, this);
	//this->Bind(wxEVT_TEXT_ENTER, (wxObjectEventFunction)&ContrastWindow::Process, this);
	//this->Bind(wxEVT_TEXT, (wxObjectEventFunction)&ContrastWindow::Process, this);
	this->Bind(wxEVT_BUTTON, (wxObjectEventFunction)&ContrastWindow::Process, this, EditWindow::ID_PROCESS_EDITS);

	this->SetSizer(mainSizer);
	this->FitInside();
	this->SetScrollRate(5, 5);

	this->SetClientSize(this->GetVirtualSize());
}

void ContrastWindow::Process(wxCommandEvent& WXUNUSED(event)) {

	wxCommandEvent evt(REPROCESS_IMAGE_EVENT, ID_REPROCESS_IMAGE);
	wxPostEvent(parWindow, evt);
}

void ContrastWindow::AddEditToProcessor() {

	ProcessorEdit * contrastEdit = new ProcessorEdit(ProcessorEdit::EditType::ADJUST_CONTRAST);
	contrastEdit->AddParam(allContrastSlider->GetValue());
	contrastEdit->AddParam(redContrastSlider->GetValue());
	contrastEdit->AddParam(greenContrastSlider->GetValue());
	contrastEdit->AddParam(blueContrastSlider->GetValue());

	proc->AddEdit(contrastEdit);
}
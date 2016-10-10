#include "ShiftRGBWindow.h"

ShiftRGBWindow::ShiftRGBWindow(wxWindow * parent, wxString editName, Processor * processor) : EditWindow(parent, editName, processor) {

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

	processButton = new wxButton(this, EditWindow::ID_PROCESS_EDITS, "Process Edits", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	processButton->SetForegroundColour(Colors::TextLightGrey);
	processButton->SetBackgroundColour(Colors::BackGrey);
	processButton->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

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

	this->Bind(wxEVT_SCROLL_CHANGED, (wxObjectEventFunction)&ShiftRGBWindow::OnUpdate, this);
	this->Bind(wxEVT_SCROLL_THUMBTRACK, (wxObjectEventFunction)&ShiftRGBWindow::OnUpdate, this);
	this->Bind(wxEVT_TEXT_ENTER, (wxObjectEventFunction)&ShiftRGBWindow::OnUpdate, this);

	this->SetSizer(mainSizer);
	this->FitInside();
	this->SetScrollRate(5, 5);

	this->SetClientSize(this->GetVirtualSize());

	this->StartWatchdog();
}

void ShiftRGBWindow::SetParamsAndFlags(ProcessorEdit * edit){

	if(edit == NULL){ return; }
	// Populate sliders based on edit loaded
	if (edit->GetParamsSize() == 4 && edit->GetEditType() == ProcessorEdit::EditType::SHIFT_RGB) {
		allBrightSlider->SetValue(edit->GetParam(0));
		redBrightSlider->SetValue(edit->GetParam(1));
		greenBrightSlider->SetValue(edit->GetParam(2));
		blueBrightSlider->SetValue(edit->GetParam(3));
	}
}

ProcessorEdit * ShiftRGBWindow::GetParamsAndFlags(){

	ProcessorEdit * shiftRgbEdit = new ProcessorEdit(ProcessorEdit::EditType::SHIFT_RGB);
	shiftRgbEdit->AddParam(allBrightSlider->GetValue());
	shiftRgbEdit->AddParam(redBrightSlider->GetValue());
	shiftRgbEdit->AddParam(greenBrightSlider->GetValue());
	shiftRgbEdit->AddParam(blueBrightSlider->GetValue());

	shiftRgbEdit->SetDisabled(isDisabled);

	return shiftRgbEdit;
}

bool ShiftRGBWindow::CheckCopiedParamsAndFlags(){

	ProcessorEdit * edit = proc->GetEditForCopyPaste();
	if(edit == NULL){ return false; }

	if (edit->GetParamsSize() == 4 && edit->GetEditType() == ProcessorEdit::EditType::SHIFT_RGB) {
		return true;
	}
	return false;
}
// Copyright 2016 Jacob Chesley

#include "CropWindow.h"

CropWindow::CropWindow(wxWindow * parent, wxString editName, Processor * processor) : EditWindow(parent, editName, processor) {

	this->SetBackgroundColour(parent->GetBackgroundColour());

	mainSizer = new wxBoxSizer(wxVERTICAL);

	// 2 Columns, 15 pixel vertical gap, 5 pixel horizontal gap
	gridSizer = new wxFlexGridSizer(2, 15, 5);

	editLabel = new wxStaticText(this, -1, editName);
	editLabel->SetForegroundColour(Colors::TextWhite);
	editLabel->SetFont(wxFont(13, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));

	startXLabel = new wxStaticText(this, -1, "Starting X Position");
	startYLabel = new wxStaticText(this, -1, "Starting Y Position");
	widthLabel = new wxStaticText(this, -1, "Width");
	heightLabel = new wxStaticText(this, -1, "Height");
	startXLabel->SetForegroundColour(Colors::TextLightGrey);
	startYLabel->SetForegroundColour(Colors::TextLightGrey);
	widthLabel ->SetForegroundColour(Colors::TextLightGrey);
	heightLabel->SetForegroundColour(Colors::TextLightGrey);

	startXCtrl = new wxTextCtrl(this, -1, "0",  wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
	startYCtrl = new wxTextCtrl(this, -1, "0",  wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
	widthCtrl = new wxTextCtrl(this, -1, wxString::Format(wxT("%i"), processor->GetOriginalImage()->GetWidth()),  wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
	heightCtrl = new wxTextCtrl(this, -1, wxString::Format(wxT("%i"), processor->GetOriginalImage()->GetHeight()),  wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
	
	startXCtrl->SetForegroundColour(Colors::TextLightGrey);
	startYCtrl->SetForegroundColour(Colors::TextLightGrey);
	widthCtrl->SetForegroundColour(Colors::TextLightGrey);
	heightCtrl->SetForegroundColour(Colors::TextLightGrey);

	startXCtrl->SetBackgroundColour(this->GetBackgroundColour());
	startYCtrl->SetBackgroundColour(this->GetBackgroundColour());
	widthCtrl->SetBackgroundColour(this->GetBackgroundColour());
	heightCtrl->SetBackgroundColour(this->GetBackgroundColour());

	enableCrop = new wxButton(this, EditWindow::ID_PROCESS_EDITS, "Enable Crop", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	enableCrop->SetForegroundColour(Colors::TextLightGrey);
	enableCrop->SetBackgroundColour(Colors::BackGrey);
	enableCrop->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	gridSizer->Add(startXLabel);
	gridSizer->Add(startXCtrl);
	gridSizer->Add(startYLabel);
	gridSizer->Add(startYCtrl);
	gridSizer->Add(widthLabel);
	gridSizer->Add(widthCtrl);
	gridSizer->Add(heightLabel);
	gridSizer->Add(heightCtrl);

	mainSizer->Add(editLabel);
	mainSizer->AddSpacer(10);
	mainSizer->Add(gridSizer);
	mainSizer->AddSpacer(15);
	mainSizer->Add(enableCrop, 0, wxALIGN_LEFT);

	proc = processor;
	parWindow = parent;

	this->Bind(wxEVT_TEXT_ENTER, (wxObjectEventFunction)&CropWindow::OnUpdate, this);
	this->Bind(wxEVT_BUTTON, (wxObjectEventFunction)&CropWindow::Process, this, EditWindow::ID_PROCESS_EDITS);

	this->SetSizer(mainSizer);
	this->FitInside();
	this->SetScrollRate(5, 5);

	this->SetClientSize(this->GetVirtualSize());
	this->StartWatchdog();
}

void CropWindow::SetParamsAndFlags(ProcessorEdit * edit){

	if(edit == NULL){ return;}

	if(edit->GetEditType() == ProcessorEdit::EditType::CROP && edit->GetParamsSize() == 4){

		startXCtrl->SetValue(wxString::Format(wxT("%i"),(int)edit->GetParam(0)));
		startYCtrl->SetValue(wxString::Format(wxT("%i"),(int)edit->GetParam(1)));
		widthCtrl->SetValue(wxString::Format(wxT("%i"),(int)edit->GetParam(2)));
		heightCtrl->SetValue(wxString::Format(wxT("%i"),(int)edit->GetParam(3)));
	}
}

ProcessorEdit * CropWindow::GetParamsAndFlags(){

	ProcessorEdit * cropEdit = new ProcessorEdit(ProcessorEdit::EditType::CROP);
	cropEdit->AddParam((double)wxAtoi(startXCtrl->GetValue()));
	cropEdit->AddParam((double)wxAtoi(startYCtrl->GetValue()));
	cropEdit->AddParam((double)wxAtoi(widthCtrl->GetValue()));
	cropEdit->AddParam((double)wxAtoi(heightCtrl->GetValue()));

	// Set enabled / disabled
	cropEdit->SetDisabled(isDisabled);

	return cropEdit;
}

bool CropWindow::CheckCopiedParamsAndFlags(){

	ProcessorEdit * edit = proc->GetEditForCopyPaste();
	if(edit == NULL){ return false;}

	if(edit->GetEditType() == ProcessorEdit::EditType::CROP && edit->GetParamsSize() == 4){
		return true;
	}
	return false;
}
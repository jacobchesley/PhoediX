// Copyright 2016 Jacob Chesley

#include "ScaleWindow.h"

ScaleWindow::ScaleWindow(wxWindow * parent, wxString editName, Processor * processor) : EditWindow(parent, editName, processor) {

	this->SetBackgroundColour(parent->GetBackgroundColour());

	mainSizer = new wxBoxSizer(wxVERTICAL);

	// 2 Columns, 15 pixel vertical gap, 5 pixel horizontal gap
	gridSizer = new wxFlexGridSizer(2, 15, 5);

	editLabel = new wxStaticText(this, -1, editName);
	editLabel->SetForegroundColour(Colors::TextWhite);
	editLabel->SetFont(wxFont(13, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));

	scaleMethodLabel = new wxStaticText(this, -1, "Scale Based On ");
	scaleMethodLabel->SetForegroundColour(Colors::TextLightGrey);
	scaleMethod = new wxComboBox(this, -1);
	scaleMethod->AppendString("Percentage");
	scaleMethod->AppendString("Custom Width & Height");
	scaleMethod->SetBackgroundColour(this->GetBackgroundColour());
	scaleMethod->SetForegroundColour(Colors::TextLightGrey);
	scaleMethod->SetSelection(0);

	scaleInterpolationLabel = new wxStaticText(this, -1, "Interpolation Method");
	scaleInterpolationLabel ->SetForegroundColour(Colors::TextLightGrey);
	
	scaleInterpolation = new wxComboBox(this, -1);
	scaleInterpolation->AppendString("Nearest Neighbor");
	scaleInterpolation->AppendString("Bilinear");
	scaleInterpolation->AppendString("Bicubic");
	scaleInterpolation->SetBackgroundColour(this->GetBackgroundColour());
	scaleInterpolation->SetForegroundColour(Colors::TextLightGrey);
	scaleInterpolation->SetSelection(2);

	scalePercentageLabel = new wxStaticText(this, -1, "Percentage to Scale");
	scalePercentageLabel ->SetForegroundColour(Colors::TextLightGrey);

	scalePercentage = new DoubleSlider(this, 100.0, 1.0, 500.0, 100000, 3);
	scalePercentage->SetValuePosition(DoubleSlider::VALUE_INLINE_RIGHT);
	scalePercentage->SetBackgroundColour(this->GetBackgroundColour());
	scalePercentage->SetForegroundColour(Colors::TextLightGrey);
	
	keepAspectLabel = new wxStaticText(this, -1, "Keep Aspect Ratio");
	keepAspectLabel->SetForegroundColour(Colors::TextLightGrey);

	keepAspect = new wxCheckBox(this, -1, "");
	keepAspect->SetForegroundColour(Colors::TextLightGrey);


	widthLabel = new wxStaticText(this, -1, "Width");
	widthLabel->SetForegroundColour(Colors::TextLightGrey);

	widthCtrl = new wxTextCtrl(this, ScaleWindow::CtrlIDs::WIDTH_ID, wxString::Format(wxT("%i"), processor->GetOriginalImage()->GetWidth()), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
	widthCtrl->SetForegroundColour(Colors::TextLightGrey);
	widthCtrl->SetBackgroundColour(this->GetBackgroundColour());

	heightLabel = new wxStaticText(this, -1, "Height");
	heightLabel->SetForegroundColour(Colors::TextLightGrey);

	heightCtrl = new wxTextCtrl(this, ScaleWindow::CtrlIDs::HEIGHT_ID, wxString::Format(wxT("%i"), processor->GetOriginalImage()->GetHeight()), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
	heightCtrl->SetForegroundColour(Colors::TextLightGrey);
	heightCtrl->SetBackgroundColour(this->GetBackgroundColour());

	gridSizer->Add(scaleMethodLabel);
	gridSizer->Add(scaleMethod);
	gridSizer->Add(scaleInterpolationLabel);
	gridSizer->Add(scaleInterpolation);
	gridSizer->Add(scalePercentageLabel);
	gridSizer->Add(scalePercentage);
	gridSizer->Add(keepAspectLabel);
	gridSizer->Add(keepAspect);
	gridSizer->Add(widthLabel);
	gridSizer->Add(widthCtrl);
	gridSizer->Add(heightLabel);
	gridSizer->Add(heightCtrl);

	mainSizer->Add(editLabel);
	mainSizer->AddSpacer(10);
	mainSizer->Add(gridSizer);

	proc = processor;
	parWindow = parent;

	this->Bind(wxEVT_SCROLL_CHANGED, (wxObjectEventFunction)&ScaleWindow::OnUpdate, this);
	this->Bind(wxEVT_TEXT_ENTER, (wxObjectEventFunction)&ScaleWindow::OnUpdate, this);
	this->Bind(wxEVT_BUTTON, (wxObjectEventFunction)&ScaleWindow::Process, this, EditWindow::ID_PROCESS_EDITS);
	this->Bind(wxEVT_COMBOBOX, (wxObjectEventFunction)&ScaleWindow::OnCombo, this);
	this->Bind(wxEVT_TEXT, (wxObjectEventFunction)&ScaleWindow::OnText, this);

	this->SetSizer(mainSizer);
	this->FitInside();
	this->SetScrollRate(5, 5);

	this->SetClientSize(this->GetVirtualSize());

	wxCommandEvent comboEvt(wxEVT_COMBOBOX, 0);
	wxPostEvent(this, comboEvt);

	this->StartWatchdog();
}

void ScaleWindow::OnCombo(wxCommandEvent& WXUNUSED(event)) {
	if(scaleMethod->GetSelection() == 0){
		scalePercentageLabel->Show();
		scalePercentage->Show();
		
		keepAspectLabel->Hide();
		keepAspect->Hide();
		widthLabel->Hide();
		widthCtrl->Hide();
		heightLabel->Hide();
		heightCtrl->Hide();
	}
	else{
		scalePercentageLabel->Hide();
		scalePercentage->Hide();

		keepAspectLabel->Show();
		keepAspect->Show();
		widthLabel->Show();
		widthCtrl->Show();
		heightLabel->Show();
		heightCtrl->Show();
	}
	this->Layout();
	this->DoProcess();
}

void ScaleWindow::OnText(wxCommandEvent& textEvt) {

	if (!keepAspect->GetValue()) {
		return;
	}
	double aspectRatio = (double)proc->GetOriginalImage()->GetWidth() / (double)proc->GetOriginalImage()->GetHeight();

	// Change height ctrl text based on width ctrl
	if (textEvt.GetId() == ScaleWindow::CtrlIDs::WIDTH_ID) {
		int newHeight = wxAtoi(widthCtrl->GetValue()) / aspectRatio;
		wxString newHeightStr;
		newHeightStr << newHeight;

		this->Unbind(wxEVT_TEXT, (wxObjectEventFunction)&ScaleWindow::OnText, this);
		heightCtrl->SetValue(newHeightStr);
		this->Bind(wxEVT_TEXT, (wxObjectEventFunction)&ScaleWindow::OnText, this);
	}
	
	// Change width ctrl text absed on height ctrl
	else {
		int newWidth = wxAtoi(heightCtrl->GetValue()) * aspectRatio;
		wxString newWidthStr;
		newWidthStr << newWidth;
		this->Unbind(wxEVT_TEXT, (wxObjectEventFunction)&ScaleWindow::OnText, this);
		widthCtrl->SetValue(newWidthStr);
		this->Bind(wxEVT_TEXT, (wxObjectEventFunction)&ScaleWindow::OnText, this);
	}
	textEvt.Skip();
}

void ScaleWindow::SetParamsAndFlags(ProcessorEdit * edit) {
	if(edit == NULL){ return; }

	if(edit->GetEditType() == ProcessorEdit::EditType::SCALE_NEAREST){
		if(edit->GetParamsSize() == 2 && edit->GetFlagsSize() == 2){

			scaleInterpolation->SetSelection(0);
			// Scale based on percentage
			if(edit->GetFlag(0) == 0){

				scaleMethod->SetSelection(0);
				if(edit->GetFlag(1) == 1){ keepAspect->SetValue(true); } else { keepAspect->SetValue(false); }

				// Fire combo box event to show / hide sliders
				wxCommandEvent comboEvt(wxEVT_COMBOBOX);
				wxPostEvent(this, comboEvt);

				double scaleWidth = (double)edit->GetParam(0);
				double percentage = 100.0 * scaleWidth / (double)proc->GetOriginalImage()->GetWidth();
				scalePercentage->SetValue(percentage);
			}
			// Scale based custom width and height
			else{
				scaleMethod->SetSelection(1);
				// Fire combo box event to show / hide sliders
				wxCommandEvent comboEvt(wxEVT_COMBOBOX);
				wxPostEvent(this, comboEvt);

				widthCtrl->SetValue(wxString::Format(wxT("%i"),(int)edit->GetParam(0)));
				heightCtrl->SetValue(wxString::Format(wxT("%i"),(int)edit->GetParam(1)));
			}

		}
	}

	if(edit->GetEditType() == ProcessorEdit::EditType::SCALE_BILINEAR){
		if(edit->GetParamsSize() == 2 && edit->GetFlagsSize() == 2){

			scaleInterpolation->SetSelection(1);

			// Scale based on percentage
			if(edit->GetFlag(0) == 0){

				scaleMethod->SetSelection(0);
				if(edit->GetFlag(1) == 1){ keepAspect->SetValue(true); } else { keepAspect->SetValue(false); }

				// Fire combo box event to show / hide sliders
				wxCommandEvent comboEvt(wxEVT_COMBOBOX);
				wxPostEvent(this, comboEvt);

				double scaleWidth = (double)edit->GetParam(0);
				double percentage = 100.0 * scaleWidth / (double)proc->GetOriginalImage()->GetWidth();
				scalePercentage->SetValue(percentage);
			}
			// Scale based custom width and height
			else{
				scaleMethod->SetSelection(1);
				// Fire combo box event to show / hide sliders
				wxCommandEvent comboEvt(wxEVT_COMBOBOX);
				wxPostEvent(this, comboEvt);

				widthCtrl->SetValue(wxString::Format(wxT("%i"),(int)edit->GetParam(0)));
				heightCtrl->SetValue(wxString::Format(wxT("%i"),(int)edit->GetParam(1)));
			}

		}
	}

	if(edit->GetEditType() == ProcessorEdit::EditType::SCALE_BICUBIC){
		if(edit->GetParamsSize() == 2 && edit->GetFlagsSize() == 2){

			scaleInterpolation->SetSelection(2);

			// Scale based on percentage
			if(edit->GetFlag(0) == 0){

				scaleMethod->SetSelection(0);
				if(edit->GetFlag(1) == 1){ keepAspect->SetValue(true); } else { keepAspect->SetValue(false); }

				// Fire combo box event to show / hide sliders
				wxCommandEvent comboEvt(wxEVT_COMBOBOX);
				wxPostEvent(this, comboEvt);

				double scaleWidth = (double)edit->GetParam(0);
				double percentage = 100.0 * scaleWidth / (double)proc->GetOriginalImage()->GetWidth();
				scalePercentage->SetValue(percentage);
			}
			// Scale based custom width and height
			else{
				scaleMethod->SetSelection(1);

				// Fire combo box event to show / hide sliders
				wxCommandEvent comboEvt(wxEVT_COMBOBOX);
				wxPostEvent(this, comboEvt);

				widthCtrl->SetValue(wxString::Format(wxT("%i"),(int)edit->GetParam(0)));
				heightCtrl->SetValue(wxString::Format(wxT("%i"),(int)edit->GetParam(1)));
			}

		}
	}
}

ProcessorEdit * ScaleWindow::GetParamsAndFlags(){

	// Nearest Nieghbor selection
	if (scaleInterpolation->GetSelection() == 0) {
	
		ProcessorEdit * scaleEdit = new ProcessorEdit(ProcessorEdit::EditType::SCALE_NEAREST);

		// Use a percentage to scale width and height
		if(scaleMethod->GetSelection() == 0){
			double scaleVal = scalePercentage->GetValue() / 100.0;
			scaleEdit->AddParam(proc->GetOriginalImage()->GetWidth() * scaleVal);
			scaleEdit->AddParam(proc->GetOriginalImage()->GetHeight() * scaleVal);
			scaleEdit->AddFlag(scaleMethod->GetSelection());
			scaleEdit->AddFlag(keepAspect->GetValue());

			// Set enabled / disabled
			scaleEdit->SetDisabled(isDisabled);

			return scaleEdit;
		}

		// Use custom width and height values
		else{
			scaleEdit->AddParam((double)wxAtoi(widthCtrl->GetValue()));
			scaleEdit->AddParam((double)wxAtoi(heightCtrl->GetValue()));
			scaleEdit->AddFlag(scaleMethod->GetSelection());
			scaleEdit->AddFlag(keepAspect->GetValue());

			// Set enabled / disabled
			scaleEdit->SetDisabled(isDisabled);

			return scaleEdit;
		}
	}

	// Bilinear Selection
	if (scaleInterpolation->GetSelection() == 1) {
	
		ProcessorEdit * scaleEdit = new ProcessorEdit(ProcessorEdit::EditType::SCALE_BILINEAR);

		// Use a percentage to scale width and height
		if(scaleMethod->GetSelection() == 0){
			double scaleVal = scalePercentage->GetValue() / 100.0;
			scaleEdit->AddParam(proc->GetOriginalImage()->GetWidth() * scaleVal);
			scaleEdit->AddParam(proc->GetOriginalImage()->GetHeight() * scaleVal);
			scaleEdit->AddFlag(scaleMethod->GetSelection());
			scaleEdit->AddFlag(keepAspect->GetValue());

			// Set enabled / disabled
			scaleEdit->SetDisabled(isDisabled);

			return scaleEdit;
		}

		// Use custom width and height values
		else{
			scaleEdit->AddParam((double)wxAtoi(widthCtrl->GetValue()));
			scaleEdit->AddParam((double)wxAtoi(heightCtrl->GetValue()));
			scaleEdit->AddFlag(scaleMethod->GetSelection());
			scaleEdit->AddFlag(keepAspect->GetValue());

			// Set enabled / disabled
			scaleEdit->SetDisabled(isDisabled);

			return scaleEdit;
		}
	}

	// Bicubic Selection
	if (scaleInterpolation->GetSelection() == 2) {
	
		ProcessorEdit * scaleEdit = new ProcessorEdit(ProcessorEdit::EditType::SCALE_BICUBIC);

		// Use a percentage to scale width and height
		if(scaleMethod->GetSelection() == 0){
			double scaleVal = scalePercentage->GetValue() / 100.0;
			scaleEdit->AddParam(proc->GetOriginalImage()->GetWidth() * scaleVal);
			scaleEdit->AddParam(proc->GetOriginalImage()->GetHeight() * scaleVal);
			scaleEdit->AddFlag(scaleMethod->GetSelection());
			scaleEdit->AddFlag(keepAspect->GetValue());

			// Set enabled / disabled
			scaleEdit->SetDisabled(isDisabled);

			return scaleEdit;
		}

		// Use custom width and height values
		else{
			scaleEdit->AddParam((double)wxAtoi(widthCtrl->GetValue()));
			scaleEdit->AddParam((double)wxAtoi(heightCtrl->GetValue()));
			scaleEdit->AddFlag(scaleMethod->GetSelection());
			scaleEdit->AddFlag(keepAspect->GetValue());

			// Set enabled / disabled
			scaleEdit->SetDisabled(isDisabled);

			return scaleEdit;
		}
	}
	return NULL;
}

bool ScaleWindow::CheckCopiedParamsAndFlags(){

	ProcessorEdit * edit = proc->GetEditForCopyPaste();
	if(edit == NULL){ return false;}

	if(edit->GetEditType() == ProcessorEdit::EditType::SCALE_NEAREST ||
		edit->GetEditType() == ProcessorEdit::EditType::SCALE_BILINEAR ||
		edit->GetEditType() == ProcessorEdit::EditType::SCALE_BICUBIC){

			if(edit->GetFlagsSize() == 2 && edit->GetParamsSize() == 2){
				return true;
			}

	}
	return false;
}
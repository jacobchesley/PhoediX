// Copyright 2018 Jacob Chesley.
// See LICENSE.TXT in root of project for license information.

#include "CropWindow.h"

CropWindow::CropWindow(wxWindow * parent, wxString editName, Processor * processor, ZoomImagePanel * imgPanel) : EditWindow(parent, editName, processor, imgPanel) {

	this->SetBackgroundColour(parent->GetBackgroundColour());

	mainSizer = new wxBoxSizer(wxVERTICAL);
	buttonSizer = new wxBoxSizer(wxHORIZONTAL);

	// 2 Columns, 15 pixel vertical gap, 5 pixel horizontal gap
	gridSizer = new wxFlexGridSizer(2, 15, 5);

	editLabel = new wxStaticText(this, -1, editName);
	editLabel->SetForegroundColour(Colors::TextWhite);
	editLabel->SetFont(wxFont(13, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));

	aspectsSizer = new wxBoxSizer(wxHORIZONTAL);
	customAspectSizer = new wxBoxSizer(wxHORIZONTAL);
	defaultAspects = new PhoediXComboBox(this, -1);
	defaultAspects->AppendString("No Aspect Constraint");
	defaultAspects->AppendString("4 X 6");
	defaultAspects->AppendString("5 X 7");
	defaultAspects->AppendString("6 X 8");
	defaultAspects->AppendString("8 X 10");
	defaultAspects->AppendString("8 X 12");
	defaultAspects->AppendString("10 X 12");
	defaultAspects->AppendString("11 X 14");
	defaultAspects->AppendString("11 X 17");
	defaultAspects->AppendString("12 X 15");
	defaultAspects->AppendString("12 X 16");
	defaultAspects->AppendString("12 X 18");
	defaultAspects->AppendString("13 X 19");
	defaultAspects->AppendString("Custom");
	defaultAspects->SetSelection(0);

	defaultAspects->SetForegroundColour(Colors::TextLightGrey);
	defaultAspects->SetBackgroundColour(this->GetBackgroundColour());
	defaultAspects->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	flipAspects = new PhoediXButton(this, CropWindow::ID_FLIP_ASPECTS, "Flip Aspect");
	flipAspects->SetForegroundColour(Colors::TextLightGrey);
	flipAspects->SetBackgroundColour(Colors::BackGrey);
	flipAspects->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	aspectsSizer->Add(defaultAspects);
	aspectsSizer->AddSpacer(10);
	aspectsSizer->Add(flipAspects);

	customAspectWidth = new wxTextCtrl(this, -1, "1.0");
	customAspectX = new wxStaticText(this, -1, "X");
	customAspectHeight = new wxTextCtrl(this, -1, "1.0");

	customAspectWidth->SetForegroundColour(Colors::TextLightGrey);
	customAspectWidth->SetBackgroundColour(this->GetBackgroundColour());
	customAspectWidth->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
	
	customAspectX->SetForegroundColour(Colors::TextLightGrey);
	customAspectX->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	customAspectHeight->SetForegroundColour(Colors::TextLightGrey);
	customAspectHeight->SetBackgroundColour(this->GetBackgroundColour());
	customAspectHeight->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	customAspectSizer->Add(customAspectWidth);
	customAspectSizer->AddSpacer(10);
	customAspectSizer->Add(customAspectX);
	customAspectSizer->AddSpacer(10);
	customAspectSizer->Add(customAspectHeight);

	customAspectWidth->Hide();
	customAspectX->Hide();
	customAspectHeight->Hide();

	enableCropBox = new PhoediXButton(this, CropWindow::ID_ENABLE_CROP_BOX, "Enable Grid Lines");
	enableCropBox->SetForegroundColour(Colors::TextLightGrey);
	enableCropBox->SetDisableForegroundColour(Colors::TextGrey);
	enableCropBox->SetBackgroundColour(Colors::BackGrey);
	enableCropBox->SetDisableBackgroundColour(Colors::BackGrey);
	enableCropBox->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
	boxEnabled = false;

	enableDisableCrop = new PhoediXButton(this, CropWindow::ID_ENABLE_CROP, "Enable Crop");
	enableDisableCrop->SetForegroundColour(Colors::TextLightGrey);
	enableDisableCrop->SetBackgroundColour(Colors::BackGrey);
	enableDisableCrop->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
	cropEnabled = false;

	resetCrop = new PhoediXButton(this, CropWindow::ID_RESET_CROP, "Reset Crop");
	resetCrop->SetForegroundColour(Colors::TextLightGrey);
	resetCrop->SetBackgroundColour(Colors::BackGrey);
	resetCrop->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	buttonSizer->Add(enableCropBox);
	buttonSizer->AddSpacer(20);
	buttonSizer->Add(enableDisableCrop);
	buttonSizer->AddSpacer(40);
	buttonSizer->Add(resetCrop);

	mainSizer->Add(editLabel);
	mainSizer->AddSpacer(20);
	mainSizer->Add(aspectsSizer);
	mainSizer->AddSpacer(10);
	mainSizer->Add(customAspectSizer);
	mainSizer->AddSpacer(20);
	mainSizer->Add(gridSizer);
	mainSizer->AddSpacer(10);
	mainSizer->Add(buttonSizer);

	proc = processor;
	parWindow = parent;

	this->Bind(wxEVT_TEXT_ENTER, (wxObjectEventFunction)&CropWindow::OnUpdate, this);
	this->Bind(wxEVT_TEXT, (wxObjectEventFunction)&CropWindow::OnText, this);
	this->Bind(wxEVT_COMBOBOX, (wxObjectEventFunction)&CropWindow::OnCombo, this);
	this->Bind(wxEVT_BUTTON, (wxObjectEventFunction)&CropWindow::OnEnableBox, this, CropWindow::ID_ENABLE_CROP_BOX);
	this->Bind(wxEVT_BUTTON, (wxObjectEventFunction)&CropWindow::OnEnableCrop, this, CropWindow::ID_ENABLE_CROP);
	this->Bind(wxEVT_BUTTON, (wxObjectEventFunction)&CropWindow::OnResetCrop, this, CropWindow::ID_RESET_CROP);
	this->Bind(wxEVT_BUTTON, (wxObjectEventFunction)&CropWindow::OnFlipAspect, this, CropWindow::ID_FLIP_ASPECTS);
	this->Bind(GRID_MOVE_EVENT, (wxObjectEventFunction)&CropWindow::OnGridMove, this);
	this->SetSizer(mainSizer);
	this->FitInside();
	this->SetScrollRate(5, 5);

	this->SetClientSize(this->GetVirtualSize());

	cropEnabled = false;
	forceAspect = false;
	cropGrid.startX = 0.0;
	cropGrid.endX = 1.0;
	cropGrid.startY = 0.0;
	cropGrid.endY = 1.0;

	aspectFlipped = false;
}

void CropWindow::DestroyEditWindow() {
	this->EnableBox(false);
	EditWindow::DestroyEditWindow();
}

void CropWindow::ChangeAspect(bool gridUpdate){

	cropGrid.startX = this->GetZoomImagePanel()->GetGrid().startX;
	cropGrid.startY = this->GetZoomImagePanel()->GetGrid().startY;
	cropGrid.endX = this->GetZoomImagePanel()->GetGrid().endX;
	cropGrid.endY = this->GetZoomImagePanel()->GetGrid().endY;

	// No aspect constraint
	if (defaultAspects->GetSelection() == 0) {
		if (this->GetId() == this->GetZoomImagePanel()->GetGridOwner()) {
			this->GetZoomImagePanel()->SetEnforceGridAspect(false);
			forceAspect = false;
		}

		customAspectWidth->Hide();
		customAspectX->Hide();
		customAspectHeight->Hide();
	}

	// Custom constraint
	else if (defaultAspects->GetSelection() == 13) {

		customAspectHeight->Show();
		customAspectX->Show();
		customAspectWidth->Show();

		double numerator = 0.0;
		double denominator = 0.0;

		customAspectWidth->GetValue().ToDouble(&numerator);
		customAspectHeight->GetValue().ToDouble(&denominator);

		if (numerator == 0.0) { numerator = 0.0001; }
		if (denominator == 0.0) { denominator = 0.0001; }

		aspect = numerator / denominator;

		this->GetZoomImagePanel()->SetGridAspect(aspect);
		this->GetZoomImagePanel()->SetEnforceGridAspect(true);
		if(gridUpdate){
			this->GetZoomImagePanel()->SetGrid(this->GetNewAspectGrid(aspect));	
		}
	}

	// Default aspect constraint
	else {

		customAspectWidth->Hide();
		customAspectX->Hide();
		customAspectHeight->Hide();

		wxString aspectString = defaultAspects->GetStringSelection();

		wxStringTokenizer aspectTokens(aspectString, "X");

		if (aspectTokens.CountTokens() == 2) {

			wxString aspectNumerator = aspectTokens.GetNextToken();
			wxString aspectDenominator = aspectTokens.GetNextToken();

			double numerator = 0.0;
			double denominator = 0.0;

			aspectNumerator.ToDouble(&numerator);
			aspectDenominator.ToDouble(&denominator);
			if (numerator == 0.0) { numerator = 0.0001; }
			if (denominator == 0.0) { denominator = 0.0001; }

			aspect = numerator / denominator;

			this->GetZoomImagePanel()->SetGridAspect(aspect);
			this->GetZoomImagePanel()->SetEnforceGridAspect(true);
			if(gridUpdate){
				this->GetZoomImagePanel()->SetGrid(this->GetNewAspectGrid(aspect));
			}
		}
	}
	this->SetClientSize(this->GetVirtualSize());
	this->Refresh();
	this->Update();
	
}

Grid CropWindow::GetNewAspectGrid(double aspectRatio) {
	Grid retGrid;
	retGrid.startX = cropGrid.startX;
	retGrid.startY = cropGrid.startY;
	retGrid.endX = cropGrid.endX;
	retGrid.endY = cropGrid.endY;

	double currentWidth = cropGrid.endX - cropGrid.startX;
	double currentHeight = cropGrid.endY - cropGrid.startY;

	double imgAspect = this->GetZoomImagePanel()->GetImageAspect();

	if (aspectRatio < 1.0) {
		retGrid.endX = (currentHeight * aspectRatio / imgAspect) + retGrid.startX;
	}
	else {
		retGrid.endY = (currentWidth / aspectRatio * imgAspect) + retGrid.startY;
	}

	return retGrid;
}

void CropWindow::OnCombo(wxCommandEvent& WXUNUSED(evt)) {
	this->ChangeAspect();	
}

void CropWindow::OnFlipAspect(wxCommandEvent& WXUNUSED(evt)) {
	this->FlipAspect();
}

void CropWindow::FlipAspect(){

	wxArrayString values = defaultAspects->GetStrings();

	// Flip all values in the default aspect constraints
	for(int i = 0; i < values.size(); i++){

		// Flip left and right strings of X
		if(values[i].Contains(" X ")){
			size_t xLocation = values[i].find("X");
			wxString leftString = values[i].substr(0, xLocation - 1);
			wxString rightString = values[i].substr(xLocation + 2, values[i].length());
			defaultAspects->SetString(i, rightString + " X " + leftString);
		}
	}

	// Flip values on custom aspect control
	wxString currentCustomWidth = customAspectWidth->GetValue();
	customAspectWidth->SetValue(customAspectHeight->GetValue());
	customAspectHeight->SetValue(currentCustomWidth);

	this->ChangeAspect();	
	aspectFlipped = !aspectFlipped;
}

void CropWindow::OnEnableBox(wxCommandEvent& WXUNUSED(evt)) {

	if(cropEnabled) { return; }
	boxEnabled = !boxEnabled;
	this->EnableBox(boxEnabled);
}

void CropWindow::EnableBox(bool enable) {

	if (enable) {
		this->GetZoomImagePanel()->SetGridOwner((int)this->GetId());
		this->GetZoomImagePanel()->SetGrid(cropGrid);
		this->GetZoomImagePanel()->ActivateGrid();
		enableCropBox->SetLabel("Disable Grid Lines");
	}
	else {
		cropGrid.startX = this->GetZoomImagePanel()->GetGrid().startX;
		cropGrid.startY = this->GetZoomImagePanel()->GetGrid().startY;
		cropGrid.endX = this->GetZoomImagePanel()->GetGrid().endX;
		cropGrid.endY = this->GetZoomImagePanel()->GetGrid().endY;
		this->GetZoomImagePanel()->SetGridOwner(-1);
		this->GetZoomImagePanel()->DeactivateGrid();
		enableCropBox->SetLabel("Enable Grid Lines");
	}
}

void CropWindow::OnEnableCrop(wxCommandEvent& WXUNUSED(evt)) {

	this->EnableCrop(!cropEnabled);
}

void CropWindow::EnableCrop(bool enable) {

	cropEnabled = enable;
	if(this->GetZoomImagePanel()->GetGridOwner() == this->GetId()){
		cropGrid = this->GetZoomImagePanel()->GetGrid();
	}
	if(enable) {

		// Grey out text for grid lines, can not enable while cropping
		enableDisableCrop->SetLabel("Disable Crop");
		enableCropBox->SetForegroundColour(Colors::TextGrey);
		enableCropBox->Disable();
		if(boxEnabled) {

			// Deactive grid lines
			this->GetZoomImagePanel()->SetGridOwner(-1);
			this->GetZoomImagePanel()->DeactivateGrid();
			boxEnabled = false;
			enableCropBox->SetLabel("Enable Grid Lines");
		}
	}
	else {
		enableDisableCrop->SetLabel("Enable Crop");
		enableCropBox->SetForegroundColour(Colors::TextLightGrey);
		enableCropBox->Enable();
	}

	this->SetUpdated(true);
}

void CropWindow::OnResetCrop(wxCommandEvent& WXUNUSED(evt)) {
	cropGrid.startX = 0.0;
	cropGrid.endX = 1.0;
	cropGrid.startY = 0.0;
	cropGrid.endY = 1.0;
	this->GetZoomImagePanel()->SetGrid(cropGrid);
	this->SetUpdated(true);
}

void CropWindow::OnGridMove(wxCommandEvent& WXUNUSED(evt)) {

	if (this->GetId() == this->GetZoomImagePanel()->GetGridOwner()) {
		cropGrid = this->GetZoomImagePanel()->GetGrid();
		this->SaveNoReprocess();
	}
}

void CropWindow::ResetCropValues() {

}

void CropWindow::SetParamsAndFlags(ProcessorEdit * edit){

	if(edit == NULL){ return;}
	if(edit->GetEditType() == ProcessorEdit::EditType::CROP){

		if (edit->CheckForParameter(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_NUMERATOR)) { customAspectWidth->SetValue(wxString::Format(wxT("%f"), edit->GetParam(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_NUMERATOR))); }
		if (edit->CheckForParameter(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_DENOMINATOR)) { customAspectHeight->SetValue(wxString::Format(wxT("%f"), edit->GetParam(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_DENOMINATOR))); }
		if (edit->CheckForFlag(ProcessorEdit::ParametersFlags::PHOEDIX_FLAG_FLIP_ASPECT)) { if (edit->GetFlag(ProcessorEdit::ParametersFlags::PHOEDIX_FLAG_FLIP_ASPECT) == 1) { this->FlipAspect(); } }
		if (edit->CheckForFlag(ProcessorEdit::ParametersFlags::PHOEDIX_FLAG_ASPECT_SELECTION)) { defaultAspects->SetSelection(edit->GetFlag(ProcessorEdit::ParametersFlags::PHOEDIX_FLAG_ASPECT_SELECTION)); this->ChangeAspect(); }

		// Get start and end positions
		if (edit->CheckForParameter(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_STARTX)) { cropGrid.startX = edit->GetParam(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_STARTX); }
		if (edit->CheckForParameter(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_STARTY)) { cropGrid.startY = edit->GetParam(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_STARTY); }
		if (edit->CheckForParameter(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_CROP_WIDTH)) { cropGrid.endX = edit->GetParam(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_CROP_WIDTH) + cropGrid.startX; }
		if (edit->CheckForParameter(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_CROP_HEIGHT)) { cropGrid.endY = edit->GetParam(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_CROP_HEIGHT) + cropGrid.startY; }

		int oldGridOwner = this->GetZoomImagePanel()->GetGridOwner();
		Grid oldGrid = this->GetZoomImagePanel()->GetGrid();

		// Set crop grid temporarly
		this->GetZoomImagePanel()->SetGridOwner((int)this->GetId());
		this->GetZoomImagePanel()->SetGrid(cropGrid);
		
		if (edit->CheckForFlag(ProcessorEdit::ParametersFlags::PHOEDIX_FLAG_CROP_ENABLED)) {
			if (edit->GetFlag(ProcessorEdit::ParametersFlags::PHOEDIX_FLAG_CROP_ENABLED) == 0) { this->EnableCrop(false); }
			if (edit->GetFlag(ProcessorEdit::ParametersFlags::PHOEDIX_FLAG_CROP_ENABLED) == 1) { this->EnableCrop(true); }
		}

		// Show / hide grid lines
		if (edit->CheckForFlag(ProcessorEdit::ParametersFlags::PHOEDIX_FLAG_BOX_ENABLED)) { 
			boxEnabled = (bool)edit->GetFlag(ProcessorEdit::ParametersFlags::PHOEDIX_FLAG_BOX_ENABLED);
			this->EnableBox(boxEnabled); 
		}

		this->SetUpdated(true);

		// If this crop edit does not have control of the grid, return control to previous owner
		if (!boxEnabled) {
			this->GetZoomImagePanel()->SetGridOwner(oldGridOwner);
			this->GetZoomImagePanel()->SetGrid(oldGrid);
		}
	}
}

ProcessorEdit * CropWindow::GetParamsAndFlags(){

	double numerator = 0.0;
	double denominator = 0.0;
	customAspectWidth->GetValue().ToDouble(&numerator);
	customAspectHeight->GetValue().ToDouble(&denominator);

	// Create crop edit with crop parameters
	ProcessorEdit * cropEdit = new ProcessorEdit(ProcessorEdit::EditType::CROP);

	cropEdit->AddParam(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_STARTX, cropGrid.startX);
	cropEdit->AddParam(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_STARTY, cropGrid.startY);
	cropEdit->AddParam(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_CROP_WIDTH, cropGrid.endX - cropGrid.startX);
	cropEdit->AddParam(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_CROP_HEIGHT, cropGrid.endY - cropGrid.startY);
	cropEdit->AddParam(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_NUMERATOR, numerator);
	cropEdit->AddParam(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_DENOMINATOR, denominator);

	cropEdit->AddFlag(ProcessorEdit::ParametersFlags::PHOEDIX_FLAG_ASPECT_SELECTION, defaultAspects->GetSelection());
	cropEdit->AddFlag(ProcessorEdit::ParametersFlags::PHOEDIX_FLAG_CROP_ENABLED, (int)cropEnabled);
	cropEdit->AddFlag(ProcessorEdit::ParametersFlags::PHOEDIX_FLAG_FLIP_ASPECT, (int)aspectFlipped);
	cropEdit->AddFlag(ProcessorEdit::ParametersFlags::PHOEDIX_FLAG_BOX_ENABLED, (int)boxEnabled);

	// Set enabled / disabled
	cropEdit->SetDisabled(isDisabled);
	return cropEdit;
}

bool CropWindow::CheckCopiedParamsAndFlags(){

	ProcessorEdit * edit = proc->GetEditForCopyPaste();
	if(edit == NULL){ return false;}

	if(edit->GetEditType() == ProcessorEdit::EditType::CROP){
		return true;
	}
	return false;
}

void CropWindow::OnText(wxCommandEvent& WXUNUSED(evt)) {
	this->ChangeAspect();
}

Grid CropWindow::limitGrid(Grid inGrid) {

	return inGrid;
}
// Copyright 2016 Jacob Chesley

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

	customAspectSizer = new wxBoxSizer(wxHORIZONTAL);
	defaultAspects = new wxComboBox(this, -1);
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

	int curProcesseedWidth = 0;
	int curProcesseedHeight = 0;
	this->GetProcessor()->CalcualteWidthHeightEdits(this->GetPreviousEdits(), &curProcesseedWidth, &curProcesseedHeight);
	widthCtrl = new wxTextCtrl(this, -1, wxString::Format(wxT("%i"), curProcesseedWidth),  wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
	heightCtrl = new wxTextCtrl(this, -1, wxString::Format(wxT("%i"), curProcesseedHeight),  wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
	
	startXCtrl->SetForegroundColour(Colors::TextLightGrey);
	startYCtrl->SetForegroundColour(Colors::TextLightGrey);
	widthCtrl->SetForegroundColour(Colors::TextLightGrey);
	heightCtrl->SetForegroundColour(Colors::TextLightGrey);

	startXCtrl->SetBackgroundColour(this->GetBackgroundColour());
	startYCtrl->SetBackgroundColour(this->GetBackgroundColour());
	widthCtrl->SetBackgroundColour(this->GetBackgroundColour());
	heightCtrl->SetBackgroundColour(this->GetBackgroundColour());

	enableCropBox = new wxButton(this, CropWindow::ID_ENABLE_CROP_BOX, "Enable Grid Lines", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	enableCropBox->SetForegroundColour(Colors::TextLightGrey);
	enableCropBox->SetBackgroundColour(Colors::BackGrey);
	enableCropBox->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
	boxEnabled = false;

	enableDisableCrop = new wxButton(this, CropWindow::ID_ENABLE_CROP, "Enable Crop", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	enableDisableCrop->SetForegroundColour(Colors::TextLightGrey);
	enableDisableCrop->SetBackgroundColour(Colors::BackGrey);
	enableDisableCrop->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
	cropEnabled = false;

	resetCrop = new wxButton(this, CropWindow::ID_RESET_CROP, "Reset Crop", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	resetCrop->SetForegroundColour(Colors::TextLightGrey);
	resetCrop->SetBackgroundColour(Colors::BackGrey);
	resetCrop->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	gridSizer->Add(startXLabel);
	gridSizer->Add(startXCtrl);
	gridSizer->Add(startYLabel);
	gridSizer->Add(startYCtrl);
	gridSizer->Add(widthLabel);
	gridSizer->Add(widthCtrl);
	gridSizer->Add(heightLabel);
	gridSizer->Add(heightCtrl);

	buttonSizer->Add(enableCropBox);
	buttonSizer->AddSpacer(20);
	buttonSizer->Add(enableDisableCrop);
	buttonSizer->AddSpacer(40);
	buttonSizer->Add(resetCrop);

	mainSizer->Add(editLabel);
	mainSizer->AddSpacer(20);
	mainSizer->Add(defaultAspects);
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
	this->Bind(GRID_MOVE_EVENT, (wxObjectEventFunction)&CropWindow::OnGridMove, this, ID_GRID_MOVE_EVENT);
	this->SetSizer(mainSizer);
	this->FitInside();
	this->SetScrollRate(5, 5);

	this->SetClientSize(this->GetVirtualSize());
	this->StartWatchdog();

	cropEnabled = false;
	forceAspect = false;
}

void CropWindow::OnCombo(wxCommandEvent& WXUNUSED(evt)) {

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
		}
	}

	this->FitInside();
}

void CropWindow::OnEnableBox(wxCommandEvent& WXUNUSED(evt)) {

	if (cropEnabled) { return; }

	boxEnabled = !boxEnabled;

	if (boxEnabled) {
		Grid cropGrid;
		cropGrid.startX = wxAtoi(startXCtrl->GetValue());
		cropGrid.startY = wxAtoi(startYCtrl->GetValue());
		cropGrid.width = wxAtoi(widthCtrl->GetValue());
		cropGrid.height = wxAtoi(heightCtrl->GetValue());

		this->GetZoomImagePanel()->SetGridOwner((int)this->GetId());
		this->GetZoomImagePanel()->SetGrid(cropGrid);
		this->GetZoomImagePanel()->ActivateGrid();
		enableCropBox->SetLabel("Disable Grid Lines");
	}
	else {
		this->GetZoomImagePanel()->SetGridOwner(-1);
		this->GetZoomImagePanel()->DeactivateGrid();
		enableCropBox->SetLabel("Enable Grid Lines");
	}
}

void CropWindow::OnEnableCrop(wxCommandEvent& WXUNUSED(evt)) {
	cropEnabled = !cropEnabled;

	if (cropEnabled) {

		// Grey out text for grid lines, can not enable while cropping
		enableDisableCrop->SetLabel("Disable Crop");
		enableCropBox->SetForegroundColour(Colors::TextGrey);
		if (boxEnabled) {

			// Deactive grid lines
			this->GetZoomImagePanel()->SetGridOwner(-1);
			this->GetZoomImagePanel()->DeactivateGrid();
			boxEnabled = false;
			enableCropBox->SetLabel("Enable Grid Lines");
		}

		// Disable text boxes, cannot edit crop params while cropping
		startXCtrl->SetForegroundColour(Colors::TextGrey);
		startYCtrl->SetForegroundColour(Colors::TextGrey);
		widthCtrl->SetForegroundColour(Colors::TextGrey);
		heightCtrl->SetForegroundColour(Colors::TextGrey);

		startXCtrl->Refresh(); 
		startYCtrl->Refresh();
		widthCtrl->Refresh();
		heightCtrl->Refresh();
	}
	else {
		// Set label of crop edit and restore grid lines text color
		enableDisableCrop->SetLabel("Enable Crop");
		enableCropBox->SetForegroundColour(Colors::TextLightGrey);

		// Show text boxes can be edited again
		startXCtrl->SetForegroundColour(Colors::TextLightGrey);
		startYCtrl->SetForegroundColour(Colors::TextLightGrey);
		widthCtrl->SetForegroundColour(Colors::TextLightGrey);
		heightCtrl->SetForegroundColour(Colors::TextLightGrey);

		startXCtrl->Refresh();
		startYCtrl->Refresh();
		widthCtrl->Refresh();
		heightCtrl->Refresh();
	}
	this->SetUpdated(true);
}

void CropWindow::OnResetCrop(wxCommandEvent& WXUNUSED(evt)) {
	this->ResetCropValues();
	this->SetUpdated(true);
}

void CropWindow::ResetCropValues() {

	int curProcesseedWidth = 0;
	int curProcesseedHeight = 0;
	this->GetProcessor()->CalcualteWidthHeightEdits(this->GetPreviousEdits(), &curProcesseedWidth, &curProcesseedHeight);

	startXCtrl->SetValue(wxString::Format(wxT("%i"), 0));
	startYCtrl->SetValue(wxString::Format(wxT("%i"), 0));
	widthCtrl->SetValue(wxString::Format(wxT("%i"), curProcesseedWidth));
	heightCtrl->SetValue(wxString::Format(wxT("%i"), curProcesseedHeight));

	// Set grid size of image panel 
	if (boxEnabled) {
		Grid cropGrid;
		cropGrid.startX = wxAtoi(startXCtrl->GetValue());
		cropGrid.startY = wxAtoi(startYCtrl->GetValue());
		cropGrid.width = wxAtoi(widthCtrl->GetValue());
		cropGrid.height = wxAtoi(heightCtrl->GetValue());

		if (this->GetId() == this->GetZoomImagePanel()->GetGridOwner()) {
			this->GetZoomImagePanel()->SetGrid(cropGrid);
		}
	}
}

void CropWindow::SetParamsAndFlags(ProcessorEdit * edit){

	if(edit == NULL){ return;}
	if(edit->GetEditType() == ProcessorEdit::EditType::CROP && edit->GetParamsSize() == 4){

		// Populate text box values
		this->SetCropBoxUI((int) edit->GetParam(0), (int)edit->GetParam(1), (int)edit->GetParam(2), (int)edit->GetParam(3));
	}
}

ProcessorEdit * CropWindow::GetParamsAndFlags(){

	if (cropEnabled) {

		// Create crop edit with crop parameters
		ProcessorEdit * cropEdit = new ProcessorEdit(ProcessorEdit::EditType::CROP);
		cropEdit->AddParam((double)wxAtoi(startXCtrl->GetValue()));
		cropEdit->AddParam((double)wxAtoi(startYCtrl->GetValue()));
		cropEdit->AddParam((double)wxAtoi(widthCtrl->GetValue()));
		cropEdit->AddParam((double)wxAtoi(heightCtrl->GetValue()));
			
		lastCropGrid.startX = (int)cropEdit->GetParam(0);
		lastCropGrid.startY = (int)cropEdit->GetParam(1);
		lastCropGrid.width = (int)cropEdit->GetParam(2);
		lastCropGrid.height = (int)cropEdit->GetParam(3);

		// Set enabled / disabled
		cropEdit->SetDisabled(isDisabled);
		return cropEdit;
	}
	else {

		// Create crop edit with full image size
		ProcessorEdit * cropEdit = new ProcessorEdit(ProcessorEdit::EditType::CROP);
		cropEdit->AddParam((double)0.0);
		cropEdit->AddParam((double)0.0);
		int curProcesseedWidth = 0;
		int curProcesseedHeight = 0;
		this->GetProcessor()->CalcualteWidthHeightEdits(this->GetPreviousEdits(), &curProcesseedWidth, &curProcesseedHeight);
		cropEdit->AddParam((double)curProcesseedWidth);
		cropEdit->AddParam((double)curProcesseedHeight);

		// Set enabled / disabled
		cropEdit->SetDisabled(isDisabled);
		return cropEdit;
	}
}

bool CropWindow::CheckCopiedParamsAndFlags(){

	ProcessorEdit * edit = proc->GetEditForCopyPaste();
	if(edit == NULL){ return false;}

	if(edit->GetEditType() == ProcessorEdit::EditType::CROP && edit->GetParamsSize() == 4){
		return true;
	}
	return false;
}

void CropWindow::SetCropBoxUI(int startX, int startY, int width, int height) {

	// Set crop box values
	startXCtrl->SetValue(wxString::Format(wxT("%i"), startX));
	startYCtrl->SetValue(wxString::Format(wxT("%i"), startY));
	widthCtrl->SetValue(wxString::Format(wxT("%i"), width));
	heightCtrl->SetValue(wxString::Format(wxT("%i"), height));
}

void CropWindow::OnGridMove(wxCommandEvent& WXUNUSED(evt)) {

	// Set text box text based on image panel grid
	if (this->GetId() == this->GetZoomImagePanel()->GetGridOwner()) {

		Grid cropGrid = this->GetZoomImagePanel()->GetGrid();
		{wxEventBlocker blocker(this);
			this->SetCropBoxUI(cropGrid.startX, cropGrid.startY, cropGrid.width, cropGrid.height);
		}
	}
}

void CropWindow::OnText(wxCommandEvent& evt) {

	if (cropEnabled) {

		// Block text event and set values of text boxes of edit size.  
		// Disables editing of text boxes while cropping.  Using 
		// Disable method changes color of text and background
		{wxEventBlocker blocker(this);
			this->SetCropBoxUI(lastCropGrid.startX, lastCropGrid.startY, lastCropGrid.width, lastCropGrid.height);
		}
	}

	// Get crop values
	Grid cropGrid;
	cropGrid.startX = wxAtoi(startXCtrl->GetValue());
	cropGrid.startY = wxAtoi(startYCtrl->GetValue());
	cropGrid.width = wxAtoi(widthCtrl->GetValue());
	cropGrid.height= wxAtoi(heightCtrl->GetValue());

	// Handle aspect constraints
	if (defaultAspects->GetSelection() != 0) {

		// Width control changed, update height based on width
		if (evt.GetId() == widthCtrl->GetId()) {

			cropGrid.height = cropGrid.width / aspect;
			{wxEventBlocker blocker(this);
				heightCtrl->SetValue(wxString::Format(wxT("%i"), cropGrid.height));
			}
		}

		// Height control changed, update width based on height
		if (evt.GetId() == heightCtrl->GetId()) {

			cropGrid.width = cropGrid.height * aspect;
			{wxEventBlocker blocker(this);
				widthCtrl->SetValue(wxString::Format(wxT("%i"), cropGrid.width));
			}
		}
	}

	Grid limitGrid = this->limitGrid(cropGrid);

	// Block text event and set values of text boxes of limit size
	{wxEventBlocker blocker(this);
		this->SetCropBoxUI(limitGrid.startX, limitGrid.startY, limitGrid.width, limitGrid.height);
	}

	// Set grid size if crop edit owns it
	if (this->GetId() == this->GetZoomImagePanel()->GetGridOwner()) {
		this->GetZoomImagePanel()->SetGrid(limitGrid);
	}
	
}

Grid CropWindow::limitGrid(Grid inGrid) {

	// Calculate width and height of image with edits so far
	int curProcesseedWidth = 0;
	int curProcesseedHeight = 0;
	this->GetProcessor()->CalcualteWidthHeightEdits(this->GetPreviousEdits(), &curProcesseedWidth, &curProcesseedHeight);

	// Set crop params to proper values if they are not already
	if (inGrid.startX < 0) { inGrid.startX = 0; }
	if (inGrid.startY < 0) { inGrid.startY = 0; }
	if (inGrid.width < 1) { inGrid.width = 1; }
	if (inGrid.height < 1) { inGrid.height = 1; }

	// Set crop start X and Y to be inside image size
	if (inGrid.startX > curProcesseedWidth) { inGrid.startX = curProcesseedWidth - 1; }
	if (inGrid.startY > curProcesseedHeight) { inGrid.startY = curProcesseedHeight - 1; }

	// Set width and height to valid values
	if ((inGrid.startX + inGrid.width) > curProcesseedWidth) { 

		inGrid.width = curProcesseedWidth - inGrid.startX; 
		
		// Adjust height based on new width if needed
		if (defaultAspects->GetSelection() != 0) {
			inGrid.height = inGrid.width / aspect;
		}
	}
	if ((inGrid.startY + inGrid.height) > curProcesseedHeight) { 

		inGrid.height = curProcesseedHeight - inGrid.startY;

		// Adjust width based on new height if needed
		if (defaultAspects->GetSelection() != 0) {
			inGrid.width = inGrid.height * aspect;
		}
	}
	if (inGrid.width < 1) { inGrid.width = 1; }
	if (inGrid.height < 1) { inGrid.height = 1; }

	return inGrid;
}
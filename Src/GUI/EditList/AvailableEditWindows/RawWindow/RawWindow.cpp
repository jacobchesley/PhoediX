// Copyright 2018 Jacob Chesley.
// See LICENSE.TXT in root of project for license information.

#include "RawWindow.h"

RawWindow::RawWindow(wxWindow * parent, wxString editName, Processor * processor, ZoomImagePanel * imgPanel) : EditWindow(parent, editName, processor, imgPanel) {

	parWindow = parent;
	proc = processor;

	defaultSat = proc->rawPrcoessor.imgdata.rawdata.color.maximum > 0 ? proc->rawPrcoessor.imgdata.rawdata.color.maximum : 1;
	defaultBlack = proc->rawPrcoessor.imgdata.rawdata.color.black;

	this->SetBackgroundColour(parent->GetBackgroundColour());

	mainSizer = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(mainSizer);

	dontProcessNexrGammaEvt = false;

	// --------------------------- RAW Settings panel, sizer and collapse pane --------------------------- 
	settingsPanelCollapse = new CollapsiblePane(this, "RAW Settings");
	settingsSizer = new wxFlexGridSizer(2, 15, 5);
	settingsPanel = new wxPanel(settingsPanelCollapse);
	settingsPanel->SetSizer(settingsSizer);
	settingsPanelCollapse->AttachWindow(settingsPanel);
	settingsPanelCollapse->SetTextBackgroundColour(this->GetBackgroundColour());
	settingsPanelCollapse->SetTextForegroundColour(Colors::TextLightGrey);
	settingsPanelCollapse->SetTextFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));

	// Four Color
	greenMatchingLabel = new wxStaticText(settingsPanel, -1, "Green Matching");
	greenMatchingLabel->SetForegroundColour(Colors::TextLightGrey);
	greenMatchingControl = new wxCheckBox(settingsPanel, -1, "");
	settingsPanel->GetSizer()->Add(greenMatchingLabel);
	settingsPanel->GetSizer()->Add(greenMatchingControl);

	// Interpolation
	interpolationLabel = new wxStaticText(settingsPanel, -1, "Interpolation");
	interpolationLabel->SetForegroundColour(Colors::TextLightGrey);
	interpolationControl = new PhoediXComboBox(settingsPanel, -1);
	interpolationControl->SetBackgroundColour(Colors::BackDarkDarkGrey);
	interpolationControl->SetForegroundColour(Colors::TextLightGrey);
	interpolationControl->Append("Linear");
	interpolationControl->Append("VNG");
	interpolationControl->Append("PPG");
	interpolationControl->Append("AHD");
	interpolationControl->Append("DCB");
	interpolationControl->SetSelection(3);
	settingsPanel->GetSizer()->Add(interpolationLabel);
	settingsPanel->GetSizer()->Add(interpolationControl);
	lastInterpolationVal = interpolationControl->GetSelection();

	// Flip
	flipLabel = new wxStaticText(settingsPanel, -1, "Rotation");
	flipLabel->SetForegroundColour(Colors::TextLightGrey);
	flipControl = new PhoediXComboBox(settingsPanel, -1);
	flipControl->SetBackgroundColour(Colors::BackDarkDarkGrey);
	flipControl->SetForegroundColour(Colors::TextLightGrey);
	flipControl->Append("Camera Defined");
	flipControl->Append("No Rotation");
	flipControl->Append("90 Deg. Clockwise");
	flipControl->Append("180 Deg.");
	flipControl->Append("270 Deg. Clockwise");
	flipControl->SetSelection(0);
	settingsPanel->GetSizer()->Add(flipLabel);
	settingsPanel->GetSizer()->Add(flipControl);

	// ---------------------------  RAW exposure labels and controls --------------------------- 
	exposurePanelCollapse = new CollapsiblePane(this, "Exposure");
	exposureSizer = new wxFlexGridSizer(2, 15, 5);
	exposurePanel = new wxPanel(exposurePanelCollapse);
	exposurePanel->SetSizer(exposureSizer);
	exposurePanelCollapse->AttachWindow(exposurePanel);
	exposurePanelCollapse->SetTextBackgroundColour(this->GetBackgroundColour());
	exposurePanelCollapse->SetTextForegroundColour(Colors::TextLightGrey);
	exposurePanelCollapse->SetTextFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));

	// Brightness
	brightnessLabel = new wxStaticText(exposurePanel, -1, "Brightness");
	brightnessLabel->SetForegroundColour(Colors::TextLightGrey);
	brightnessControl = new DoubleSlider(exposurePanel, 1.0, 0.0, 2.0, 100000, 3);
	this->FormatSlider(brightnessControl);
	exposurePanel->GetSizer()->Add(brightnessLabel);
	exposurePanel->GetSizer()->Add(brightnessControl);

	// Exposure
	exposureLabel = new wxStaticText(exposurePanel, -1, "Exposure");
	exposureLabel->SetForegroundColour(Colors::TextLightGrey);
	exposureControl = new DoubleSlider(exposurePanel, 0.0, -2.0, 3.0, 100000, 3);
	this->FormatSlider(exposureControl);
	exposurePanel->GetSizer()->Add(exposureLabel);
	exposurePanel->GetSizer()->Add(exposureControl);

	// Exposure Highlight Preservation
	exposurePreserveLabel = new wxStaticText(exposurePanel, -1, "Highlights");
	exposurePreserveLabel->SetForegroundColour(Colors::TextLightGrey);
	exposurePreserveControl = new DoubleSlider(exposurePanel, 0.0, 0.0, 1.0, 100000, 3);
	this->FormatSlider(exposurePreserveControl);
	exposurePanel->GetSizer()->Add(exposurePreserveLabel);
	exposurePanel->GetSizer()->Add(exposurePreserveControl);

	// Highlights
	highlightLabel = new wxStaticText(exposurePanel, -1, "Highlight Mode");
	highlightControl = new PhoediXComboBox(exposurePanel, -1);
	highlightLabel->SetForegroundColour(Colors::TextLightGrey);
	highlightControl->SetBackgroundColour(Colors::BackDarkDarkGrey);
	highlightControl->SetForegroundColour(Colors::TextLightGrey);
	highlightControl->AppendString("Clip"); // 0 
	highlightControl->AppendString("Unclip"); // 1
	highlightControl->AppendString("Blend"); // 2
	highlightControl->AppendString("Rebuild - 1"); // 3
	highlightControl->AppendString("Rebuild - 2"); // 4
	highlightControl->AppendString("Rebuild - 3"); // 5
	highlightControl->AppendString("Rebuild - 4"); // 6
	highlightControl->AppendString("Rebuild - 5"); // 7
	highlightControl->AppendString("Rebuild - 6"); // 8
	highlightControl->AppendString("Rebuild - 7"); // 9
	highlightControl->SetSelection(0);
	exposurePanel->GetSizer()->Add(highlightLabel);
	exposurePanel->GetSizer()->Add(highlightControl);

	// Saturation Level
	satLevelLabel = new wxStaticText(exposurePanel, -1, "Saturation Level");
	satLevelLabel->SetForegroundColour(Colors::TextLightGrey);
	satLevelControl = new DoubleSlider(exposurePanel, (double)defaultSat, 0.0, (double)defaultSat, defaultSat, 0);
	this->FormatSlider(satLevelControl);
	exposurePanel->GetSizer()->Add(satLevelLabel);
	exposurePanel->GetSizer()->Add(satLevelControl);

	// Auto bright 
	autoBrightLabel = new wxStaticText(exposurePanel, -1, "Auto Bright Adj.");
	autoBrightLabel->SetForegroundColour(Colors::TextLightGrey);
	autoBrightControl = new wxCheckBox(exposurePanel, ID_AUTO_BRIGHT_CHECK, "");
	autoBrightControl->SetValue(true);
	exposurePanel->GetSizer()->Add(autoBrightLabel);
	exposurePanel->GetSizer()->Add(autoBrightControl);

	autoBrightThrLabel = new wxStaticText(exposurePanel, -1, "Auto Bright Threshold");
	autoBrightThrLabel->SetForegroundColour(Colors::TextLightGrey);
	autoBrightThrControl = new DoubleSlider(exposurePanel, 0.01, 0.00003, 0.1, 500000, 5);
	this->FormatSlider(autoBrightThrControl);
	exposurePanel->GetSizer()->Add(autoBrightThrLabel);
	exposurePanel->GetSizer()->Add(autoBrightThrControl);

	// Maximum Threshold
	maxThrLabel = new wxStaticText(exposurePanel, -1, "Maximum Threshold");
	maxThrLabel->SetForegroundColour(Colors::TextLightGrey);
	maxThrControl = new DoubleSlider(exposurePanel, 0.75, 0.0, 1.0, 500000, 3);
	this->FormatSlider(maxThrControl);
	exposurePanel->GetSizer()->Add(maxThrLabel);
	exposurePanel->GetSizer()->Add(maxThrControl);

	// Gamma
	gammaPresetsLabel = new wxStaticText(exposurePanel, -1, "Gamma Presets");
	gammaPresetsControl = new PhoediXComboBox(exposurePanel, ID_GAMMA_PRESET_COMBO);
	gammaPresetsLabel->SetForegroundColour(Colors::TextLightGrey);
	gammaPresetsControl->SetBackgroundColour(Colors::BackDarkDarkGrey);
	gammaPresetsControl->SetForegroundColour(Colors::TextLightGrey);
	gammaPresetsControl->AppendString("BT.709");
	gammaPresetsControl->AppendString("sRGB");
	gammaPresetsControl->AppendString("Linear");
	gammaPresetsControl->AppendString("Custom");
	gammaPresetsControl->SetSelection(0);
	exposurePanel->GetSizer()->Add(gammaPresetsLabel);
	exposurePanel->GetSizer()->Add(gammaPresetsControl);

	gammaLevelLabel = new wxStaticText(exposurePanel, -1, "Inverted Gamma Val");
	gammaLevelLabel->SetForegroundColour(Colors::TextLightGrey);
	gammaLevelControl = new DoubleSlider(exposurePanel, 2.222, 0.01, 5.0, 100000, 3);
	gammaLevelControl->SetId(ID_GAMMA_LEVEL_SLIDER);
	lastGammaLevel = gammaLevelControl->GetValue();
	this->FormatSlider(gammaLevelControl);
	exposurePanel->GetSizer()->Add(gammaLevelLabel);
	exposurePanel->GetSizer()->Add(gammaLevelControl);

	gammaSlopeLabel = new wxStaticText(exposurePanel, -1, "Gamma Slope");
	gammaSlopeLabel->SetForegroundColour(Colors::TextLightGrey);
	gammaSlopeControl = new DoubleSlider(exposurePanel, 4.5, 1.0, 15.0, 100000, 3);
	gammaSlopeControl->SetId(ID_GAMMA_SLOPE_SLIDER);
	lastGammaSlope = gammaSlopeControl->GetValue();
	this->FormatSlider(gammaSlopeControl);
	exposurePanel->GetSizer()->Add(gammaSlopeLabel);
	exposurePanel->GetSizer()->Add(gammaSlopeControl);

	// ---------------------------  RAW color labels and controls --------------------------- 
	colorPanelCollapse = new CollapsiblePane(this, "Color");
	colorSizer = new wxFlexGridSizer(2, 15, 5);
	colorPanel = new wxPanel(colorPanelCollapse);
	colorPanel->SetSizer(colorSizer);
	colorPanelCollapse->AttachWindow(colorPanel);
	colorPanelCollapse->SetTextBackgroundColour(this->GetBackgroundColour());
	colorPanelCollapse->SetTextForegroundColour(Colors::TextLightGrey);
	colorPanelCollapse->SetTextFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));

	// White Balance
	whiteBalancePresetsLabel = new wxStaticText(colorPanel, -1, "White Balance");
	whiteBalancePresetsControl = new PhoediXComboBox(colorPanel, ID_WHITE_BALANCE_COMBO);
	whiteBalancePresetsLabel->SetForegroundColour(Colors::TextLightGrey);
	whiteBalancePresetsControl->SetBackgroundColour(Colors::BackDarkDarkGrey);
	whiteBalancePresetsControl->SetForegroundColour(Colors::TextLightGrey);
	whiteBalancePresetsControl->AppendString("Use Camera WB");
	whiteBalancePresetsControl->AppendString("Use Auto WB");
	whiteBalancePresetsControl->AppendString("Tungsten");
	whiteBalancePresetsControl->AppendString("Flourescent");
	whiteBalancePresetsControl->AppendString("Daylight");
	whiteBalancePresetsControl->AppendString("Shade");
	whiteBalancePresetsControl->AppendString("Cloudy");
	whiteBalancePresetsControl->AppendString("Flash");
	whiteBalancePresetsControl->AppendString("Custom");
	whiteBalancePresetsControl->SetSelection(0);
	colorPanel->GetSizer()->Add(whiteBalancePresetsLabel);
	colorPanel->GetSizer()->Add(whiteBalancePresetsControl);

	redMultiplierLabel = new wxStaticText(colorPanel, -1, "Red WB Multiplier");
	redMultiplierLabel->SetForegroundColour(Colors::TextLightGrey);
	redMultiplierControl = new DoubleSlider(colorPanel, 1.0, 0.0, 3.0, 100000, 3);
	redMultiplierControl->SetId(ID_RED_MULTIPLIER_SLIDER);
	this->FormatSlider(redMultiplierControl);
	colorPanel->GetSizer()->Add(redMultiplierLabel);
	colorPanel->GetSizer()->Add(redMultiplierControl);

	greenMultiplierLabel = new wxStaticText(colorPanel, -1, "Green WB Multiplier");
	greenMultiplierLabel->SetForegroundColour(Colors::TextLightGrey);
	greenMultiplierControl = new DoubleSlider(colorPanel, 1.0, 0.0, 3.0, 100000, 3);
	greenMultiplierControl->SetId(ID_GREEN_MULTIPLIER_SLIDER);
	this->FormatSlider(greenMultiplierControl);
	colorPanel->GetSizer()->Add(greenMultiplierLabel);
	colorPanel->GetSizer()->Add(greenMultiplierControl);

	blueMultiplierLabel = new wxStaticText(colorPanel, -1, "Blue WB Multiplier");
	blueMultiplierLabel->SetForegroundColour(Colors::TextLightGrey);
	blueMultiplierControl = new DoubleSlider(colorPanel, 1.0, 0.0, 3.0, 100000, 3);
	blueMultiplierControl->SetId(ID_BLUE_MULTIPLIER_SLIDER);
	this->FormatSlider(blueMultiplierControl);
	colorPanel->GetSizer()->Add(blueMultiplierLabel);
	colorPanel->GetSizer()->Add(blueMultiplierControl);

	// ---------------------------  RAW noise labels and controls --------------------------- 
	noisePanelCollapse = new CollapsiblePane(this, "Noise Reduction");
	noiseSizer = new wxFlexGridSizer(2, 15, 5);
	noisePanel = new wxPanel(noisePanelCollapse);
	noisePanel->SetSizer(noiseSizer);
	noisePanelCollapse->AttachWindow(noisePanel);
	noisePanelCollapse->SetTextBackgroundColour(this->GetBackgroundColour());
	noisePanelCollapse->SetTextForegroundColour(Colors::TextLightGrey);
	noisePanelCollapse->SetTextFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));

	// Wavelet noise reduction
	waveletNoiseLabel = new wxStaticText(noisePanel, -1, "Wavelet NR Threshold");
	waveletNoiseLabel->SetForegroundColour(Colors::TextLightGrey);
	waveletNoiseControl = new DoubleSlider(noisePanel, 0.0, 0.0, 1000.0, 1000, 0);
	this->FormatSlider(waveletNoiseControl);
	noisePanel->GetSizer()->Add(waveletNoiseLabel);
	noisePanel->GetSizer()->Add(waveletNoiseControl);
		
	// ---------------------------  RAW info labels and controls --------------------------- 
	infoPanelCollapse = new CollapsiblePane(this, " RAW Information");
	infoPanelCollapse->SetTextBackgroundColour(this->GetBackgroundColour());
	infoPanelCollapse->SetTextForegroundColour(Colors::TextLightGrey);
	infoPanelCollapse->SetTextFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));

	infoList = new wxListCtrl(infoPanelCollapse, -1, wxDefaultPosition, wxDefaultSize, wxLC_REPORT);
	infoList->SetBackgroundColour(this->GetBackgroundColour());
	infoList->SetForegroundColour(Colors::TextLightGrey);

	infoList->InsertColumn(0, "");
	infoList->InsertColumn(1, "");

	this->PopulateRawInfo();
	
	infoPanelCollapse->AttachWindow(infoList);
	
	// Add all collapsable panes to this sizer
	this->GetSizer()->Add(settingsPanelCollapse, 0, wxGROW | wxALL, 5);
	this->GetSizer()->Add(exposurePanelCollapse, 0, wxGROW | wxALL, 5);
	this->GetSizer()->Add(colorPanelCollapse, 0, wxGROW | wxALL, 5);
	this->GetSizer()->Add(noisePanelCollapse, 0, wxGROW | wxALL, 5);
	this->GetSizer()->Add(infoPanelCollapse, 1, wxEXPAND | wxBOTTOM, 5);

	// Collapse all panes for clean look
	settingsPanelCollapse->Collapse();
	exposurePanelCollapse->Collapse();
	colorPanelCollapse->Collapse();
	noisePanelCollapse->Collapse();
	infoPanelCollapse->Collapse();

	this->FitInside();
	this->SetScrollRate(5, 5);
	this->SetClientSize(this->GetVirtualSize());

	this->Bind(wxEVT_SCROLL_THUMBRELEASE, (wxObjectEventFunction)&RawWindow::OnSlide, this);
	this->Bind(wxEVT_TEXT_ENTER, (wxObjectEventFunction)&RawWindow::OnSlide, this);
	this->Bind(wxEVT_COMBOBOX, (wxObjectEventFunction)&RawWindow::OnCombo, this);
	this->Bind(wxEVT_CHECKBOX, (wxObjectEventFunction)&RawWindow::OnCheck, this);
	this->Bind(COLLAPSE_PANE_OPEN_CLOSE, (wxObjectEventFunction)&RawWindow::OnCollapseOpenClose, this, ID_COLLAPSE_PANE_OPEN_CLOSE);
	this->Bind(REPROCESS_IMAGE_EVENT, (wxObjectEventFunction)&RawWindow::ProcessEvt, this, EditWindow::ID_PROCESS_EDITS);
	
	this->StartWatchdog();

	wxCommandEvent comboEvt(wxEVT_COMBOBOX, ID_WHITE_BALANCE_COMBO);
	wxPostEvent(this, comboEvt);
}

void RawWindow::PopulateRawInfo(){
	this->AddRawInfo("File Name", proc->GetFileName());
	this->AddRawInfo("Date Time", ctime(&proc->rawPrcoessor.imgdata.other.timestamp));
	this->AddRawInfo("Artist", wxString(proc->rawPrcoessor.imgdata.other.artist));
	this->AddRawInfo("Description", wxString(proc->rawPrcoessor.imgdata.other.desc));
	this->AddRawInfo("Shot Order (Image Serial)", wxString::Format(wxT("%i"), proc->rawPrcoessor.imgdata.other.shot_order));

	this->AddRawInfo("", "");
	// 90 degree rotation clockwise or counter clockwise.  Flip width and height
	if (proc->rawPrcoessor.imgdata.sizes.flip == 5 || proc->rawPrcoessor.imgdata.sizes.flip == 6) {
		this->AddRawInfo("Width", wxString::Format(wxT("%i"), proc->rawPrcoessor.imgdata.sizes.iheight));
		this->AddRawInfo("Height", wxString::Format(wxT("%i"), proc->rawPrcoessor.imgdata.sizes.iwidth));
	}
	else {
		this->AddRawInfo("Width", wxString::Format(wxT("%i"), proc->rawPrcoessor.imgdata.sizes.iwidth));
		this->AddRawInfo("Height", wxString::Format(wxT("%i"), proc->rawPrcoessor.imgdata.sizes.iheight));
	}
	this->AddRawInfo("", "");
	this->AddRawInfo("Shutter Speed", this->GetShutterSpeedStr(proc->rawPrcoessor.imgdata.other.shutter) + " sec");
	this->AddRawInfo("Aperature", "f / " + wxString::Format(wxT("%.1f"), proc->rawPrcoessor.imgdata.other.aperture));
	this->AddRawInfo("ISO", wxString::Format(wxT("%.0f"), proc->rawPrcoessor.imgdata.other.iso_speed));
	this->AddRawInfo("Focal Length", wxString::Format(wxT("%.1f"), proc->rawPrcoessor.imgdata.other.focal_len) + " mm");

	wxString flashFired = "No";
	if(proc->rawPrcoessor.imgdata.color.flash_used > 0.0){
		flashFired = "Yes";
	}
	this->AddRawInfo("Flash Fired?", flashFired);

	this->AddRawInfo("", "");
	this->AddRawInfo("Camera Make", wxString(proc->rawPrcoessor.imgdata.idata.make));
	this->AddRawInfo("Camera Model", wxString(proc->rawPrcoessor.imgdata.idata.model));
	this->AddRawInfo("Software", wxString(proc->rawPrcoessor.imgdata.idata.software));

	this->AddRawInfo("", "");
	this->AddRawInfo("Lens Make", wxString(proc->rawPrcoessor.imgdata.lens.LensMake), true);
	this->AddRawInfo("Lens Model", wxString(proc->rawPrcoessor.imgdata.lens.Lens), true);

	if(!this->AddRawInfo("Lens Min Focal Length", wxString::Format(wxT("%.0f"), proc->rawPrcoessor.imgdata.lens.makernotes.MinFocal), true)){
		this->AddRawInfo("Lens Min Focal Length", wxString::Format(wxT("%.0f"), proc->rawPrcoessor.imgdata.lens.MinFocal), true);
	}

	if(!this->AddRawInfo("Lens Max Focal Length", wxString::Format(wxT("%.0f"), proc->rawPrcoessor.imgdata.lens.makernotes.MaxFocal), true)){
		this->AddRawInfo("Lens Max Focal Length", wxString::Format(wxT("%.0f"), proc->rawPrcoessor.imgdata.lens.MaxFocal), true);
	}

	if(!this->AddRawInfo("Lens Max Aperature", wxString::Format(wxT("%.1f"), proc->rawPrcoessor.imgdata.lens.makernotes.MaxAp), true)){}
	if(!this->AddRawInfo("Lens Min Aperature", wxString::Format(wxT("%.1f"), proc->rawPrcoessor.imgdata.lens.makernotes.MinAp), true)){}

	if(!this->AddRawInfo("Lens Max Aperature / Min Focal", wxString::Format(wxT("%.1f"), proc->rawPrcoessor.imgdata.lens.makernotes.MaxAp4MinFocal), true)){
		this->AddRawInfo("Lens Max Aperature / Min Focal", wxString::Format(wxT("%.1f"), proc->rawPrcoessor.imgdata.lens.MaxAp4MinFocal), true);
	}
	if(!this->AddRawInfo("Lens Max Aperature / Max Focal", wxString::Format(wxT("%.1f"), proc->rawPrcoessor.imgdata.lens.makernotes.MaxAp4MaxFocal), true)){
		this->AddRawInfo("Lens Max Aperature / Max Focal", wxString::Format(wxT("%.1f"), proc->rawPrcoessor.imgdata.lens.MaxAp4MaxFocal), true);
	}

	if(!this->AddRawInfo("Lens Max Aperature / Cur Focal", wxString::Format(wxT("%.1f"), proc->rawPrcoessor.imgdata.lens.makernotes.MaxAp4CurFocal), true)){}
	if(!this->AddRawInfo("Lens Min Aperature / Min Focal", wxString::Format(wxT("%.1f"), proc->rawPrcoessor.imgdata.lens.makernotes.MinAp4MinFocal), true)){}
	if(!this->AddRawInfo("Lens Min Aperature / Max Focal", wxString::Format(wxT("%.1f"), proc->rawPrcoessor.imgdata.lens.makernotes.MinAp4MaxFocal), true)){}
	if(!this->AddRawInfo("Lens Min Aperature / Cur Focal", wxString::Format(wxT("%.1f"), proc->rawPrcoessor.imgdata.lens.makernotes.MinAp4CurFocal), true)){}

}

bool RawWindow::AddRawInfo(wxString tag, wxString value, bool doNotAddZero){

	if(doNotAddZero){
		if(value == "" || value == "0" || value == "0.0" || value == "0.00" || value == "0.000"){
			return false;
		}
	}
	long index = infoList->InsertItem(infoList->GetItemCount(), tag);
	infoList->SetItem(index, 1, value);

	// Size each column to the max size needed to show all the text
	infoList->SetColumnWidth(0, wxLIST_AUTOSIZE);
	infoList->SetColumnWidth(1, wxLIST_AUTOSIZE);

	return true;
}

wxString RawWindow::GetShutterSpeedStr(float shutterSpeed){

	if(shutterSpeed > 1.0){
		return wxString::Format(wxT("%f"), shutterSpeed);
	}

	for(int i = 1; i < 50001; i++){
		if((1.0f / (float)i) == shutterSpeed){
			return wxString("1/" + wxString::Format(wxT("%i"), i));
		}
	}

	return wxString::Format(wxT("%f"), shutterSpeed);
}

void RawWindow::OnCheck(wxCommandEvent& checkEvent) {

	// Auto brightness
	if (checkEvent.GetId() == ID_AUTO_BRIGHT_CHECK) {
		if (autoBrightControl->GetValue()) {
			autoBrightThrControl->Show();
			autoBrightThrLabel->Show();
		}
		else {
			autoBrightThrControl->Hide();
			autoBrightThrLabel->Hide();
		}
	}

	this->FitInside();
	this->SetUpdated(true);
}

void RawWindow::OnCombo(wxCommandEvent& checkEvent) {

	// Gamma Preset 
	if (checkEvent.GetId() == ID_GAMMA_PRESET_COMBO) {


		switch (gammaPresetsControl->GetSelection()) {

			// BT.709
			case 0:
				gammaLevelControl->SetValue(2.222);
				gammaSlopeControl->SetValue(4.5);
				break;

			// sRGB
			case 1:
				gammaLevelControl->SetValue(2.2);
				gammaSlopeControl->SetValue(12.92);
				break;

			// Linear
			case 2:
				gammaLevelControl->SetValue(1.0);
				gammaSlopeControl->SetValue(1.0);
				break;

			// Custom
			case 3:

				dontProcessNexrGammaEvt = true;
				// Load the last used custom gamma values
				gammaLevelControl->SetValue(lastGammaLevel);
				gammaSlopeControl->SetValue(lastGammaSlope);
				break;
		}
	}

	// White Balance Preset
	if (checkEvent.GetId() == ID_WHITE_BALANCE_COMBO) {

		redMultiplierLabel->Show();
		greenMultiplierLabel->Show();
		blueMultiplierLabel->Show();


		redMultiplierControl->Show();
		greenMultiplierControl->Show();
		blueMultiplierControl->Show();

		redMultiplierControl->Enable();
		greenMultiplierControl->Enable();
		blueMultiplierControl->Enable();

		switch (whiteBalancePresetsControl->GetSelection()) {

			// Camera WB
			case 0:{

				double redMult = proc->rawPrcoessor.imgdata.color.cam_mul[0];
				double greenMult = proc->rawPrcoessor.imgdata.color.cam_mul[1];
				double blueMult = proc->rawPrcoessor.imgdata.color.cam_mul[2];

				redMultiplierControl->SetValue(redMult / greenMult);
				greenMultiplierControl->SetValue(1.0);
				blueMultiplierControl->SetValue(blueMult / greenMult);

				redMultiplierControl->Disable();
				greenMultiplierControl->Disable();
				blueMultiplierControl->Disable();

				break;
			}

			// Auto WB
			case 1:

				redMultiplierLabel->Hide();
				greenMultiplierLabel->Hide();
				blueMultiplierLabel->Hide();

				redMultiplierControl->Hide();
				greenMultiplierControl->Hide();
				blueMultiplierControl->Hide();

				redMultiplierControl->Disable();
				greenMultiplierControl->Disable();
				blueMultiplierControl->Disable();
				break;

			// Tungsten
			case 2:
				redMultiplierControl->SetValue(1.392498);
				greenMultiplierControl->SetValue(1.0);
				blueMultiplierControl->SetValue(2.375114);

				redMultiplierControl->Disable();
				greenMultiplierControl->Disable();
				blueMultiplierControl->Disable();

				break;

			// Flourescent
			case 3:
				redMultiplierControl->SetValue(1.783446);
				greenMultiplierControl->SetValue(1.0);
				blueMultiplierControl->SetValue(1.997113);

				redMultiplierControl->Disable();
				greenMultiplierControl->Disable();
				blueMultiplierControl->Disable();

				break;

			// Daylight
			case 4:
				redMultiplierControl->SetValue(2.132483);
				greenMultiplierControl->SetValue(1.0);
				blueMultiplierControl->SetValue(1.480864);

				redMultiplierControl->Disable();
				greenMultiplierControl->Disable();
				blueMultiplierControl->Disable();

				break;

			// Shade
			case 5:
				redMultiplierControl->SetValue(2.531894);
				greenMultiplierControl->SetValue(1.0);
				blueMultiplierControl->SetValue(1.223749);

				redMultiplierControl->Disable();
				greenMultiplierControl->Disable();
				blueMultiplierControl->Disable();
				break;

			// Cloudy
			case 6:
				redMultiplierControl->SetValue(2.336605);
				greenMultiplierControl->SetValue(1.0);
				blueMultiplierControl->SetValue(1.334642);

				redMultiplierControl->Disable();
				greenMultiplierControl->Disable();
				blueMultiplierControl->Disable();

				break;

			// Flash
			case 7:
				redMultiplierControl->SetValue(2.429833);
				greenMultiplierControl->SetValue(1.0);
				blueMultiplierControl->SetValue(1.284593);

				redMultiplierControl->Disable();
				greenMultiplierControl->Disable();
				blueMultiplierControl->Disable();

				break;
		}
	}

	this->FitInside();
	this->SetUpdated(true);
}

void RawWindow::OnSlide(wxCommandEvent& slideEvt) {
	
	if (slideEvt.GetId() == ID_GAMMA_LEVEL_SLIDER || slideEvt.GetId() == ID_GAMMA_SLOPE_SLIDER) {

		if (dontProcessNexrGammaEvt) {
			dontProcessNexrGammaEvt = false;
			return;
		}

		// Set gamma selection to custom and save the current values for custom gamma
		gammaPresetsControl->SetSelection(3);
		lastGammaLevel = gammaLevelControl->GetValue();
		lastGammaSlope = gammaSlopeControl->GetValue();
	}
	
	this->FitInside();
	this->SetUpdated(true);
}

void RawWindow::ProcessEvt(wxCommandEvent& WXUNUSED(event)) {
	this->Process();
}

void RawWindow::Process() {
	
	// Set Params
	proc->rawPrcoessor.imgdata.params.bright = (float) brightnessControl->GetValue();
	proc->rawPrcoessor.imgdata.params.highlight = highlightControl->GetSelection();
	proc->rawPrcoessor.imgdata.params.user_sat = (int)satLevelControl->GetValue();
	proc->rawPrcoessor.imgdata.params.auto_bright_thr = (float)autoBrightThrControl->GetValue();
	proc->rawPrcoessor.imgdata.params.adjust_maximum_thr = (float)maxThrControl->GetValue();
	proc->rawPrcoessor.imgdata.params.gamm[0] = (float)(1.0f/(float)gammaLevelControl->GetValue());
	proc->rawPrcoessor.imgdata.params.gamm[1] = (float)gammaSlopeControl->GetValue();
	proc->rawPrcoessor.imgdata.params.user_qual = interpolationControl->GetSelection();
	proc->rawPrcoessor.imgdata.params.green_matching = greenMatchingControl->GetValue();
	proc->rawPrcoessor.imgdata.params.user_mul[0] = redMultiplierControl->GetValue();
	proc->rawPrcoessor.imgdata.params.user_mul[1] = greenMultiplierControl->GetValue();
	proc->rawPrcoessor.imgdata.params.user_mul[2] = blueMultiplierControl->GetValue();
	proc->rawPrcoessor.imgdata.params.user_mul[3] = greenMultiplierControl->GetValue();
	proc->rawPrcoessor.imgdata.params.threshold = (float)waveletNoiseControl->GetValue();

	// Get bit depth of processor image, and set output bps to this
	if(proc->GetImage() != NULL && proc->GetImage()->GetWidth() > 0 && proc->GetImage()->GetHeight() > 0){
		proc->rawPrcoessor.imgdata.params.output_bps = proc->GetImage()->GetColorDepth();
	}

	// White Balance

	// Camera White Balance
	if(whiteBalancePresetsControl->GetSelection() == 0){
		proc->rawPrcoessor.imgdata.params.use_camera_wb = 1;
		proc->rawPrcoessor.imgdata.params.use_auto_wb = 0;

	}
	else if(whiteBalancePresetsControl->GetSelection() == 1) {
		proc->rawPrcoessor.imgdata.params.use_camera_wb = 0;
		proc->rawPrcoessor.imgdata.params.use_auto_wb = 1;

	}
	else {
		proc->rawPrcoessor.imgdata.params.use_camera_wb = 0;
		proc->rawPrcoessor.imgdata.params.use_auto_wb = 0;
	}
	
	// Auto Bright
	if (!autoBrightControl->GetValue()) { proc->rawPrcoessor.imgdata.params.no_auto_bright = true; }
	else { proc->rawPrcoessor.imgdata.params.no_auto_bright = false; }

	// Exposure Correction
	if(exposureControl->GetValue() != 0.0){
		proc->rawPrcoessor.imgdata.params.exp_correc = 1;

		// Calculate stops to float value for exposure
		float exposeCorrect = (float) pow(2.0, exposureControl->GetValue());
		proc->rawPrcoessor.imgdata.params.exp_shift = exposeCorrect;

		proc->rawPrcoessor.imgdata.params.exp_preser = (float)exposurePreserveControl->GetValue();
	}
	else{
		proc->rawPrcoessor.imgdata.params.exp_correc = 0;
	}

	// Choose Color Space
	switch (proc->GetColorSpace()) {
		case ColorSpaceENUM::sRGB:
			proc->rawPrcoessor.imgdata.params.output_color = 1;
			break;
		case ColorSpaceENUM::ADOBE_RGB:
			proc->rawPrcoessor.imgdata.params.output_color = 2;
			break;
		case ColorSpaceENUM::WIDE_GAMUT_RGB:
			proc->rawPrcoessor.imgdata.params.output_color = 3;
			break;
		case ColorSpaceENUM::PROPHOTO_RGB:
			proc->rawPrcoessor.imgdata.params.output_color = 4;
			break;
	}
	
	// Choose Image Flip
	switch (flipControl->GetSelection()) {
	case 0:
		proc->rawPrcoessor.imgdata.params.user_flip = -1;
		break;
	case 1:
		proc->rawPrcoessor.imgdata.params.user_flip = 0;
		break;
	case 2:
		proc->rawPrcoessor.imgdata.params.user_flip = 6;
		break;
	case 3:
		proc->rawPrcoessor.imgdata.params.user_flip = 3;
		break;
	case 4:
		proc->rawPrcoessor.imgdata.params.user_flip = 5;
		break;
	}
	
	// Send reprocess raw event
	wxCommandEvent evt(REPROCESS_IMAGE_RAW_EVENT, ID_REPROCESS_IMAGE_RAW);
	wxPostEvent(parWindow, evt);

	this->SetUpdated(false);
}

void RawWindow::SetParamsAndFlags(ProcessorEdit * edit) {

	if(edit == NULL){ return;}

	if(edit->GetEditType() == ProcessorEdit::EditType::RAW){

		if(edit->CheckForParameter(PHOEDIX_PARAMETER_RAW_BRIGHTNESS)) { brightnessControl->SetValue(edit->GetParam(PHOEDIX_PARAMETER_RAW_BRIGHTNESS)); }
		if(edit->CheckForParameter(PHOEDIX_PARAMETER_RAW_HIGHLIGHTS)) {	highlightControl->SetSelection((int)edit->GetParam(PHOEDIX_PARAMETER_RAW_HIGHLIGHTS)); }
		if(edit->CheckForParameter(PHOEDIX_PARAMETER_RAW_SATURATION)) {	satLevelControl->SetValue(edit->GetParam(PHOEDIX_PARAMETER_RAW_SATURATION)); }
		if(edit->CheckForParameter(PHOEDIX_PARAMETER_RAW_AUTOBRIGHT)) {if(edit->GetParam(PHOEDIX_PARAMETER_RAW_AUTOBRIGHT) == 1){ autoBrightControl->SetValue(true); } else { autoBrightControl->SetValue(false); } }
		if(edit->CheckForParameter(PHOEDIX_PARAMETER_RAW_AUTOBRIGHT_THRESH)) { autoBrightThrControl->SetValue(edit->GetParam(PHOEDIX_PARAMETER_RAW_AUTOBRIGHT_THRESH)); }
		if(edit->CheckForParameter(PHOEDIX_PARAMETER_RAW_MAXBRIGHT_THRESH)) {	maxThrControl->SetValue(edit->GetParam(PHOEDIX_PARAMETER_RAW_MAXBRIGHT_THRESH)); }
		if(edit->CheckForParameter(PHOEDIX_PARAMETER_RAW_EXPOSURE)) { exposureControl->SetValue(edit->GetParam(PHOEDIX_PARAMETER_RAW_EXPOSURE)); }
		if(edit->CheckForParameter(PHOEDIX_PARAMETER_RAW_EXPOSURE_PRESERVE)) { exposurePreserveControl->SetValue(edit->GetParam(PHOEDIX_PARAMETER_RAW_EXPOSURE_PRESERVE)); }
		if(edit->CheckForParameter(PHOEDIX_PARAMETER_RAW_GAMMA_LEVEL)) { gammaLevelControl->SetValue(edit->GetParam(PHOEDIX_PARAMETER_RAW_GAMMA_LEVEL)); }
		if(edit->CheckForParameter(PHOEDIX_PARAMETER_RAW_GAMMA_SLOPE)) { gammaSlopeControl->SetValue(edit->GetParam(PHOEDIX_PARAMETER_RAW_GAMMA_SLOPE)); }
		if(edit->CheckForParameter(PHOEDIX_PARAMETER_RAW_GREEN_MATCH)) { if(edit->GetParam(PHOEDIX_PARAMETER_RAW_GREEN_MATCH) == 1){ greenMatchingControl->SetValue(true); } else { greenMatchingControl->SetValue(false); } }
		if(edit->CheckForParameter(PHOEDIX_PARAMETER_RAW_INTERPOLATION)) { interpolationControl->SetSelection((int)edit->GetParam(PHOEDIX_PARAMETER_RAW_INTERPOLATION)); }
		if(edit->CheckForParameter(PHOEDIX_PARAMETER_RAW_WB_PRESET)) { whiteBalancePresetsControl->SetSelection((int)edit->GetParam(PHOEDIX_PARAMETER_RAW_WB_PRESET)); }
		if(edit->CheckForParameter(PHOEDIX_PARAMETER_RAW_WB_RED)) { redMultiplierControl->SetValue(edit->GetParam(PHOEDIX_PARAMETER_RAW_WB_RED)); }
		if(edit->CheckForParameter(PHOEDIX_PARAMETER_RAW_WB_GREEN)) { greenMultiplierControl->SetValue(edit->GetParam(PHOEDIX_PARAMETER_RAW_WB_GREEN)); }
		if(edit->CheckForParameter(PHOEDIX_PARAMETER_RAW_WB_BLUE)) { blueMultiplierControl->SetValue(edit->GetParam(PHOEDIX_PARAMETER_RAW_WB_BLUE)); }
		if(edit->CheckForParameter(PHOEDIX_PARAMETER_RAW_WAVELET_NOISE_RD)) { waveletNoiseControl->SetValue(edit->GetParam(PHOEDIX_PARAMETER_RAW_WAVELET_NOISE_RD)); }
		if(edit->CheckForParameter(PHOEDIX_PARAMETER_FLIP)) { flipControl->SetSelection((int)edit->GetParam(PHOEDIX_PARAMETER_FLIP)); }
		
		if (edit->CheckForFlag(PHOEDIX_FLAG_RAW_TAB_OPEN_SETTINGS)) { if (edit->GetFlag(PHOEDIX_FLAG_RAW_TAB_OPEN_SETTINGS) == 1) { settingsPanelCollapse->Open(); } else { settingsPanelCollapse->Collapse(); } }
		if (edit->CheckForFlag(PHOEDIX_FLAG_RAW_TAB_OPEN_EXPOSURE)) { if (edit->GetFlag(PHOEDIX_FLAG_RAW_TAB_OPEN_EXPOSURE) == 1) { exposurePanelCollapse->Open(); } else { exposurePanelCollapse->Collapse(); } }
		if (edit->CheckForFlag(PHOEDIX_FLAG_RAW_TAB_OPEN_COLOR)) { if (edit->GetFlag(PHOEDIX_FLAG_RAW_TAB_OPEN_COLOR) == 1) { colorPanelCollapse->Open(); } else { colorPanelCollapse->Collapse(); } }
		if (edit->CheckForFlag(PHOEDIX_FLAG_RAW_TAB_OPEN_NOISE)) { if (edit->GetFlag(PHOEDIX_FLAG_RAW_TAB_OPEN_NOISE) == 1) { noisePanelCollapse->Open(); } else { noisePanelCollapse->Collapse(); } }
		if (edit->CheckForFlag(PHOEDIX_FLAG_RAW_TAB_OPEN_INFO)) { if (edit->GetFlag(PHOEDIX_FLAG_RAW_TAB_OPEN_INFO) == 1) { infoPanelCollapse->Open(); } else { infoPanelCollapse->Collapse(); } }

		this->Process();
	}
}

ProcessorEdit * RawWindow::GetParamsAndFlags(){

	ProcessorEdit * rawEdit = new ProcessorEdit(ProcessorEdit::EditType::RAW);

	rawEdit->AddParam(PHOEDIX_PARAMETER_RAW_BRIGHTNESS, brightnessControl->GetValue());
	rawEdit->AddParam(PHOEDIX_PARAMETER_RAW_HIGHLIGHTS, (double) highlightControl->GetSelection());
	rawEdit->AddParam(PHOEDIX_PARAMETER_RAW_SATURATION, satLevelControl->GetValue());
	rawEdit->AddParam(PHOEDIX_PARAMETER_RAW_AUTOBRIGHT_THRESH, autoBrightThrControl->GetValue());
	rawEdit->AddParam(PHOEDIX_PARAMETER_RAW_MAXBRIGHT_THRESH, maxThrControl->GetValue());

	rawEdit->AddParam(PHOEDIX_PARAMETER_RAW_GAMMA_LEVEL, gammaLevelControl->GetValue());
	rawEdit->AddParam(PHOEDIX_PARAMETER_RAW_GAMMA_SLOPE, gammaSlopeControl->GetValue());
	rawEdit->AddParam(PHOEDIX_PARAMETER_RAW_INTERPOLATION, (double)interpolationControl->GetSelection());
	rawEdit->AddParam(PHOEDIX_PARAMETER_RAW_GREEN_MATCH, (double)greenMatchingControl->GetValue());
	
	rawEdit->AddParam(PHOEDIX_PARAMETER_RAW_WB_PRESET, (double)whiteBalancePresetsControl->GetSelection());
	rawEdit->AddParam(PHOEDIX_PARAMETER_RAW_WB_RED, redMultiplierControl->GetValue());
	rawEdit->AddParam(PHOEDIX_PARAMETER_RAW_WB_GREEN, greenMultiplierControl->GetValue());
	rawEdit->AddParam(PHOEDIX_PARAMETER_RAW_WB_BLUE, blueMultiplierControl->GetValue());

	rawEdit->AddParam(PHOEDIX_PARAMETER_RAW_WAVELET_NOISE_RD, waveletNoiseControl->GetValue());

	rawEdit->AddParam(PHOEDIX_PARAMETER_RAW_AUTOBRIGHT, (double)autoBrightControl->GetValue());
	rawEdit->AddParam(PHOEDIX_PARAMETER_RAW_EXPOSURE, exposureControl->GetValue());
	rawEdit->AddParam(PHOEDIX_PARAMETER_RAW_EXPOSURE_PRESERVE, exposurePreserveControl->GetValue());
	rawEdit->AddParam(PHOEDIX_PARAMETER_FLIP, (double)flipControl->GetSelection());	

	rawEdit->AddFlag(PHOEDIX_FLAG_RAW_TAB_OPEN_SETTINGS, (int)settingsPanelCollapse->IsOpen());
	rawEdit->AddFlag(PHOEDIX_FLAG_RAW_TAB_OPEN_EXPOSURE, (int)exposurePanelCollapse->IsOpen());
	rawEdit->AddFlag(PHOEDIX_FLAG_RAW_TAB_OPEN_COLOR, (int)colorPanelCollapse->IsOpen());
	rawEdit->AddFlag(PHOEDIX_FLAG_RAW_TAB_OPEN_NOISE, (int)noisePanelCollapse->IsOpen());
	rawEdit->AddFlag(PHOEDIX_FLAG_RAW_TAB_OPEN_INFO, (int)infoPanelCollapse->IsOpen());

	return rawEdit;
}

void RawWindow::OnCollapseOpenClose(wxCommandEvent& WXUNUSED(evt)) {
	this->SaveNoReprocess();
}

bool RawWindow::CheckCopiedParamsAndFlags(){

	ProcessorEdit * edit = proc->GetEditForCopyPaste();

	if(edit == NULL){ return false;}
	if(edit->GetEditType() == ProcessorEdit::EditType::RAW){
		return true;
	}
	return false;
}

void RawWindow::FormatSlider(DoubleSlider * slider) {

	slider->SetValuePosition(DoubleSlider::VALUE_INLINE_RIGHT);
	slider->SetForegroundColour(Colors::TextLightGrey);
	slider->SetBackgroundColour(this->GetBackgroundColour());
}
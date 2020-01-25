// Copyright 2018 Jacob Chesley.
// See LICENSE.TXT in root of project for license information.

#include "ChannelMixerWindow.h"

ChannelMixerWindow::ChannelMixerWindow(wxWindow * parent, wxString editName, Processor * processor) : EditWindow(parent, editName, processor) {

	this->SetBackgroundColour(parent->GetBackgroundColour());

	mainSizer = new wxBoxSizer(wxVERTICAL);

	presetSizer = new wxBoxSizer(wxHORIZONTAL);

	// 2 Columns, 15 pixel vertical gap, 5 pixel horizontal gap
	redGridSizer = new wxFlexGridSizer(2, 8, 5);
	greenGridSizer = new wxFlexGridSizer(2, 8, 5);
	blueGridSizer = new wxFlexGridSizer(2, 8, 5);

	editLabel = new wxStaticText(this, -1, editName);
	editLabel->SetForegroundColour(Colors::TextWhite);
	editLabel->SetFont(wxFont(13, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));

	presetLabel = new wxStaticText(this, -1, "Preset");
	presetLabel->SetForegroundColour(Colors::TextLightGrey);

	presetBox = new PhoediXComboBox(this, -1);
	presetBox->SetBackgroundColour(Colors::BackDarkDarkGrey);
	presetBox->SetForegroundColour(Colors::TextLightGrey);

	presetSizer->Add(presetLabel);
	presetSizer->AddSpacer(15);
	presetSizer->Add(presetBox);

	redRedLabel = new wxStaticText(this, -1, "Red Main - Red Sub");
	redGreenLabel = new wxStaticText(this, -1, "Red Main - Green Sub");
	redBlueLabel = new wxStaticText(this, -1, "Red Main - Blue Sub");
	redRedLabel->SetForegroundColour(Colors::TextLightGrey);
	redGreenLabel->SetForegroundColour(Colors::TextLightGrey);
	redBlueLabel->SetForegroundColour(Colors::TextLightGrey);

	redRedSlider = new DoubleSlider(this, 1.0, -1.0, 1.0, 100000);
	redGreenSlider = new DoubleSlider(this, 0.0, -1.0, 1.0, 100000);
	redBlueSlider = new DoubleSlider(this, 0.0, -1.0, 1.0, 100000);
	redRedSlider->SetValuePosition(DoubleSlider::VALUE_INLINE_RIGHT);
	redGreenSlider->SetValuePosition(DoubleSlider::VALUE_INLINE_RIGHT);
	redBlueSlider->SetValuePosition(DoubleSlider::VALUE_INLINE_RIGHT);

	redRedSlider->SetForegroundColour(Colors::TextLightGrey);
	redRedSlider->SetBackgroundColour(parent->GetBackgroundColour());
	redGreenSlider->SetForegroundColour(Colors::TextLightGrey);
	redGreenSlider->SetBackgroundColour(parent->GetBackgroundColour());
	redBlueSlider->SetForegroundColour(Colors::TextLightGrey);
	redBlueSlider->SetBackgroundColour(parent->GetBackgroundColour());

	redGridSizer->Add(redRedLabel);
	redGridSizer->Add(redRedSlider);
	redGridSizer->Add(redGreenLabel);
	redGridSizer->Add(redGreenSlider);
	redGridSizer->Add(redBlueLabel);
	redGridSizer->Add(redBlueSlider);

	greenRedLabel = new wxStaticText(this, -1, "Green Main - Red Sub");
	greenGreenLabel = new wxStaticText(this, -1, "Green Main - Green Sub");
	greenBlueLabel = new wxStaticText(this, -1, "Green Main - Blue Sub");
	greenRedLabel->SetForegroundColour(Colors::TextLightGrey); 
	greenGreenLabel->SetForegroundColour(Colors::TextLightGrey);
	greenBlueLabel->SetForegroundColour(Colors::TextLightGrey);

	greenRedSlider = new DoubleSlider(this, 0.0, -1.0, 1.0, 100000);
	greenGreenSlider = new DoubleSlider(this, 1.0, -1.0, 1.0, 100000);
	greenBlueSlider = new DoubleSlider(this, 0.0, -1.0, 1.0, 100000);
	greenRedSlider->SetValuePosition(DoubleSlider::VALUE_INLINE_RIGHT);
	greenGreenSlider->SetValuePosition(DoubleSlider::VALUE_INLINE_RIGHT);
	greenBlueSlider->SetValuePosition(DoubleSlider::VALUE_INLINE_RIGHT);

	greenRedSlider->SetForegroundColour(Colors::TextLightGrey);
	greenRedSlider->SetBackgroundColour(parent->GetBackgroundColour());
	greenGreenSlider->SetForegroundColour(Colors::TextLightGrey);
	greenGreenSlider->SetBackgroundColour(parent->GetBackgroundColour());
	greenBlueSlider->SetForegroundColour(Colors::TextLightGrey);
	greenBlueSlider->SetBackgroundColour(parent->GetBackgroundColour());

	greenGridSizer->Add(greenRedLabel);
	greenGridSizer->Add(greenRedSlider);
	greenGridSizer->Add(greenGreenLabel);
	greenGridSizer->Add(greenGreenSlider);
	greenGridSizer->Add(greenBlueLabel);
	greenGridSizer->Add(greenBlueSlider);

	blueRedLabel = new wxStaticText(this, -1, "Blue Main - Red Sub");
	blueGreenLabel = new wxStaticText(this, -1, "Blue Main - Green Sub");
	blueBlueLabel = new wxStaticText(this, -1, "Blue Main - Blue Sub");
	blueRedLabel->SetForegroundColour(Colors::TextLightGrey);
	blueGreenLabel->SetForegroundColour(Colors::TextLightGrey);
	blueBlueLabel->SetForegroundColour(Colors::TextLightGrey);

	blueRedSlider = new DoubleSlider(this, 0.0, -1.0, 1.0, 100000);
	blueGreenSlider = new DoubleSlider(this, 0.0, -1.0, 1.0, 100000);
	blueBlueSlider = new DoubleSlider(this, 1.0, -1.0, 1.0, 100000);
	blueRedSlider->SetValuePosition(DoubleSlider::VALUE_INLINE_RIGHT);
	blueGreenSlider->SetValuePosition(DoubleSlider::VALUE_INLINE_RIGHT);
	blueBlueSlider->SetValuePosition(DoubleSlider::VALUE_INLINE_RIGHT);

	blueRedSlider->SetForegroundColour(Colors::TextLightGrey);
	blueRedSlider->SetBackgroundColour(parent->GetBackgroundColour());
	blueGreenSlider->SetForegroundColour(Colors::TextLightGrey);
	blueGreenSlider->SetBackgroundColour(parent->GetBackgroundColour());
	blueBlueSlider->SetForegroundColour(Colors::TextLightGrey);
	blueBlueSlider->SetBackgroundColour(parent->GetBackgroundColour());

	blueGridSizer->Add(blueRedLabel);
	blueGridSizer->Add(blueRedSlider);
	blueGridSizer->Add(blueGreenLabel);
	blueGridSizer->Add(blueGreenSlider);
	blueGridSizer->Add(blueBlueLabel);
	blueGridSizer->Add(blueBlueSlider);

	mainSizer->Add(editLabel);
	mainSizer->AddSpacer(25);
	mainSizer->Add(presetSizer);
	mainSizer->AddSpacer(35);
	mainSizer->Add(redGridSizer);
	mainSizer->AddSpacer(35);
	mainSizer->Add(greenGridSizer);
	mainSizer->AddSpacer(35);
	mainSizer->Add(blueGridSizer);

	justSetPreset = false;
	proc = processor;
	parWindow = parent;

	this->Bind(wxEVT_SCROLL_THUMBRELEASE, (wxObjectEventFunction)&ChannelMixerWindow::OnSlide, this);
	this->Bind(wxEVT_TEXT_ENTER, (wxObjectEventFunction)&ChannelMixerWindow::OnSlide, this);

	this->Bind(wxEVT_COMBOBOX, (wxObjectEventFunction)&ChannelMixerWindow::PresetChange, this);

	this->SetSizer(mainSizer);
	this->FitInside();
	this->SetScrollRate(5, 5);

	this->SetClientSize(this->GetVirtualSize());

	this->PopulateIntialPresets();
	presetBox->SetValue(presetList.at(0).GetName());
}

void ChannelMixerWindow::PopulateIntialPresets() {

	ChannelMixerPreset defaultPreset = ChannelMixerPreset("Default", 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);
	ChannelMixerPreset sepiaPreset = ChannelMixerPreset("Sepia", 0.393, 0.769, 0.189, 0.349, 0.686, 0.168, 0.272, 0.534, 0.131);
	ChannelMixerPreset greyscaleEyesight = ChannelMixerPreset("Greyscale Human Eyesight", 0.2126, 0.7152, 0.0722, 0.2126, 0.7152, 0.0722, 0.2126, 0.7152, 0.0722);
	ChannelMixerPreset greyscaleAverage = ChannelMixerPreset("Greyscale Average", (1.0/3.0), (1.0 / 3.0), (1.0 / 3.0), (1.0 / 3.0), (1.0 / 3.0), (1.0 / 3.0), (1.0 / 3.0), (1.0 / 3.0), (1.0 / 3.0));
	ChannelMixerPreset customPreset = ChannelMixerPreset("Custom", 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);

	presetList.push_back(defaultPreset);
	presetList.push_back(sepiaPreset);
	presetList.push_back(greyscaleEyesight);
	presetList.push_back(greyscaleAverage);
	presetList.push_back(customPreset);

	this->CreateComboPresetList();
}

void ChannelMixerWindow::CreateComboPresetList() {

	for(size_t i = 0; i < presetList.size(); i++){
		presetBox->AppendString(presetList.at(i).GetName());
	}
}

ChannelMixerWindow::ChannelMixerPreset ChannelMixerWindow::GetChannelMixerPresetByName(wxString name){

	for (size_t i = 0; i < presetList.size(); i++) {
		if (name == presetList.at(i).GetName()) {
			return presetList.at(i);
		}
	}

	return presetList.at(0);
}

void ChannelMixerWindow::SetValuesFromPreset(ChannelMixerWindow::ChannelMixerPreset preset) {

	// Do not modify custom preset
	if (presetBox->GetSelection() == (int)(presetList.size() - 1)) {
		return;
	}

	redRedSlider->SetValue(preset.GetRedRedScale());
	redGreenSlider->SetValue(preset.GetRedGreenScale());
	redBlueSlider->SetValue(preset.GetRedBlueScale());
	greenRedSlider->SetValue(preset.GetGreenRedScale());
	greenGreenSlider->SetValue(preset.GetGreenGreenScale());
	greenBlueSlider->SetValue(preset.GetGreenBlueScale());
	blueRedSlider->SetValue(preset.GetBlueRedScale());
	blueGreenSlider->SetValue(preset.GetBlueGreenScale());
	blueBlueSlider->SetValue(preset.GetBlueBlueScale());

	this->SetUpdated(true);
}

void ChannelMixerWindow::PresetChange(wxCommandEvent& WXUNUSED(event)) {
	
	wxString selectedName = presetBox->GetValue();
	ChannelMixerWindow::ChannelMixerPreset selectedPreset = this->GetChannelMixerPresetByName(selectedName);
	justSetPreset = true;
	this->SetValuesFromPreset(selectedPreset);
	this->SetUpdated(true);
}

void ChannelMixerWindow::OnSlide(wxCommandEvent& WXUNUSED(event)) {

	// Do not reset the preset selection if it was just selected
	if (justSetPreset) {
		justSetPreset = false;
		return;
	}

	// Set preset selection to custom
	presetBox->SetSelection(presetList.size() - 1);
	this->SetUpdated(true);
}

void ChannelMixerWindow::SetParamsAndFlags(ProcessorEdit * edit) {

	if(edit == NULL){ return;}

	justSetPreset = true;

	// Populate sliders based on edit loaded
	if (edit->GetEditType() == ProcessorEdit::EditType::CHANNEL_MIXER) {

		if (edit->CheckForFlag(ProcessorEdit::ParametersFlags::PHOEDIX_FLAG_PRESET)) { presetBox->SetSelection(edit->GetFlag(ProcessorEdit::ParametersFlags::PHOEDIX_FLAG_PRESET)); }

		if (edit->CheckForParameter(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_RED_RED)) { redRedSlider->SetValue(edit->GetParam(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_RED_RED)); }
		if (edit->CheckForParameter(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_RED_GREEN)) { redGreenSlider->SetValue(edit->GetParam(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_RED_GREEN)); }
		if (edit->CheckForParameter(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_RED_BLUE)) { redBlueSlider->SetValue(edit->GetParam(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_RED_BLUE)); }
		if (edit->CheckForParameter(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_GREEN_RED)) { greenRedSlider->SetValue(edit->GetParam(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_GREEN_RED)); }
		if (edit->CheckForParameter(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_GREEN_GREEN)) { greenGreenSlider->SetValue(edit->GetParam(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_GREEN_GREEN)); }
		if (edit->CheckForParameter(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_GREEN_BLUE)) { greenBlueSlider->SetValue(edit->GetParam(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_GREEN_BLUE)); }
		if (edit->CheckForParameter(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_BLUE_RED)) { blueRedSlider->SetValue(edit->GetParam(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_BLUE_RED)); }
		if (edit->CheckForParameter(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_BLUE_GREEN)) { blueGreenSlider->SetValue(edit->GetParam(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_BLUE_GREEN)); }
		if (edit->CheckForParameter(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_BLUE_BLUE)) { blueBlueSlider->SetValue(edit->GetParam(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_BLUE_BLUE)); }
	}
}

ProcessorEdit * ChannelMixerWindow::GetParamsAndFlags(){

	ProcessorEdit * mixerEdit = new ProcessorEdit(ProcessorEdit::EditType::CHANNEL_MIXER);

	mixerEdit->AddParam(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_RED_RED, redRedSlider->GetValue());
	mixerEdit->AddParam(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_RED_GREEN, redGreenSlider->GetValue());
	mixerEdit->AddParam(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_RED_BLUE, redBlueSlider->GetValue());
	mixerEdit->AddParam(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_GREEN_RED, greenRedSlider->GetValue());
	mixerEdit->AddParam(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_GREEN_GREEN, greenGreenSlider->GetValue());
	mixerEdit->AddParam(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_GREEN_BLUE, greenBlueSlider->GetValue());
	mixerEdit->AddParam(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_BLUE_RED, blueRedSlider->GetValue());
	mixerEdit->AddParam(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_BLUE_GREEN, blueGreenSlider->GetValue());
	mixerEdit->AddParam(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_BLUE_BLUE, blueBlueSlider->GetValue());

	// Set enabled / disabled
	mixerEdit->SetDisabled(isDisabled);

	// Add preset number
	mixerEdit->AddFlag(ProcessorEdit::ParametersFlags::PHOEDIX_FLAG_PRESET, presetBox->GetSelection());

	return mixerEdit;
}

bool ChannelMixerWindow::CheckCopiedParamsAndFlags(){

	ProcessorEdit * edit = proc->GetEditForCopyPaste();
	if(edit == NULL){ return false;}

	if (edit->GetFlagsSize() == 1 && edit->GetEditType() == ProcessorEdit::EditType::CHANNEL_MIXER) {
		return true;
	}

	if (edit->GetParamsSize() == 9 && edit->GetEditType() == ProcessorEdit::EditType::CHANNEL_MIXER) {
		return true;
	}

	return false;
}

ChannelMixerWindow::ChannelMixerPreset::ChannelMixerPreset(wxString name, double redRedScale, double redGreenScale, double redBlueScale,
	double greenRedScale, double greenGreenScale, double greenBlueScale,
	double blueRedScale, double blueGreenScale, double blueBlueScale) {

	presetName = name;
	redRed = redRedScale;
	redGreen = redGreenScale;
	redBlue = redBlueScale;
	greenRed = greenRedScale;
	greenGreen = greenGreenScale;
	greenBlue = greenBlueScale;
	blueRed = blueRedScale;
	blueGreen = blueGreenScale;
	blueBlue = blueBlueScale;
}

void ChannelMixerWindow::ChannelMixerPreset::SetName(wxString name) {
	presetName = name;
}

wxString ChannelMixerWindow::ChannelMixerPreset::GetName() {
	return presetName;
}

void ChannelMixerWindow::ChannelMixerPreset::SetRedRedScale(double newRedRedScale) {
	redRed = newRedRedScale;
}

void ChannelMixerWindow::ChannelMixerPreset::SetRedGreenScale(double newRedGreenScale) {
	redGreen = newRedGreenScale;
}

void ChannelMixerWindow::ChannelMixerPreset::SetRedBlueScale(double newRedBlueScale) {
	redBlue = newRedBlueScale;
}

void ChannelMixerWindow::ChannelMixerPreset::SetGreenRedScale(double newGreenRedScale) {
	greenRed= newGreenRedScale;
}

void ChannelMixerWindow::ChannelMixerPreset::SetGreenGreenScale(double newGreenGreenScale) {
	greenGreen = newGreenGreenScale;
}

void ChannelMixerWindow::ChannelMixerPreset::SetGreenBlueScale(double newGreenBlueScale) {
	greenBlue = newGreenBlueScale;
}

void ChannelMixerWindow::ChannelMixerPreset::SetBlueRedScale(double newBlueRedScale) {
	blueRed = newBlueRedScale;
}

void ChannelMixerWindow::ChannelMixerPreset::SetBlueGreenScale(double newBlueGreenScale) {
	blueGreen = newBlueGreenScale;
}

void ChannelMixerWindow::ChannelMixerPreset::SetBlueBlueScale(double newBlueBlueScale) {
	blueBlue = newBlueBlueScale;
}

double ChannelMixerWindow::ChannelMixerPreset::GetRedRedScale() {
	return redRed;
}

double ChannelMixerWindow::ChannelMixerPreset::GetRedGreenScale() {
	return redGreen;
}

double ChannelMixerWindow::ChannelMixerPreset::GetRedBlueScale() {
	return redBlue;
}

double ChannelMixerWindow::ChannelMixerPreset::GetGreenRedScale() {
	return greenRed;
}

double ChannelMixerWindow::ChannelMixerPreset::GetGreenGreenScale() {
	return greenGreen;
}

double ChannelMixerWindow::ChannelMixerPreset::GetGreenBlueScale() {
	return greenBlue;
}

double ChannelMixerWindow::ChannelMixerPreset::GetBlueRedScale() {
	return blueRed;
}

double ChannelMixerWindow::ChannelMixerPreset::GetBlueGreenScale() {
	return blueGreen;
}

double ChannelMixerWindow::ChannelMixerPreset::GetBlueBlueScale() {
	return blueBlue;
}
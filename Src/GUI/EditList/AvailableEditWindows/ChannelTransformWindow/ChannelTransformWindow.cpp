#include "ChannelTransformWindow.h"

ChannelTransformWindow::ChannelTransformWindow(wxWindow * parent, wxString editName, Processor * processor) : EditWindow(parent, editName) {

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

	presetBox = new wxComboBox(this, -1);

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

	processButton = new wxButton(this, EditWindow::ID_PROCESS_EDITS, "Process Edits");

	mainSizer->Add(editLabel);
	mainSizer->AddSpacer(20);
	mainSizer->Add(presetSizer);
	mainSizer->AddSpacer(25);
	mainSizer->Add(redGridSizer);
	mainSizer->AddSpacer(25);
	mainSizer->Add(greenGridSizer);
	mainSizer->AddSpacer(25);
	mainSizer->Add(blueGridSizer);
	mainSizer->AddSpacer(25);

	mainSizer->Add(processButton, 0, wxALIGN_LEFT);

	proc = processor;
	parWindow = parent;

	//this->Bind(wxEVT_SCROLL_CHANGED, (wxObjectEventFunction)&ShiftBrightnessWindow::Process, this);
	//this->Bind(wxEVT_TEXT_ENTER, (wxObjectEventFunction)&ShiftBrightnessWindow::Process, this);
	//this->Bind(wxEVT_TEXT, (wxObjectEventFunction)&ShiftBrightnessWindow::Process, this);

	this->Bind(wxEVT_COMBOBOX, (wxObjectEventFunction)&ChannelTransformWindow::PresetChange, this);
	this->Bind(wxEVT_BUTTON, (wxObjectEventFunction)&ChannelTransformWindow::Process, this, EditWindow::ID_PROCESS_EDITS);

	this->SetSizer(mainSizer);
	this->FitInside();
	this->SetScrollRate(5, 5);

	this->SetClientSize(this->GetVirtualSize());

	this->PopulateIntialPresets();
	presetBox->SetValue(presetList.at(0).GetName());
}

void ChannelTransformWindow::PopulateIntialPresets() {

	ChannelTransformPreset defaultPreset = ChannelTransformPreset("Default", 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);
	ChannelTransformPreset sepiaPreset = ChannelTransformPreset("Sepia", 0.393, 0.769, 0.189, 0.349, 0.686, 0.168, 0.272, 0.534, 0.131);

	presetList.push_back(defaultPreset);
	presetList.push_back(sepiaPreset);

	this->CreateComboPresetList();
}

void ChannelTransformWindow::CreateComboPresetList() {

	for(size_t i = 0; i < presetList.size(); i++){
		presetBox->AppendString(presetList.at(i).GetName());
	}
}

ChannelTransformWindow::ChannelTransformPreset ChannelTransformWindow::GetChannelTransformPresetByName(wxString name){

	for (size_t i = 0; i < presetList.size(); i++) {
		if (name == presetList.at(i).GetName()) {
			return presetList.at(i);
		}
	}

	return presetList.at(0);
}

void ChannelTransformWindow::SetValuesFromPreset(ChannelTransformWindow::ChannelTransformPreset preset) {

	redRedSlider->SetValue(preset.GetRedRedScale());
	redGreenSlider->SetValue(preset.GetRedGreenScale());
	redBlueSlider->SetValue(preset.GetRedBlueScale());
	greenRedSlider->SetValue(preset.GetGreenRedScale());
	greenGreenSlider->SetValue(preset.GetGreenGreenScale());
	greenBlueSlider->SetValue(preset.GetGreenBlueScale());
	blueRedSlider->SetValue(preset.GetBlueRedScale());
	blueGreenSlider->SetValue(preset.GetBlueGreenScale());
	blueBlueSlider->SetValue(preset.GetBlueBlueScale());
}

void ChannelTransformWindow::PresetChange(wxCommandEvent& WXUNUSED(event)) {
	
	wxString selectedName = presetBox->GetValue();
	ChannelTransformWindow::ChannelTransformPreset selectedPreset = this->GetChannelTransformPresetByName(selectedName);
	this->SetValuesFromPreset(selectedPreset);
}

void ChannelTransformWindow::Process(wxCommandEvent& WXUNUSED(event)) {

	wxCommandEvent evt(REPROCESS_IMAGE_EVENT, ID_REPROCESS_IMAGE);
	wxPostEvent(parWindow, evt);
}

void ChannelTransformWindow::AddEditToProcessor() {

	ProcessorEdit * transformEdit = new ProcessorEdit(ProcessorEdit::EditType::CHANNEL_TRANSFORM);

	transformEdit->AddParam(redRedSlider->GetValue());
	transformEdit->AddParam(redGreenSlider->GetValue());
	transformEdit->AddParam(redBlueSlider->GetValue());
	transformEdit->AddParam(greenRedSlider->GetValue());
	transformEdit->AddParam(greenGreenSlider->GetValue());
	transformEdit->AddParam(greenBlueSlider->GetValue());
	transformEdit->AddParam(blueRedSlider->GetValue());
	transformEdit->AddParam(blueGreenSlider->GetValue());
	transformEdit->AddParam(blueBlueSlider->GetValue());

	proc->AddEdit(transformEdit);
}

ChannelTransformWindow::ChannelTransformPreset::ChannelTransformPreset(wxString name, double redRedScale, double redGreenScale, double redBlueScale,
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

void ChannelTransformWindow::ChannelTransformPreset::SetName(wxString name) {
	presetName = name;
}

wxString ChannelTransformWindow::ChannelTransformPreset::GetName() {
	return presetName;
}

void ChannelTransformWindow::ChannelTransformPreset::SetRedRedScale(double newRedRedScale) {
	redRed = newRedRedScale;
}

void ChannelTransformWindow::ChannelTransformPreset::SetRedGreenScale(double newRedGreenScale) {
	redGreen = newRedGreenScale;
}

void ChannelTransformWindow::ChannelTransformPreset::SetRedBlueScale(double newRedBlueScale) {
	redBlue = newRedBlueScale;
}

void ChannelTransformWindow::ChannelTransformPreset::SetGreenRedScale(double newGreenRedScale) {
	greenRed= newGreenRedScale;
}

void ChannelTransformWindow::ChannelTransformPreset::SetGreenGreenScale(double newGreenGreenScale) {
	greenGreen = newGreenGreenScale;
}

void ChannelTransformWindow::ChannelTransformPreset::SetGreenBlueScale(double newGreenBlueScale) {
	greenBlue = newGreenBlueScale;
}

void ChannelTransformWindow::ChannelTransformPreset::SetBlueRedScale(double newBlueRedScale) {
	blueRed = newBlueRedScale;
}

void ChannelTransformWindow::ChannelTransformPreset::SetBlueGreenScale(double newBlueGreenScale) {
	blueGreen = newBlueGreenScale;
}

void ChannelTransformWindow::ChannelTransformPreset::SetBlueBlueScale(double newBlueBlueScale) {
	blueBlue = newBlueBlueScale;
}

double ChannelTransformWindow::ChannelTransformPreset::GetRedRedScale() {
	return redRed;
}

double ChannelTransformWindow::ChannelTransformPreset::GetRedGreenScale() {
	return redGreen;
}

double ChannelTransformWindow::ChannelTransformPreset::GetRedBlueScale() {
	return redBlue;
}

double ChannelTransformWindow::ChannelTransformPreset::GetGreenRedScale() {
	return greenRed;
}

double ChannelTransformWindow::ChannelTransformPreset::GetGreenGreenScale() {
	return greenGreen;
}

double ChannelTransformWindow::ChannelTransformPreset::GetGreenBlueScale() {
	return greenBlue;
}

double ChannelTransformWindow::ChannelTransformPreset::GetBlueRedScale() {
	return blueRed;
}

double ChannelTransformWindow::ChannelTransformPreset::GetBlueGreenScale() {
	return blueGreen;
}

double ChannelTransformWindow::ChannelTransformPreset::GetBlueBlueScale() {
	return blueBlue;
}
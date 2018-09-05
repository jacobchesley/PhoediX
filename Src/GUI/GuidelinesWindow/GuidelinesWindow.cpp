#include "GuidelinesWindow.h"

GuidelinesWindow::GuidelinesWindow(wxWindow * parent, ZoomImagePanel * mainImagePanel) : wxScrolledWindow(parent) {

	panel = mainImagePanel;
	
	this->SetBackgroundColour(parent->GetBackgroundColour());

	mainSizer = new wxBoxSizer(wxVERTICAL);

	// 2 Columns, 15 pixel vertical gap, 15 pixel horizontal gap
	gridSizerGuidelines = new wxFlexGridSizer(2, 15, 15);
	gridSizerCrop = new wxFlexGridSizer(2, 15, 15);

	// Guidelines label
	guidelinesLabel = new wxStaticText(this, -1, "Image Guidelines Settings");
	guidelinesLabel->SetForegroundColour(Colors::TextWhite);
	guidelinesLabel->SetFont(wxFont(13, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));

	// Guidelines type
	guidelinesTypeLabel = new wxStaticText(this, -1, "Guideline Type");
	guidelinesTypeLabel->SetForegroundColour(Colors::TextLightGrey);
	guidelinesType = new PhoediXComboBox(this, -1);
	guidelinesType->SetBackgroundColour(Colors::BackDarkDarkGrey);
	guidelinesType->SetForegroundColour(Colors::TextLightGrey);
	guidelinesType->Append("2 X 2");
	guidelinesType->Append("3 X 3");
	guidelinesType->Append("4 X 4");
	guidelinesType->SetSelection(1);

	// Guidelines Color
	guidelinesColorLabel = new wxStaticText(this, -1, "Guideline Color");
	guidelinesColorLabel->SetForegroundColour(Colors::TextLightGrey);
	guidelinesColor = new PhoediXButton(this, GuidelinesWindow::Buttons::GUIDE_COLOR, "     ");
	guidelinesColor->SetBackgroundColour(wxColor(128, 128, 128));

	gridSizerGuidelines->Add(guidelinesTypeLabel);
	gridSizerGuidelines->Add(guidelinesType);
	gridSizerGuidelines->Add(guidelinesColorLabel);
	gridSizerGuidelines->Add(guidelinesColor);

	// Crop grid label
	cropgridColorLabel = new wxStaticText(this, -1, "Crop Grid Settings");
	cropgridColorLabel->SetForegroundColour(Colors::TextWhite);
	cropgridColorLabel->SetFont(wxFont(13, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));

	// Crop grid color 1
	gridColor1Label = new wxStaticText(this, -1, "Crop Grid Color 1");
	gridColor1Label->SetForegroundColour(Colors::TextLightGrey);
	gridColor1 = new PhoediXButton(this, GuidelinesWindow::Buttons::GRID_COLOR1, "     ");
	gridColor1->SetBackgroundColour(wxColor(0, 0, 0));

	// Crop grid color 2
	gridColor2Label = new wxStaticText(this, -1, "Crop Grid Color 2");
	gridColor2Label->SetForegroundColour(Colors::TextLightGrey);
	gridColor2 = new PhoediXButton(this, GuidelinesWindow::Buttons::GRID_COLOR2, "     ");
	gridColor2->SetBackgroundColour(wxColor(255, 255, 255));

	gridSizerCrop->Add(gridColor1Label);
	gridSizerCrop->Add(gridColor1);
	gridSizerCrop->Add(gridColor2Label);
	gridSizerCrop->Add(gridColor2);

	mainSizer->Add(guidelinesLabel);
	mainSizer->AddSpacer(10);
	mainSizer->Add(gridSizerGuidelines);
	mainSizer->AddSpacer(25);
	mainSizer->Add(cropgridColorLabel);
	mainSizer->AddSpacer(10);
	mainSizer->Add(gridSizerCrop);

	this->SetSizer(mainSizer);
	this->FitInside();
	this->SetScrollRate(5, 5);

	this->Bind(wxEVT_COMBOBOX, (wxObjectEventFunction)&GuidelinesWindow::OnCombo, this);
	this->Bind(wxEVT_BUTTON, (wxObjectEventFunction)&GuidelinesWindow::OnGuideColor, this, GuidelinesWindow::Buttons::GUIDE_COLOR);
	this->Bind(wxEVT_BUTTON, (wxObjectEventFunction)&GuidelinesWindow::OnGridColor1, this, GuidelinesWindow::Buttons::GRID_COLOR1);
	this->Bind(wxEVT_BUTTON, (wxObjectEventFunction)&GuidelinesWindow::OnGridColor2, this, GuidelinesWindow::Buttons::GRID_COLOR2);
}

void GuidelinesWindow::OnCombo(wxCommandEvent& WXUNUSED(event)) {

	Guidelines guide;
	guide.guidelineX = 3;
	guide.guidelineY = 3;

	if (guidelinesType->GetSelection() == 0) {
		guide.guidelineX = 2;
		guide.guidelineY = 2;
	}
	if (guidelinesType->GetSelection() == 1) {
		guide.guidelineX = 3;
		guide.guidelineY = 3;
	}
	if (guidelinesType->GetSelection() == 2) {
		guide.guidelineX = 4;
		guide.guidelineY = 4;
	}
	panel->SetGuidelines(guide);
	this->Save();
}

void GuidelinesWindow::OnGuideColor(wxCommandEvent& WXUNUSED(event)) {

	// Display color dialog and set color for guideline
	wxColourDialog colorDialog(this);
	colorDialog.GetColourData().SetColour(guideColor);

	if (colorDialog.ShowModal() == wxID_OK) {
		guideColor = colorDialog.GetColourData().GetColour();
		panel->SetGuidelineColor(guideColor);
		guidelinesColor->SetBackgroundColour(guideColor);
		guidelinesColor->Refresh();
		this->Save();
	}
}

void GuidelinesWindow::OnGridColor1(wxCommandEvent& WXUNUSED(event)) {

	// Display color dialog and set color for grid color 1
	wxColourDialog colorDialog(this);
	colorDialog.GetColourData().SetColour(grid1Color);

	if (colorDialog.ShowModal() == wxID_OK) {
		grid1Color = colorDialog.GetColourData().GetColour();
		panel->SetGridColors(grid1Color, grid2Color);
		gridColor1->SetBackgroundColour(grid1Color);
		gridColor1->Refresh();
		this->Save();
	}
}

void GuidelinesWindow::OnGridColor2(wxCommandEvent& WXUNUSED(event)) {

	// Display color dialog and set color for grid color 2
	wxColourDialog colorDialog(this);
	colorDialog.GetColourData().SetColour(grid2Color);

	if (colorDialog.ShowModal() == wxID_OK) {
		grid2Color = colorDialog.GetColourData().GetColour();
		panel->SetGridColors(grid1Color, grid2Color);
		gridColor2->SetBackgroundColour(grid2Color);
		gridColor2->Refresh();
		this->Save();
	}
}

void GuidelinesWindow::Save() {
	wxCommandEvent evt(SAVE_PROJECT_EVENT, ID_SAVE_PROJECT);
	wxPostEvent(this->GetParent(), evt);
}

wxColour GuidelinesWindow::GetGuideColor() {
	return guideColor;
}

void GuidelinesWindow::SetGuideColor(wxColour color) {
	guideColor = color;
	panel->SetGuidelineColor(guideColor);
	guidelinesColor->SetBackgroundColour(guideColor);
	guidelinesColor->Refresh();

}

wxColour GuidelinesWindow::GetGridColor1() {
	return grid1Color;
}

void GuidelinesWindow::SetGridColor1(wxColour color) {
	grid1Color = color;
	panel->SetGridColors(grid1Color, grid2Color);
	gridColor1->SetBackgroundColour(grid1Color);
	gridColor1->Refresh();
}

wxColour GuidelinesWindow::GetGridColor2() {
	return grid2Color;
}

void GuidelinesWindow::SetGridColor2(wxColour color) {
	grid2Color = color;
	panel->SetGridColors(grid1Color, grid2Color);
	gridColor2->SetBackgroundColour(grid2Color);
	gridColor2->Refresh();
}

int GuidelinesWindow::GetGuidelineType() {
	return guidelinesType->GetSelection();
}

void GuidelinesWindow::SetGuidelineType(int type) {

	guidelinesType->SetSelection(type);

	Guidelines guide;
	guide.guidelineX = 3;
	guide.guidelineY = 3;

	if (guidelinesType->GetSelection() == 0) {
		guide.guidelineX = 2;
		guide.guidelineY = 2;
	}
	if (guidelinesType->GetSelection() == 1) {
		guide.guidelineX = 3;
		guide.guidelineY = 3;
	}
	if (guidelinesType->GetSelection() == 2) {
		guide.guidelineX = 4;
		guide.guidelineY = 4;
	}
	panel->SetGuidelines(guide);
}
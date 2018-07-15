// Copyright 2016 Jacob Chesley

#include "HistogramDisplay.h"

HistogramDisplay::HistogramDisplay(wxWindow * parent, Processor * processor) : wxPanel(parent) {
	
	this->SetBackgroundColour(parent->GetBackgroundColour());
	mainLayout = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(mainLayout);

	// 2 Columns, 15 pixel vertical gap, 5 pixel horizontal gap
	controlsLayout = new wxFlexGridSizer(2, 15, 5);

	selectionLabel = new wxStaticText(this, -1, "Hisograms Display");
	selectionLabel->SetForegroundColour(Colors::TextLightGrey);

	histogramSelector = new PhoediXComboBox(this, -1);
	histogramSelector->AppendString("All Hsitograms");
	histogramSelector->AppendString("Only RGBW combo");
	histogramSelector->AppendString("R, G, B, and W");
	histogramSelector->AppendString("R, G, B");
	histogramSelector->SetSelection(0);
	
	histogramSelector->SetBackgroundColour(this->GetBackgroundColour());
	histogramSelector->SetForegroundColour(Colors::TextLightGrey);

	controlsLayout->Add(selectionLabel);
	controlsLayout->Add(histogramSelector);

	histograms = new HistogramScrolled(this, processor);

	this->GetSizer()->Add(controlsLayout);
	this->GetSizer()->AddSpacer(10);
	this->GetSizer()->Add(histograms, 1 , wxEXPAND);

	this->Bind(wxEVT_COMBOBOX, (wxObjectEventFunction)&HistogramDisplay::OnCombo, this);

	this->SetSize(wxSize(300, 600));
	this->SetMinSize(wxSize(100, 100));
}

void HistogramDisplay::DestroyHistograms(){
	histograms->DestroyImages();
}

void HistogramDisplay::SetHistogramDisplay(int selection) {
	histogramSelector->SetSelection(selection);
	this->ProcessComboChange();
}

int HistogramDisplay::GetHistogramDisplay() {
	return histogramSelector->GetSelection();

}

void HistogramDisplay::OnCombo(wxCommandEvent& WXUNUSED(event)) {
	this->ProcessComboChange();
}

void HistogramDisplay::ProcessComboChange() {
	histograms->HideAll();
	histograms->HideRed();
	histograms->HideGreen();
	histograms->HideBlue();
	histograms->HideGrey();

	if (histogramSelector->GetSelection() == 0) {
		histograms->ShowAll();
		histograms->ShowRed();
		histograms->ShowGreen();
		histograms->ShowBlue();
		histograms->ShowGrey();
	}
	else if (histogramSelector->GetSelection() == 1) {
		histograms->ShowAll();
	}
	else if (histogramSelector->GetSelection() == 2) {
		histograms->ShowRed();
		histograms->ShowGreen();
		histograms->ShowBlue();
		histograms->ShowGrey();
	}
	else if (histogramSelector->GetSelection() == 3) {
		histograms->ShowRed();
		histograms->ShowGreen();
		histograms->ShowBlue();
	}
	histograms->RedrawHistograms();
}

void HistogramDisplay::UpdateHistograms() {
	histograms->UpdateHistograms();
}

void HistogramDisplay::ZeroOutHistograms(){
	histograms->ZeroDisplay();
}
HistogramDisplay::HistogramScrolled::HistogramScrolled(wxWindow * parent, Processor * processor) : wxScrolledWindow(parent) {

	this->SetBackgroundColour(parent->GetBackgroundColour());
	proc = processor;
	redHistogram = new wxImage(256, 256);
	greenHistogram = new wxImage(256, 256);
	blueHistogram = new wxImage(256, 256);
	greyHistogram = new wxImage(256, 256);
	allHistogram = new wxImage(256, 256);

	redHistogramPanel = new WXImagePanel(this, redHistogram, false);
	greenHistogramPanel = new WXImagePanel(this, greenHistogram, false);
	blueHistogramPanel = new WXImagePanel(this, blueHistogram, false);
	greyHistogramPanel = new WXImagePanel(this, greyHistogram, false);
	allHistogramPanel = new WXImagePanel(this, allHistogram, false);

	showRed = true;
	showGreen = true;
	showBlue = true;
	showGrey = true;
	showAll = true;

	histogramLayout = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(histogramLayout);
	this->FitInside();
	this->SetScrollRate(5, 5);

	this->RedrawHistograms();
}

void HistogramDisplay::HistogramScrolled::DestroyImages(){

	redHistogramPanel->StopDrawing();
	greenHistogramPanel->StopDrawing();
	blueHistogramPanel->StopDrawing();
	greyHistogramPanel->StopDrawing();
	allHistogramPanel->StopDrawing();

	if (redHistogram != NULL) {
		delete redHistogram;
		redHistogram = NULL;
	}
	if (greenHistogram != NULL) {
		delete greenHistogram;
		greenHistogram = NULL;
	}
	if (blueHistogram != NULL) {
		delete blueHistogram;
		blueHistogram = NULL;
	}
	if (greyHistogram != NULL) {
		delete greyHistogram;
		greyHistogram = NULL;
	}
	if (allHistogram != NULL) {
		delete allHistogram;
		allHistogram = NULL;
	}
}
void HistogramDisplay::HistogramScrolled::ShowRed() {
	showRed = true;
}
void HistogramDisplay::HistogramScrolled::ShowGreen() {
	showGreen = true;
}
void HistogramDisplay::HistogramScrolled::ShowBlue() {
	showBlue = true;
}
void HistogramDisplay::HistogramScrolled::ShowGrey() {
	showGrey = true;
}
void HistogramDisplay::HistogramScrolled::ShowAll() {
	showAll = true;
}
void HistogramDisplay::HistogramScrolled::HideRed() {
	showRed = false;
}
void HistogramDisplay::HistogramScrolled::HideGreen() {
	showGreen = false;
}
void HistogramDisplay::HistogramScrolled::HideBlue() {
	showBlue = false;
}
void HistogramDisplay::HistogramScrolled::HideGrey() {
	showGrey = false;
}
void HistogramDisplay::HistogramScrolled::HideAll() {
	showAll = false;
}
void HistogramDisplay::HistogramScrolled::RedrawHistograms() {

	histogramLayout->Clear(false);
	allHistogramPanel->Hide();
	redHistogramPanel->Hide();
	greenHistogramPanel->Hide();
	blueHistogramPanel->Hide();
	greyHistogramPanel->Hide();

	if (showAll) {
		allHistogramPanel->Show();
		histogramLayout->Add(allHistogramPanel, 0, wxEXPAND);
		histogramLayout->AddSpacer(15);
	}
	if (showRed) {
		redHistogramPanel->Show();
		histogramLayout->Add(redHistogramPanel, 0, wxEXPAND);
		histogramLayout->AddSpacer(15);
	}
	if (showGreen) {
		greenHistogramPanel->Show();
		histogramLayout->Add(greenHistogramPanel, 0, wxEXPAND);
		histogramLayout->AddSpacer(15);
	}
	if (showBlue) {
		blueHistogramPanel->Show();
		histogramLayout->Add(blueHistogramPanel, 0, wxEXPAND);
		histogramLayout->AddSpacer(15);
	}
	if (showGrey) {
		greyHistogramPanel->Show();
		histogramLayout->Add(greyHistogramPanel, 0, wxEXPAND);
		histogramLayout->Layout();
	}
	this->FitInside();

}

void HistogramDisplay::HistogramScrolled::UpdateHistograms() {
	
	this->GenerateHistograms();

	redHistogramPanel->Redraw();
	greenHistogramPanel->Redraw();
	blueHistogramPanel->Redraw();
	greyHistogramPanel->Redraw();
	allHistogramPanel->Redraw();
	
}

void HistogramDisplay::HistogramScrolled::GenerateHistograms() {

	if (redHistogram == NULL || greenHistogram == NULL || blueHistogram == NULL ||
		greyHistogram == NULL || allHistogram == NULL) {
		return;
	}

	// 8 bit histogram data
	uint32_t redHistogram8[256];
	uint32_t greenHistogram8[256];
	uint32_t blueHistogram8[256];
	uint32_t greyHistogram8[256];

	// Get histogram data for red, green, blue and grey level (8 bit)
	proc->Get8BitHistrogram(redHistogram8, greenHistogram8, blueHistogram8, greyHistogram8);

	// Get number of pixels in image
	uint32_t maxRedCount = 0;
	uint32_t maxGreenCount = 0;
	uint32_t maxBlueCount = 0;
	uint32_t maxGreyCount = 0;

	// Get max for each histogram
	for (int i = 0; i < 256; i++) {
		if (redHistogram8[i] > maxRedCount) { maxRedCount = redHistogram8[i]; }
		if (greenHistogram8[i] > maxGreenCount) { maxGreenCount = greenHistogram8[i]; }
		if (blueHistogram8[i] > maxBlueCount) { maxBlueCount = blueHistogram8[i]; }
		if (greyHistogram8[i] > maxGreyCount) { maxGreyCount = greyHistogram8[i]; }
	}

	this->ZeroDisplay();

	// Normalize histogram data to 0-256 for each element
	for (int i = 0; i < 256; i++) {

		redHistogram8[i] = ((double)redHistogram8[i] / (double)maxRedCount)*256.0;
		greenHistogram8[i] = ((double)greenHistogram8[i] / (double)maxGreenCount)*256.0;
		blueHistogram8[i] = ((double)blueHistogram8[i] / (double)maxBlueCount)*256.0;
		greyHistogram8[i] = ((double)greyHistogram8[i] / (double)maxGreyCount)*256.0;

		if (redHistogram8[i] > 255) { redHistogram8[i] = 255; }
		if (greenHistogram8[i] > 255) { greenHistogram8[i] = 255; }
		if (blueHistogram8[i] > 255) { blueHistogram8[i] = 255; }
		if (greyHistogram8[i] > 255) { greyHistogram8[i] = 255; }
	}

	bool fillIn = true;

	for (int i = 0; i < 256; i++) {
		redHistogram->SetRGB(i, 255 - redHistogram8[i], 255, 0, 0);
		greenHistogram->SetRGB(i, 255 - greenHistogram8[i], 0, 255, 0);
		blueHistogram->SetRGB(i, 255 - blueHistogram8[i], 0, 0, 255);
		greyHistogram->SetRGB(i, 255 - greyHistogram8[i], 200, 200, 200);
		
		allHistogram->SetRGB(i, 255 - redHistogram8[i], 255, 0, 0);
		allHistogram->SetRGB(i, 255 - greenHistogram8[i], 0, 255, 0);
		allHistogram->SetRGB(i, 255 - blueHistogram8[i], 0, 0, 255);
		allHistogram->SetRGB(i, 255 - greyHistogram8[i], 200, 200, 200);

		if (fillIn) {

			// Fill in area below each histograms image markers
			for (int j = redHistogram8[i]; j >= 0; j--) { redHistogram->SetRGB(i, 255 - j, 255, 0, 0); }
			for (int j = greenHistogram8[i]; j >= 0; j--) { greenHistogram->SetRGB(i, 255 - j, 0, 255, 0); }
			for (int j = blueHistogram8[i]; j >= 0; j--) { blueHistogram->SetRGB(i, 255 - j, 0, 0, 255); }
			for (int j = greyHistogram8[i]; j >= 0; j--) { greyHistogram->SetRGB(i, 255 - j, 200, 200, 200); }

			unsigned char redMix = 0;
			unsigned char greenMix = 0;
			unsigned char blueMix = 0;

			for (int j = 0; j < 256; j++) {
				if (redHistogram->GetRed(i, j) > 0) { redMix = redHistogram->GetRed(i,j); }
				if (greenHistogram->GetGreen(i, j) > 0) { greenMix = greenHistogram->GetGreen(i, j); }
				if (blueHistogram->GetBlue(i, j) > 0) { blueMix = blueHistogram->GetBlue(i, j); }
				allHistogram->SetRGB(i, j, redMix, greenMix, blueMix);

				if(greyHistogram->GetRed(i, j) > 0 && greyHistogram->GetGreen(i, j) > 0 && greyHistogram->GetBlue(i, j) > 0){
					redMix = greyHistogram->GetRed(i, j);
					greenMix = greyHistogram->GetGreen(i, j);
					blueMix = greyHistogram->GetBlue(i, j);
					allHistogram->SetRGB(i, j, redMix, greenMix, blueMix);
				}

			}
		}
	}
}

void HistogramDisplay::HistogramScrolled::ZeroDisplay(){

	if (redHistogram == NULL || greenHistogram == NULL || blueHistogram == NULL ||
		greyHistogram == NULL || allHistogram == NULL) {
		return;
	}
	for (int i = 0; i < 256; i++) {
		for (int j = 0; j < 256; j++) {
			redHistogram->SetRGB(i, j, 0, 0, 0);
			greenHistogram->SetRGB(i, j, 0, 0, 0);
			blueHistogram->SetRGB(i, j, 0, 0, 0);
			greyHistogram->SetRGB(i, j, 0, 0, 0);
			allHistogram->SetRGB(i, j, 0, 0, 0);
		}
	}

	redHistogramPanel->Redraw();
	greenHistogramPanel->Redraw();
	blueHistogramPanel->Redraw();
	greyHistogramPanel->Redraw();
	allHistogramPanel->Redraw();
}
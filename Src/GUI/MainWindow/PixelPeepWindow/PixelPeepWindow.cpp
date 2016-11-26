// Copyright 2016 Jacob Chesley

#include "PixelPeepWindow.h"

PixelPeepWindow::PixelPeepWindow(wxWindow * parent) : wxScrolledWindow(parent){

	this->SetBackgroundColour(parent->GetBackgroundColour());
	mainSizer = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(mainSizer);
	this->SetDoubleBuffered(true);

	gridSize = new wxComboBox(this, -1);
	gridSize->SetBackgroundColour(this->GetBackgroundColour());
	gridSize->SetForegroundColour(Colors::TextLightGrey);
	gridSize->AppendString("1 x 1");
	gridSize->AppendString("3 x 3");
	gridSize->AppendString("5 x 5");
	gridSize->AppendString("7 x 7");
	gridSize->AppendString("9 x 9");
	pixelGrid = new PixelGrid(this);

	this->GetSizer()->Add(gridSize);
	this->GetSizer()->AddSpacer(10);
	this->GetSizer()->Add(pixelGrid, 1, wxEXPAND);

	this->Bind(wxEVT_COMBOBOX, (wxObjectEventFunction)&PixelPeepWindow::OnCombo, this);
	this->FitInside();
	this->SetScrollRate(5, 5);

	this->SetClientSize(this->GetVirtualSize());
}

void PixelPeepWindow::OnCombo(wxCommandEvent & WXUNUSED(evt)) {
	pixelGrid->SetGridSize((2 * gridSize->GetSelection()) + 1);
}
void PixelPeepWindow::UpdateImage(Image * newImage) {
	pixelGrid->UpdateImage(newImage);
}

void PixelPeepWindow::UpdatePosition(int x, int y) {
	pixelGrid->UpdatePosition(x, y);
}

PixelPeepWindow::PixelGrid::PixelGrid(wxWindow * parent) : wxPanel(parent) {
	gridSize = 3;
	posX = 50;
	posY = 50;
	image = NULL;

	this->Bind(wxEVT_PAINT, (wxObjectEventFunction)&PixelPeepWindow::PixelGrid::OnPaint, this);
	this->SetDoubleBuffered(true);
}

void PixelPeepWindow::PixelGrid::SetGridSize(int newSize) {
	if (newSize % 2 == 0) {
		gridSize = newSize + 1;
	}
	else {
		gridSize = newSize;
	}
	this->PaintNow();
}

void PixelPeepWindow::PixelGrid::OnPaint(wxPaintEvent& event) {
	wxBufferedPaintDC paintDC(this);
	this->Render(paintDC);
	event.Skip();
}

void PixelPeepWindow::PixelGrid::OnSize(wxSizeEvent& event) {
	this->PaintNow();
	event.Skip();
}

void PixelPeepWindow::PixelGrid::UpdateImage(Image * newImage) {
	image = newImage;
	this->PaintNow();
}

void PixelPeepWindow::PixelGrid::PaintNow() {

	// Get the drawing context, then the buffered drawing context.
	// Paint with buffered drawing context to avoid flicker.

	if (this->IsShown()) {
		wxClientDC dc(this);
		// Make sure the client dc is okay, if not, return
		if (!dc.IsOk()) {
			return;
		}
		Render(dc);
	}
}

void PixelPeepWindow::PixelGrid::UpdatePosition(int x, int y) {
	posX = x;
	posY = y;
	this->PaintNow();
}

void PixelPeepWindow::PixelGrid::Render(wxDC & dc) {

	wxBrush backgroundBrush(wxColor(0,0,0));
	dc.SetBackground(backgroundBrush);
	dc.Clear();

	if (image == NULL) {
		return;
	}
	// Get current width and height
	int width = this->GetSize().GetWidth();
	int height = this->GetSize().GetHeight();

	// Grid size needs to be at least 1
	if (gridSize < 1) { gridSize = 1; }

	// Get width and height of each block representing a pixel
	int pixelWidth = width / gridSize;
	int pixelHeight = height / gridSize;

	// Grid size over 2 to go left and right of cursor on image
	int gridSizeOver2 = gridSize / 2.0;

	for (int i = 0; i < gridSize; i++) {
		for (int j = 0; j < gridSize; j++) {

			// Colors that will represent each pixel block
			char red = 0;
			char green = 0;
			char blue = 0;

			// Go left and right of center of cursor on image
			int tempX = posX - gridSizeOver2 + i;
			int tempY = posY - gridSizeOver2 + j;

			// Verify not on edge
			if (tempX < 0 || tempX > image->GetWidth() - 1) { continue;  }
			if (tempY < 0 || tempY > image->GetHeight() - 1) { continue; }

			// 8 bit, get RGB for block
			if (image->GetColorDepth() == 8) {
				red = image->Get8BitDataRed()[tempX + image->GetWidth()*tempY];
				green = image->Get8BitDataGreen()[tempX + image->GetWidth()*tempY];
				blue = image->Get8BitDataBlue()[tempX + image->GetWidth()*tempY];
			}

			// 16 bit, get RGB for block
			else {
				red = image->Get16BitDataRed()[tempX + image->GetWidth()*tempY] / 256.0;
				green = image->Get16BitDataGreen()[tempX + image->GetWidth()*tempY] / 256.0;
				blue = image->Get16BitDataBlue()[tempX + image->GetWidth()*tempY] / 256.0;
			}

			// Start point  and size of pixel block
			wxPoint pixelStartPoint(i * pixelWidth, j * pixelHeight);
			wxSize pixelSize(pixelWidth, pixelHeight);

			// Set color of block
			wxBrush pixelBrush(wxColor(red, green, blue));
			dc.SetBrush(pixelBrush);

			// Draw block
			dc.DrawRectangle(pixelStartPoint, pixelSize);
		}
	}
}
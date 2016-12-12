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
	gridSize->SetSelection(1);

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
	this->InitGrid();

	posX = 50;
	posY = 50;
	image = NULL;

	this->Bind(wxEVT_PAINT, (wxObjectEventFunction)&PixelPeepWindow::PixelGrid::OnPaint, this);
	this->Bind(wxEVT_LEFT_DOWN, (wxObjectEventFunction)&PixelPeepWindow::PixelGrid::OnClick, this);
	this->Bind(wxEVT_RIGHT_DOWN, (wxObjectEventFunction)&PixelPeepWindow::PixelGrid::OnClick, this);
	this->SetDoubleBuffered(true);
}

void PixelPeepWindow::PixelGrid::InitGrid() {
	
	// Clear inside vectors
	for (size_t i = 0; i < colorGrid.size(); i++) {
		colorGrid.at(i).clear();
	}
	// Clear whole vector
	colorGrid.clear();

	// Create new color grid
	for (size_t i = 0; i < gridSize; i++) {

		// New row of colors
		wxVector<ColorValue> newColorRow = wxVector<ColorValue>();

		// Create new color for each element in row
		for (size_t j = 0; j < gridSize; j++) {
			newColorRow.push_back(ColorValue());
		}

		// Push to columns
		colorGrid.push_back(newColorRow);
	}
}

void PixelPeepWindow::PixelGrid::SetGridSize(int newSize) {
	if (newSize % 2 == 0) {
		gridSize = newSize + 1;
	}
	else {
		gridSize = newSize;
	}
	this->InitGrid();
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

void PixelPeepWindow::PixelGrid::OnClick(wxMouseEvent & evt) {

	// Get current mouse position
	int mouseX = evt.GetPosition().x;
	int mouseY = evt.GetPosition().y;

	// Get current width and height
	int width = this->GetSize().GetWidth();
	int height = this->GetSize().GetHeight();

	int pixelWidth = width / gridSize;
	int pixelHeight = height / gridSize;

	for (size_t i = 0; i < gridSize; i++) {
		for (size_t j = 0; j < gridSize; j++) {

			int gridX1 = pixelWidth * i;
			int gridY1 = pixelHeight * j;
			int gridX2 = pixelWidth * (i + 1);
			int gridY2 = pixelHeight * (j + 1);

			// Found the element in the grid the mouse location is in
			if (mouseX >= gridX1 && mouseX < gridX2 && mouseY >= gridY1 && mouseY < gridY2) {

				selectedI = i;
				selectedJ = j;
				ColorValue colorVal = colorGrid.at(i).at(j);
				wxMenu popupMenu;

				// Grid size over 2 to go left and right of cursor on image
				int gridSizeOver2 = gridSize / 2.0;

				// Go left and right of center of cursor on image
				int tempX = posX - gridSizeOver2 + i;
				int tempY = posY - gridSizeOver2 + j;

				wxString pixelLocation = wxString::Format(wxT("%i"), tempX) + " , " + wxString::Format(wxT("%i"), tempY);
				// Pixel location not in bounds of image
				if (colorVal.R_8 < 0 && colorVal.G_8 < 0 && colorVal.B_8 < 0) {

					popupMenu.Append(-1, "Pixel not in image, location: " + pixelLocation);
				}
				else {
					// Convert RGB (8 bit) values to strings
					wxString red8String = "Red: " + wxString::Format(wxT("%i"), colorVal.R_8);
					wxString green8String = "Green: " + wxString::Format(wxT("%i"), colorVal.G_8);
					wxString blue8String = "Blue: " + wxString::Format(wxT("%i"), colorVal.B_8);
					wxString hex8String = "Hex: " + colorVal.HEX_8;

					popupMenu.Append(-1, "Pixel Location: " + pixelLocation);
					popupMenu.Append(-1, "8 Bit Values");
					popupMenu.Append(PixelPeepWindow::PixelGrid::ID_COPY_R8, red8String);
					popupMenu.Append(PixelPeepWindow::PixelGrid::ID_COPY_G8, green8String);
					popupMenu.Append(PixelPeepWindow::PixelGrid::ID_COPY_B8, blue8String);
					popupMenu.Append(PixelPeepWindow::PixelGrid::ID_COPY_HEX8, hex8String);
					popupMenu.GetMenuItems()[0]->Enable(false);
					popupMenu.GetMenuItems()[1]->Enable(false);

					// Convert RGB (16 bit) values to strings (if they exist)
					if (colorVal.R_16 >= 0 && colorVal.G_16 >= 0 && colorVal.B_16 >= 0) {
						wxString red16String = "Red: " + wxString::Format(wxT("%i"), colorVal.R_16);
						wxString green16String = "Green: " + wxString::Format(wxT("%i"), colorVal.G_16);
						wxString blue16String = "Blue: " + wxString::Format(wxT("%i"), colorVal.B_16);
						wxString hex16String = "Hex: " + colorVal.HEX_16;

						popupMenu.Append(-1, " ");
						popupMenu.Append(-1, "16 Bit Values");
						popupMenu.Append(PixelPeepWindow::PixelGrid::ID_COPY_R16, red16String);
						popupMenu.Append(PixelPeepWindow::PixelGrid::ID_COPY_G16, green16String);
						popupMenu.Append(PixelPeepWindow::PixelGrid::ID_COPY_B16, blue16String);
						popupMenu.Append(PixelPeepWindow::PixelGrid::ID_COPY_HEX16, hex16String);
						popupMenu.GetMenuItems()[7]->Enable(false);
					}
				}
				popupMenu.Connect(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(PixelPeepWindow::PixelGrid::OnPopupMenuClick), NULL, this);
				this->PopupMenu(&popupMenu);
			}
		}
	}
}

void PixelPeepWindow::PixelGrid::OnPopupMenuClick(wxCommandEvent& evt) {

	if (wxTheClipboard->Open()) {

		int id = evt.GetId();
		ColorValue colorVal = colorGrid.at(selectedI).at(selectedJ);

		// Switch through the diffrent menu options for copy
		switch (id) {

			// Copy 8 bit red value
			case PixelPeepWindow::PixelGrid::ID_COPY_R8: {
				wxTheClipboard->SetData(new wxTextDataObject(wxString::Format(wxT("%i"), colorVal.R_8)));
				break;
			}

			// Copy 8 bit green value
			case PixelPeepWindow::PixelGrid::ID_COPY_G8: {
				wxTheClipboard->SetData(new wxTextDataObject(wxString::Format(wxT("%i"), colorVal.G_8)));
				break;
			}

			// Copy 8 bit blue value
			case PixelPeepWindow::PixelGrid::ID_COPY_B8: {
				wxTheClipboard->SetData(new wxTextDataObject(wxString::Format(wxT("%i"), colorVal.B_8)));
				break;
			}

			// Copy 8 bit hex value
			case PixelPeepWindow::PixelGrid::ID_COPY_HEX8: {
				wxTheClipboard->SetData(new wxTextDataObject(colorVal.HEX_8));
				break;
			}

			// Copy 16 bit red value
			case PixelPeepWindow::PixelGrid::ID_COPY_R16: {
				wxTheClipboard->SetData(new wxTextDataObject(wxString::Format(wxT("%i"), colorVal.R_16)));
				break;
			}

			// Copy 16 bit green value
			case PixelPeepWindow::PixelGrid::ID_COPY_G16: {
				wxTheClipboard->SetData(new wxTextDataObject(wxString::Format(wxT("%i"), colorVal.G_16)));
				break;
			}

			// Copy 16 bit blue value
			case PixelPeepWindow::PixelGrid::ID_COPY_B16: {
				wxTheClipboard->SetData(new wxTextDataObject(wxString::Format(wxT("%i"), colorVal.B_16)));
				break;
			}

			// Copy 16 bit hex value
			case PixelPeepWindow::PixelGrid::ID_COPY_HEX16: {
				wxTheClipboard->SetData(new wxTextDataObject(colorVal.HEX_16));
				break;
			}
		}
	}
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

	for (size_t i = 0; i < gridSize; i++) {
		for (size_t j = 0; j < gridSize; j++) {

			// Colors that will represent each pixel block
			int red = 0;
			int green = 0;
			int blue = 0;

			// Go left and right of center of cursor on image
			int tempX = posX - gridSizeOver2 + i;
			int tempY = posY - gridSizeOver2 + j;

			// Verify not on edge
			if (tempX < 0 || tempX > image->GetWidth() - 1) { continue;  }
			if (tempY < 0 || tempY > image->GetHeight() - 1) { continue; }

			// 8 bit, get RGB for block

			int red8 = 0;
			int green8 = 0;
			int blue8 = 0;

			if (image->GetColorDepth() == 8) {
				red = red8 = image->Get8BitDataRed()[tempX + image->GetWidth()*tempY];
				green = green8 = image->Get8BitDataGreen()[tempX + image->GetWidth()*tempY];
				blue = blue8 = image->Get8BitDataBlue()[tempX + image->GetWidth()*tempY];

				// Set 8 bit values to color grid
				colorGrid.at(i).at(j).R_8 = red;
				colorGrid.at(i).at(j).G_8 = green;
				colorGrid.at(i).at(j).B_8 = blue;
				colorGrid.at(i).at(j).HEX_8 = this->GetHex8FromRGB8(red8, green8, blue8);

				// 16 bit values not valid
				colorGrid.at(i).at(j).R_16 = -1;
				colorGrid.at(i).at(j).G_16 = -1;
				colorGrid.at(i).at(j).B_16 = -1;
				colorGrid.at(i).at(j).HEX_16 = "";
			}

			// 16 bit, get RGB for block
			else {
				red = image->Get16BitDataRed()[tempX + image->GetWidth()*tempY];
				green = image->Get16BitDataGreen()[tempX + image->GetWidth()*tempY];
				blue = image->Get16BitDataBlue()[tempX + image->GetWidth()*tempY];

				red8 = red / 256.0;
				green8 = green / 256.0;
				blue8 = blue / 256.0;

				// Set 8 bit values to color grid
				colorGrid.at(i).at(j).R_8 = red8;
				colorGrid.at(i).at(j).G_8 = green8;
				colorGrid.at(i).at(j).B_8 = blue8;
				colorGrid.at(i).at(j).HEX_8 = this->GetHex8FromRGB8(red8, green8, blue8);

				// 16 bit values not valid
				colorGrid.at(i).at(j).R_16 = red;
				colorGrid.at(i).at(j).G_16 = green;
				colorGrid.at(i).at(j).B_16 = blue;
				colorGrid.at(i).at(j).HEX_16 = this->GetHex16FromRGB16(red, green, blue); 
			}

			// Start point  and size of pixel block
			wxPoint pixelStartPoint(i * pixelWidth, j * pixelHeight);
			wxSize pixelSize(pixelWidth, pixelHeight);

			// Set color of block
			wxBrush pixelBrush(wxColor(red8, green8, blue8));
			dc.SetBrush(pixelBrush);

			// Draw block
			dc.DrawRectangle(pixelStartPoint, pixelSize);
		}
	}
}

wxString PixelPeepWindow::PixelGrid::GetHex8FromRGB8(int r, int g, int b) {
	
	if (r > 255 || g > 255 || b > 255) { return ""; }

	// Length of 3 (2 digits plus null terminator)
	char redBuff[3];
	char greenBuff[3];
	char blueBuff[3];

	// Convert ints to char array
	_itoa(r, redBuff, 16);
	_itoa(g, greenBuff, 16);
	_itoa(b, blueBuff, 16);

	// Convert char arrays to strings
	wxString redStr = wxString::FromUTF8(redBuff);
	wxString greenStr = wxString::FromUTF8(greenBuff);
	wxString blueStr = wxString::FromUTF8(blueBuff);

	// Place leading zeros in front of strings to fill all 2 digits
	if (redStr.Length() < 2) { redStr = "0" + redStr; }
	if (greenStr.Length() < 2) { greenStr = "0" + greenStr; }
	if (blueStr.Length() < 2) { blueStr = "0" + blueStr; }

	// Make upper case and return
	redStr.MakeUpper();
	greenStr.MakeUpper();
	blueStr.MakeUpper();

	return "#" + redStr + greenStr + blueStr;
}

wxString PixelPeepWindow::PixelGrid::GetHex16FromRGB16(int r, int g, int b) {

	if (r > 65535 || g > 65535 || b > 65535) { return ""; }

	// Length of 5 (4 digits plus null terminator)
	char redBuff[5];
	char greenBuff[5];
	char blueBuff[5];

	// Convert ints to char array
	_itoa(r, redBuff, 16);
	_itoa(g, greenBuff, 16);
	_itoa(b, blueBuff, 16);

	// Convert char arrays to strings
	wxString redStr = wxString::FromUTF8(redBuff);
	wxString greenStr = wxString::FromUTF8(greenBuff);
	wxString blueStr = wxString::FromUTF8(blueBuff);

	// Place leading zeros in front of strings to fill all 4 digits
	wxString redPrepend;
	wxString greenPrepend;
	wxString bluePrepend;
	for (size_t i = 0; i < 4 - redStr.Length(); i++) { redPrepend += "0"; }
	for (size_t i = 0; i < 4 - greenStr.Length(); i++) { greenPrepend += "0"; }
	for (size_t i = 0; i < 4 - blueStr.Length(); i++) { bluePrepend += "0"; }

	redStr = redPrepend + redStr;
	greenStr = greenPrepend + greenStr;
	blueStr = bluePrepend + blueStr;

	// Make upper case and return
	redStr.MakeUpper();
	greenStr.MakeUpper();
	blueStr.MakeUpper();

	return "#" + redStr + greenStr + blueStr;
}
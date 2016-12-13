// Copyright 2016 Jacob Chesley

#ifndef PIXEL_PEEP_WINDOW_H
#define PIXEL_PEEP_WINDOW_H

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif

#include "wx/clipbrd.h"
#include "wx/dc.h"
#include "wx/dcbuffer.h"
#include "GUI/Colors/Colors.h"
#include "Processing/Image/Image.h"

class PixelPeepWindow : public wxScrolledWindow{

public:
	 PixelPeepWindow (wxWindow * parent);
	 void UpdateImage(Image * newImage);
	 void UpdatePosition(int x, int y);

private:

	wxBoxSizer * mainSizer;
	wxComboBox * gridSize;
	void OnCombo(wxCommandEvent & WXUNUSED(evt));

	class PixelGrid : public wxPanel {
	public:

		PixelGrid(wxWindow * parent);
		void SetGridSize(int newSize);
		void UpdateImage(Image * newImage);
		void UpdatePosition(int x, int y);
		void OnClick(wxMouseEvent & evt);
		void OnPopupMenuClick(wxCommandEvent& evt);
		void PaintNow();
		void InitGrid();
		wxString GetHex8FromRGB8(int r, int g, int b);
		wxString GetHex16FromRGB16(int r, int g, int b);

	private:

		struct ColorValue {
			int R_8;
			int G_8;
			int B_8;

			int R_16;
			int G_16;
			int B_16;

			wxString HEX_8;
			wxString HEX_16;
		};
		enum {
			ID_COPY_R8,
			ID_COPY_G8,
			ID_COPY_B8,
			ID_COPY_R16,
			ID_COPY_G16,
			ID_COPY_B16,
			ID_COPY_HEX8,
			ID_COPY_HEX16
		};

		int selectedI;
		int selectedJ;

		size_t gridSize;
		void OnPaint(wxPaintEvent & evt);
		void OnSize(wxSizeEvent& event);
		void Render(wxDC& dc);

		Image * image;
		int posX;
		int posY;

		wxVector<wxVector<ColorValue>> colorGrid;
	};
	PixelGrid * pixelGrid;
};

#endif

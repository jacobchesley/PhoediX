// Copyright 2016 Jacob Chesley

#ifndef PIXEL_PEEP_WINDOW_H
#define PIXEL_PEEP_WINDOW_H


// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif

#include "wx\dc.h"
#include "wx\dcbuffer.h"
#include "GUI\Colors\Colors.h"
#include "Processing\Image\Image.h"

class PixelPeepWindow : public wxScrolledWindow{

public:
	 PixelPeepWindow (wxWindow * parent);
	 void UpdateImage(Image * newImage);
	 void UpdatePosition(int x, int y);
	 void ZeroOutPixels();

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
		void PaintNow();
		void ZeroOutPixels();
	private:

		int gridSize;
		void OnPaint(wxPaintEvent & evt);
		void OnSize(wxSizeEvent& event);
		void Render(wxDC& dc);

		Image * image;
		int posX;
		int posY;
	};
	PixelGrid * pixelGrid;
};

#endif

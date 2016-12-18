// Copyright 2016 Jacob Chesley

#ifndef IMAGE_PANEL_H
#define IMAGE_PANEL_H

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/dc.h"
#include "wx/dcbuffer.h"

#include "Processing/ImageHandler/ImageHandler.h"
#include "Debugging/MemoryLeakCheck.h"

class ImagePanel : public wxPanel {
public:
	ImagePanel(wxWindow * parent, bool keepAspect = true);
	ImagePanel(wxWindow * parent, Image * image, bool keepAspect = true);
	void ChangeImage(Image * newImage);
	void Redraw();
	void SetZoom(double zoomFactor);
	void SetKeepAspect(bool doKeepAspect);


private:
	void OnPaint(wxPaintEvent & evt);
	void OnSize(wxSizeEvent& event);
	void Render(wxDC& dc);

	Image * img;
	wxBitmap bitmapDraw;
	int oldWidth;
	int oldHeight;

	double zoom;
	bool keepAspect;
	bool resize;

};

#endif
// Copyright 2018 Jacob Chesley.
// See LICENSE.TXT in root of project for license information.

#ifndef WXIMAGE_PANEL_H
#define WXIMAGE_PANEL_H

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/dc.h"
#include "wx/dcbuffer.h"
#include "Debugging/MemoryLeakCheck.h"

class WXImagePanel : public wxPanel {
public:
	WXImagePanel(wxWindow * parent, bool keepAspect = true);
	WXImagePanel(wxWindow * parent, wxImage * image, bool keepAspect = true, bool staticImg = false);
	void ChangeImage(wxImage * newImage);
	void Redraw();
	void StopDrawing();
	void SetZoom(double zoomFactor);
	void SetKeepAspect(bool doKeepAspect);
	void SetResize(bool doResize);

private:
	void OnPaint(wxPaintEvent & evt);
	void OnSize(wxSizeEvent& event);
	void Render(wxDC& dc);

	wxImage * img;
	wxBitmap bitmapDraw;

	double zoom;
	bool keepAspect;
	bool resize;
	bool staticImage;
	bool doDraw;

	bool currentlyDrawing;
};


#endif
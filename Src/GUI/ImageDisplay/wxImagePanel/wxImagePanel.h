// Copyright 2016 Jacob Chesley

#ifndef WXIMAGE_PANEL_H
#define WXIMAGE_PANEL_H

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx\dc.h"
#include "wx\dcbuffer.h"
#include "Debugging\MemoryLeakCheck.h"

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
	int oldWidth;
	int oldHeight;

	double zoom;
	bool keepAspect;
	bool resize;
	bool staticImage;
	bool doDraw;
};


#endif
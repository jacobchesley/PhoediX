#ifndef IMAGE_PANEL_H
#define IMAGE_PANEL_H

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx\dc.h"
#include "wx\dcbuffer.h"

#include "Processing\Processor\Processor.h"
#include "Processing\ImageHandler\ImageHandler.h"

#ifdef CHECK_MEMORY_LEAK
#define CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG
#endif

class ImagePanel : public wxPanel {
public:
	ImagePanel(wxWindow * parent, Image * image);
	void Redraw();

private:
	void OnPaint(wxPaintEvent & evt);
	void OnSize(wxSizeEvent& event);
	void Render(wxDC& dc);

	Image * img;
	wxBitmap bitmapDraw;
	int oldWidth;
	int oldHeight;

	bool resizeImageOnResize;
	bool keepAspect;

};

#endif
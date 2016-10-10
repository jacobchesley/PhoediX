#ifndef ZOOM_IMAGE_PANEL_H
#define ZOOM_IMAGE_PANEL_H

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx\dc.h"
#include "wx\dcbuffer.h"
#include "wx\rawbmp.h"

#include "GUI\Colors\Colors.h"
#include "GUI\Controls\DoubleSlider\DoubleSlider.h"
#include "Processing\Image\Image.h"
#include "Processing\ImageHandler\ImageHandler.h"

#include "Debugging\MemoryLeakCheck.h"

class ZoomImagePanel : public wxPanel {
public:
	ZoomImagePanel(wxWindow * parent);
	ZoomImagePanel(wxWindow * parent, Image * img);
	void Redraw();
	void ChangeImage(Image * newImage);
	void SetZoom(float zoom);
	float GetZoom();

	int GetDragX();
	int GetDragY();
	void SetDrag(int x, int y);
	void FitImage();

private:

	void OnZoom(wxCommandEvent& slideEvent);
	void OnFitImage(wxCommandEvent& WXUNUSED(event));
	void OnZoom100(wxCommandEvent& WXUNUSED(event));

	wxBoxSizer * mainSizer;
	wxBoxSizer * controlSizer;
	DoubleSlider * zoomSlider;
	wxButton * fullImageView;
	wxButton * viewImage100;

	enum Buttons {
		ZOOM_100,
		ZOOM_FIT
	};

	class ImageScroll : public wxScrolledWindow {
	public:
		ImageScroll(wxWindow * parent);
		ImageScroll(wxWindow * parent, Image * img);
		void Redraw();
		void SetZoom(double zoomFactor);
		void ChangeImage(Image * newImage);
		void FitImage();
		double GetZoom();
		void DisreguardScroll();
		void ReguardScroll();

	private:

		bool disreguardScroll;
		void OnDragStart(wxMouseEvent & evt);
		void OnDragContinue(wxMouseEvent & evt);
		void OnPaint(wxPaintEvent & evt);
		void Render(wxDC& dc);

		wxBitmap bitmapDraw;
		int oldWidth;
		int oldHeight;

		double zoom;
		bool keepAspect;
		bool resize;

		int lastWidth;
		int lastHeight;

		int dragStartX;
		int dragStartY;
		int scrollStartX;
		int scrollStartY;
	};

	ImageScroll * scroller;

};

#endif
// Copyright 2016 Jacob Chesley

#ifndef ZOOM_IMAGE_PANEL_H
#define ZOOM_IMAGE_PANEL_H

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/dc.h"
#include "wx/dcbuffer.h"
#include "wx/graphics.h"
#include "wx/rawbmp.h"

#include "GUI/Colors/Colors.h"
#include "GUI/Controls/DoubleSlider/DoubleSlider.h"
#include "Processing/Image/Image.h"
#include "Processing/ImageHandler/ImageHandler.h"

#include "Debugging/MemoryLeakCheck.h"

enum {
	ID_GRID_MOVE_EVENT
};

wxDECLARE_EVENT(GRID_MOVE_EVENT, wxCommandEvent);

struct Grid {
	double startX;
	double startY;
	double endX;
	double endY;
};

class ZoomImagePanel : public wxPanel {
public:
	ZoomImagePanel(wxWindow * parent);
	ZoomImagePanel(wxWindow * parent, Image * img);
	ZoomImagePanel(wxWindow * parent, wxImage * img);
	void Redraw();
	void ChangeImage(Image * newImage);
	void ChangeImage(wxImage * newImage);
	void NoImage();
	void SetZoom(float zoom);
	float GetZoom();

	int GetDragX();
	int GetDragY();
	void SetTempSize(int tempWidth, int tempHeight);

	bool GetFitImage();
	void SetFitImage(bool fitImage);

	void SetDrag(int x, int y);
	void FitImage();
	void DestroyTimer();

	void ActivateGrid();
	void DeactivateGrid();
	bool GetGridActive();
	Grid GetGrid();
	void SetGrid(Grid newGrid);
	void SetGridOwner(int ownerID);
	int GetGridOwner();
	void SetGridAspect(double aspect);
	double GetGridAspect();
	void SetEnforceGridAspect(bool enforceGridAspect);
	bool GetEnforceGridAspect();

	void EnableHalfSize();
	void DisableHalfSize();
	double GetImageAspect();

private:

	void InitControls();
	void OnZoom(wxCommandEvent& slideEvent);
	void OnFitImage(wxCommandEvent& WXUNUSED(event));
	void OnZoom100(wxCommandEvent& WXUNUSED(event));
	void OnReguardScrollTimer(wxTimerEvent & WXUNUSED(evt));
	void OnScrollRightDown(wxMouseEvent & evt);

	wxWindow * par;
	wxBoxSizer * mainSizer;
	wxBoxSizer * controlSizer;
	DoubleSlider * zoomSlider;
	wxButton * fullImageView;
	wxButton * viewImage100;

	int lastDragX;
	int lastDragY;

	enum Buttons {
		ZOOM_100,
		ZOOM_FIT
	};

	enum GridHitTaget {
		TOP_LEFT_CORNER,
		TOP_RIGHT_CORNER,
		BOTTOM_LEFT_CORNER,
		BOTTOM_RIGHT_CORNER,
		TOP,
		BOTTOM,
		LEFT,
		RIGHT
	};

	enum GridMoveDirection {
		GRID_MOVE_WIDTH,
		GRID_MOVE_HEIGHT
	};

	wxTimer * reguardScrollCountdown;

	class ImageScroll : public wxScrolledWindow {
	public:

		ImageScroll(wxWindow * parent);
		ImageScroll(wxWindow * parent, Image * img);
		ImageScroll(wxWindow * parent, wxImage * img);
		void Redraw();
		void SetZoom(double zoomFactor, bool refresh = true);
		void ChangeImage(Image * newImage);
		void ChangeImage(wxImage * newImage);
		void NoImage();
		void FitImage(bool refresh = true);
		void EnableFitImage();
		void DisableFitImage();
		bool GetFitImage();
		double GetZoom();
		void DisreguardScroll();
		void ReguardScroll();
		bool GetReguardScroll();
		void ActivateGrid();
		void DeactivateGrid();
		bool GetGridActive();
		Grid GetGrid();
		void SetGrid(Grid newGrid);
		void SetGridOwner(int newOwner);
		int GetGridOwner();
		void SetGridAspect(double aspect);
		double GetGridAspect();
		void SetEnforceGridAspect(bool enforceGridAspect);
		bool GetEnforceGridAspect();

		void EnableHalfSize();
		void DisableHalfSize();

		double GetImageAspect();

	private:

		bool disreguardScroll;
		void OnDragStart(wxMouseEvent & evt);
		void OnPaint(wxPaintEvent & evt);
		void Render(wxDC& dc);
		void OnRightDown(wxMouseEvent & evt);
		bool GetGridMoving();
		wxBitmap bitmapDraw;
	
		double zoom;
		bool keepAspect;

		int lastWidth;
		int lastHeight;

		int dragStartX;
		int dragStartY;
		int scrollStartX;
		int scrollStartY;

		bool currentlyDrawing;
		bool gridActive;

		bool gridMoving;
		int gridOwner;

		bool enforceGridAspect;
		double gridAspect;
		bool noImage;
		bool doFit;

		double scalar;

		Grid scaleGrid;
		Grid drawGrid;
	};

	ImageScroll * scroller;

};

#endif
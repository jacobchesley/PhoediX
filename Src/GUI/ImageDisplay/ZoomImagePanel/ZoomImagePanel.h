// Copyright 2018 Jacob Chesley.
// See LICENSE.TXT in root of project for license information.

#ifndef ZOOM_IMAGE_PANEL_H
#define ZOOM_IMAGE_PANEL_H

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "GUI/Controls/PhoediXButton/PhoediXButton.h"
#include "wx/dc.h"
#include "wx/dcgraph.h"
#include "wx/dcbuffer.h"
#include "wx/graphics.h"
#include "wx/rawbmp.h"

#include "GUI/Controls/PhoediXButton/PhoediXButton.h"
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
	wxColour color1;
	wxColour color2;
};

struct Guidelines{
	int guidelineX;
	int guidelineY;
	wxColour color;
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
	void SetGridColors(wxColour color1, wxColour color2);
	void SetGridOwner(int ownerID);
	int GetGridOwner();
	void SetGridAspect(double aspect);
	double GetGridAspect();
	void SetEnforceGridAspect(bool enforceGridAspect);
	bool GetEnforceGridAspect();

	void EnableHalfSize();
	void DisableHalfSize();
	double GetImageAspect();

	void ShowGuidelinesOption();
	void HideGuidelinesOption();
	void ActivateGuidelines();
	void DeactivateGuidelines();
	void SetGuidelines(Guidelines guide);
	Guidelines GetGuidelines();
	void SetGuidelineColor(wxColour color);
	bool GetGuidelinesActive();

private:

	void InitControls();
	void OnZoom(wxCommandEvent& slideEvent);
	void OnFitImage(wxCommandEvent& WXUNUSED(event));
	void OnZoom100(wxCommandEvent& WXUNUSED(event));
	void OnGuidelines(wxTimerEvent & WXUNUSED(evt));
	void OnReguardScrollTimer(wxTimerEvent & WXUNUSED(evt));
	void OnScrollRightDown(wxMouseEvent & evt);

	wxWindow * par;
	wxBoxSizer * mainSizer;
	wxBoxSizer * controlSizer;
	DoubleSlider * zoomSlider;
	PhoediXButton * fullImageView;
	PhoediXButton * viewImage100;
	PhoediXButton * enableGuidelines;
	
	int lastDragX;
	int lastDragY;

	enum Buttons {
		ZOOM_100,
		ZOOM_FIT,
		GUIDELINES
	};

	enum GridHitTaget {
		TOP_LEFT_CORNER,
		TOP_RIGHT_CORNER,
		BOTTOM_LEFT_CORNER,
		BOTTOM_RIGHT_CORNER,
		TOP,
		BOTTOM,
		LEFT,
		RIGHT,
		INSIDE
	};

	enum GridMoveDirection {
		GRID_MOVE_WIDTH,
		GRID_MOVE_HEIGHT,
		GRID_MOVE_ALL
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
		void SetGridColors(wxColour color1, wxColour color2);
		void SetGridOwner(int newOwner);
		int GetGridOwner();
		void SetGridAspect(double aspect);
		double GetGridAspect();
		void SetEnforceGridAspect(bool enforceGridAspect);
		bool GetEnforceGridAspect();

		void EnableHalfSize();
		void DisableHalfSize();

		double GetImageAspect();

		void ActivateGuidelines();
		void DeactivateGuidelines();
		void SetGuidelines(Guidelines guide);
		Guidelines GetGuidelines();
		bool GetGuidelinesActive();
		void SetGuidelineColor(wxColour color);

	private:

		bool disreguardScroll;
		void OnDragStart(wxMouseEvent & evt);
		void OnPaint(wxPaintEvent & evt);
		void Render(wxGCDC& dc);
		void OnRightDown(wxMouseEvent & evt);
		bool GetGridMoving();
		void OnScroll(wxCommandEvent & evt);
		wxBitmap bitmapDraw;
		wxBitmap bitmapDrawDisable;
	
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

		bool showGuidelines;
		Guidelines guidelines;

		wxInterpolationQuality quality;
	};

	ImageScroll * scroller;

};

#endif
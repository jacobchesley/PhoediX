#ifndef CURVES_H
#define CURVES_H

#include "wx/wxprec.h"

#ifndef WX_PRECOMP	
#include "wx/wx.h"

#include "spline/spline.h"
#include "wx/dcbuffer.h"
#endif

#include "GUI\Colors\Colors.h"
#include "Debugging\MemoryLeakCheck.h"

enum {
	CURVE_CHANNEL_BRIGHT,
	CURVE_CHANNEL_RED,
	CURVE_CHANNEL_GREEN,
	CURVE_CHANNEL_BLUE
};

class CurvePanel : public wxPanel {
public:

	int curvePaddingSize;
	int extraLefttX;
	int extraLefttY;
	int channelColor;

	std::vector<int> colorCurveMap;
	std::vector<Point> lastPoints;
	Spline * splineCurve;
	Spline * displayCurve;

	CurvePanel(wxWindow * Parent, int Channel, int x, int y, int width, int height);
	~CurvePanel();
	void RightClick(wxMouseEvent& evt);
	void LeftClick(wxMouseEvent& evt);
	void PaintEvent(wxPaintEvent& evt);
	void OnSize(wxSizeEvent& evt);
	void Render(wxDC& dc);
	void PaintNow();
	void DestroySpline();
	bool CheckForChanges();
	std::vector<int> GetColorCurveMap(int numSteps, float scale);
	std::vector<Point> ClipCurve(std::vector<Point> points);
	std::vector<Point> GetControlPoints();
	void SetControlPoints(std::vector<Point> newPoints);
};

#endif
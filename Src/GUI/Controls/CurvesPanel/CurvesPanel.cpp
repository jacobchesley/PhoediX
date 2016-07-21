#include "curvespanel.h"

CurvePanel::CurvePanel(wxWindow * Parent, int Channel, int x, int y, int width, int height)
	:wxPanel(Parent, -1, wxPoint(x, y), wxSize(width, height)) {

	curvePaddingSize = 7;
	channelColor = Channel;

	// Create a spline with 1000  points between each control point
	displayCurve = new Spline(1000, true);

	wxSize curSize = this->GetClientSize();

	// add the left and right control points
	displayCurve->AddPoint(0.0, 1.0);
	displayCurve->AddPoint(1.0, 0.0);

	PaintNow();

	this->Bind(wxEVT_RIGHT_DOWN, (wxObjectEventFunction)&CurvePanel::RightClick, this);
	this->Bind(wxEVT_LEFT_DOWN, (wxObjectEventFunction)&CurvePanel::LeftClick, this);
	this->Bind(wxEVT_SIZE, (wxObjectEventFunction)&CurvePanel::OnSize, this);
	this->Bind(wxEVT_PAINT, (wxObjectEventFunction)&CurvePanel::PaintEvent, this);
}

CurvePanel::~CurvePanel() {

	if (displayCurve != NULL) {
		delete displayCurve;
		displayCurve = NULL;
	}
}

// left click is used to modify the position of control points
void CurvePanel::LeftClick(wxMouseEvent& evt) {

	// set the background to grey (to prevent white flickering)
	wxBrush brush(Colors::BackMedDarkGrey);

	int id = 0;
	int dx = 1;
	int dy = 1;
	int OldX = 0;
	int OldY = 0;
	int x = 0;
	int y = 0;
	double scaledX = 0.0f;
	double scaledY = 0.0f;

	// get the current mouse state
	wxMouseState mouse(wxGetMouseState());
	mouse.SetState(wxGetMouseState());

	// get the local mouse position (relative to the panel)
	wxPoint Local = this->ScreenToClient(mouse.GetPosition());
	x = Local.x;
	y = Local.y;

	// Scale the x and y values between 0 and 1
	wxSize curSize = this->GetClientSize();
	scaledX = (double)x / (double)curSize.GetWidth();
	scaledY = (double)y / (double)curSize.GetHeight();

	// get the ID of the control point to be modified, with +- 7 pixel
	// tolerance (so it is not as difficult to select the control point)
	id = displayCurve->PointExists(scaledX, scaledY, 0.018);

	while (mouse.LeftIsDown()) {

		mouse.SetState(wxGetMouseState());

		// get the local mouse position (relative to the panel)
		Local = this->ScreenToClient(mouse.GetPosition());
		x = Local.x;
		y = Local.y;

		scaledX = (double)x / (double)curSize.GetWidth();
		scaledY = (double)y / (double)curSize.GetHeight();

		dx = x - OldX;
		dy = y - OldY;

		// if the mouse position has changed since last time...
		if (dx != 0 || OldY != 0) {

			// if a valid control point has been selected
			if (id > -1) {

				// if this is the left most point on the plot...
				if (id == 0) {

					// keep it pushed all the way to the left
					scaledX = 0.0;
				}

				// if this is the right most point on the plot...
				else if (id == displayCurve->NumControlPoint - 1) {

					// keep it pushed all the way to the right
					scaledX = 1.0;
				}

				// if this is a cotnrol point between the left and right most points...
				else {
					if (displayCurve->ControlPoints[id - 1].x > scaledX) {
						scaledX = displayCurve->ControlPoints[id - 1].x;
					}
					if (displayCurve->ControlPoints[id + 1].x < scaledX) {
						scaledX = displayCurve->ControlPoints[id + 1].x;
					}
				}

				// Make sure the new conrol points are in correct bounds
				scaledX = (scaledX < 0.0) ? 0.0 : scaledX;
				scaledX = (scaledX > 1.0) ? 1.0 : scaledX;

				scaledY = (scaledY < 0.0) ? 0.0 : scaledY;
				scaledY = (scaledY > 1.0) ? 1.0 : scaledY;

				// Set the control point that was selected to the new value.
				displayCurve->ModifyPoint(id, scaledX, scaledY);

				// draw the new spline
				PaintNow();
			}
		}

		// used to detect changes in mouse position
		OldX = x;
		OldY = y;
	}
	evt.Skip(false);
}

// right click is used to create a remove control points
void CurvePanel::RightClick(wxMouseEvent& evt) {

	int x = 0;
	int y = 0;
	double scaledX = 0.0;
	double scaledY = 0.0;

	// get the current mouse state
	wxMouseState mouse(wxGetMouseState());
	wxKeyboardState keyboard(wxGetMouseState());

	// get the local mouse position (relative to the panel)
	wxPoint Local = this->ScreenToClient(mouse.GetPosition());
	x = Local.x;
	y = Local.y;

	// Scale the x and y values between 0 and 1
	wxSize curSize = this->GetClientSize();
	scaledX = (double)x / (double)curSize.GetWidth();
	scaledY = (double)y / (double)curSize.GetHeight();

	std::vector<Point> AllControls = displayCurve->GetControlPoints();

	// If shift is pressed during a right click
	if (keyboard.ShiftDown() == true) {

		// find the ID of the point to be removed
		int delID = displayCurve->PointExists(scaledX, scaledY, 0.018);

		// If the ID is not of the left or right points
		if (delID != 0 && delID != displayCurve->NumControlPoint - 1 &&
			delID > -1 && displayCurve->NumControlPoint > 2) {

			// remove that point
			displayCurve->RemovePoint(delID);
		}
	}

	// If just a right click occured
	else {
		if (displayCurve->NumControlPoint > 1) {

			for (int i = 0; i < displayCurve->NumControlPoint - 1; i++) {

				if (scaledX > AllControls[i].x && scaledX < AllControls[i + 1].x) {

					// Add the point to the spline
					displayCurve->AddPoint(i + 1, scaledX, scaledY);

				}
			}
		}
		else {
			displayCurve->AddPoint(scaledX, scaledY);
		}
	}
	PaintNow();
	evt.Skip(false);
}

void CurvePanel::OnSize(wxSizeEvent& evt) {

	PaintNow();
	evt.Skip();
}

void CurvePanel::PaintEvent(wxPaintEvent& evt) {

	// get the drawing context
	wxBufferedPaintDC dc(this);

	// set the background to grey
	wxBrush brush(Colors::BackMedDarkGrey);
	dc.SetBackground(brush);
	Render(dc);

	evt.Skip(false);
}
void CurvePanel::PaintNow() {

	// get the drawing context
	wxClientDC dc(this);
	wxBufferedDC dcBuf(&dc);

	// set the background to grey (to prevent white flickering)
	wxBrush brush(Colors::BackMedDarkGrey);
	dcBuf.SetBackground(brush);
	Render(dcBuf);
}
void CurvePanel::Render(wxDC& dc) {

	// clear the drawing context
	dc.Clear();

	// define all of the colos
	wxColour black(0, 0, 0);
	wxColour white(255, 255, 255);
	wxColour red(255, 0, 0);
	wxColour green(0, 255, 0);
	wxColour blue(0, 0, 255);

	// create all of the brushes
	wxBrush greyBrush(Colors::BackMedDarkGrey);
	wxBrush whiteBrush(white);
	wxBrush redBrush(red);
	wxBrush greenBrush(green);
	wxBrush blueBrush(blue);

	// set the background to grey
	dc.SetBackground(greyBrush);

	// create the pen that will draw the spline
	wxPen CurvePen(white, 2, wxPENSTYLE_SOLID);

	// set the color of the spline according to the
	// color channel to be modified
	switch (channelColor) {

	case CURVE_CHANNEL_BRIGHT:
		CurvePen.SetColour(white);
		dc.SetBrush(whiteBrush);
		break;

	case CURVE_CHANNEL_RED:
		CurvePen.SetColour(red);
		dc.SetBrush(redBrush);
		break;

	case CURVE_CHANNEL_GREEN:
		CurvePen.SetColour(green);
		dc.SetBrush(greenBrush);
		break;

	case CURVE_CHANNEL_BLUE:
		CurvePen.SetColour(blue);
		dc.SetBrush(blueBrush);
		break;

	}

	// create the pen to draw the grid
	wxPen GridPen(Colors::BackLightGrey, 2, wxPENSTYLE_SOLID);

	// get the spline and control points
	std::vector<Point> SplinePoints = displayCurve->GetCurve(0.5, CATMULL_ROM_SPLINE, 300);
	std::vector<Point> ControlPoints = displayCurve->GetControlPoints();

	// clip to curve so it does not go off the panel
	SplinePoints = ClipCurve(SplinePoints);

	// calculate the points used to draw the grid
	// the grid has 3 vertical and 3 horizontal lines
	// evenly spaced
	wxSize Size = this->GetClientSize();

	int W14 = (int)(Size.GetWidth() / 4);
	int W24 = (int)(2 * Size.GetWidth() / 4);
	int W34 = (int)(3 * Size.GetWidth() / 4);
	int W44 = (int)(4 * Size.GetWidth() / 4);

	int H14 = (int)(Size.GetHeight() / 4);
	int H24 = (int)(2 * Size.GetHeight() / 4);
	int H34 = (int)(3 * Size.GetHeight() / 4);
	int H44 = (int)(4 * Size.GetHeight() / 4);

	dc.SetPen(GridPen);

	// draw the vertical lines
	dc.DrawLine(wxPoint(W14, 0), wxPoint(W14, H44));
	dc.DrawLine(wxPoint(W24, 0), wxPoint(W24, H44));
	dc.DrawLine(wxPoint(W34, 0), wxPoint(W34, H44));

	// draw the horizontal lines
	dc.DrawLine(wxPoint(0, H14), wxPoint(W44, H14));
	dc.DrawLine(wxPoint(0, H24), wxPoint(W44, H24));
	dc.DrawLine(wxPoint(0, H34), wxPoint(W44, H34));

	dc.SetPen(CurvePen);

	// draw the spline
	double x1 = 0.0;
	double x2 = 0.0;
	double y1 = 0.0;
	double y2 = 0.0;

	for (int i = 0; i < (int)SplinePoints.size() - 2; i++) {

		x1 = SplinePoints[i].x * Size.GetWidth();
		x2 = SplinePoints[i + 1].x * Size.GetWidth();

		y1 = (SplinePoints[i].y * Size.GetHeight());
		y2 = (SplinePoints[i + 1].y * Size.GetHeight());

		dc.DrawLine(wxPoint((int)x1, (int)y1), wxPoint((int)x2, (int)y2));
	}

	// draw the control points (larger circles)
	double x1Circle = 0.0;
	double y1Circle = 0.0;

	for (int i = 0; i < (int)ControlPoints.size(); i++) {

		if (ControlPoints[i].x < 0.0) {
			ControlPoints[i].x = 0.0;
		}
		if (ControlPoints[i].x > 1.0) {
			ControlPoints[i].x = 1.0;
		}

		if (ControlPoints[i].y < 0.0) {
			ControlPoints[i].y = 0.0;
		}
		if (ControlPoints[i].y > 1.0) {
			ControlPoints[i].y = 1.0;
		}

		x1Circle = ControlPoints[i].x * Size.GetWidth();
		y1Circle = ControlPoints[i].y * Size.GetHeight();
		dc.DrawCircle(wxPoint((int)x1Circle, (int)y1Circle), 4);
	}
}
std::vector<Point> CurvePanel::ClipCurve(std::vector<Point> Points) {

	for (int i = 0; i < (int)Points.size() - 1; i++) {

		if (Points[i].x < 0.0) {
			Points[i].x = 0.0;
		}
		if (Points[i].x > 1.0) {
			Points[i].x = 1.0;
		}
		if (Points[i].y < 0.0) {
			Points[i].y = 0.0;
		}
		if (Points[i].y > 1.0) {
			Points[i].y = 1.0;
		}
	}
	return Points;
}
std::vector<int> CurvePanel::GetColorCurveMap(int numSteps, float scale) {


	splineCurve = new Spline(numSteps * 3, true);
	std::vector<Point> controlPoints = displayCurve->GetControlPoints();

	for (int i = 0; i < (int)controlPoints.size(); i++) {
		splineCurve->AddPoint(controlPoints[i].x, controlPoints[i].y);
		lastPoints.push_back(controlPoints[i]);
	}

	std::vector<Point> SplinePoints = splineCurve->GetCurve(0.5, CATMULL_ROM_SPLINE);
	for (int i = 0; i < (int)SplinePoints.size(); i++) {
		SplinePoints[i].x *= scale;
		SplinePoints[i].y *= scale;

		SplinePoints[i].y = scale - SplinePoints[i].y;

		if (SplinePoints[i].y > scale) {
			SplinePoints[i].y = scale;
		}
	}

	colorCurveMap.resize(numSteps);

	int SplinePos = 0;
	float Bright = 0.0;

	for (int i = 0; i < numSteps; i++) {

		while (SplinePoints[SplinePos].x >= Bright && SplinePoints[SplinePos].x < Bright + 1.0) {

			colorCurveMap[i] = SplinePoints[SplinePos].y;
			SplinePos += 1;
		}
		Bright += 1.0;
	}

	delete splineCurve;
	return colorCurveMap;
}

std::vector<Point> CurvePanel::GetControlPoints(){
	return displayCurve->GetControlPoints();
}

void CurvePanel::SetControlPoints(std::vector<Point> newPoints) {

}

void CurvePanel::DestroySpline() {
	delete displayCurve;
	displayCurve = NULL;
}

bool CurvePanel::CheckForChanges() {

	if (lastPoints.size() != displayCurve->GetControlPoints().size()) {
		return true;
	}
	for (int i = 0; i < (int)lastPoints.size(); i++) {
		if (displayCurve->GetControlPoints()[i].x != lastPoints[i].x ||
			displayCurve->GetControlPoints()[i].y != lastPoints[i].y) {
			return true;
		}
	}
	return false;
}
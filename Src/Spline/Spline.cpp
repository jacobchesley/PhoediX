// Copyright 2016 Jacob Chesley

#include "Spline.h"

Spline::Spline(int Precision, bool PreventXOverlap) {

	this->Precision = Precision;
	this->CurrentID = 0;
	this->NumControlPoint = 0;
	this->Prev_x_overlap = PreventXOverlap;
	ControlPoints = wxVector<Point>();
	OutPoints = wxVector<Point>();
}

Spline::~Spline() {}

double Spline::HermiteInterpolate(double y0, double y1, double tangenty0, double tangenty1, double mu) {

	double  h1, h2, h3, h4, mu3, mu2, ret;

	mu2 = mu*mu;
	mu3 = mu2*mu;

	h1 = (2 * mu3) - (3 * mu2) + 1;
	h2 = (-2 * mu3) + (3 * mu2);
	h3 = mu3 - (2 * mu2) + mu;
	h4 = mu3 - mu2;

	ret = (y0 * h1) + (y1 * h2) + (tangenty0 * h3) + (tangenty1 * h4);

	return ret;
}

double Spline::CalculateTangentCardinalX(Point Left, Point Right, double Tension) {

	return Tension * (Right.x - Left.x);
}
double Spline::CalculateTangentCardinalY(Point Left, Point Right, double Tension) {

	return Tension * (Right.y - Left.y);
}

wxVector <Point> Spline::GetHermiteSpline(double Tension, bool NoXOverlap) {

	double LeftTangentY, RightTangentY, LeftTangentX, RightTangentX;

	LeftTangentY = 0;
	RightTangentY = 0;
	LeftTangentX = 0;
	RightTangentX = 0;

	// Calculate the t interval for the interpolation
	double inc = 1.0 / Precision;

	int OutInc = 0;
	double CurMaxX = 0.0;

	for (int i = 0; i < NumControlPoint - 1; i++) {

		// 2 Control points, this will be treated as a line
		if (i == 0 && NumControlPoint == 2) {

			LeftTangentX = ControlPoints.at(i + 1).x - ControlPoints.at(i).x;
			RightTangentX = ControlPoints.at(i + 1).x - ControlPoints.at(i).x;
			LeftTangentY = ControlPoints.at(i + 1).y - ControlPoints.at(i).y;
			RightTangentY = ControlPoints.at(i + 1).y - ControlPoints.at(i).y;
		}

		// More than 2 control points, get the tangent for first end
		if (i == 0 && NumControlPoint != 2) {

			LeftTangentX = 0;
			RightTangentX = CalculateTangentCardinalX(ControlPoints.at(i), ControlPoints[i + 2], Tension);
			LeftTangentY = 0;
			RightTangentY = CalculateTangentCardinalY(ControlPoints.at(i), ControlPoints[i + 2], Tension);
		}

		// More than 2 control points, get the tangent for first end
		if (i == NumControlPoint - 2 && NumControlPoint != 2) {

			LeftTangentX = CalculateTangentCardinalX(ControlPoints.at(i - 1), ControlPoints.at(i + 1), Tension);
			RightTangentX = 0;
			LeftTangentY = CalculateTangentCardinalY(ControlPoints.at(i - 1), ControlPoints.at(i + 1), Tension);
			RightTangentY = 0;
		}

		// More than 2 control points, get the tangent for a middle segment
		if (i > 0 && i < NumControlPoint - 2 && NumControlPoint != 2) {

			LeftTangentX = CalculateTangentCardinalX(ControlPoints.at(i - 1), ControlPoints.at(i + 1), Tension);
			RightTangentX = CalculateTangentCardinalX(ControlPoints.at(i), ControlPoints[i + 2], Tension);
			LeftTangentY = CalculateTangentCardinalY(ControlPoints.at(i - 1), ControlPoints.at(i + 1), Tension);
			RightTangentY = CalculateTangentCardinalY(ControlPoints.at(i), ControlPoints[i + 2], Tension);
		}

		// Generate the curve between control points
		for (double t = 0; t < 1.0; t += inc) {

			OutPoints[OutInc].x = HermiteInterpolate(ControlPoints.at(i).x, ControlPoints.at(i + 1).x, LeftTangentX, RightTangentX, t);
			OutPoints[OutInc].y = HermiteInterpolate(ControlPoints.at(i).y, ControlPoints.at(i + 1).y, LeftTangentY, RightTangentY, t);

			// if this is the very first point on the curve, set the maximum 
			if (i == 0 && t == 0) { CurMaxX = OutPoints[OutInc].x; }

			// If the x axis has been specified not to overlap...
			if (OutPoints[OutInc].x < CurMaxX && NoXOverlap) {
				OutPoints[OutInc].x = CurMaxX;
			}
			if (OutPoints[OutInc].x > CurMaxX) {
				CurMaxX = OutPoints[OutInc].x;
			}
			OutInc += 1;
		}
	}
	return OutPoints;
}
bool Spline::AddPoint(double x, double y) {
	
	// Create point
	Point NewPoint;
	NewPoint.x = x;
	NewPoint.y = y;
	NewPoint.id = CurrentID;

	// Push new point to the end of control points
	ControlPoints.push_back(NewPoint);

	CurrentID += 1;
	NumControlPoint += 1;

	// Resize the output points
	OutPoints.resize(NumControlPoint + (NumControlPoint - 1) * (Precision + 1));
	return true;
}

bool Spline::AddPoint(int position, double x, double y) {

	// Create point
	Point NewPoint;

	if (position < 0) {
		position = 0;
	}
	if (position > NumControlPoint) {
		position = NumControlPoint;
	}
	NewPoint.x = x;
	NewPoint.y = y;
	NewPoint.id = position;

	// Insert at desired position
	ControlPoints.insert(ControlPoints.begin() + position, NewPoint);

	// Re ID everything after the new point
	for (int i = position + 1; i < NumControlPoint + 1; i++) {

		ControlPoints.at(i).id = i;
	}

	CurrentID += 1;
	NumControlPoint += 1;

	// Resize the output points
	OutPoints.resize(NumControlPoint + (NumControlPoint - 1) * Precision);
	return true;
}

bool Spline::ModifyPoint(double OriginalX, double OriginalY, double NewX, double NewY) {

	for (int i = 0; i < NumControlPoint; i++) {

		if (OriginalX == ControlPoints.at(i).x && OriginalY == ControlPoints.at(i).y) {

			ControlPoints.at(i).x = NewX;
			ControlPoints.at(i).y = NewY;
			return true;
		}
	}
	return false;
}

bool Spline::ModifyPoint(int ID, double NewX, double NewY) {

	for (int i = 0; i < NumControlPoint; i++) {

		if (ControlPoints.at(i).id == ID) {

			LastModifiedPointID = ID;
			ControlPoints.at(i).x = NewX;
			ControlPoints.at(i).y = NewY;
			return true;
		}
	}
	return false;
}

int Spline::RemovePoint(double x, double y, double PlusMinXY) {

	int ID = -1;
	for (int i = 0; i < NumControlPoint; i++) {

		if (((x - PlusMinXY) <= ControlPoints.at(i).x) && ((x + PlusMinXY) >= ControlPoints.at(i).x) &&
			((y - PlusMinXY) <= ControlPoints.at(i).y) && ((y + PlusMinXY) >= ControlPoints.at(i).y)) {

			// Remove the point
			ID = ControlPoints.at(i).id;
			ControlPoints.erase(ControlPoints.begin() + i);
			NumControlPoint -= 1;
			CurrentID -= 1;

			// Re ID all points
			for (int j = 0; j < NumControlPoint; j++) {

				ControlPoints[j].id = j;
			}

			// Resize the output points
			OutPoints.resize(NumControlPoint + (NumControlPoint - 1) * Precision);
			return ID;
		}
	}
	return -1;
}
bool Spline::RemovePoint(int id) {

	// Remove the point
	ControlPoints.erase(ControlPoints.begin() + id);
	NumControlPoint -= 1;
	CurrentID -= 1;

	// Re ID all points
	for (int i = 0; i < NumControlPoint; i++) {

		ControlPoints.at(i).id = i;
	}

	// Resize the output points
	OutPoints.resize(NumControlPoint + (NumControlPoint - 1) * Precision);
	return true;
}

int Spline::PointExists(double x, double y, double PlusMinXY) {

	for (int i = 0; i < NumControlPoint; i++) {

		if (((x - PlusMinXY) <= ControlPoints.at(i).x) && ((x + PlusMinXY) >= ControlPoints.at(i).x) &&
			((y - PlusMinXY) <= ControlPoints.at(i).y) && ((y + PlusMinXY) >= ControlPoints.at(i).y)) {

			return ControlPoints.at(i).id;
		}
	}
	return -1;
}

wxVector<Point> Spline::GetControlPoints() {

	return ControlPoints;
}



wxVector<Point> Spline::GetCurve(double Tension, int SplineType, int limit) {


	switch (SplineType) {

	case CATMULL_ROM_SPLINE:
		GetHermiteSpline(0.5, Prev_x_overlap);
		break;
	case CUBIC_HERMITE_SPLINE:
		GetHermiteSpline(Tension, Prev_x_overlap);
		break;
	}

	if (limit > 0) {
		wxVector <Point> retPoints = wxVector<Point>();
		int inc = (int)OutPoints.size() / limit;

		for (wxVector<Point>::size_type i = 0; i < OutPoints.size(); i += inc) {
			retPoints.push_back(OutPoints.at(i));
		}

		return retPoints;
	}

	return OutPoints;
}


bool Spline::GetXCanOverlap() {

	return Prev_x_overlap;
}

bool Spline::SetXCanOverlap(bool PreventXOverlap) {

	Prev_x_overlap = PreventXOverlap;
	return Prev_x_overlap;

}

size_t Spline::GetNumControlPoints() {

	return ControlPoints.size();
}

size_t Spline::GetNumCurvePoints() {

	return OutPoints.size();
}

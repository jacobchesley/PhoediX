#ifndef SPLINE_H
#define SPLINE_H

#include <vector>

enum {
	CATMULL_ROM_SPLINE,
	CUBIC_HERMITE_SPLINE
};

struct Point {

	double x;
	double y;
	int id;
};

class Spline {

public:
	Spline(int Precision, bool PreventXOverlap);
	~Spline();

	double HermiteInterpolate(double y0, double y1, double tangenty0, double tangenty1, double mu);
	double CalculateTangentCardinalX(Point Left, Point Right, double Tension);
	double CalculateTangentCardinalY(Point Left, Point Right, double Tension);
	std::vector <Point> GetHermiteSpline(double Tension, bool NoXOverlap = false);

	bool AddPoint(double x, double y);
	bool AddPoint(int position, double x, double y);
	int RemovePoint(double x, double y, double PlusMinXY);
	bool RemovePoint(int id);

	bool ModifyPoint(double OriginalX, double OriginalY, double NewX, double NewY);
	bool ModifyPoint(int id, double NewX, double NewY);

	int PointExists(double x, double y, double PlusMinXY);

	std::vector<Point> GetControlPoints();
	std::vector<Point> GetCurve(double Tension, int SplineType = CATMULL_ROM_SPLINE, int limit = -1);

	size_t GetNumControlPoints();
	size_t GetNumCurvePoints();

	bool GetXCanOverlap();
	bool SetXCanOverlap(bool PreventXOverlap);

	std::vector <Point> ControlPoints;
	std::vector <Point> OutPoints;
	int NumControlPoint, Precision, CurrentID;
	double MinX, MaxX;
	bool Prev_x_overlap;

private:
	int LastModifiedPointID;

};

#endif

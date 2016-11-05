// Copyright 2016 Jacob Chesley

#include "HSLCurvesWindow.h"

HSLCurvesWindow::HSLCurvesWindow(wxWindow * parent, wxString editName, Processor * processor) : EditWindow(parent, editName, processor) {

	parWindow = parent;
	this->SetBackgroundColour(parent->GetBackgroundColour());

	curveTabs = new wxNotebook(this, -1);

	hCurve = new CurvePanel(curveTabs, CURVE_CHANNEL_BRIGHT);
	sCurve = new CurvePanel(curveTabs, CURVE_CHANNEL_RED);
	lCurve = new CurvePanel(curveTabs, CURVE_CHANNEL_BLUE);

	curveTabs->SetBackgroundColour(wxColour(54, 54, 54));
	curveTabs->AddPage(hCurve, "H Channel", true);
	curveTabs->AddPage(sCurve, "S Channel", false);
	curveTabs->AddPage(lCurve, "L Channel", false);

	container = new wxBoxSizer(wxVERTICAL);
	container->Add(curveTabs, 1, wxEXPAND);

	this->SetSizerAndFit(container);
	container->Layout();

	proc = processor;

	this->Bind(CURVE_CHANGED_EVENT, (wxObjectEventFunction)&HSLCurvesWindow::OnUpdate, this);
	this->StartWatchdog();
}

void HSLCurvesWindow::SetParamsAndFlags(ProcessorEdit * edit){

	if(edit == NULL){ return; }

	if(edit->GetEditType() != ProcessorEdit::EditType::HSL_CURVES){ return; }

	std::vector<Point> hControlPoints;
	std::vector<Point> sControlPoints;
	std::vector<Point> lControlPoints;

	// Must have 3 double arrays (h, s, and l curves)
	if(edit->GetNumDoubleArrays() != 3){ return; }

	// Verify that the array size of each array is at least 4 (2 points) and even (each point has x and y)
	int doubleArrSize = 0;
	for(int i = 0; i < 3; i++){
		doubleArrSize = edit->GetDoubleArraySize(i);
		if(doubleArrSize < 4 || doubleArrSize %2 != 0){
			return;
		}
	}

	// Verify all point values are between 0 and 1
	doubleArrSize = 0;
	for(int i = 0; i < 3; i++){

		// Get size of current array and array
		doubleArrSize = edit->GetDoubleArraySize(i);
		double * arr = edit->GetDoubleArray(i);

		// Check each element in array to verify it is between 0 and 1
		for(int j = 0; j < doubleArrSize; j++){
			if(arr[j] < 0.0 || arr[j] > 1.0){
				return;
			}
		}
	}

	// Set h control points
	double * lArray = edit->GetDoubleArray(0);
	int curID = 0;
	for(int i = 0; i < edit->GetDoubleArraySize(0); i+=2){

		Point hPoint;
		hPoint.x = lArray[i];
		hPoint.y = lArray[i + 1];
		hPoint.id = curID;
		hControlPoints.push_back(hPoint);
		curID += 1;
	}

	// Set s control points
	double * aArray = edit->GetDoubleArray(1);
	curID = 0;
	for(int i = 0; i < edit->GetDoubleArraySize(1); i+=2){

		Point sPoint;
		sPoint.x = aArray[i];
		sPoint.y = aArray[i + 1];
		sPoint.id = curID;
		sControlPoints.push_back(sPoint);

		curID += 1;
	}

	// Set l control points
	double * bArray = edit->GetDoubleArray(2);
	curID = 0;
	for(int i = 0; i < edit->GetDoubleArraySize(2); i+=2){

		Point lPoint;
		lPoint.x = bArray[i];
		lPoint.y = bArray[i + 1];
		lPoint.id = curID;
		lControlPoints.push_back(lPoint);

		curID += 1;
	}

	// Set curves
	hCurve->SetControlPoints(hControlPoints);
	sCurve->SetControlPoints(sControlPoints);
	lCurve->SetControlPoints(lControlPoints);
}

ProcessorEdit * HSLCurvesWindow::GetParamsAndFlags(){

	std::vector<Point> lControlPoints = hCurve->GetControlPoints();
	std::vector<Point> aControlPoints = sCurve->GetControlPoints();
	std::vector<Point> bControlPoints = lCurve->GetControlPoints();

	// Create double array that will store control points
	double * hPoints = new double[lControlPoints.size() * 2];
	double * sPoints = new double[aControlPoints.size() * 2];
	double * lPoints = new double[bControlPoints.size() * 2];

	unsigned int idx = 0;

	// Add h control points to a double array
	for (size_t p = 0; p < lControlPoints.size(); p ++) {
		hPoints[idx] = lControlPoints[p].x;
		hPoints[idx + 1] =lControlPoints[p].y;
		idx += 2;
	}
	idx = 0;

	// Add s control points to a double array
	for (size_t p = 0; p < aControlPoints.size(); p++) {
		sPoints[idx] = aControlPoints[p].x;
		sPoints[idx + 1] = aControlPoints[p].y;
		idx += 2;
	}
	idx = 0;

	// Add l control points to a double array
	for (size_t p = 0; p < bControlPoints.size(); p++) {
		lPoints[idx] = bControlPoints[p].x;
		lPoints[idx + 1] = bControlPoints[p].y;
		idx += 2;
	}
	idx = 0;
	
	ProcessorEdit * HSLCurveEdit = new ProcessorEdit(ProcessorEdit::EditType::HSL_CURVES);

	if (!isDisabled) {
		int numSteps16 = 65536;
		std::vector<int> lVector16 = hCurve->GetColorCurveMap(numSteps16, (float)numSteps16);
		std::vector<int> aVector16 = sCurve->GetColorCurveMap(numSteps16, (float)numSteps16);
		std::vector<int> bVector16 = lCurve->GetColorCurveMap(numSteps16, (float)numSteps16);

		int * lCurve16 = new int[numSteps16];
		int * aCurve16 = new int[numSteps16];
		int * bCurve16 = new int[numSteps16];

		for (int i = 0; i < numSteps16; i++) {
			lCurve16[i] = lVector16[i];
			aCurve16[i] = aVector16[i];
			bCurve16[i] = bVector16[i];
		}

		HSLCurveEdit->AddIntArray(lCurve16, numSteps16);
		HSLCurveEdit->AddIntArray(aCurve16, numSteps16);
		HSLCurveEdit->AddIntArray(bCurve16, numSteps16);
	}

	// Add control points double arrays to edit
	HSLCurveEdit->AddDoubleArray(hPoints, lControlPoints.size() * 2);
	HSLCurveEdit->AddDoubleArray(sPoints, aControlPoints.size() * 2);
	HSLCurveEdit->AddDoubleArray(lPoints, bControlPoints.size() * 2);

	// Set enabled / disabled
	HSLCurveEdit->SetDisabled(isDisabled);

	return HSLCurveEdit;
}

bool HSLCurvesWindow::CheckCopiedParamsAndFlags(){

	ProcessorEdit * edit = proc->GetEditForCopyPaste();
	if(edit == NULL){ return false; }

	if(edit->GetEditType() != ProcessorEdit::EditType::HSL_CURVES){ return false; }

	// Need 3 double arrars (H, S and L)
	if(edit->GetNumDoubleArrays() != 3){
		return false;
	}

	// Verify that the array size of each array is at least 4 (2 points) and even (each point has x and y)
	int doubleArrSize = 0;
	for(int i = 0; i < 3; i++){
		doubleArrSize = edit->GetDoubleArraySize(i);
		if(doubleArrSize < 4 || doubleArrSize %2 != 0){
			return false;
		}
	}

	return true;
}
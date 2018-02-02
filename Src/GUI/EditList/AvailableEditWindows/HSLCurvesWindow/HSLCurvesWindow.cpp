// Copyright 2016 Jacob Chesley

#include "HSLCurvesWindow.h"

HSLCurvesWindow::HSLCurvesWindow(wxWindow * parent, wxString editName, Processor * processor) : EditWindow(parent, editName, processor) {

	parWindow = parent;
	this->SetBackgroundColour(parent->GetBackgroundColour());

	curveTabs = new wxNotebook(this, -1);

	hCurve = new CurvePanel(curveTabs, CURVE_CHANNEL_BRIGHT);
	sCurve = new CurvePanel(curveTabs, CURVE_CHANNEL_RED);
	lCurve = new CurvePanel(curveTabs, CURVE_CHANNEL_BLUE);

	hCurve->SetMinSize(wxSize(100, 100));
	sCurve->SetMinSize(wxSize(100, 100));
	lCurve->SetMinSize(wxSize(100, 100));

	// 2 Columns, 15 pixel vertical gap, 5 pixel horizontal gap
	gridSizer = new wxFlexGridSizer(2, 15, 5);

	rLabel = new wxStaticText(this, -1, "Red");
	gLabel = new wxStaticText(this, -1, "Green");
	bLabel = new wxStaticText(this, -1, "Blue");

	rScaleSlider = new DoubleSlider(this, 1.0, 0.0, 1.0, 100000);
	gScaleSlider = new DoubleSlider(this, 1.0, 0.0, 1.0, 100000);
	bScaleSlider = new DoubleSlider(this, 1.0, 0.0, 1.0, 100000);

	rLabel->SetForegroundColour(Colors::TextLightGrey);
	gLabel->SetForegroundColour(Colors::TextLightGrey);
	bLabel->SetForegroundColour(Colors::TextLightGrey);

	rScaleSlider->SetForegroundColour(Colors::TextLightGrey);
	rScaleSlider->SetBackgroundColour(parent->GetBackgroundColour());
	gScaleSlider->SetForegroundColour(Colors::TextLightGrey);
	gScaleSlider->SetBackgroundColour(parent->GetBackgroundColour());
	bScaleSlider->SetForegroundColour(Colors::TextLightGrey);
	bScaleSlider->SetBackgroundColour(parent->GetBackgroundColour());

	rScaleSlider->SetValuePosition(DoubleSlider::VALUE_INLINE_RIGHT);;
	gScaleSlider->SetValuePosition(DoubleSlider::VALUE_INLINE_RIGHT);
	bScaleSlider->SetValuePosition(DoubleSlider::VALUE_INLINE_RIGHT);

	curveTabs->SetBackgroundColour(wxColour(54, 54, 54));
	curveTabs->AddPage(hCurve, "H Channel", true);
	curveTabs->AddPage(sCurve, "S Channel", false);
	curveTabs->AddPage(lCurve, "L Channel", false);

	gridSizer->Add(rLabel);
	gridSizer->Add(rScaleSlider);
	gridSizer->Add(gLabel);
	gridSizer->Add(gScaleSlider);
	gridSizer->Add(bLabel);
	gridSizer->Add(bScaleSlider);

	container = new wxBoxSizer(wxVERTICAL);
	container->Add(curveTabs, 1, wxEXPAND);
	container->AddSpacer(10);
	container->Add(gridSizer);

	this->SetSizerAndFit(container);
	container->Layout();

	proc = processor;

	this->Bind(CURVE_CHANGED_EVENT, (wxObjectEventFunction)&HSLCurvesWindow::OnUpdate, this);
	this->Bind(wxEVT_SCROLL_CHANGED, (wxObjectEventFunction)&HSLCurvesWindow::OnUpdate, this);
	this->Bind(wxEVT_TEXT_ENTER, (wxObjectEventFunction)&HSLCurvesWindow::OnUpdate, this);
	this->StartWatchdog();
}

void HSLCurvesWindow::SetParamsAndFlags(ProcessorEdit * edit){
	
	if(edit == NULL){ return; }

	if(edit->GetEditType() != ProcessorEdit::EditType::HSL_CURVES){ return; }

	wxVector<Point> hControlPoints;
	wxVector<Point> sControlPoints;
	wxVector<Point> lControlPoints;

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
	for(size_t i = 0; i < edit->GetDoubleArraySize(0); i+=2){

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
	for(size_t i = 0; i < edit->GetDoubleArraySize(1); i+=2){

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
	for(size_t i = 0; i < edit->GetDoubleArraySize(2); i+=2){

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
	rScaleSlider->SetValue(edit->GetParam(0));
	bScaleSlider->SetValue(edit->GetParam(1));
	gScaleSlider->SetValue(edit->GetParam(2));
}

ProcessorEdit * HSLCurvesWindow::GetParamsAndFlags(){

	wxVector<Point> hControlPoints = hCurve->GetControlPoints();
	wxVector<Point> sControlPoints = sCurve->GetControlPoints();
	wxVector<Point> lControlPoints = lCurve->GetControlPoints();

	// Create double array that will store control points
	double * hPoints = new double[hControlPoints.size() * 2];
	double * sPoints = new double[sControlPoints.size() * 2];
	double * lPoints = new double[lControlPoints.size() * 2];

	unsigned int idx = 0;

	// Add h control points to a double array
	for (size_t p = 0; p < hControlPoints.size(); p ++) {
		hPoints[idx] = hControlPoints[p].x;
		hPoints[idx + 1] = hControlPoints[p].y;
		idx += 2;
	}
	idx = 0;

	// Add s control points to a double array
	for (size_t p = 0; p < sControlPoints.size(); p++) {
		sPoints[idx] = sControlPoints[p].x;
		sPoints[idx + 1] = sControlPoints[p].y;
		idx += 2;
	}
	idx = 0;

	// Add l control points to a double array
	for (size_t p = 0; p < lControlPoints.size(); p++) {
		lPoints[idx] = lControlPoints[p].x;
		lPoints[idx + 1] = lControlPoints[p].y;
		idx += 2;
	}
	idx = 0;
	
	ProcessorEdit * HSLCurveEdit = new ProcessorEdit(ProcessorEdit::EditType::HSL_CURVES);

	
	if (!isDisabled) {
		int numSteps16 = 65536;
		wxVector<int> hVector16 = hCurve->GetColorCurveMap(numSteps16, (float)numSteps16);
		wxVector<int> sVector16 = sCurve->GetColorCurveMap(numSteps16, (float)numSteps16);
		wxVector<int> lVector16 = lCurve->GetColorCurveMap(numSteps16, (float)numSteps16);
	
		int * hCurve16 = new int[numSteps16];
		int * sCurve16 = new int[numSteps16];
		int * lCurve16 = new int[numSteps16];

		for (int i = 0; i < numSteps16; i++) {
			hCurve16[i] = hVector16[i];
			sCurve16[i] = sVector16[i];
			lCurve16[i] = lVector16[i];
		}

		HSLCurveEdit->AddIntArray(hCurve16, numSteps16);
		HSLCurveEdit->AddIntArray(sCurve16, numSteps16);
		HSLCurveEdit->AddIntArray(lCurve16, numSteps16);
		
	}

	// Add control points double arrays to edit
	HSLCurveEdit->AddDoubleArray(hPoints, hControlPoints.size() * 2);
	HSLCurveEdit->AddDoubleArray(sPoints, sControlPoints.size() * 2);
	HSLCurveEdit->AddDoubleArray(lPoints, lControlPoints.size() * 2);
	HSLCurveEdit->AddParam(rScaleSlider->GetValue());
	HSLCurveEdit->AddParam(gScaleSlider->GetValue());
	HSLCurveEdit->AddParam(bScaleSlider->GetValue());

	// Set enabled / disabled
	HSLCurveEdit->SetDisabled(isDisabled);
	
	return HSLCurveEdit;
}

bool HSLCurvesWindow::CheckCopiedParamsAndFlags(){

	ProcessorEdit * edit = proc->GetEditForCopyPaste();
	if(edit == NULL){ return false; }

	if(edit->GetEditType() != ProcessorEdit::EditType::HSL_CURVES){ return false; }

	// Need 3 double arrars (H, S and L)
	if(edit->GetNumDoubleArrays() != 3 && edit->GetParamsSize() != 3){
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
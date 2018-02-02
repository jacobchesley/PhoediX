// Copyright 2016 Jacob Chesley

#include "LABCurvesWindow.h"

LABCurvesWindow::LABCurvesWindow(wxWindow * parent, wxString editName, Processor * processor) : EditWindow(parent, editName, processor) {

	parWindow = parent;
	this->SetBackgroundColour(parent->GetBackgroundColour());

	container = new wxBoxSizer(wxVERTICAL);
	curveTabs = new wxNotebook(this, -1);
	
	lCurve = new CurvePanel(curveTabs, CURVE_CHANNEL_BRIGHT);
	aCurve = new CurvePanel(curveTabs, CURVE_CHANNEL_RED);
	bCurve = new CurvePanel(curveTabs, CURVE_CHANNEL_BLUE);

	lCurve->SetMinSize(wxSize(100, 100));
	aCurve->SetMinSize(wxSize(100, 100));
	bCurve->SetMinSize(wxSize(100, 100));

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
	curveTabs->AddPage(lCurve, "L Channel", true);
	curveTabs->AddPage(aCurve, "A Channel", false);
	curveTabs->AddPage(bCurve, "B Channel", false);

	gridSizer->Add(rLabel);
	gridSizer->Add(rScaleSlider);
	gridSizer->Add(gLabel);
	gridSizer->Add(gScaleSlider);
	gridSizer->Add(bLabel);
	gridSizer->Add(bScaleSlider);

	container->Add(curveTabs, 1, wxEXPAND);
	container->AddSpacer(10);
	container->Add(gridSizer);

	this->SetSizerAndFit(container);
	container->Layout();

	proc = processor;

	this->Bind(CURVE_CHANGED_EVENT, (wxObjectEventFunction)&LABCurvesWindow::OnUpdate, this);
	this->Bind(wxEVT_SCROLL_CHANGED, (wxObjectEventFunction)&LABCurvesWindow::OnUpdate, this);
	this->Bind(wxEVT_TEXT_ENTER, (wxObjectEventFunction)&LABCurvesWindow::OnUpdate, this);
	this->StartWatchdog();
}

void LABCurvesWindow::SetParamsAndFlags(ProcessorEdit * edit){

	if(edit == NULL){ return; }

	wxVector<Point> lControlPoints;
	wxVector<Point> aControlPoints;
	wxVector<Point> bControlPoints;

	// Must have 3 double arrays (l, a, and b curves)
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

	// Set l control points
	double * lArray = edit->GetDoubleArray(0);
	int curID = 0;
	for(size_t i = 0; i < edit->GetDoubleArraySize(0); i+=2){

		Point lPoint;
		lPoint.x = lArray[i];
		lPoint.y = lArray[i + 1];
		lPoint.id = curID;
		lControlPoints.push_back(lPoint);
		curID += 1;
	}

	// Set a control points
	double * aArray = edit->GetDoubleArray(1);
	curID = 0;
	for(size_t i = 0; i < edit->GetDoubleArraySize(1); i+=2){

		Point aPoint;
		aPoint.x = aArray[i];
		aPoint.y = aArray[i + 1];
		aPoint.id = curID;
		aControlPoints.push_back(aPoint);

		curID += 1;
	}

	// Set b control points
	double * bArray = edit->GetDoubleArray(2);
	curID = 0;
	for(size_t i = 0; i < edit->GetDoubleArraySize(2); i+=2){

		Point bPoint;
		bPoint.x = bArray[i];
		bPoint.y = bArray[i + 1];
		bPoint.id = curID;
		bControlPoints.push_back(bPoint);

		curID += 1;
	}

	// Set curves
	lCurve->SetControlPoints(lControlPoints);
	aCurve->SetControlPoints(aControlPoints);
	bCurve->SetControlPoints(bControlPoints);
	rScaleSlider->SetValue(edit->GetParam(0));
	bScaleSlider->SetValue(edit->GetParam(1));
	gScaleSlider->SetValue(edit->GetParam(2));
}

ProcessorEdit * LABCurvesWindow::GetParamsAndFlags(){

	wxVector<Point> lControlPoints = lCurve->GetControlPoints();
	wxVector<Point> aControlPoints = aCurve->GetControlPoints();
	wxVector<Point> bControlPoints = bCurve->GetControlPoints();

	// Create double array that will store control points
	double * lPoints = new double[lControlPoints.size() * 2];
	double * aPoints = new double[aControlPoints.size() * 2];
	double * bPoints = new double[bControlPoints.size() * 2];

	unsigned int idx = 0;

	// Add l control points to a double array
	for (size_t p = 0; p < lControlPoints.size(); p ++) {
		lPoints[idx] = lControlPoints[p].x;
		lPoints[idx + 1] =lControlPoints[p].y;
		idx += 2;
	}
	idx = 0;

	// Add a control points to a double array
	for (size_t p = 0; p < aControlPoints.size(); p++) {
		aPoints[idx] = aControlPoints[p].x;
		aPoints[idx + 1] = aControlPoints[p].y;
		idx += 2;
	}
	idx = 0;

	// Add b control points to a double array
	for (size_t p = 0; p < bControlPoints.size(); p++) {
		bPoints[idx] = bControlPoints[p].x;
		bPoints[idx + 1] = bControlPoints[p].y;
		idx += 2;
	}
	idx = 0;
	
	ProcessorEdit * labCurveEdit = new ProcessorEdit(ProcessorEdit::EditType::LAB_CURVES);

	if (!isDisabled) {
		int numSteps16 = 65536;
		wxVector<int> lVector16 = lCurve->GetColorCurveMap(numSteps16, (float)numSteps16);
		wxVector<int> aVector16 = aCurve->GetColorCurveMap(numSteps16, (float)numSteps16);
		wxVector<int> bVector16 = bCurve->GetColorCurveMap(numSteps16, (float)numSteps16);

		int * lCurve16 = new int[numSteps16];
		int * aCurve16 = new int[numSteps16];
		int * bCurve16 = new int[numSteps16];

		for (int i = 0; i < numSteps16; i++) {
			lCurve16[i] = lVector16[i];
			aCurve16[i] = aVector16[i];
			bCurve16[i] = bVector16[i];
		}

		labCurveEdit->AddIntArray(lCurve16, numSteps16);
		labCurveEdit->AddIntArray(aCurve16, numSteps16);
		labCurveEdit->AddIntArray(bCurve16, numSteps16);
	}

	// Add control points double arrays to edit
	labCurveEdit->AddDoubleArray(lPoints, lControlPoints.size() * 2);
	labCurveEdit->AddDoubleArray(aPoints, aControlPoints.size() * 2);
	labCurveEdit->AddDoubleArray(bPoints, bControlPoints.size() * 2);
	labCurveEdit->AddParam(rScaleSlider->GetValue());
	labCurveEdit->AddParam(gScaleSlider->GetValue());
	labCurveEdit->AddParam(bScaleSlider->GetValue());

	// Set enabled / disabled
	labCurveEdit->SetDisabled(isDisabled);

	return labCurveEdit;
}

bool LABCurvesWindow::CheckCopiedParamsAndFlags(){

	ProcessorEdit * edit = proc->GetEditForCopyPaste();
	if(edit == NULL){ return false; }

	if(edit->GetEditType() != ProcessorEdit::EditType::LAB_CURVES){ return false; }

	// Need 3 double arrars (L, A and B)
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
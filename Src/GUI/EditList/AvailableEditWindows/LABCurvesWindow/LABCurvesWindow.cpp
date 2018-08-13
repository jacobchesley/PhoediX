// Copyright 2018 Jacob Chesley.
// See LICENSE.TXT in root of project for license information.

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
	this->Bind(wxEVT_SCROLL_THUMBRELEASE, (wxObjectEventFunction)&LABCurvesWindow::OnUpdate, this);
	this->Bind(wxEVT_TEXT_ENTER, (wxObjectEventFunction)&LABCurvesWindow::OnUpdate, this);
	this->StartWatchdog();
}

void LABCurvesWindow::SetParamsAndFlags(ProcessorEdit * edit){

	if(edit == NULL){ return; }

	wxVector<Point> lControlPoints;
	wxVector<Point> aControlPoints;
	wxVector<Point> bControlPoints;

	double * lArray = edit->GetDoubleArray(PHOEDIX_PARAMETER_L_CURVE);
	double * aArray = edit->GetDoubleArray(PHOEDIX_PARAMETER_A_CURVE);
	double * bArray = edit->GetDoubleArray(PHOEDIX_PARAMETER_B_CURVE);

	int lArraySize = edit->GetDoubleArraySize(PHOEDIX_PARAMETER_L_CURVE);
	int aArraySize = edit->GetDoubleArraySize(PHOEDIX_PARAMETER_A_CURVE);
	int bArraySize = edit->GetDoubleArraySize(PHOEDIX_PARAMETER_B_CURVE);

	// L Array has at least 4 points and is an even number (each point has x and y)
	if(lArraySize >= 4 || lArraySize %2 == 0){

		bool hArrayValid = true;

		// Check each element in array to verify it is between 0 and 1
		for(int i = 0; i < lArraySize; i++){
			if(lArray[i] < 0.0 || lArray[i] > 1.0){
				hArrayValid = false;
				break;
			}
		}

		// Push points from array to control point vector
		if(hArrayValid){
			int curID = 0;
			for(size_t i = 0; i < lArraySize; i+=2){

				Point lPoint;
				lPoint.x = lArray[i];
				lPoint.y = lArray[i + 1];
				lPoint.id = curID;
				lControlPoints.push_back(lPoint);
				curID += 1;
			}
		}
	}

	// A Array has at least 4 points and is an even number (each point has x and y)
	if(aArraySize >= 4 || aArraySize %2 == 0){

		bool aArrayValid = true;

		// Check each element in array to verify it is between 0 and 1
		for(int i = 0; i < aArraySize; i++){
			if(aArray[i] < 0.0 || aArray[i] > 1.0){
				aArrayValid = false;
				break;
			}
		}

		// Push points from array to control point vector
		if(aArrayValid){
			int curID = 0;
			for(size_t i = 0; i < aArraySize; i+=2){

				Point aPoint;
				aPoint.x = aArray[i];
				aPoint.y = aArray[i + 1];
				aPoint.id = curID;
				aControlPoints.push_back(aPoint);
				curID += 1;
			}
		}
	}

	// B Array has at least 4 points and is an even number (each point has x and y)
	if(bArraySize >= 4 || bArraySize %2 == 0){

		bool bArrayValid = true;

		// Check each element in array to verify it is between 0 and 1
		for(int i = 0; i < bArraySize; i++){
			if(bArray[i] < 0.0 || bArray[i] > 1.0){
				bArrayValid = false;
				break;
			}
		}

		// Push points from array to control point vector
		if(bArrayValid){
			int curID = 0;
			for(size_t i = 0; i < bArraySize; i+=2){

				Point bPoint;
				bPoint.x = bArray[i];
				bPoint.y = bArray[i + 1];
				bPoint.id = curID;
				bControlPoints.push_back(bPoint);
				curID += 1;
			}
		}
	}

	// Set curves
	lCurve->SetControlPoints(lControlPoints);
	aCurve->SetControlPoints(aControlPoints);
	bCurve->SetControlPoints(bControlPoints);

	if(edit->CheckForParameter(PHOEDIX_PARAMETER_RED_SCALE)) { rScaleSlider->SetValue(edit->GetParam(PHOEDIX_PARAMETER_RED_SCALE)); }
	if(edit->CheckForParameter(PHOEDIX_PARAMETER_GREEN_SCALE)) { gScaleSlider->SetValue(edit->GetParam(PHOEDIX_PARAMETER_GREEN_SCALE)); }
	if(edit->CheckForParameter(PHOEDIX_PARAMETER_BLUE_SCALE)) { bScaleSlider->SetValue(edit->GetParam(PHOEDIX_PARAMETER_BLUE_SCALE)); }
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

		int numCurves = 3;
		int curveComplete = 0;
		wxMutex mutexLock;
		wxCondition wait(mutexLock);

		int * lCurve16 = new int[numSteps16];
		int * aCurve16 = new int[numSteps16];
		int * bCurve16 = new int[numSteps16];

		GetCurveThread * lCurveThread = new GetCurveThread(lCurve, numSteps16, lCurve16, numCurves, &curveComplete, &mutexLock, &wait); lCurveThread->Run();
		GetCurveThread * aCurveThread = new GetCurveThread(aCurve, numSteps16, aCurve16, numCurves, &curveComplete, &mutexLock, &wait); aCurveThread->Run();
		GetCurveThread * bCurveThread = new GetCurveThread(bCurve, numSteps16, bCurve16, numCurves, &curveComplete, &mutexLock, &wait); bCurveThread->Run();

		wait.Wait();

		labCurveEdit->AddIntArray(PHOEDIX_PARAMETER_L_CURVE, lCurve16, numSteps16);
		labCurveEdit->AddIntArray(PHOEDIX_PARAMETER_A_CURVE, aCurve16, numSteps16);
		labCurveEdit->AddIntArray(PHOEDIX_PARAMETER_B_CURVE, bCurve16, numSteps16);
	}

	// Add control points double arrays to edit
	labCurveEdit->AddDoubleArray(PHOEDIX_PARAMETER_L_CURVE, lPoints, lControlPoints.size() * 2);
	labCurveEdit->AddDoubleArray(PHOEDIX_PARAMETER_A_CURVE,aPoints, aControlPoints.size() * 2);
	labCurveEdit->AddDoubleArray(PHOEDIX_PARAMETER_B_CURVE,bPoints, bControlPoints.size() * 2);
	labCurveEdit->AddParam(PHOEDIX_PARAMETER_RED_SCALE, rScaleSlider->GetValue());
	labCurveEdit->AddParam(PHOEDIX_PARAMETER_GREEN_SCALE, gScaleSlider->GetValue());
	labCurveEdit->AddParam(PHOEDIX_PARAMETER_BLUE_SCALE, bScaleSlider->GetValue());

	// Set enabled / disabled
	labCurveEdit->SetDisabled(isDisabled);

	return labCurveEdit;
}

bool LABCurvesWindow::CheckCopiedParamsAndFlags(){

	ProcessorEdit * edit = proc->GetEditForCopyPaste();
	if(edit == NULL){ return false; }

	if(edit->GetEditType() != ProcessorEdit::EditType::LAB_CURVES){ return false; }

	return true;
}

LABCurvesWindow::GetCurveThread::GetCurveThread(CurvePanel * curvePanelIn, int curveSize, int * curveOut, int numCurves, int * numCurveComplete, wxMutex * mutLockIn, wxCondition * condition) : wxThread(wxTHREAD_DETACHED){
	curvePanel = curvePanelIn;
	numPoints = curveSize;
	curve = curveOut;

	threads = numCurves;
	complete = numCurveComplete;
	mutLock = mutLockIn;
	cond = condition;
}

wxThread::ExitCode LABCurvesWindow::GetCurveThread::Entry(){

	curvePanel->GetColorCurveMap(numPoints, curve, (float) numPoints);

	mutLock->Lock();
	*complete += 1;

	// All worker threads have finished, signal condition to continue
	if (*complete == threads) {
		cond->Broadcast();
	}
	
	mutLock->Unlock();
	return (wxThread::ExitCode) 0;
}
// Copyright 2018 Jacob Chesley.
// See LICENSE.TXT in root of project for license information.

#include "HSLCurvesWindow.h"

HSLCurvesWindow::HSLCurvesWindow(wxWindow * parent, wxString editName, Processor * processor) : EditWindow(parent, editName, processor) {

	parWindow = parent;
	this->SetBackgroundColour(parent->GetBackgroundColour());

	curveTabs = new wxNotebook(this, -1);

	hCurve = new CurvePanel(curveTabs, CURVE_CHANNEL_HUE);
	sCurve = new CurvePanel(curveTabs, CURVE_CHANNEL_GREY_TO_WHITE);
	lCurve = new CurvePanel(curveTabs, CURVE_CHANNEL_BRIGHT);

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

	curveTabs->SetBackgroundColour(parent->GetBackgroundColour());
	curveTabs->SetForegroundColour(Colors::TextLightGrey);
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
	this->Bind(wxEVT_SCROLL_THUMBRELEASE, (wxObjectEventFunction)&HSLCurvesWindow::OnUpdate, this);
	this->Bind(wxEVT_TEXT_ENTER, (wxObjectEventFunction)&HSLCurvesWindow::OnUpdate, this);
	this->StartWatchdog();
}

void HSLCurvesWindow::SetParamsAndFlags(ProcessorEdit * edit){
	
	if(edit == NULL){ return; }

	if(edit->GetEditType() != ProcessorEdit::EditType::HSL_CURVES){ return; }

	wxVector<Point> hControlPoints;
	wxVector<Point> sControlPoints;
	wxVector<Point> lControlPoints;

	double * hArray = edit->GetDoubleArray(PHOEDIX_PARAMETER_H_CURVE);
	double * sArray = edit->GetDoubleArray(PHOEDIX_PARAMETER_S_CURVE);
	double * lArray = edit->GetDoubleArray(PHOEDIX_PARAMETER_L_CURVE);

	int hArraySize = edit->GetDoubleArraySize(PHOEDIX_PARAMETER_H_CURVE);
	int sArraySize = edit->GetDoubleArraySize(PHOEDIX_PARAMETER_S_CURVE);
	int lArraySize = edit->GetDoubleArraySize(PHOEDIX_PARAMETER_L_CURVE);

	Point defaultPoint1;
	Point defaultPoint2;

	defaultPoint1.x = 0.0;
	defaultPoint1.y = 1.0;
	defaultPoint1.id = 0;

	defaultPoint2.x = 1.0;
	defaultPoint2.y = 0.0;
	defaultPoint2.id = 1;

	// H Array has at least 4 points and is an even number (each point has x and y)
	if(hArraySize >= 4 || hArraySize %2 == 0){

		bool hArrayValid = true;

		// Check each element in array to verify it is between 0 and 1
		for(int i = 0; i < hArraySize; i++){
			if(hArray[i] < 0.0 || hArray[i] > 1.0){
				hArrayValid = false;
				break;
			}
		}

		// Push points from array to control point vector
		if(hArrayValid){
			int curID = 0;
			for(size_t i = 0; i < hArraySize; i+=2){

				Point hPoint;
				hPoint.x = hArray[i];
				hPoint.y = hArray[i + 1];
				hPoint.id = curID;
				hControlPoints.push_back(hPoint);
				curID += 1;
			}
		}
		else {
			hControlPoints.push_back(defaultPoint1);
			hControlPoints.push_back(defaultPoint2);
		}
	}
	else {
		hControlPoints.push_back(defaultPoint1);
		hControlPoints.push_back(defaultPoint2);
	}

	// S Array has at least 4 points and is an even number (each point has x and y)
	if(sArraySize >= 4 || sArraySize %2 == 0){

		bool sArrayValid = true;

		// Check each element in array to verify it is between 0 and 1
		for(int i = 0; i < sArraySize; i++){
			if(sArray[i] < 0.0 || sArray[i] > 1.0){
				sArrayValid = false;
				break;
			}
		}

		// Push points from array to control point vector
		if(sArrayValid){
			int curID = 0;
			for(size_t i = 0; i < sArraySize; i+=2){

				Point sPoint;
				sPoint.x = sArray[i];
				sPoint.y = sArray[i + 1];
				sPoint.id = curID;
				sControlPoints.push_back(sPoint);
				curID += 1;
			}
		}
		else {
			sControlPoints.push_back(defaultPoint1);
			sControlPoints.push_back(defaultPoint2);
		}
	}
	else {
		sControlPoints.push_back(defaultPoint1);
		sControlPoints.push_back(defaultPoint2);
	}

	// L Array has at least 4 points and is an even number (each point has x and y)
	if(sArraySize >= 4 || sArraySize %2 == 0){

		bool lArrayValid = true;

		// Check each element in array to verify it is between 0 and 1
		for(int i = 0; i < lArraySize; i++){
			if(lArray[i] < 0.0 || lArray[i] > 1.0){
				lArrayValid = false;
				break;
			}
		}

		// Push points from array to control point vector
		if(lArrayValid){
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
		else {
			lControlPoints.push_back(defaultPoint1);
			lControlPoints.push_back(defaultPoint2);
		}
	}
	else {
		lControlPoints.push_back(defaultPoint1);
		lControlPoints.push_back(defaultPoint2);
	}

	// Set curves
	hCurve->SetControlPoints(hControlPoints);
	sCurve->SetControlPoints(sControlPoints);
	lCurve->SetControlPoints(lControlPoints);

	if(edit->CheckForParameter(PHOEDIX_PARAMETER_RED_SCALE)) { rScaleSlider->SetValue(edit->GetParam(PHOEDIX_PARAMETER_RED_SCALE)); }
	if(edit->CheckForParameter(PHOEDIX_PARAMETER_GREEN_SCALE)) { gScaleSlider->SetValue(edit->GetParam(PHOEDIX_PARAMETER_GREEN_SCALE)); }
	if(edit->CheckForParameter(PHOEDIX_PARAMETER_BLUE_SCALE)) { bScaleSlider->SetValue(edit->GetParam(PHOEDIX_PARAMETER_BLUE_SCALE)); }
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

		int numCurves = 3;
		int curveComplete = 0;
		wxMutex mutexLock;
		wxCondition wait(mutexLock);

		int numSteps16 = 65536;
		int * hCurve16 = new int[numSteps16];
		int * sCurve16 = new int[numSteps16];
		int * lCurve16 = new int[numSteps16];

		GetCurveThread * hCurveThread = new GetCurveThread(hCurve, numSteps16, hCurve16, numCurves, &curveComplete, &mutexLock, &wait); hCurveThread->Run();
		GetCurveThread * sCurveThread = new GetCurveThread(sCurve, numSteps16, sCurve16, numCurves, &curveComplete, &mutexLock, &wait); sCurveThread->Run();
		GetCurveThread * lCurveThread = new GetCurveThread(lCurve, numSteps16, lCurve16, numCurves, &curveComplete, &mutexLock, &wait); lCurveThread->Run();

		wait.Wait();
		mutexLock.Unlock();

		HSLCurveEdit->AddIntArray(PHOEDIX_PARAMETER_H_CURVE, hCurve16, numSteps16);
		HSLCurveEdit->AddIntArray(PHOEDIX_PARAMETER_S_CURVE, sCurve16, numSteps16);
		HSLCurveEdit->AddIntArray(PHOEDIX_PARAMETER_L_CURVE, lCurve16, numSteps16);
		
	}

	// Add control points double arrays to edit
	HSLCurveEdit->AddDoubleArray(PHOEDIX_PARAMETER_H_CURVE, hPoints, hControlPoints.size() * 2);
	HSLCurveEdit->AddDoubleArray(PHOEDIX_PARAMETER_S_CURVE, sPoints, sControlPoints.size() * 2);
	HSLCurveEdit->AddDoubleArray(PHOEDIX_PARAMETER_L_CURVE, lPoints, lControlPoints.size() * 2);
	HSLCurveEdit->AddParam(PHOEDIX_PARAMETER_RED_SCALE, rScaleSlider->GetValue());
	HSLCurveEdit->AddParam(PHOEDIX_PARAMETER_GREEN_SCALE, gScaleSlider->GetValue());
	HSLCurveEdit->AddParam(PHOEDIX_PARAMETER_BLUE_SCALE, bScaleSlider->GetValue());

	// Set enabled / disabled
	HSLCurveEdit->SetDisabled(isDisabled);
	
	return HSLCurveEdit;
}

bool HSLCurvesWindow::CheckCopiedParamsAndFlags(){

	ProcessorEdit * edit = proc->GetEditForCopyPaste();
	if(edit == NULL){ return false; }

	if(edit->GetEditType() != ProcessorEdit::EditType::HSL_CURVES){ return false; }

	return true;
}

HSLCurvesWindow::GetCurveThread::GetCurveThread(CurvePanel * curvePanelIn, int curveSize, int * curveOut, int numCurves, int * numCurveComplete, wxMutex * mutLockIn, wxCondition * condition) : wxThread(wxTHREAD_DETACHED){
	curvePanel = curvePanelIn;
	numPoints = curveSize;
	curve = curveOut;

	threads = numCurves;
	complete = numCurveComplete;
	mutLock = mutLockIn;
	cond = condition;
}

wxThread::ExitCode HSLCurvesWindow::GetCurveThread::Entry(){

	curvePanel->GetColorCurveMap(numPoints, curve, (float) numPoints);

	mutLock->Lock();
	*complete += 1;
	mutLock->Unlock();

	// All worker threads have finished, signal condition to continue
	if (*complete == threads) {
		cond->Broadcast();
	}
	
	return (wxThread::ExitCode) 0;
}
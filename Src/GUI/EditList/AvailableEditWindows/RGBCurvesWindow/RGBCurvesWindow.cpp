// Copyright 2018 Jacob Chesley.
// See LICENSE.TXT in root of project for license information.

#include "RGBCurvesWindow.h"

RGBCurvesWindow::RGBCurvesWindow(wxWindow * parent, wxString editName, Processor * processor) : EditWindow(parent, editName, processor) {

	parWindow = parent;
	this->SetBackgroundColour(parent->GetBackgroundColour());

	curveTabs = new wxNotebook(this, -1);
	
	brightCurve = new CurvePanel(curveTabs, CURVE_CHANNEL_BRIGHT);
	redCurve = new CurvePanel(curveTabs, CURVE_CHANNEL_RED);
	greenCurve = new CurvePanel(curveTabs, CURVE_CHANNEL_GREEN);
	blueCurve = new CurvePanel(curveTabs, CURVE_CHANNEL_BLUE);

	curveTabs->SetBackgroundColour(parent->GetBackgroundColour());
	curveTabs->SetForegroundColour(Colors::TextLightGrey);
	curveTabs->AddPage(brightCurve, "All Channels", true);
	curveTabs->AddPage(redCurve, "Red Channel", false);
	curveTabs->AddPage(greenCurve, "Green Channel", false);
	curveTabs->AddPage(blueCurve, "Blue Channel", false);

	container = new wxBoxSizer(wxVERTICAL);

	container->Add(curveTabs, 1, wxEXPAND);

	this->SetSizerAndFit(container);
	container->Layout();
	
	proc = processor;

	this->Bind(CURVE_CHANGED_EVENT, (wxObjectEventFunction)&RGBCurvesWindow::OnUpdate, this);

	this->SetSize(350, 350);
}

void RGBCurvesWindow::SetParamsAndFlags(ProcessorEdit * edit){

	wxVector<Point> brightControlPoints;
	wxVector<Point> redControlPoints;
	wxVector<Point> greenControlPoints;
	wxVector<Point> blueControlPoints;

	double * brightArray = edit->GetDoubleArray(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_BRIGHT_CURVE);
	double * rArray = edit->GetDoubleArray(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_R_CURVE);
	double * gArray = edit->GetDoubleArray(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_G_CURVE);
	double * bArray = edit->GetDoubleArray(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_B_CURVE);

	int brightArraySize = edit->GetDoubleArraySize(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_BRIGHT_CURVE);
	int rArraySize = edit->GetDoubleArraySize(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_R_CURVE);
	int gArraySize = edit->GetDoubleArraySize(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_G_CURVE);
	int bArraySize = edit->GetDoubleArraySize(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_B_CURVE);

	Point defaultPoint1;
	Point defaultPoint2;

	defaultPoint1.x = 0.0;
	defaultPoint1.y = 1.0;
	defaultPoint1.id = 0;

	defaultPoint2.x = 1.0;
	defaultPoint2.y = 0.0;
	defaultPoint2.id = 1;

	// Bright Array has at least 4 points and is an even number (each point has x and y)
	if(brightArraySize >= 4 && brightArraySize %2 == 0){

		bool brightArrayValid = true;

		// Check each element in array to verify it is between 0 and 1
		for(int i = 0; i < brightArraySize; i++){
			if(brightArray[i] < 0.0 || brightArray[i] > 1.0){
				brightArrayValid = false;
				break;
			}
		}

		// Push points from array to control point vector
		if(brightArrayValid){
			int curID = 0;
			for(size_t i = 0; i < brightArraySize; i+=2){

				Point brightPoint;
				brightPoint.x = brightArray[i];
				brightPoint.y = brightArray[i + 1];
				brightPoint.id = curID;
				brightControlPoints.push_back(brightPoint);
				curID += 1;
			}
		}
		else {
			brightControlPoints.push_back(defaultPoint1);
			brightControlPoints.push_back(defaultPoint2);
		}
	}
	else {
		brightControlPoints.push_back(defaultPoint1);
		brightControlPoints.push_back(defaultPoint2);
	}

	// R Array has at least 4 points and is an even number (each point has x and y)
	if(rArraySize >= 4 && rArraySize %2 == 0){

		bool rArrayValid = true;

		// Check each element in array to verify it is between 0 and 1
		for(int i = 0; i < rArraySize; i++){
			if(rArray[i] < 0.0 || rArray[i] > 1.0){
				rArrayValid = false;
				break;
			}
		}

		// Push points from array to control point vector
		if(rArrayValid){
			int curID = 0;
			for(size_t i = 0; i < rArraySize; i+=2){

				Point rPoint;
				rPoint.x = rArray[i];
				rPoint.y = rArray[i + 1];
				rPoint.id = curID;
				redControlPoints.push_back(rPoint);
				curID += 1;
			}
		}
		else {
			redControlPoints.push_back(defaultPoint1);
			redControlPoints.push_back(defaultPoint2);
		}
	}
	else {
		redControlPoints.push_back(defaultPoint1);
		redControlPoints.push_back(defaultPoint2);
	}

	// G Array has at least 4 points and is an even number (each point has x and y)
	if(gArraySize >= 4 && gArraySize %2 == 0){

		bool gArrayValid = true;

		// Check each element in array to verify it is between 0 and 1
		for(int i = 0; i < gArraySize; i++){
			if(gArray[i] < 0.0 || gArray[i] > 1.0){
				gArrayValid = false;
				break;
			}
		}

		// Push points from array to control point vector
		if(gArrayValid){
			int curID = 0;
			for(size_t i = 0; i < gArraySize; i+=2){

				Point gPoint;
				gPoint.x = gArray[i];
				gPoint.y = gArray[i + 1];
				gPoint.id = curID;
				greenControlPoints.push_back(gPoint);
				curID += 1;
			}
		}
		else {
			greenControlPoints.push_back(defaultPoint1);
			greenControlPoints.push_back(defaultPoint2);
		}
	}
	else {
		greenControlPoints.push_back(defaultPoint1);
		greenControlPoints.push_back(defaultPoint2);
	}

	// B Array has at least 4 points and is an even number (each point has x and y)
	if(bArraySize >= 4 && bArraySize %2 == 0){

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
				blueControlPoints.push_back(bPoint);
				curID += 1;
			}
		}
		else {
			blueControlPoints.push_back(defaultPoint1);
			blueControlPoints.push_back(defaultPoint2);
		}
	}
	else {
		blueControlPoints.push_back(defaultPoint1);
		blueControlPoints.push_back(defaultPoint2);
	}

	// Set curves
	brightCurve->SetControlPoints(brightControlPoints);
	redCurve->SetControlPoints(redControlPoints);
	greenCurve->SetControlPoints(greenControlPoints);
	blueCurve->SetControlPoints(blueControlPoints);
}

ProcessorEdit * RGBCurvesWindow::GetParamsAndFlags(){

	wxVector<Point> brightControlPoints = brightCurve->GetControlPoints();
	wxVector<Point> redControlPoints = redCurve->GetControlPoints();
	wxVector<Point> greenControlPoints = greenCurve->GetControlPoints();
	wxVector<Point> blueControlPoints = blueCurve->GetControlPoints();

	// Create double array that will store control points
	double * brightPoints = new double[brightControlPoints.size() * 2];
	double * redPoints = new double[redControlPoints.size() * 2];
	double * greenPoints = new double[greenControlPoints.size() * 2];
	double * bluePoints = new double[blueControlPoints.size() * 2];

	unsigned int idx = 0;

	// Add brightness control points to a double array
	for (size_t p = 0; p < brightControlPoints.size(); p ++) {
		brightPoints[idx] = brightControlPoints[p].x;
		brightPoints[idx + 1] = brightControlPoints[p].y;
		idx += 2;
	}
	idx = 0;

	// Add red control points to a double array
	for (size_t p = 0; p < redControlPoints.size(); p++) {
		redPoints[idx] = redControlPoints[p].x;
		redPoints[idx + 1] = redControlPoints[p].y;
		idx += 2;
	}
	idx = 0;

	// Add green control points to a double array
	for (size_t p = 0; p < greenControlPoints.size(); p++) {
		greenPoints[idx] = greenControlPoints[p].x;
		greenPoints[idx + 1] = greenControlPoints[p].y;
		idx += 2;
	}
	idx = 0;

	// Add blue control points to a double array
	for (size_t p = 0; p < blueControlPoints.size(); p++) {
		bluePoints[idx] = blueControlPoints[p].x;
		bluePoints[idx + 1] = blueControlPoints[p].y;
		idx += 2;
	}
	idx = 0;

	// Create RGB Curve edit
	ProcessorEdit * rgbCurveEdit = new ProcessorEdit(ProcessorEdit::EditType::RGB_CURVES);

	// Get 8 bit color map
	if (!isDisabled) {

		int numSteps8 = 256;
		int numSteps16 = 65536;

		// Create 8 bit int arrays
		int * brightCurve8 = new int[numSteps8];
		int * redCurve8 = new int[numSteps8];
		int * greenCurve8 = new int[numSteps8];
		int * blueCurve8 = new int[numSteps8];

		// Create 16 bit int arrays
		int * brightCurve16 = new int[numSteps16];
		int * redCurve16 = new int[numSteps16];
		int * greenCurve16 = new int[numSteps16];
		int * blueCurve16 = new int[numSteps16];

		GetCurveThread * lCurveThread8 = new GetCurveThread(brightCurve, numSteps8, brightCurve8); lCurveThread8->Run();
		GetCurveThread * rCurveThread8 = new GetCurveThread(redCurve, numSteps8, redCurve8); rCurveThread8->Run();
		GetCurveThread * gCurveThread8 = new GetCurveThread(greenCurve, numSteps8, greenCurve8); gCurveThread8->Run();
		GetCurveThread * bCurveThread8 = new GetCurveThread(blueCurve, numSteps8, blueCurve8); bCurveThread8->Run();

		lCurveThread8->Wait(); wxYield();
		rCurveThread8->Wait(); wxYield();
		gCurveThread8->Wait(); wxYield();
		bCurveThread8->Wait(); wxYield();

		delete lCurveThread8;
		delete rCurveThread8;
		delete gCurveThread8;
		delete bCurveThread8;

		GetCurveThread * lCurveThread16 = new GetCurveThread(brightCurve, numSteps16, brightCurve16); lCurveThread16->Run();
		GetCurveThread * rCurveThread16 = new GetCurveThread(redCurve, numSteps16, redCurve16); rCurveThread16->Run();
		GetCurveThread * gCurveThread16 = new GetCurveThread(greenCurve, numSteps16, greenCurve16); gCurveThread16->Run();
		GetCurveThread * bCurveThread16 = new GetCurveThread(blueCurve, numSteps16, blueCurve16); bCurveThread16->Run();

		lCurveThread16->Wait(); wxYield();
		rCurveThread16->Wait(); wxYield();
		gCurveThread16->Wait(); wxYield();
		bCurveThread16->Wait(); wxYield();

		delete lCurveThread16;
		delete rCurveThread16;
		delete gCurveThread16;
		delete bCurveThread16;

		// Add 8 bit map
		rgbCurveEdit->AddIntArray(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_BRIGHT_CURVE, brightCurve8, numSteps8);
		rgbCurveEdit->AddIntArray(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_R_CURVE, redCurve8, numSteps8);
		rgbCurveEdit->AddIntArray(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_G_CURVE, greenCurve8, numSteps8);
		rgbCurveEdit->AddIntArray(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_B_CURVE, blueCurve8, numSteps8);

		// Add 16 bit map
		rgbCurveEdit->AddIntArray(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_BRIGHT_CURVE_16, brightCurve16, numSteps16);
		rgbCurveEdit->AddIntArray(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_R_CURVE_16, redCurve16, numSteps16);
		rgbCurveEdit->AddIntArray(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_G_CURVE_16, greenCurve16, numSteps16);
		rgbCurveEdit->AddIntArray(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_B_CURVE_16, blueCurve16, numSteps16);
	}

	// Add control points double arrays to edit
	rgbCurveEdit->AddDoubleArray(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_BRIGHT_CURVE, brightPoints, brightControlPoints.size() * 2);
	rgbCurveEdit->AddDoubleArray(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_R_CURVE, redPoints, redControlPoints.size() * 2);
	rgbCurveEdit->AddDoubleArray(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_G_CURVE, greenPoints, greenControlPoints.size() * 2);
	rgbCurveEdit->AddDoubleArray(ProcessorEdit::ParametersFlags::PHOEDIX_PARAMETER_B_CURVE, bluePoints, blueControlPoints.size() * 2);

	// Set enabled / disabled
	rgbCurveEdit->SetDisabled(isDisabled);

	return rgbCurveEdit;
}

bool RGBCurvesWindow::CheckCopiedParamsAndFlags(){

	ProcessorEdit * edit = proc->GetEditForCopyPaste();
	if(edit == NULL){ return false; }

	if(edit->GetEditType() != ProcessorEdit::EditType::RGB_CURVES){ return false; }

	return true;
}

RGBCurvesWindow::GetCurveThread::GetCurveThread(CurvePanel * curvePanelIn, int curveSize, int * curveOut) : wxThread(wxTHREAD_JOINABLE){
	curvePanel = curvePanelIn;
	numPoints = curveSize;
	curve = curveOut;
}

wxThread::ExitCode RGBCurvesWindow::GetCurveThread::Entry(){

	if(curvePanel == NULL){ return (wxThread::ExitCode) 0; }
	curvePanel->GetColorCurveMap(numPoints, curve, (float) numPoints);	
	return (wxThread::ExitCode) 0;
}
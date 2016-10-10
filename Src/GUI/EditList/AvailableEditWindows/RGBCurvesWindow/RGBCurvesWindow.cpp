#include "RGBCurvesWindow.h"

RGBCurvesWindow::RGBCurvesWindow(wxWindow * parent, wxString editName, Processor * processor) : EditWindow(parent, editName, processor) {

	parWindow = parent;
	this->SetBackgroundColour(parent->GetBackgroundColour());

	curveTabs = new wxNotebook(this, -1);
	
	brightCurve = new CurvePanel(curveTabs, CURVE_CHANNEL_BRIGHT);
	redCurve = new CurvePanel(curveTabs, CURVE_CHANNEL_RED);
	greenCurve = new CurvePanel(curveTabs, CURVE_CHANNEL_GREEN);
	blueCurve = new CurvePanel(curveTabs, CURVE_CHANNEL_BLUE);

	processButton = new wxButton(this, EditWindow::ID_PROCESS_EDITS, "Process Edits", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	processButton->SetForegroundColour(Colors::TextLightGrey);
	processButton->SetBackgroundColour(Colors::BackGrey);
	processButton->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	curveTabs->SetBackgroundColour(wxColour(54, 54, 54));
	curveTabs->AddPage(brightCurve, "All Channels", true);
	curveTabs->AddPage(redCurve, "Red Channel", false);
	curveTabs->AddPage(greenCurve, "Green Channel", false);
	curveTabs->AddPage(blueCurve, "Blue Channel", false);

	container = new wxBoxSizer(wxVERTICAL);
	buttonContainer = new wxBoxSizer(wxHORIZONTAL);

	buttonContainer->Add(processButton);

	container->Add(curveTabs, 1, wxEXPAND);
	container->Add(buttonContainer);

	this->SetSizerAndFit(container);
	container->Layout();
	
	proc = processor;

	this->Bind(wxEVT_BUTTON, (wxObjectEventFunction)&RGBCurvesWindow::Process, this, EditWindow::ID_PROCESS_EDITS);

	this->SetSize(350, 350);
	
}

void RGBCurvesWindow::SetParamsAndFlags(ProcessorEdit * edit){

	std::vector<Point> brightControlPoints;
	std::vector<Point> redControlPoints;
	std::vector<Point> greenControlPoints;
	std::vector<Point> blueControlPoints;

	// Must have 4 double arrays (bright, red, green, and blue curves)
	if(edit->GetNumDoubleArrays() != 4){ return; }

	// Verify that the array size of each array is at least 4 (2 points) and even (each point has x and y)
	int doubleArrSize = 0;
	for(int i = 0; i < 4; i++){
		doubleArrSize = edit->GetDoubleArraySize(i);
		if(doubleArrSize < 4 || doubleArrSize %2 != 0){
			return;
		}
	}

	// Verify all point values are between 0 and 1
	doubleArrSize = 0;
	for(int i = 0; i < 4; i++){

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

	// Set brightness control points
	double * brightArray = edit->GetDoubleArray(0);
	int curID = 0;
	for(int i = 0; i < edit->GetDoubleArraySize(0); i+=2){

		Point brightPoint;
		brightPoint.x = brightArray[i];
		brightPoint.y = brightArray[i + 1];
		brightPoint.id = curID;
		brightControlPoints.push_back(brightPoint);
		curID += 1;
	}

	// Set red control points
	double * redArray = edit->GetDoubleArray(1);
	curID = 0;
	for(int i = 0; i < edit->GetDoubleArraySize(1); i+=2){

		Point redPoint;
		redPoint.x = redArray[i];
		redPoint.y = redArray[i + 1];
		redPoint.id = curID;
		redControlPoints.push_back(redPoint);

		curID += 1;
	}

	// Set green control points
	double * greenArray = edit->GetDoubleArray(2);
	curID = 0;
	for(int i = 0; i < edit->GetDoubleArraySize(2); i+=2){

		Point greenPoint;
		greenPoint.x = greenArray[i];
		greenPoint.y = greenArray[i + 1];
		greenPoint.id = curID;
		greenControlPoints.push_back(greenPoint);

		curID += 1;
	}

	// Set blue control points
	double * blueArray = edit->GetDoubleArray(3);
	curID = 0;
	for(int i = 0; i < edit->GetDoubleArraySize(3); i+=2){

		Point bluePoint;
		bluePoint.x = blueArray[i];
		bluePoint.y = blueArray[i + 1];
		bluePoint.id = curID;
		blueControlPoints.push_back(bluePoint);

		curID += 1;
	}

	// Set curves
	brightCurve->SetControlPoints(brightControlPoints);
	redCurve->SetControlPoints(redControlPoints);
	greenCurve->SetControlPoints(greenControlPoints);
	blueCurve->SetControlPoints(blueControlPoints);
}

ProcessorEdit * RGBCurvesWindow::GetParamsAndFlags(){

	std::vector<Point> brightControlPoints = brightCurve->GetControlPoints();
	std::vector<Point> redControlPoints = redCurve->GetControlPoints();
	std::vector<Point> greenControlPoints = greenCurve->GetControlPoints();
	std::vector<Point> blueControlPoints = blueCurve->GetControlPoints();

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
		std::vector<int> brightVector8 = brightCurve->GetColorCurveMap(numSteps8, (float)numSteps8);
		std::vector<int> redVector8 = redCurve->GetColorCurveMap(numSteps8, (float)numSteps8);
		std::vector<int> greenVector8 = greenCurve->GetColorCurveMap(numSteps8, (float)numSteps8);
		std::vector<int> blueVector8 = blueCurve->GetColorCurveMap(numSteps8, (float)numSteps8);

		// Create 8 bit int arrays
		int * brightCurve8 = new int[numSteps8];
		int * redCurve8 = new int[numSteps8];
		int * greenCurve8 = new int[numSteps8];
		int * blueCurve8 = new int[numSteps8];

		// Fill 8 bit int array from map
		for (int i = 0; i < numSteps8; i++) {
			brightCurve8[i] = brightVector8[i];
			redCurve8[i] = redVector8[i];
			greenCurve8[i] = greenVector8[i];
			blueCurve8[i] = blueVector8[i];
		}

		// Get 16 bit color map
		int numSteps16 = 65536;
		std::vector<int> brightVector16 = brightCurve->GetColorCurveMap(numSteps16, (float)numSteps16);
		std::vector<int> redVector16 = redCurve->GetColorCurveMap(numSteps16, (float)numSteps16);
		std::vector<int> greenVector16 = greenCurve->GetColorCurveMap(numSteps16, (float)numSteps16);
		std::vector<int> blueVector16 = blueCurve->GetColorCurveMap(numSteps16, (float)numSteps16);

		// Create 16 bit int arrays
		int * brightCurve16 = new int[numSteps16];
		int * redCurve16 = new int[numSteps16];
		int * greenCurve16 = new int[numSteps16];
		int * blueCurve16 = new int[numSteps16];

		// Fill 16 bit int array from map
		for (int i = 0; i < numSteps16; i++) {
			brightCurve16[i] = brightVector16[i];
			redCurve16[i] = redVector16[i];
			greenCurve16[i] = greenVector16[i];
			blueCurve16[i] = blueVector16[i];
		}
		
		// Add 8 bit map
		rgbCurveEdit->AddIntArray(brightCurve8, numSteps8);
		rgbCurveEdit->AddIntArray(redCurve8, numSteps8);
		rgbCurveEdit->AddIntArray(greenCurve8, numSteps8);
		rgbCurveEdit->AddIntArray(blueCurve8, numSteps8);

		// Add 16 bit map
		rgbCurveEdit->AddIntArray(brightCurve16, numSteps16);
		rgbCurveEdit->AddIntArray(redCurve16, numSteps16);
		rgbCurveEdit->AddIntArray(greenCurve16, numSteps16);
		rgbCurveEdit->AddIntArray(blueCurve16, numSteps16);
	}

	// Add control points double arrays to edit
	rgbCurveEdit->AddDoubleArray(brightPoints, brightControlPoints.size() * 2);
	rgbCurveEdit->AddDoubleArray(redPoints, redControlPoints.size() * 2);
	rgbCurveEdit->AddDoubleArray(greenPoints, greenControlPoints.size() * 2);
	rgbCurveEdit->AddDoubleArray(bluePoints, blueControlPoints.size() * 2);

	// Set enabled / disabled
	rgbCurveEdit->SetDisabled(isDisabled);

	return rgbCurveEdit;
}

bool RGBCurvesWindow::CheckCopiedParamsAndFlags(){

	ProcessorEdit * edit = proc->GetEditForCopyPaste();
	if(edit == NULL){ return false; }

	if(edit->GetEditType() != ProcessorEdit::EditType::RGB_CURVES){ return false; }

	// Need 4 double arrars (W, R, G and B)
	if(edit->GetNumDoubleArrays() != 4){
		return false;
	}

	// Verify that the array size of each array is at least 4 (2 points) and even (each point has x and y)
	int doubleArrSize = 0;
	for(int i = 0; i < 4; i++){
		doubleArrSize = edit->GetDoubleArraySize(i);
		if(doubleArrSize < 4 || doubleArrSize %2 != 0){
			return false;
		}
	}

	return true;
}
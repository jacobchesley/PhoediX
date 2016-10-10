#include "LABCurvesWindow.h"

LABCurvesWindow::LABCurvesWindow(wxWindow * parent, wxString editName, Processor * processor) : EditWindow(parent, editName, processor) {

	parWindow = parent;
	this->SetBackgroundColour(parent->GetBackgroundColour());

	colorSpaceSizer = new wxBoxSizer(wxHORIZONTAL);
	colorSpaceLabel = new wxStaticText(this, -1, "Color Space");
	colorSpaceLabel->SetForegroundColour(Colors::TextLightGrey);

	colorSpaceSelection = new wxComboBox(this, -1);
	colorSpaceSelection->AppendString("Adobe RGB");
	colorSpaceSelection->AppendString("Pro Photo RGB");
	colorSpaceSelection->AppendString("sRGB");
	colorSpaceSelection->AppendString("Wide Gamut RGB");
	colorSpaceSelection->SetSelection(2);

	colorSpaceSizer->Add(colorSpaceLabel);
	colorSpaceSizer->AddSpacer(10);
	colorSpaceSizer->Add(colorSpaceSelection);

	curveTabs = new wxNotebook(this, -1);

	lCurve = new CurvePanel(curveTabs, CURVE_CHANNEL_BRIGHT);
	aCurve = new CurvePanel(curveTabs, CURVE_CHANNEL_RED);
	bCurve = new CurvePanel(curveTabs, CURVE_CHANNEL_BLUE);

	processButton = new wxButton(this, EditWindow::ID_PROCESS_EDITS, "Process Edits", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	processButton->SetForegroundColour(Colors::TextLightGrey);
	processButton->SetBackgroundColour(Colors::BackGrey);
	processButton->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	curveTabs->SetBackgroundColour(wxColour(54, 54, 54));
	curveTabs->AddPage(lCurve, "L Channel", true);
	curveTabs->AddPage(aCurve, "A Channel", false);
	curveTabs->AddPage(bCurve, "B Channel", false);

	container = new wxBoxSizer(wxVERTICAL);
	buttonContainer = new wxBoxSizer(wxHORIZONTAL);

	buttonContainer->Add(processButton);

	container->Add(colorSpaceSizer);
	container->Add(curveTabs, 1, wxEXPAND);
	container->Add(buttonContainer);

	this->SetSizerAndFit(container);
	container->Layout();

	proc = processor;

	this->Bind(wxEVT_BUTTON, (wxObjectEventFunction)&LABCurvesWindow::Process, this, EditWindow::ID_PROCESS_EDITS);
}

void LABCurvesWindow::SetParamsAndFlags(ProcessorEdit * edit){

	if(edit == NULL){ return; }

	std::vector<Point> lControlPoints;
	std::vector<Point> aControlPoints;
	std::vector<Point> bControlPoints;

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
	for(int i = 0; i < edit->GetDoubleArraySize(0); i+=2){

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
	for(int i = 0; i < edit->GetDoubleArraySize(1); i+=2){

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
	for(int i = 0; i < edit->GetDoubleArraySize(2); i+=2){

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
}

ProcessorEdit * LABCurvesWindow::GetParamsAndFlags(){

	std::vector<Point> lControlPoints = lCurve->GetControlPoints();
	std::vector<Point> aControlPoints = aCurve->GetControlPoints();
	std::vector<Point> bControlPoints = bCurve->GetControlPoints();

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
		std::vector<int> lVector16 = lCurve->GetColorCurveMap(numSteps16, (float)numSteps16);
		std::vector<int> aVector16 = aCurve->GetColorCurveMap(numSteps16, (float)numSteps16);
		std::vector<int> bVector16 = bCurve->GetColorCurveMap(numSteps16, (float)numSteps16);

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

	int colorSpace = 0;
	if (colorSpaceSelection->GetSelection() == 0) { colorSpace = ColorSpaceENUM::ADOBE_RGB; }
	if (colorSpaceSelection->GetSelection() == 1) { colorSpace = ColorSpaceENUM::PROPHOTO_RGB; }
	if (colorSpaceSelection->GetSelection() == 2) { colorSpace = ColorSpaceENUM::sRGB; }
	if (colorSpaceSelection->GetSelection() == 3) { colorSpace = ColorSpaceENUM::WIDE_GAMUT_RGB; }

	labCurveEdit->AddParam(colorSpace);

	// Set enabled / disabled
	labCurveEdit->SetDisabled(isDisabled);

	return labCurveEdit;
}

bool LABCurvesWindow::CheckCopiedParamsAndFlags(){

	ProcessorEdit * edit = proc->GetEditForCopyPaste();
	if(edit == NULL){ return false; }

	if(edit->GetEditType() != ProcessorEdit::EditType::LAB_CURVES){ return false; }

	// Need 3 double arrars (L, A and B)
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
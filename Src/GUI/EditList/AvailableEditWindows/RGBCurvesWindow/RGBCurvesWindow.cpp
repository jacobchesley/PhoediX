#include "RGBCurvesWindow.h"

RGBCurvesWindow::RGBCurvesWindow(wxWindow * parent, wxString editName, Processor * processor) : EditWindow(parent, editName) {

	parWindow = parent;
	this->SetBackgroundColour(parent->GetBackgroundColour());

	int w = 500;
	int h = 500;

	curveTabs = new wxNotebook(this, -1);

	brightCurve = new CurvePanel(curveTabs, CURVE_CHANNEL_BRIGHT, 0, 0, w, h);
	redCurve = new CurvePanel(curveTabs, CURVE_CHANNEL_RED, 0, 0, w, h);
	greenCurve = new CurvePanel(curveTabs, CURVE_CHANNEL_GREEN, 0, 0, w, h);
	blueCurve = new CurvePanel(curveTabs, CURVE_CHANNEL_BLUE, 0, 0, w, h);

	processButton = new wxButton(this, EditWindow::ID_PROCESS_EDITS, "Process Edits");

	curveTabs->SetBackgroundColour(wxColour(54, 54, 54));
	curveTabs->AddPage(brightCurve, "All Channels", true);
	curveTabs->AddPage(redCurve, "Red Channel", false);
	curveTabs->AddPage(greenCurve, "Green Channel", false);
	curveTabs->AddPage(blueCurve, "Blue Channel", false);

	container = new wxBoxSizer(wxVERTICAL);
	buttonContainer = new wxBoxSizer(wxHORIZONTAL);

	buttonContainer->Add(processButton);

	container->Add(curveTabs);
	container->Add(buttonContainer);

	this->SetSizerAndFit(container);
	container->Layout();
	
	proc = processor;

	this->Bind(wxEVT_BUTTON, (wxObjectEventFunction)&RGBCurvesWindow::Process, this, EditWindow::ID_PROCESS_EDITS);
}

void RGBCurvesWindow::Process(wxCommandEvent& WXUNUSED(event)) {

	wxCommandEvent evt(REPROCESS_IMAGE_EVENT, ID_REPROCESS_IMAGE);
	wxPostEvent(parWindow, evt);
}

void RGBCurvesWindow::AddEditToProcessor() {

	std::vector<Point> brightControlPoints = brightCurve->GetControlPoints();
	std::vector<Point> redControlPoints = redCurve->GetControlPoints();
	std::vector<Point> greenControlPoints = greenCurve->GetControlPoints();
	std::vector<Point> blueControlPoints = blueCurve->GetControlPoints();

	// Create double array that will store control points
	double * brightPoints = new double[brightControlPoints.size() * 2];
	double * redPoints = new double[redControlPoints.size() * 2];
	double * greenPoints = new double[greenControlPoints.size() * 2];
	double * bluePoints = new double[blueControlPoints.size() * 2];

	int idx = 0;

	// Add brightness control points to a double array
	for (int p = 0; p < brightControlPoints.size(); p ++) {
		brightPoints[idx] = brightControlPoints[p].x;
		brightPoints[idx + 1] = brightControlPoints[p].y;
		idx += 2;
	}
	idx = 0;

	// Add red control points to a double array
	for (int p = 0; p < redControlPoints.size(); p++) {
		redPoints[idx] = redControlPoints[p].x;
		redPoints[idx + 1] = redControlPoints[p].y;
		idx += 2;
	}
	idx = 0;

	// Add green control points to a double array
	for (int p = 0; p < greenControlPoints.size(); p++) {
		greenPoints[idx] = greenControlPoints[p].x;
		greenPoints[idx + 1] = greenControlPoints[p].y;
		idx += 2;
	}
	idx = 0;

	// Add blue control points to a double array
	for (int p = 0; p < blueControlPoints.size(); p++) {
		bluePoints[idx] = blueControlPoints[p].x;
		bluePoints[idx + 1] = blueControlPoints[p].y;
		idx += 2;
	}
	idx = 0;

	// Get 8 bit color map
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

	// Fill int array from map
	for (int i = 0; i < numSteps8; i++) {
		brightCurve8[i] = brightVector8[i];
		redCurve8[i] = redVector8[i];
		greenCurve8[i] =greenVector8[i];
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

	// Fill int array from map
	for (int i = 0; i < numSteps8; i++) {
		brightCurve16[i] = brightVector16[i];
		redCurve16[i] = redVector16[i];
		greenCurve16[i] = greenVector16[i];
		blueCurve16[i] = blueVector16[i];
	}

	// Create RGB Curve edit
	ProcessorEdit * rgbCurveEdit = new ProcessorEdit(ProcessorEdit::EditType::RGB_CURVES);

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

	// Add control points double arrays to edit
	rgbCurveEdit->AddDoubleArray(brightPoints, brightControlPoints.size() * 2);
	rgbCurveEdit->AddDoubleArray(redPoints, redControlPoints.size() * 2);
	rgbCurveEdit->AddDoubleArray(greenPoints, greenControlPoints.size() * 2);
	rgbCurveEdit->AddDoubleArray(bluePoints, blueControlPoints.size() * 2);

	// Set enabled / disabled
	rgbCurveEdit->SetDisabled(isDisabled);

	proc->AddEdit(rgbCurveEdit);
}
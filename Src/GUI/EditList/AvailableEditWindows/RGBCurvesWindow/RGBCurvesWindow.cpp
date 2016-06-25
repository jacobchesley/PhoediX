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

	int numSteps8 = 256;
	std::vector<int> brightVector8 = brightCurve->GetColorCurveMap(numSteps8, (float)numSteps8);
	std::vector<int> redVector8 = redCurve->GetColorCurveMap(numSteps8, (float)numSteps8);
	std::vector<int> greenVector8 = greenCurve->GetColorCurveMap(numSteps8, (float)numSteps8);
	std::vector<int> blueVector8 = blueCurve->GetColorCurveMap(numSteps8, (float)numSteps8);

	int * brightCurve8 = new int[numSteps8];
	int * redCurve8 = new int[numSteps8];
	int * greenCurve8 = new int[numSteps8];
	int * blueCurve8 = new int[numSteps8];

	for (size_t i = 0; i < numSteps8; i++) {
		brightCurve8[i] = brightVector8[i];
		redCurve8[i] = redVector8[i];
		greenCurve8[i] =greenVector8[i];
		blueCurve8[i] = blueVector8[i];
	}

	int numSteps16 = 65536;
	std::vector<int> brightVector16 = brightCurve->GetColorCurveMap(numSteps16, (float)numSteps16);
	std::vector<int> redVector16 = redCurve->GetColorCurveMap(numSteps16, (float)numSteps16);
	std::vector<int> greenVector16 = greenCurve->GetColorCurveMap(numSteps16, (float)numSteps16);
	std::vector<int> blueVector16 = blueCurve->GetColorCurveMap(numSteps16, (float)numSteps16);

	int * brightCurve16 = new int[numSteps16];
	int * redCurve16 = new int[numSteps16];
	int * greenCurve16 = new int[numSteps16];
	int * blueCurve16 = new int[numSteps16];

	for (size_t i = 0; i < numSteps8; i++) {
		brightCurve16[i] = brightVector16[i];
		redCurve16[i] = redVector16[i];
		greenCurve16[i] = greenVector16[i];
		blueCurve16[i] = blueVector16[i];
	}

	ProcessorEdit * rgbCurveEdit = new ProcessorEdit(ProcessorEdit::EditType::RGB_CURVES);
	rgbCurveEdit->AddIntArray(brightCurve8, numSteps8);
	rgbCurveEdit->AddIntArray(redCurve8, numSteps8);
	rgbCurveEdit->AddIntArray(greenCurve8, numSteps8);
	rgbCurveEdit->AddIntArray(blueCurve8, numSteps8);

	rgbCurveEdit->AddIntArray(brightCurve16, numSteps16);
	rgbCurveEdit->AddIntArray(redCurve16, numSteps16);
	rgbCurveEdit->AddIntArray(greenCurve16, numSteps16);
	rgbCurveEdit->AddIntArray(blueCurve16, numSteps16);

	proc->AddEdit(rgbCurveEdit);

	/*
	std::vector<float> brightVectorF;
	std::vector<float> redVectorF;
	std::vector<float> greenVectorF;
	std::vector<float> blueVectorF;

	
	brightVectorF.resize(brightVector.size());
	redVectorF.resize(redVector.size());
	greenVectorF.resize(greenVector.size());
	blueVectorF.resize(blueVector.size());

	for (std::vector<float>::size_type i = 0; i < brightVector.size(); i++) {

		brightVectorF[i] = (float)brightVector[i];
		redVectorF[i] = (float)redVector[i];
		greenVectorF[i] = (float)greenVector[i];
		blueVectorF[i] = (float)blueVector[i];
	}
	*/
}
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

	int numSteps = 65536;
	

	std::vector<int> brightVector = brightCurve->GetColorCurveMap(numSteps, (float)numSteps);
	std::vector<int> redVector = redCurve->GetColorCurveMap(numSteps, (float)numSteps);
	std::vector<int> greenVector = greenCurve->GetColorCurveMap(numSteps, (float)numSteps);
	std::vector<int> blueVector = blueCurve->GetColorCurveMap(numSteps, (float)numSteps);

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
}
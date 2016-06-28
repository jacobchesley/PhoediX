#include "LABCurvesWindow.h"

LABCurvesWindow::LABCurvesWindow(wxWindow * parent, wxString editName, Processor * processor) : EditWindow(parent, editName) {

	parWindow = parent;
	this->SetBackgroundColour(parent->GetBackgroundColour());

	int w = 500;
	int h = 500;

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

	lCurve = new CurvePanel(curveTabs, CURVE_CHANNEL_BRIGHT, 0, 0, w, h);
	aCurve = new CurvePanel(curveTabs, CURVE_CHANNEL_RED, 0, 0, w, h);
	bCurve = new CurvePanel(curveTabs, CURVE_CHANNEL_BLUE, 0, 0, w, h);

	processButton = new wxButton(this, EditWindow::ID_PROCESS_EDITS, "Process Edits");

	curveTabs->SetBackgroundColour(wxColour(54, 54, 54));
	curveTabs->AddPage(lCurve, "L Channel", true);
	curveTabs->AddPage(aCurve, "A Channel", false);
	curveTabs->AddPage(bCurve, "B Channel", false);

	container = new wxBoxSizer(wxVERTICAL);
	buttonContainer = new wxBoxSizer(wxHORIZONTAL);

	buttonContainer->Add(processButton);

	container->Add(colorSpaceSizer);
	container->Add(curveTabs);
	container->Add(buttonContainer);

	this->SetSizerAndFit(container);
	container->Layout();

	proc = processor;

	this->Bind(wxEVT_BUTTON, (wxObjectEventFunction)&LABCurvesWindow::Process, this, EditWindow::ID_PROCESS_EDITS);
}

void LABCurvesWindow::Process(wxCommandEvent& WXUNUSED(event)) {

	wxCommandEvent evt(REPROCESS_IMAGE_EVENT, ID_REPROCESS_IMAGE);
	wxPostEvent(parWindow, evt);
}

void LABCurvesWindow::AddEditToProcessor() {

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

	ProcessorEdit * labCurveEdit = new ProcessorEdit(ProcessorEdit::EditType::LAB_CURVES);

	labCurveEdit->AddIntArray(lCurve16, numSteps16);
	labCurveEdit->AddIntArray(aCurve16, numSteps16);
	labCurveEdit->AddIntArray(bCurve16, numSteps16);

	int colorSpace = 0;
	if (colorSpaceSelection->GetSelection() == 0) { colorSpace = ColorSpaceENUM::ADOBE_RGB; }
	if (colorSpaceSelection->GetSelection() == 1) { colorSpace = ColorSpaceENUM::PROPHOTO_RGB; }
	if (colorSpaceSelection->GetSelection() == 2) { colorSpace = ColorSpaceENUM::sRGB; }
	if (colorSpaceSelection->GetSelection() == 3) { colorSpace = ColorSpaceENUM::WIDE_GAMUT_RGB; }

	labCurveEdit->AddParam(colorSpace);
	proc->AddEdit(labCurveEdit);
}
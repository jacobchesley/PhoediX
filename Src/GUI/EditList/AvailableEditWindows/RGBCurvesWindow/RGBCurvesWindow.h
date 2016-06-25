#ifndef RGB_CURVE_H
#define RGB_CURVE_H

#include "wx\notebook.h"

#include "GUI\EditList\EditWindow\EditWindow.h"
#include "GUI\Controls\DoubleSlider\DoubleSlider.h"

#include "GUI\Colors\Colors.h"
#include "Processing\Processor\Processor.h"
#include "gui\controls\curvespanel\curvespanel.h"
#include "Debugging\MemoryLeakCheck.h"

class RGBCurvesWindow :public EditWindow {
public:

	wxWindow * parWindow;
	wxButton * processButton;
	wxButton * resetCurveButton;

	wxBoxSizer * buttonContainer;

	CurvePanel * brightCurve;
	CurvePanel * redCurve;
	CurvePanel* greenCurve;
	CurvePanel * blueCurve;

	wxNotebook * curveTabs;
	wxBoxSizer * container;

	wxTextCtrl * text;

	Processor * proc;

	RGBCurvesWindow(wxWindow * parent, wxString editName, Processor * processor);
	void Process(wxCommandEvent& WXUNUSED(event));

	void AddEditToProcessor();
};

#endif
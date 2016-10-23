#ifndef HSL_CURVE_H
#define HSL_CURVE_H

#include "wx\notebook.h"

#include "GUI\EditList\EditWindow\EditWindow.h"
#include "GUI\Controls\DoubleSlider\DoubleSlider.h"

#include "GUI\Colors\Colors.h"
#include "Processing\Processor\Processor.h"
#include "gui\controls\curvespanel\curvespanel.h"
#include "Debugging\MemoryLeakCheck.h"

class HSLCurvesWindow :public EditWindow {
public:
	HSLCurvesWindow(wxWindow * parent, wxString editName, Processor * processor);

	void AddEditToProcessor();
	void SetParamsAndFlags(ProcessorEdit * edit);
	ProcessorEdit * GetParamsAndFlags();
	bool CheckCopiedParamsAndFlags();

private:

	wxWindow * parWindow;
	wxButton * resetCurveButton;
	
	CurvePanel * hCurve;
	CurvePanel * sCurve;
	CurvePanel * lCurve;

	wxNotebook * curveTabs;
	wxBoxSizer * container;

	wxTextCtrl * text;

	Processor * proc;
};

#endif
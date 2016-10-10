#ifndef LAB_CURVE_H
#define LAB_CURVE_H

#include "wx\notebook.h"

#include "GUI\EditList\EditWindow\EditWindow.h"
#include "GUI\Controls\DoubleSlider\DoubleSlider.h"

#include "GUI\Colors\Colors.h"
#include "Processing\Processor\Processor.h"
#include "gui\controls\curvespanel\curvespanel.h"
#include "Debugging\MemoryLeakCheck.h"

class LABCurvesWindow :public EditWindow {
public:

	wxWindow * parWindow;
	wxButton * processButton;
	wxButton * resetCurveButton;

	wxBoxSizer * buttonContainer;
	
	wxBoxSizer * colorSpaceSizer;
	wxStaticText * colorSpaceLabel;
	wxComboBox * colorSpaceSelection;
	CurvePanel * lCurve;
	CurvePanel * aCurve;
	CurvePanel * bCurve;

	wxNotebook * curveTabs;
	wxBoxSizer * container;

	wxTextCtrl * text;

	Processor * proc;

	LABCurvesWindow(wxWindow * parent, wxString editName, Processor * processor);

	void AddEditToProcessor();
	void SetParamsAndFlags(ProcessorEdit * edit);
	ProcessorEdit * GetParamsAndFlags();
	bool CheckCopiedParamsAndFlags();
};

#endif
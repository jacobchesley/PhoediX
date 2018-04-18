// Copyright 2016 Jacob Chesley

#ifndef RGB_CURVE_H
#define RGB_CURVE_H

#include "wx/notebook.h"

#include "GUI/EditList/EditWindow/EditWindow.h"
#include "GUI/Controls/DoubleSlider/DoubleSlider.h"
#include "GUI/Controls/CurvesPanel/CurvesPanel.h"

#include "GUI/Colors/Colors.h"
#include "Processing/Processor/Processor.h"
#include "Debugging/MemoryLeakCheck.h"

class RGBCurvesWindow :public EditWindow {
public:
	RGBCurvesWindow(wxWindow * parent, wxString editName, Processor * processor);

	void AddEditToProcessor();
	void SetParamsAndFlags(ProcessorEdit * edit);
	ProcessorEdit * GetParamsAndFlags();
	bool CheckCopiedParamsAndFlags();

private:

	wxWindow * parWindow;
	wxButton * resetCurveButton;

	CurvePanel * brightCurve;
	CurvePanel * redCurve;
	CurvePanel * greenCurve;
	CurvePanel * blueCurve;

	wxNotebook * curveTabs;
	wxBoxSizer * container;

	wxTextCtrl * text;

	Processor * proc;

	class GetCurveThread : public wxThread{
	public:
		GetCurveThread(CurvePanel * curvePanelIn, int curveSize, int * curveOut, int numCurves, int * numCurveComplete, wxMutex * mutLockIn, wxCondition * condition);

	protected:
		virtual ExitCode Entry();
	private:
		
		CurvePanel * curvePanel;
		int numPoints;
		int * curve;

		wxMutex * mutLock;
		wxCondition * cond;
		int threads;
		int * complete;
	};
};

#endif
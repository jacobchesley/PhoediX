// Copyright 2018 Jacob Chesley.
// See LICENSE.TXT in root of project for license information.

#ifndef LAB_CURVE_H
#define LAB_CURVE_H

#include "wx/notebook.h"

#include "GUI/EditList/EditWindow/EditWindow.h"
#include "GUI/Controls/DoubleSlider/DoubleSlider.h"
#include "GUI/Controls/CurvesPanel/CurvesPanel.h"

#include "GUI/Colors/Colors.h"
#include "Processing/Processor/Processor.h"
#include "Debugging/MemoryLeakCheck.h"

class LABCurvesWindow :public EditWindow {
public:
	LABCurvesWindow(wxWindow * parent, wxString editName, Processor * processor);

	void SetParamsAndFlags(ProcessorEdit * edit);
	ProcessorEdit * GetParamsAndFlags();
	bool CheckCopiedParamsAndFlags();

private:

	wxWindow * parWindow;
	
	CurvePanel * lCurve;
	CurvePanel * aCurve;
	CurvePanel * bCurve;

	wxNotebook * curveTabs;
	wxBoxSizer * container;

	wxGridSizer * gridSizer;

	wxStaticText * rLabel;
	wxStaticText * gLabel;
	wxStaticText * bLabel;

	DoubleSlider * rScaleSlider;
	DoubleSlider * gScaleSlider;
	DoubleSlider * bScaleSlider;

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
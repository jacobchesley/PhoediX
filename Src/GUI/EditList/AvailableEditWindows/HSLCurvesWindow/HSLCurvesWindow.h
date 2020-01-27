// Copyright 2018 Jacob Chesley.
// See LICENSE.TXT in root of project for license information.

#ifndef HSL_CURVE_H
#define HSL_CURVE_H

#include "wx/notebook.h"

#include "GUI/Controls/PhoediXButton/PhoediXButton.h"
#include "GUI/EditList/EditWindow/EditWindow.h"
#include "GUI/Controls/DoubleSlider/DoubleSlider.h"
#include "GUI/Controls/CurvesPanel/CurvesPanel.h"

#include "GUI/Colors/Colors.h"
#include "Processing/Processor/Processor.h"
#include "Debugging/MemoryLeakCheck.h"

class HSLCurvesWindow :public EditWindow {
public:
	HSLCurvesWindow(wxWindow * parent, wxString editName, Processor * processor);

	void SetParamsAndFlags(ProcessorEdit * edit);
	ProcessorEdit * GetParamsAndFlags();
	bool CheckCopiedParamsAndFlags();

private:

	wxWindow * parWindow;
	PhoediXButton * resetCurveButton;
	
	CurvePanel * hCurve;
	CurvePanel * sCurve;
	CurvePanel * lCurve;

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

	enum CurveChannels{
		HUE,
		SATURATION,
		LUMINANCE
	};

	class GetCurveThread : public wxThread{
	public:
		GetCurveThread(CurvePanel * curvePanelIn, int curveSize, int * curveOut);

	protected:
		virtual ExitCode Entry();
	private:
		
		CurvePanel * curvePanel;
		int numPoints;
		int * curve;
	};
};

#endif
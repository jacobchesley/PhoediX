// Copyright 2016 Jacob Chesley

#ifndef ADJUST_LAB_WINDOW_H
#define ADJUST_LAB_WINDOW_H

#include "GUI/EditList/EditWindow/EditWindow.h"
#include "GUI/Controls/DoubleSlider/DoubleSlider.h"
#include "GUI/Colors/Colors.h"
#include "Processing/Processor/Processor.h"
#include "Debugging/MemoryLeakCheck.h"

class AdjustLABWindow : public EditWindow {
public:
	AdjustLABWindow(wxWindow * parent, wxString editName, Processor * processor);
	void SetParamsAndFlags(ProcessorEdit * edit);
	bool CheckCopiedParamsAndFlags();
	ProcessorEdit * GetParamsAndFlags();

private:

	wxWindow * parWindow;

	wxBoxSizer * mainSizer;
	wxFlexGridSizer * gridSizer;

	wxStaticText * editLabel;

	wxStaticText * luminaceScaleLabel;
	wxStaticText * aLabel;
	wxStaticText * bLabel;
	wxStaticText * redLabel;
	wxStaticText * greenLabel;
	wxStaticText * blueLabel;


	DoubleSlider * lSlider;
	DoubleSlider * aSlider;
	DoubleSlider * bSlider;

	DoubleSlider * rScaleSlider;
	DoubleSlider * gScaleSlider;
	DoubleSlider * bScaleSlider;

	Processor * proc;
};
#endif
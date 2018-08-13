// Copyright 2018 Jacob Chesley.
// See LICENSE.TXT in root of project for license information.

#ifndef Adjust_RGB_WINDOW_H
#define Adjust_RGB_WINDOW_H

#include "GUI/EditList/EditWindow/EditWindow.h"
#include "GUI/Controls/DoubleSlider/DoubleSlider.h"
#include "GUI/Colors/Colors.h"
#include "Processing/Processor/Processor.h"
#include "Debugging/MemoryLeakCheck.h"

class AdjustRGBWindow : public EditWindow {
public:
	AdjustRGBWindow(wxWindow * parent, wxString editName, Processor * processor);
	void SetParamsAndFlags(ProcessorEdit * edit);
	ProcessorEdit * GetParamsAndFlags();
	bool CheckCopiedParamsAndFlags();

private:

	wxWindow * parWindow;

	wxBoxSizer * mainSizer;
	wxFlexGridSizer * gridSizer;

	wxStaticText * editLabel;

	wxStaticText * allBrightLabel;
	wxStaticText * redBrightLabel;
	wxStaticText * greenBrightLabel;
	wxStaticText * blueBrightLabel;

	DoubleSlider * allBrightSlider;
	DoubleSlider * redBrightSlider;
	DoubleSlider * greenBrightSlider;
	DoubleSlider * blueBrightSlider;

	Processor * proc;

};
#endif

#ifndef SHIFT_BRIGHTNESS_WINDOW_H
#define SHIFT_BRIGHTNESS_WINDOW_H

#include "GUI\EditList\EditWindow\EditWindow.h"
#include "GUI\Controls\DoubleSlider\DoubleSlider.h"
#include "GUI\Colors\Colors.h"
#include "Processing\Processor\Processor.h"
#include "Debugging\MemoryLeakCheck.h"

class ShiftBrightnessWindow : public EditWindow {
public:
	ShiftBrightnessWindow(wxWindow * parent, wxString editName, Processor * processor);
	void AddEditToProcessor();
	void SetParamsAndFlags(ProcessorEdit * edit);
	void Process(wxCommandEvent& WXUNUSED(event));

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

	wxButton * processButton;

	Processor * proc;

};
#endif#pragma once

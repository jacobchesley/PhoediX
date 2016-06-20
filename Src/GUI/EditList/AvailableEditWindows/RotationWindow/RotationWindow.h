#ifndef ROTATION_WINDOW_H
#define ROTATION_WINDOW_H

#include "GUI\EditList\EditWindow\EditWindow.h"
#include "GUI\Controls\DoubleSlider\DoubleSlider.h"

#include "GUI\Colors\Colors.h"
#include "Processing\Processor\Processor.h"
#include "Debugging\MemoryLeakCheck.h"

class RotationWindow : public EditWindow {
public:
	RotationWindow(wxWindow * parent, wxString editName, Processor * processor);
	void AddEditToProcessor();
	void Process(wxCommandEvent& WXUNUSED(event));

private:

	wxWindow * parWindow;

	wxBoxSizer * mainSizer;
	wxFlexGridSizer * gridSizer;

	wxStaticText * editLabel;

	wxStaticText * rotationMethodLabel;
	wxStaticText * customRotationLabel;

	wxComboBox * rotationMethod;
	wxRadioBox * customRotationInterpolation;
	DoubleSlider * customRotationSlider;

	wxButton * processButton;

	Processor * proc;

};
#endif#pragma once

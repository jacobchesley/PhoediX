// Copyright 2018 Jacob Chesley.
// See LICENSE.TXT in root of project for license information.

#ifndef CONVERT_GREYSCALE_WINDOW_H
#define CONVERT_GREYSCALE_WINDOW_H

#include "GUI/EditList/EditWindow/EditWindow.h"
#include "GUI/Controls/DoubleSlider/DoubleSlider.h"
#include "GUI/Controls//PhoediXComboBox/PhoediXComboBox.h"

#include "GUI/Colors/Colors.h"
#include "Processing/Processor/Processor.h"
#include "Debugging/MemoryLeakCheck.h"

class ConvertGreyscaleWindow : public EditWindow {
public:
	ConvertGreyscaleWindow(wxWindow * parent, wxString editName, Processor * processor);
	void AddEditToProcessor();
	void OnCombo(wxCommandEvent& WXUNUSED(event));
	void SetParamsAndFlags(ProcessorEdit * edit);
	bool CheckCopiedParamsAndFlags();
	ProcessorEdit * GetParamsAndFlags();

private:

	wxWindow * parWindow;

	wxBoxSizer * mainSizer;
	wxFlexGridSizer * gridSizer;

	wxStaticText * editLabel;

	wxStaticText * greyscaleMethodLabel;
	wxStaticText * redBrightLabel;
	wxStaticText * greenBrightLabel;
	wxStaticText * blueBrightLabel;

	PhoediXComboBox * greyscaleMethod;
	DoubleSlider * redBrightSlider;
	DoubleSlider * greenBrightSlider;
	DoubleSlider * blueBrightSlider;

	Processor * proc;

};
#endif

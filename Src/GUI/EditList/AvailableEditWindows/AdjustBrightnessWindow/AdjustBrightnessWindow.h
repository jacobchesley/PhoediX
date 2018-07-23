// Copyright 2016 Jacob Chesley

#ifndef ADJUST_BRIGHTNESS_WINDOW_H
#define ADJUST_BRIGHTNESS_WINDOW_H

#include "GUI/EditList/EditWindow/EditWindow.h"
#include "GUI/Controls/DoubleSlider/DoubleSlider.h"
#include "GUI/Colors/Colors.h"
#include "Processing/Processor/Processor.h"
#include "Debugging/MemoryLeakCheck.h"

#include "GUI/Controls/PhoediXComboBox/PhoediXComboBox.h"

class AdjustBrightnessWindow : public EditWindow {
public:
	AdjustBrightnessWindow(wxWindow * parent, wxString editName, Processor * processor);
	void AddEditToProcessor();
	void SetParamsAndFlags(ProcessorEdit * edit);
	bool CheckCopiedParamsAndFlags();
	ProcessorEdit * GetParamsAndFlags();

private:

	wxWindow * parWindow;

	wxBoxSizer * mainSizer;
	wxFlexGridSizer * gridSizer;

	wxStaticText * editLabel;

	wxStaticText * brightLabel;
	wxStaticText * detailsLabel;
	wxStaticText * toneLabel;
	wxStaticText * preservationLabel;

	DoubleSlider * brightSlider;
	PhoediXComboBox * detailsSelect;
	DoubleSlider * toneSlider;
	DoubleSlider * preservationSlider;

	Processor * proc;
};
#endif
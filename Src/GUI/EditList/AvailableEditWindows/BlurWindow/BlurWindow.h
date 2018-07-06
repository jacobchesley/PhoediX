// Copyright 2016 Jacob Chesley

#ifndef BLUR_WINDOW_H
#define BLUR_WINDOW_H

#include "GUI/EditList/EditWindow/EditWindow.h"
#include "GUI/Controls/DoubleSlider/DoubleSlider.h"

#include "GUI/Colors/Colors.h"
#include "Processing/Processor/Processor.h"
#include "Debugging/MemoryLeakCheck.h"

class BlurWindow : public EditWindow {
public:
	BlurWindow(wxWindow * parent, wxString editName, Processor * processor);
	void AddEditToProcessor();
	void SetParamsAndFlags(ProcessorEdit * edit);
	ProcessorEdit * GetParamsAndFlags();
	bool CheckCopiedParamsAndFlags();

private:

	wxWindow * parWindow;

	wxBoxSizer * mainSizer;
	wxFlexGridSizer * gridSizer;

	wxStaticText * editLabel;

	wxStaticText * blurDirectionLabel;
	wxStaticText * blurSizeLabel;
	wxStaticText * numPassesLabel;

	wxComboBox * blurDirection;
	DoubleSlider * blurSizeSlider;
	DoubleSlider * numPassesSlider;

	Processor * proc;

	void OnCombo(wxCommandEvent& WXUNUSED(event));
};
#endif

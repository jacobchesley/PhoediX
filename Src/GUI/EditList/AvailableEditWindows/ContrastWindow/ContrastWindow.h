// Copyright 2016 Jacob Chesley

#ifndef CONTRAST_WINDOW_H
#define CONTRAST_WINDOW_H

#include "GUI\EditList\EditWindow\EditWindow.h"
#include "GUI\Controls\DoubleSlider\DoubleSlider.h"
#include "GUI\Colors\Colors.h"
#include "Processing\Processor\Processor.h"
#include "Debugging\MemoryLeakCheck.h"

class ContrastWindow : public EditWindow {
	public:
		ContrastWindow(wxWindow * parent, wxString editName, Processor * processor);
		void SetParamsAndFlags(ProcessorEdit * edit);
		bool CheckCopiedParamsAndFlags();
		ProcessorEdit * GetParamsAndFlags();

	private:

		wxWindow * parWindow;

		wxBoxSizer * mainSizer;
		wxFlexGridSizer * gridSizer;

		wxStaticText * editLabel;

		wxStaticText * allContrastLabel;
		wxStaticText * allContrastCenterLabel;
		wxStaticText * redContrastLabel;
		wxStaticText * redContrastCenterLabel;
		wxStaticText * greenContrastLabel;
		wxStaticText * greenContrastCenterLabel;
		wxStaticText * blueContrastLabel;
		wxStaticText * blueContrastCenterLabel;
		wxStaticText * curveSelectionLabel;

		DoubleSlider * allContrastSlider;
		DoubleSlider * allContrastCenterSlider;
		DoubleSlider * redContrastSlider;
		DoubleSlider * redContrastCenterSlider;
		DoubleSlider * greenContrastSlider;
		DoubleSlider * greenContrastCenterSlider;
		DoubleSlider * blueContrastSlider;
		DoubleSlider * blueContrastCenterSlider;
		wxCheckBox * curveSelection;

		Processor * proc;
};
#endif
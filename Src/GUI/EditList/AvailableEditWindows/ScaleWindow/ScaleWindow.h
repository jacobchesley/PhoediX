// Copyright 2018 Jacob Chesley.
// See LICENSE.TXT in root of project for license information.

#ifndef SCALE_WINDOW_H
#define SCALE_WINDOW_H

#include "GUI/EditList/EditWindow/EditWindow.h"
#include "GUI/Controls/DoubleSlider/DoubleSlider.h"
#include "GUI/Controls//PhoediXComboBox/PhoediXComboBox.h"
#include "GUI/Colors/Colors.h"
#include "Processing/Processor/Processor.h"
#include "Debugging/MemoryLeakCheck.h"

class ScaleWindow : public EditWindow {
public:
	ScaleWindow(wxWindow * parent, wxString editName, Processor * processor);
	void AddEditToProcessor();
	void SetParamsAndFlags(ProcessorEdit * edit);
	ProcessorEdit * GetParamsAndFlags();
	bool CheckCopiedParamsAndFlags();

private:

	wxWindow * parWindow;

	wxBoxSizer * mainSizer;
	wxFlexGridSizer * gridSizer;

	wxStaticText * editLabel;

	wxStaticText * scaleMethodLabel;
	PhoediXComboBox * scaleMethod;

	wxStaticText * scaleInterpolationLabel;
	PhoediXComboBox * scaleInterpolation;

	wxStaticText * scalePercentageLabel;
	DoubleSlider * scalePercentage;

	wxStaticText * keepAspectLabel;
	wxCheckBox * keepAspect;

	wxStaticText * widthLabel;
	wxTextCtrl * widthCtrl;

	wxStaticText * heightLabel;
	wxTextCtrl * heightCtrl;

	Processor * proc;

	void OnCombo(wxCommandEvent& WXUNUSED(event));
	void OnText(wxCommandEvent& textEvt);

	enum CtrlIDs {
		WIDTH_ID,
		HEIGHT_ID
	};
};

#endif
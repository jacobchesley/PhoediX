// Copyright 2018 Jacob Chesley.
// See LICENSE.TXT in root of project for license information.

#ifndef ROTATION_WINDOW_H
#define ROTATION_WINDOW_H

#include "GUI/EditList/EditWindow/EditWindow.h"
#include "GUI/Controls/DoubleSlider/DoubleSlider.h"
#include "GUI/Controls//PhoediXComboBox/PhoediXComboBox.h"
#include "GUI/Colors/Colors.h"
#include "Processing/Processor/Processor.h"
#include "Debugging/MemoryLeakCheck.h"

class RotationWindow : public EditWindow {
public:
	RotationWindow(wxWindow * parent, wxString editName, Processor * processor);
	void AddEditToProcessor();
	void SetParamsAndFlags(ProcessorEdit * edit);
	ProcessorEdit * GetParamsAndFlags();
	bool CheckCopiedParamsAndFlags();

private:

	wxWindow * parWindow;

	wxBoxSizer * mainSizer;
	wxFlexGridSizer * gridSizer;

	wxStaticText * editLabel;

	wxStaticText * rotationMethodLabel;
	wxStaticText * rotationInterpolationLabel;
	wxStaticText * rotationCropLabel;
	wxStaticText * customRotationLabel;

	PhoediXComboBox * rotationMethod;
	PhoediXComboBox * customRotationInterpolation;
	PhoediXComboBox * customRotationCrop;
	DoubleSlider * customRotationSlider;

	Processor * proc;

	void OnCombo(wxCommandEvent& WXUNUSED(event));
};
#endif

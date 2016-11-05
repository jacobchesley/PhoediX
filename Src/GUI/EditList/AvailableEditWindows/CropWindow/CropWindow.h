// Copyright 2016 Jacob Chesley

#ifndef CROP_WINDOW_H
#define CROP_WINDOW_H

#include "GUI\EditList\EditWindow\EditWindow.h"
#include "GUI\Controls\DoubleSlider\DoubleSlider.h"
#include "GUI\Colors\Colors.h"
#include "Processing\Processor\Processor.h"
#include "Debugging\MemoryLeakCheck.h"

class CropWindow : public EditWindow {
public:
	CropWindow(wxWindow * parent, wxString editName, Processor * processor);
	void AddEditToProcessor();
	void SetParamsAndFlags(ProcessorEdit * edit);
	bool CheckCopiedParamsAndFlags();
	ProcessorEdit * GetParamsAndFlags();

private:

	wxWindow * parWindow;

	wxBoxSizer * mainSizer;
	wxFlexGridSizer * gridSizer;

	wxStaticText * editLabel;

	wxStaticText * startXLabel;
	wxStaticText * startYLabel;
	wxStaticText * widthLabel;
	wxStaticText * heightLabel;

	wxTextCtrl * startXCtrl;
	wxTextCtrl * startYCtrl;
	wxTextCtrl * widthCtrl;
	wxTextCtrl * heightCtrl;

	wxButton * enableCrop;

	Processor * proc;
};
#endif
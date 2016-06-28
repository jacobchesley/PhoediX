#ifndef MIRROR_WINDOW_H
#define MIRROR_WINDOW_H

#include "GUI\EditList\EditWindow\EditWindow.h"
#include "GUI\Controls\DoubleSlider\DoubleSlider.h"

#include "GUI\Colors\Colors.h"
#include "Processing\Processor\Processor.h"
#include "Debugging\MemoryLeakCheck.h"

class MirrorWindow : public EditWindow {
public:
	MirrorWindow(wxWindow * parent, wxString editName, Processor * processor);
	void AddEditToProcessor();
	void SetParamsAndFlags(ProcessorEdit * edit);
	void Process(wxCommandEvent& WXUNUSED(event));

private:

	wxWindow * parWindow;

	wxBoxSizer * mainSizer;
	wxFlexGridSizer * gridSizer;

	wxStaticText * editLabel;

	wxStaticText * mirrorMethodLabel;

	wxComboBox * mirrorMethod;
	
	wxButton * processButton;

	Processor * proc;
};
#endif#pragma once

#ifndef ADJUST_HSL_WINDOW_H
#define ADJUST_HSL_WINDOW_H

#include "GUI\EditList\EditWindow\EditWindow.h"
#include "GUI\Controls\DoubleSlider\DoubleSlider.h"
#include "GUI\Colors\Colors.h"
#include "Processing\Processor\Processor.h"
#include "Debugging\MemoryLeakCheck.h"

class AdjustHSLWindow : public EditWindow {
public:
	AdjustHSLWindow(wxWindow * parent, wxString editName, Processor * processor);
	void AddEditToProcessor();
	void SetParamsAndFlags(ProcessorEdit * edit);
	bool CheckCopiedParamsAndFlags();
	ProcessorEdit * GetParamsAndFlags();

private:

	wxWindow * parWindow;

	wxBoxSizer * mainSizer;
	wxFlexGridSizer * gridSizer;

	wxStaticText * editLabel;

	wxStaticText * hueShiftLabel;
	wxStaticText * saturationScaleLabel;
	wxStaticText * luminaceScaleLabel;

	DoubleSlider * hueShiftSlider;
	DoubleSlider * saturationScaleSlider;
	DoubleSlider * greenBrightSlider;
	DoubleSlider * luminaceScaleSlider;

	Processor * proc;
};
#endif
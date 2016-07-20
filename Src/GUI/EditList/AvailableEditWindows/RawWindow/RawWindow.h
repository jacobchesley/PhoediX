#ifndef RAW_WINDOW_H
#define RAW_WINDOW_H

#include "GUI\EditList\EditWindow\EditWindow.h"
#include "GUI\Controls\DoubleSlider\DoubleSlider.h"
#include "GUI\Colors\Colors.h"
#include "Processing\Processor\Processor.h"
#include "Debugging\MemoryLeakCheck.h"

class RawWindow : public EditWindow {
	public:
		RawWindow(wxWindow * parent, wxString editName, Processor * processor);
		void Process(wxCommandEvent& WXUNUSED(event));
		void SetParamsAndFlags(ProcessorEdit * edit);

	private:

		wxWindow * parWindow;

		wxBoxSizer * mainSizer;
		wxFlexGridSizer * gridSizer;

		wxStaticText * editLabel;

		wxStaticText * allContrastLabel;
		wxStaticText * redContrastLabel;
		wxStaticText * greenContrastLabel;
		wxStaticText * blueContrastLabel;

		DoubleSlider * allContrastSlider;
		DoubleSlider * redContrastSlider;
		DoubleSlider * greenContrastSlider;
		DoubleSlider * blueContrastSlider;

		wxButton * processButton;
		Processor * proc;
};
#endif
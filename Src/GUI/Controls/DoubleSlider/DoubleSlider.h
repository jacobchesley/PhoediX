#ifndef DOUBLE_SLIDER_H
#define DOUBLE_SLIDER_H

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "Debugging\MemoryLeakCheck.h"

class DoubleSlider : public wxPanel {
public:

	DoubleSlider(wxWindow * parent, double initVal, double minVal, double maxVal, int interval, int textPrecision = 3);
	void ShowMinMax();
	void HideMinMax();
	void ShowVal();
	void HideVal();
	void SetId(wxWindowID id);
	void Disable();
	void Enable();

	void SetBackgroundColour(wxColor newColor);
	void SetForegroundColour(wxColor newColor);

	double GetValue();
	void SetValue(double newVal);
	void SetValuePosition(int newValuePosition);

	enum ValPosition {
		VALUE_TOP_LEFT,
		VALUE_TOP_CENTER,
		VALUE_TOP_RIGHT,
		VALUE_BOTTOM_LEFT,
		VALUE_BOTTOM_CENTER,
		VALUE_BOTTOM_RIGHT,
		VALUE_INLINE_LEFT,
		VALUE_INLINE_RIGHT
	};

private:

	void OnSlide(wxCommandEvent& slideEvt);
	void OnTextEnter(wxCommandEvent& textEvt);
	void OnText(wxCommandEvent& textEvt);
	void ResizeValueText();

	wxBoxSizer * sizer;
	wxBoxSizer * topSizer;
	wxBoxSizer * bottomSizer;

	wxStaticText * minText;
	wxStaticText * maxText;
	wxTextCtrl * valText;
	wxSlider * slide;

	int interval;
	double min;
	double max;

	int textPrec;
	int valPos;
	bool firedFromSetValue;
	bool isDisabled;
};

#endif
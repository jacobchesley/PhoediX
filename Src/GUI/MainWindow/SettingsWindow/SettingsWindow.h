// Copyright 2016 Jacob Chesley

#ifndef SETTINGS_WINDOW_H
#define SETTINGS_WINDOW_H


// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "GUI\Colors\Colors.h"
#include "Processing\Image\Image.h"
#include "Processing\Processor\Processor.h"

enum {
	ID_RELOAD_IMAGE
};

wxDECLARE_EVENT(RELOAD_IMAGE_EVENT, wxCommandEvent);

class SettingsWindow : public wxScrolledWindow {

public:
	SettingsWindow(wxWindow * parent, Processor * processor);
	void ApplySettings();

private:

	void OnApply(wxCommandEvent& WXUNUSED(evt));

	wxWindow * parWindow;
	wxBoxSizer * mainSizer;
	wxFlexGridSizer * gridSizer;

	wxStaticText * settingsLabel;

	wxStaticText * colorDepthLabel;
	wxComboBox * colorDepth;
	int lastColorDepth;

	wxStaticText * numThreadsLabel;
	wxSlider * numThreads;

	wxBoxSizer * buttonSizer;
	wxButton * applySettingsButton;
	wxButton * cancelButton;

	Processor * proc;
	enum {
		ID_APPLY_SETTINGS,
		ID_CANCEL
	};

};

#endif

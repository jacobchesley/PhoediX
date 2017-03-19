// Copyright 2016 Jacob Chesley

#ifndef SETTINGS_WINDOW_H
#define SETTINGS_WINDOW_H


// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/tokenzr.h"
#include "wx/stdpaths.h"
#include "wx/textfile.h"

#include "GUI/AUIManager/AUIManager.h"
#include "GUI/Colors/Colors.h"
#include "Processing/Image/Image.h"
#include "Processing/Processor/Processor.h"
#include "GUI/EditList/EditListPanel/EditListPanel.h"
#include "Settings/Settings.h"

enum {
	ID_RELOAD_IMAGE
};

struct Settings{
	int bitDepth;
	int colorSpace;
	int numThreads;
};

wxDECLARE_EVENT(RELOAD_IMAGE_EVENT, wxCommandEvent);

class SettingsWindow : public wxScrolledWindow {

public:
	SettingsWindow(wxWindow * parent, Processor * processor, EditListPanel * editLst);
	void ApplySettings(bool ShowMessage = false);
	void ReadSettings();
	void WriteSettings();
	void Cleanup();
	
private:

	void OnApply(wxCommandEvent& WXUNUSED(evt));
	void OnCancel(wxCommandEvent& WXUNUSED(evt));
	void WriteLines(wxTextFile * file);
	void SendBlankMessageTimer(wxTimerEvent& WXUNUSED(event));

	wxWindow * parWindow;
	wxBoxSizer * mainSizer;
	wxFlexGridSizer * gridSizer;

	wxStaticText * settingsLabel;

	wxStaticText * colorDepthLabel;
	wxComboBox * colorDepth;
	int lastColorDepth;

	wxStaticText * colorSpaceLabel;
	wxComboBox * colorSpace;

	wxStaticText * numThreadsLabel;
	wxSlider * numThreads;

	wxBoxSizer * buttonSizer;
	wxButton * applySettingsButton;
	wxButton * cancelButton;

	Processor * proc;
	EditListPanel * editList;
	
	wxTimer * blankMessageTimer;
	Settings lastSettings;
	enum {
		ID_APPLY_SETTINGS,
		ID_CANCEL
	};

};

#endif

// Copyright 2018 Jacob Chesley.
// See LICENSE.TXT in root of project for license information.

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
#include "wx/dir.h"

#include "GUI/Controls/PhoediXButton/PhoediXButton.h"
#include "GUI/Controls/PhoediXComboBox/PhoediXComboBox.h"
#include "GUI/Controls/DoubleSlider/DoubleSlider.h"
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
	int libraryPreview;
};

wxDECLARE_EVENT(RELOAD_IMAGE_EVENT, wxCommandEvent);

class SettingsWindow : public wxFrame {

public:
	SettingsWindow(wxWindow * parent, Processor * processor, EditListPanel * editLst);
	void ApplySettings(bool ShowMessage = false, bool overwriteLast = true);
	void ReadSettings();
	void WriteSettings();
	void Cleanup();
	wxString GetSettingsFile();

private:

	void OnApply(wxCommandEvent& WXUNUSED(evt));
	void OnOkay(wxCommandEvent& WXUNUSED(evt));
	void OnCancel(wxCommandEvent& WXUNUSED(evt));
	void OnClose(wxCloseEvent& WXUNUSED(evt));
	void WriteLines(wxTextFile * file);
	void SendBlankMessageTimer(wxTimerEvent& WXUNUSED(event));
	void SendMessageToParent(wxString message, int displayTime);
	void ReprocessIfNeeded();

	wxWindow * parWindow;
	wxBoxSizer * mainSizer;
	wxFlexGridSizer * gridSizer;

	wxStaticText * settingsLabel;

	wxStaticText * colorDepthLabel;
	PhoediXComboBox * colorDepth;
	int lastColorDepth;

	wxStaticText * colorSpaceLabel;
	PhoediXComboBox * colorSpace;

	wxStaticText * libraryImagePreviewLabel;
	PhoediXComboBox *libraryImagePreview;

	wxStaticText * numThreadsLabel;
	DoubleSlider * numThreads;

	wxBoxSizer * buttonSizer;
	PhoediXButton * okSettingsButton;
	PhoediXButton * applySettingsButton;
	PhoediXButton * cancelButton;

	Processor * proc;
	EditListPanel * editList;
	
	wxTimer * blankMessageTimer;
	Settings lastSettings;
	
	enum {
		ID_OK_SETTINGS,
		ID_APPLY_SETTINGS,
		ID_CANCEL
	};

	class GeneralSettingsPane : wxPanel{
	public:
		GeneralSettingsPane(wxWindow * parent);

	private:
		wxBoxSizer * mainSizer;
		wxFlexGridSizer * gridSizer;

		wxStaticText * generalSettingsLabel;

		wxStaticText * languageLabel;
		PhoediXComboBox * language;		
	};

	class PerformanceSettingsPane : wxPanel{
	public:
		PerformanceSettingsPane(wxWindow * parent);

	private:
		wxBoxSizer * mainSizer;
		wxFlexGridSizer * gridSizer;

		wxStaticText * performanceSettingsLabel;

		wxStaticText * numThreadsLabel;
		DoubleSlider * numThreads;
	};

	class LibrarySettingsPane : wxPanel{
	public:
		LibrarySettingsPane(wxWindow * parent);

	private:
		wxBoxSizer * mainSizer;
		wxFlexGridSizer * gridSizer;

		wxStaticText * librarySettingsLabel;
	};

};

#endif

// Copyright 2016 Jacob Chesley

#ifndef ABOUT_WINDOW_H
#define ABOUT_WINDOW_H


// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "GUI\Icons\Icons.h"
#include "GUI\Colors\Colors.h"
#include "GUI\ImageDisplay\ZoomImagePanel\ZoomImagePanel.h"

class AboutWindow : public wxFrame {

public:
	AboutWindow(wxWindow * parent);
	
private:

	void OnClose(wxCloseEvent& WXUNUSED(evt));
	void OnOkay(wxCommandEvent& WXUNUSED(evt));

	wxBoxSizer * mainSizer;
	wxFlexGridSizer * aboutSizer;

	wxStaticText * aboutPhoediX;

	wxStaticText * versionTitle;
	wxStaticText * versionValue;

	wxStaticText * buildDateTitle;
	wxStaticText * buildDateValue;

	wxStaticText * authorTitle;
	wxStaticText * authorValue;

	wxStaticText * websiteTitle;
	wxStaticText * websiteValue;

	wxFlexGridSizer * libraryAboutSizer;
	wxStaticText * aboutLibraries;

	wxStaticText * wxWidgetsVersionTitle;
	wxStaticText * wxWidgetsVersionValue;

	wxStaticText * librawVersionTitle;
	wxStaticText * librawVersionValue;

	wxStaticText * libtiffVersionTitle;
	wxStaticText * libtiffVersionValue;

	wxButton * okayButton;

	enum{
		ID_OKAY
	};
	
};

#endif
// Copyright 2018 Jacob Chesley.
// See LICENSE.TXT in root of project for license information.

#ifndef PHOEDIX_BUTTON_H
#define PHOEDIX_BUTTON_H

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "Debugging/MemoryLeakCheck.h"

class PhoediXButton : public wxPanel {
public:

	PhoediXButton(wxWindow * parent, wxWindowID id, const wxString& label = wxEmptyString);
	void SetBackgroundColour(wxColor newColor);
	void SetForegroundColour(wxColor newColor);
	bool SetFont(const wxFont &font);
	wxString GetLabel();
	void SetLabel(wxString label);
	void SetBorder(int xBorder, int yBorder);

	void Disable();
	void Enable();
	void SetDisableBackgroundColour(wxColor disableColor);
	void SetDisableForegroundColour(wxColor disableColor);
	bool GetEnabled();

private:

	wxBoxSizer * sizer;
	wxBoxSizer * outerSizer;
	wxStaticText * text;
	void LeftDown(wxCommandEvent& WXUNUSED(clickEvent));
	void RightDown(wxCommandEvent& WXUNUSED(clickEvent));

	wxColor enableBackground;
	wxColor enableForeground;
	wxColor disableBackground;
	wxColor disableForeground;
	bool disabled;
};

#endif
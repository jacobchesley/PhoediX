// Copyright 2016 Jacob Chesley

#ifndef SUPPORTED_CAMERAS_WINDOW_H
#define SUPPORTED_CAMERAS_WINDOW_H

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/tokenzr.h"
#include "wx/treectrl.h"

#include "GUI/Icons/Icons.h"
#include "GUI/Colors/Colors.h"

struct CameraMakeRoot {
	wxString cameraMake;
	wxTreeItemId cameraMakeID;
};

class SupportedCamerasWindow : public wxFrame {

public:
	SupportedCamerasWindow(wxWindow * parent);

private:

	void OnClose(wxCloseEvent& WXUNUSED(evt));
	void OnOkay(wxCommandEvent& WXUNUSED(evt));
	void PopulateCameras();
	bool CheckCameraMakeInList(wxString cameraMake);

	wxVector<CameraMakeRoot> cameraMakeList;
	wxTreeItemId rootId;

	wxBoxSizer * mainSizer;
	wxStaticText * librawVersionTitle;
	wxTreeCtrl * supportedCameras;
	wxButton * okayButton;

	enum {
		ID_OKAY
	};
};

#endif
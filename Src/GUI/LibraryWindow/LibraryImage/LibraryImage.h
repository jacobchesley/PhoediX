// Copyright 2016 Jacob Chesley

#ifndef LIBRARY_IMAGE_H
#define LIBRARY_IMAGE_H

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "GUI/Colors/Colors.h"
#include "GUI/ImageDisplay/wxImagePanel/wxImagePanel.h"
#include "GUI/ImageDisplay/ZoomImageFrame/ZoomImageFrame.h"
#include "Processing/ImageHandler/ImageHandler.h"
#include "GUI/AUIManager/AUIManager.h"
#include "LibRaw.h"

enum {
	ID_ADD_LIB_IMAGE_EVT,
	ID_OPEN_IMAGE_NEW_PROJECT,
	ID_LIB_IMAGE_CHECK_EVT
};

wxDECLARE_EVENT(OPEN_IMAGE_NEW_PROJECT_EVENT, wxCommandEvent);
wxDECLARE_EVENT(LIBRARY_IMAGE_CHECK_EVENT, wxCommandEvent);


class LibraryImage : public wxPanel {

public:
	LibraryImage(wxWindow * parent, wxWindow * libParent, wxImage * img, wxString fileName, wxString filePath);
	void ChangeImage(wxImage * newImage);
	void SetName(wxString name);
	wxString GetName();
	void SetPath(wxString path);
	wxString GetPath();

	bool GetSelected();
	wxImage * GetImage();

private:

	void OnLeftDoubleClick(wxMouseEvent& WXUNUSED(evt));
	void OnRightClick(wxMouseEvent& WXUNUSED(evt));
	void OnPopupMenuClick(wxCommandEvent& inEvt);
	void OnCheck(wxCommandEvent& WXUNUSED(evt));

	wxImage * img;
	wxBoxSizer * wrapLayout;
	wxBoxSizer * mainLayout;
	wxBoxSizer * subLayout;
	WXImagePanel * imageDisplay;
	wxCheckBox * selectBox;
	wxTextCtrl * nameDisplay;
	wxString path;
	wxWindow * libraryParent;

	enum PopupMenuActions {
		OPEN_IN_NEW_PROJECT = 1,
		VIEW_IMAGE_DETAILS
	};
};

class AddLibraryImageEvent : public wxEvent{

public:
	AddLibraryImageEvent(wxEventType eventType, int winid, wxImage * img, wxString fileName, wxString filePath);
	wxImage * GetImage() const;
	wxString GetFileName() const;
	wxString GetFilePath() const;
	virtual wxEvent * Clone() const;

private:
	wxImage * libImage;
	wxString name;
	wxString path;
};

wxDECLARE_EVENT(ADD_LIB_IMAGE_EVENT, AddLibraryImageEvent);

#endif
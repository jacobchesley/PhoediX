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

enum {
	ID_ADD_LIB_IMAGE_EVT,
	ID_OPEN_IMAGE_NEW_PROJECT
};

wxDECLARE_EVENT(OPEN_IMAGE_NEW_PROJECT_EVENT, wxCommandEvent);


class LibraryImage : public wxPanel {

public:
	LibraryImage(wxWindow * parent, wxImage * img, wxString fileName, wxString filePath);
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

	wxImage * img;
	wxBoxSizer * mainLayout;
	wxBoxSizer * subLayout;
	WXImagePanel * imageDisplay;
	wxCheckBox * selectBox;
	wxStaticText * nameDisplay;
	wxString path;

	enum PopupMenuActions {
		OPEN_IN_NEW_PROJECT,
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
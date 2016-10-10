#ifndef AVAILABLE_EDITS_H
#define AVAILABLE_EDITS_H

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx\listctrl.h"
#include "GUI\Colors\Colors.h"
#include "Debugging\MemoryLeakCheck.h"

enum AvailableEditIDS {
	EDIT_ID_SHIFT_RGB,
	EDIT_ID_ADJUST_HSL,
	EDIT_ID_ADJUST_BRIGHTNESS,
	EDIT_ID_CONTRAST,
	EDIT_ID_GREYSCALE,
	EDIT_ID_CHANNEL_MIXER,
	EDIT_ID_ROTATE,
	EDIT_ID_MIRROR,
	EDIT_ID_RGB_CURVES,
	EDIT_ID_LAB_CURVES,
	EDIT_ID_HSL_CURVES,
	EDIT_ID_RAW,
	EDIT_ID_SCALE,
	EDIT_ID_CROP
};

class Edit {
public:
	Edit(int editID, wxString editName, wxString editDescription);
	int GetID();
	wxString GetName();
	wxString GetDescription();

private:
	int id;
	wxString name;
	wxString description;
};

class AvailableEdits {
public:
	AvailableEdits();
	wxVector<Edit> GetAvailableEdits();
	wxString GetDescriptionFromID(int id);
	wxString GetNameFromID(int id);
	int GetIDFromName(wxString name);
	int GetIDFromDescription(wxString description);

private:
	wxVector<Edit> editVector;
};
#endif
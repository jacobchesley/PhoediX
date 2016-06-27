#ifndef EDIT_WINDOW_H
#define EDIT_WINDOW_H

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "Processing\ProcessorEdit\ProcessorEdit.h"
#include "wx\event.h"

enum {
	ID_REPROCESS_IMAGE
};

wxDECLARE_EVENT(REPROCESS_IMAGE_EVENT, wxCommandEvent);

class EditWindow : public wxScrolledWindow{
	public:
		EditWindow(wxWindow * parent, wxString editName);
		virtual void AddEditToProcessor();
		wxString GetName();
		void SetName(wxString editName);

		void SetParamsAndFlags(ProcessorEdit * edit);

		enum {
			ID_PROCESS_EDITS
		};
	private:
		wxString editNme;
};

#endif
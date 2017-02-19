// Copyright 2016 Jacob Chesley

#ifndef EDIT_LIST_PANEL_H
#define EDIT_LIST_PANEL_H

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/thread.h"

#include "GUI/EditList/AvailableEdits/AvailableEdits.h"
#include "GUI/EditList/AvailableEditWindows/AvailableEditWindows.h"
#include "GUI/EditList/EditListItem/EditListItem.h"
#include "GUI/EditList/EditSelection/EditSelection.h"
#include "GUI/AUIManager/AUIManager.h"
#include "GUI/Colors/Colors.h"
#include "Processing/Processor/Processor.h"
#include "Debugging/MemoryLeakCheck.h"

wxDECLARE_EVENT(START_EDITS_COMPLETE, wxThreadEvent);

enum {
	ID_START_EDITS_COMPLETE
};

class EditListPanel : public wxPanel {
public:

	EditListPanel(wxWindow * parent, Processor * processor, ZoomImagePanel * imgPanel);
	wxVector<Edit> edits;

	void AddRawWindow();
	void AddRawWindow(ProcessorEdit* editForParams);
	void RemoveRawWindow();
	void RemoveAllWindows();
	void AddEditsToProcessor();
	void AddEditWindows(wxVector<ProcessorEdit*> edits);
	void ReprocessImage();
	void ReprocessImageRaw(bool unpack = false);

private:

	void InitializeEdits();

	void OnRightClick(wxMouseEvent& WXUNUSED(event));
	void OnPopupMenuClick(wxCommandEvent& inEvt);
	void OnAddEdit(wxCommandEvent& WXUNUSED(event));

	void ReprocessImageEvt(wxCommandEvent& WXUNUSED(event));
	void ReprocessImageRawEvt(wxCommandEvent& WXUNUSED(event));
	void ReprocessUnpackImageRawEvt(wxCommandEvent& WXUNUSED(event));
	void PopulatePreviousEdits();

	void AddEditToPanel(wxCommandEvent& addEvt);
	void AddEditWindowToPanel(EditWindow * window, int editID, bool disable, bool autoActviate);
	void MoveEditUp(wxCommandEvent& upEvt);
	void MoveEditDown(wxCommandEvent& downEvt);
	void DeleteEdit(wxCommandEvent& deleteEvt);
	void DisableEdit(wxCommandEvent& WXUNUSED(event));

	void CopyEdit(wxCommandEvent& copyEvt);
	void PasteEdit(wxCommandEvent& pasteEvt);

	void StartEditsComplete();

	wxStaticText * titleText;
	wxBoxSizer * mainSizer;
	wxBoxSizer * bottomButtonsSizer;
	wxButton * addEditButton;

	EditSelection * editSelection;

	Processor * proc;
	bool hasRaw;
	wxWindow * par;

	ZoomImagePanel * imagePanel;

	enum Buttons {
		ADD_EDIT_BUTTON = 100
	};

	enum PopupMenuActions{
		COPY_EDIT_LIST,
		PASTE_EDIT_LIST

	};

	class EditListScroll : public wxScrolledWindow {
	public:
		EditListScroll(wxWindow * parent);
		void AddEdit(EditListItem * item);
		void DeleteEdit(size_t index);
		void DeleteAllEdits();
		void MoveEditUp(size_t index);
		void MoveEditDown(size_t index);
		int GetNextID();
		void SetNumTopEdits(size_t numTop);
		size_t GetNumTopEdits();

		wxVector<EditListItem*> GetEditList();

	private:

		void OnCropActivate(wxCommandEvent& evt);
		void OnCropDeactivate(wxCommandEvent& evt);

		wxWindow * parWindow;
		void RedrawEdits();
		wxBoxSizer * sizer;
		wxVector<EditListItem*> editList;
		size_t topEdits;
	};
	
	EditListScroll * scroller;

	class StartEditsThread : public wxThread {
	public:
		StartEditsThread(EditListPanel * parent, Processor * processor, bool processRaw, bool unpack = false);
		void Stop();

	protected:
		virtual wxThread::ExitCode Entry();

	private:
		EditListPanel * par;
		Processor * proc;
		bool stop;
		bool raw;
		bool unpck;
	};

	StartEditsThread * startEdits;
	bool editsStarted;
};
#endif
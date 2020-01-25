// Copyright 2018 Jacob Chesley.
// See LICENSE.TXT in root of project for license information.

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
#include "GUI/Controls/PhoediXButton/PhoediXButton.h"
#include "Processing/Processor/Processor.h"
#include "Session/Session.h"
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
	bool GetRawWindowOpen();
	void RemoveAllWindows(bool removeRawWindow = true);
	void AddEditsToProcessor();
	void AddEditWindows(wxVector<ProcessorEdit*> edits);
	void ReprocessImage();
	void ReprocessImageRaw(bool unpack = false);
	void SaveNoReprocess();
	void SaveNoReprocessWithSnapshots(wxVector<Snapshot*> snapshots);
	void EnableAllEdits();
	void DisableAllEdits();
	void SetSession(PhoediXSession * newSession);

private:

	void InitializeEdits();

	void OnRightClick(wxMouseEvent& WXUNUSED(event));
	void OnPopupMenuClick(wxCommandEvent& inEvt);
	void OnAddEdit(wxCommandEvent& WXUNUSED(event));

	void ReprocessImageEvt(wxCommandEvent& WXUNUSED(event));
	void ReprocessImageRawEvt(wxCommandEvent& WXUNUSED(event));
	void ReprocessUnpackImageRawEvt(wxCommandEvent& WXUNUSED(event));
	void OnSaveNoReprocess(wxMouseEvent& WXUNUSED(event));

	void AddEditToPanel(wxCommandEvent& addEvt);
	void AddEditWindowToPanel(EditWindow * window, int editID, bool disable, bool autoActviate);
	void MoveEditUp(wxCommandEvent& upEvt);
	void MoveEditDown(wxCommandEvent& downEvt);
	void DeleteEdit(wxCommandEvent& deleteEvt);
	void DisableEdit(wxCommandEvent& WXUNUSED(event));

	void CopyEdit(wxCommandEvent& copyEvt);
	void PasteEdit(wxCommandEvent& pasteEvt);

	void OnEditDrag(wxCommandEvent& evt);
	void OnEditDragComplete(wxCommandEvent& WXUNUSED(evt));

	void StartEditsComplete();
	void InformParentReprocess();

	wxStaticText * titleText;
	wxBoxSizer * mainSizer;
	wxBoxSizer * bottomButtonsSizer;
	PhoediXButton * addEditButton;

	EditSelection * editSelection;

	Processor * proc;
	bool hasRaw;
	wxWindow * par;
	bool preventProcessing;

	ZoomImagePanel * imagePanel;
	PhoediXSession * currentSession;
	EditListItem* highlightedItem;
	int lastDragPos;

	enum Buttons {
		ADD_EDIT_BUTTON = 100
	};

	enum PopupMenuActions{
		COPY_EDIT_LIST = 1,
		PASTE_EDIT_LIST,
		ENABLE_ALL_EDITS,
		DISABLE_ALL_EDITS,
		DELETE_ALL_EDITS
	};

	class EditListScroll : public wxScrolledWindow {
	public:
		EditListScroll(wxWindow * parent);
		void AddEdit(EditListItem * item);
		void DeleteEdit(size_t index);
		void DeleteAllEdits();
		void DeleteNonTopEdits();
		void MoveEditUp(size_t index);
		void MoveEditDown(size_t index);
		int GetNextID();
		void SetNumTopEdits(size_t numTop);
		size_t GetNumTopEdits();
		void SetEditDragCursor(int index);
		void CompleteDrag(EditListItem* item, int newIndex);
		wxVector<EditListItem*> GetEditList();

	private:

		void OnCropActivate(wxCommandEvent& evt);
		void OnCropDeactivate(wxCommandEvent& evt);

		wxWindow * parWindow;
		void RedrawEdits();
		wxBoxSizer * sizer;
		wxVector<EditListItem*> editList;
		size_t topEdits;
		wxPanel* dragCursor;
		int dragCursorPos;
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
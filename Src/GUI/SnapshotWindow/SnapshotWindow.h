// Copyright 2018 Jacob Chesley.
// See LICENSE.TXT in root of project for license information.

#ifndef SNAPSHOT_WINDOW_H
#define SNAPSHOT_WINDOW_H

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif

#include "wx/dataview.h"
#include "wx/wrapsizer.h"

#include "GUI/Controls/PhoediXButton/PhoediXButton.h"
#include "Processing/Snapshot/Snapshot.h"
#include "Processing/Snapshot/Snapshot.h"
#include "GUI/AUIManager/AUIManager.h"
#include "GUI/EditList/EditListPanel/EditListPanel.h"
#include "GUI/SnapshotWindow/SnapshotRenameDialog/SnapshotRenameDialog.h"
#include "GUI/Colors/Colors.h"
#include "Debugging/MemoryLeakCheck.h"

class SnapshotWindow : public wxScrolledWindow {

public:
	SnapshotWindow(wxWindow * parent, EditListPanel * editListPanel, Processor * processor);
	void AddSnapshots(wxVector<Snapshot*> snapshotsToLoad);
	void DeleteAllSnapshots();
	wxVector<Snapshot*> GetSnapshots();

private:

	void OnResize(wxSizeEvent& WXUNUSED(evt));
	void OnRenameSnapshot(wxCommandEvent & WXUNUSED(evt));
	void OnRemoveSnapshot(wxCommandEvent & WXUNUSED(evt));
	void OnRestoreSnapshot(wxCommandEvent & WXUNUSED(evt));
	void OnTakeSnapshot(wxCommandEvent & WXUNUSED(evt));

	wxString GetUniqueName(wxString tryName);

	wxVector<Snapshot*> snapshots;

	wxBoxSizer * mainSizer;
	wxDataViewListCtrl * snapshotList;

	wxWrapSizer * buttonSizer;

	PhoediXButton * removeSnapshot;
	PhoediXButton * renameSnapshot;
	PhoediXButton * restoreSnapshot;
	PhoediXButton * takeSnapshot;

	int curID;

	enum Buttons{
		ID_REMOVE_SNAPSHOT,
		ID_RENAME_SNAPSHOT,
		ID_RESTORE_SNAPSHOT,
		ID_TAKE_SNAPSHOT
	};

	EditListPanel * editPanel;
	Processor * proc;
};

#endif
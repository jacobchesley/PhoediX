// Copyright 2018 Jacob Chesley.
// See LICENSE.TXT in root of project for license information.

#include "SnapshotWindow.h"

SnapshotWindow::SnapshotWindow(wxWindow * parent, EditListPanel * editListPanel, Processor * processor) : wxScrolledWindow(parent){

	editPanel = editListPanel;
	proc = processor;
	curID = 0;
	snapshots = wxVector<Snapshot*>();
	
	this->SetBackgroundColour(parent->GetBackgroundColour());

	mainSizer = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(mainSizer);

	snapshotList = new wxListCtrl(this, -1, wxDefaultPosition, wxDefaultSize, wxLC_SINGLE_SEL| wxLC_REPORT);
	snapshotList->AppendColumn("Snapshot Name");
	snapshotList->SetColumnWidth(0, wxLIST_AUTOSIZE_USEHEADER);
	
	snapshotList->SetBackgroundColour(this->GetBackgroundColour());
	snapshotList->SetForegroundColour(Colors::TextLightGrey);

	buttonSizer = new wxWrapSizer();

	removeSnapshot = new PhoediXButton(this, SnapshotWindow::Buttons::ID_REMOVE_SNAPSHOT, "Remove");
	removeSnapshot->SetForegroundColour(Colors::TextLightGrey);
	removeSnapshot->SetBackgroundColour(Colors::BackGrey);

	renameSnapshot = new PhoediXButton(this, SnapshotWindow::Buttons::ID_RENAME_SNAPSHOT, "Rename");
	renameSnapshot->SetForegroundColour(Colors::TextLightGrey);
	renameSnapshot->SetBackgroundColour(Colors::BackGrey);

	restoreSnapshot = new PhoediXButton(this, SnapshotWindow::Buttons::ID_RESTORE_SNAPSHOT, "Restore Snapshot");
	restoreSnapshot->SetForegroundColour(Colors::TextLightGrey);
	restoreSnapshot->SetBackgroundColour(Colors::BackGrey);

	takeSnapshot = new PhoediXButton(this, SnapshotWindow::Buttons::ID_TAKE_SNAPSHOT, "Take Snapshot");
	takeSnapshot->SetForegroundColour(Colors::TextLightGrey);
	takeSnapshot->SetBackgroundColour(Colors::BackGrey);

	buttonSizer->Add(removeSnapshot);
	buttonSizer->AddSpacer(10);
	buttonSizer->Add(renameSnapshot);
	buttonSizer->AddSpacer(10);
	buttonSizer->Add(restoreSnapshot);
	buttonSizer->AddSpacer(10);
	buttonSizer->Add(takeSnapshot);

	this->GetSizer()->Add(snapshotList, 1, wxEXPAND);
	this->GetSizer()->Add(buttonSizer);

	this->SetScrollRate(5, 5);

	this->Layout();
	this->Fit();
	this->SetClientSize(this->GetVirtualSize());

	this->Bind(wxEVT_SIZE, (wxObjectEventFunction)&SnapshotWindow::OnResize, this);
	this->Bind(wxEVT_BUTTON, (wxObjectEventFunction)&SnapshotWindow::OnRemoveSnapshot, this, SnapshotWindow::Buttons::ID_REMOVE_SNAPSHOT);
	this->Bind(wxEVT_BUTTON, (wxObjectEventFunction)&SnapshotWindow::OnRenameSnapshot, this, SnapshotWindow::Buttons::ID_RENAME_SNAPSHOT);
	this->Bind(wxEVT_BUTTON, (wxObjectEventFunction)&SnapshotWindow::OnRestoreSnapshot, this, SnapshotWindow::Buttons::ID_RESTORE_SNAPSHOT);
	this->Bind(wxEVT_BUTTON, (wxObjectEventFunction)&SnapshotWindow::OnTakeSnapshot, this, SnapshotWindow::Buttons::ID_TAKE_SNAPSHOT);
}

void SnapshotWindow::OnResize(wxSizeEvent& WXUNUSED(evt)) {

	this->FitInside();
}

void SnapshotWindow::OnRemoveSnapshot(wxCommandEvent & WXUNUSED(evt)){
	
	
	size_t numberRemoved = 0;
	Snapshot * test;

	int toDelete = this->GetSelectedRow();
	for(size_t i = 0; i < snapshotList->GetItemCount() + numberRemoved; i++){

		// If row is selected, delete it from the GUI and internal snapshot list
		if(toDelete == i){

			// Remove name from GUI
			snapshotList->DeleteItem(i - numberRemoved);

			// Delete all processor edits for snapshot
			for (size_t j = 0; j < snapshots.at(i - numberRemoved)->editList.size(); j++) {
				delete snapshots.at(i - numberRemoved)->editList.at(j);
			}

			test = snapshots.at(i - numberRemoved);
			wxString deleteName = test->snapshotName;

			delete snapshots.at(i - numberRemoved);

			// Delete from snapshot list
			snapshots.erase(snapshots.begin() + i - numberRemoved);
			
			numberRemoved += 1;
		}
	}

	// Re index all the snapshots
	for (size_t i = 0; i < snapshots.size(); i++) {
		snapshots.at(i)->snapshotIndex = i;
	}
	
	// Save project
	editPanel->SaveNoReprocessWithSnapshots(this->GetSnapshots());
}

void SnapshotWindow::OnRenameSnapshot(wxCommandEvent & WXUNUSED(evt)){

	size_t selectedRow = this->GetSelectedRow();

	// If selected row is okay and only one selected row
	if(selectedRow != wxNOT_FOUND && selectedRow < snapshots.size()){

		SnapshotRenameDialog * renameDialog = new SnapshotRenameDialog(this, snapshots.at(selectedRow)->snapshotName);

		if(renameDialog->ShowModal() == wxID_OK){

			// Get unique name of dialogs name value and set snapshot name
			wxString newName = this->GetUniqueName(renameDialog->GetNewName());
			snapshots.at(selectedRow)->snapshotName = newName;

			// Delete snapshot from GUI, and insert new name at the selected row
			snapshotList->DeleteItem(selectedRow);
			snapshotList->InsertItem(selectedRow, newName);
		}
		renameDialog->Destroy();
	}

	// Save project
	editPanel->SaveNoReprocessWithSnapshots(this->GetSnapshots());
}

void SnapshotWindow::OnRestoreSnapshot(wxCommandEvent & WXUNUSED(evt)){

	
	if(this->GetSelectedRow() != wxNOT_FOUND && (size_t)this->GetSelectedRow() < snapshots.size()){
	
		editPanel->AddEditWindows(snapshots.at(this->GetSelectedRow())->editList);
		PhoedixAUIManager::GetPhoedixAUIManager()->LoadPerspective(snapshots.at(this->GetSelectedRow())->auiPerspective);
		PhoedixAUIManager::GetPhoedixAUIManager()->Update();
	}

	// Reprocess
	editPanel->ReprocessImageRaw();
}

void SnapshotWindow::OnTakeSnapshot(wxCommandEvent & WXUNUSED(evt)){
		
	editPanel->AddEditsToProcessor();

	// Create a new snapshot
	Snapshot * newSnapshot = new Snapshot();
	newSnapshot->snapshotIndex = snapshots.size();
	newSnapshot->snapshotName = this->GetUniqueName(wxNow());
	newSnapshot->auiPerspective = PhoedixAUIManager::GetPhoedixAUIManager()->SavePerspective();

	// Copy edit list from processor
	newSnapshot->editList.clear();
	for(size_t i = 0; i < proc->GetEditVector().size(); i++){
		newSnapshot->editList.push_back(new ProcessorEdit(*proc->GetEditVector().at(i)));
	}

	// Add new snapshot to internal list
	snapshots.push_back(newSnapshot);

	// Add snapshot name to GUI
	wxVector<wxVariant> snapshotListData;
	snapshotListData.push_back(newSnapshot->snapshotName);
	snapshotList->InsertItem(snapshotList->GetItemCount(), newSnapshot->snapshotName);

	// Save project
	editPanel->SaveNoReprocessWithSnapshots(this->GetSnapshots());
}

wxVector<Snapshot*> SnapshotWindow::GetSnapshots(){
	return snapshots;
}

void SnapshotWindow::AddSnapshots(wxVector<Snapshot*> snapshotsToLoad) {
	
	for (size_t i = 0; i < snapshotsToLoad.size(); i++) {

		Snapshot * tempSnapshot = new Snapshot();

		// Copy snapshot index, name and perspective
		tempSnapshot->snapshotName = this->GetUniqueName(snapshotsToLoad.at(i)->snapshotName);
		tempSnapshot->auiPerspective = snapshotsToLoad.at(i)->auiPerspective;
		tempSnapshot->snapshotIndex = snapshotsToLoad.at(i)->snapshotIndex;

		// Copy snapshot edit list
		tempSnapshot->editList.clear();
		for (size_t j = 0; j < snapshotsToLoad.at(i)->editList.size(); j++) {
			tempSnapshot->editList.push_back(new ProcessorEdit(*snapshotsToLoad.at(i)->editList.at(j)));
		}
		snapshots.push_back(tempSnapshot);

		// Add snapshot name to GUI
		wxVector<wxVariant> snapshotListData;
		snapshotListData.push_back(tempSnapshot->snapshotName);
		snapshotList->InsertItem(snapshotList->GetItemCount(), tempSnapshot->snapshotName);
	}
}

void SnapshotWindow::DeleteAllSnapshots(){

	int numberRemoved = 0;
	for (size_t i = 0; i < snapshots.size(); i++) {

		// Delete all processor edits for snapshot
		for (size_t j = 0; j < snapshots.at(i - numberRemoved)->editList.size(); j++) {
			delete snapshots.at(i - numberRemoved)->editList.at(j);
		}

		delete snapshots.at(i);
	}

	snapshotList->DeleteAllItems();
	snapshots.clear();
}

int SnapshotWindow::GetSelectedRow() {

	for (size_t i = 0; i < snapshotList->GetItemCount(); i++) {
		if (snapshotList->GetItemState(i, wxLIST_STATE_SELECTED)) {
			return i;
		}
	}
	return wxNOT_FOUND;
}

wxString SnapshotWindow::GetUniqueName(wxString tryName){

	for(size_t i = 0; i < snapshots.size(); i++){

		// This matches.  Check to see how many matches there are.
		// Example - if we have snapshot name test, and the same name is entered in for the next
		// 4 times, we will have:
		// test
		// test - 1
		// test - 2
		// test - 3
		// test - 4

		// Name already exists
		if(tryName.CmpNoCase(snapshots.at(i)->snapshotName) == 0){
			
			// Find how many times name exists and name it that
			int idx = 0;
			wxString prefix = tryName + " - ";

			// Coninue until we find a unique name
			while(true){

				// Create possible name
				bool matchFound = false;
				idx += 1;
				wxString possibleName = prefix + wxString::Format(wxT("%i"), idx);

				// Check if possible name is already in list of names
				for(size_t j = 0; j < snapshots.size(); j++){
					if(possibleName.CmpNoCase(snapshots.at(j)->snapshotName) == 0){
						matchFound = true;
						break;
					}
				}

				// If possible name is not in list of names, return it, it is unique!
				if(!matchFound){
					return possibleName;
				}
			}
		}
	}

	// original name is unique
	return tryName;
}
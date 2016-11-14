// Copyright 2016 Jacob Chesley

#include "SnapshotWindow.h"

SnapshotWindow::SnapshotWindow(wxWindow * parent, EditListPanel * editListPanel, Processor * processor) : wxScrolledWindow(parent){

	editPanel = editListPanel;
	proc = processor;
	curID = 0;
	snapshots = wxVector<Snapshot*>();
	
	this->SetBackgroundColour(parent->GetBackgroundColour());

	mainSizer = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(mainSizer);

	snapshotList = new wxDataViewListCtrl(this, -1, wxDefaultPosition, wxDefaultSize, 0 | wxDV_NO_HEADER |wxDV_MULTIPLE);
	snapshotList->AppendTextColumn("Snapshot Name");
	snapshotList->SetBackgroundColour(this->GetBackgroundColour());
	snapshotList->SetForegroundColour(Colors::TextLightGrey);

	buttonSizer = new wxWrapSizer();

	removeSnapshot = new wxButton(this, SnapshotWindow::Buttons::ID_REMOVE_SNAPSHOT, "Remove", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	removeSnapshot->SetForegroundColour(Colors::TextLightGrey);
	removeSnapshot->SetBackgroundColour(Colors::BackGrey);

	restoreSnapshot = new wxButton(this, SnapshotWindow::Buttons::ID_RESTORE_SNAPSHOT, "Restore Snapshot", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	restoreSnapshot->SetForegroundColour(Colors::TextLightGrey);
	restoreSnapshot->SetBackgroundColour(Colors::BackGrey);

	takeSnapshot = new wxButton(this, SnapshotWindow::Buttons::ID_TAKE_SNAPSHOT, "Take Snapshot", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	takeSnapshot->SetForegroundColour(Colors::TextLightGrey);
	takeSnapshot->SetBackgroundColour(Colors::BackGrey);

	buttonSizer->Add(removeSnapshot);
	buttonSizer->AddSpacer(10);
	buttonSizer->Add(restoreSnapshot);
	buttonSizer->AddSpacer(10);
	buttonSizer->Add(takeSnapshot);

	this->GetSizer()->Add(snapshotList, 1, wxEXPAND);
	this->GetSizer()->Add(buttonSizer);

	this->SetScrollRate(5, 5);

	this->Bind(wxEVT_SIZE, (wxObjectEventFunction)&SnapshotWindow::OnResize, this);
	this->Bind(wxEVT_BUTTON, (wxObjectEventFunction)&SnapshotWindow::OnRemoveSnapshot, this, SnapshotWindow::Buttons::ID_REMOVE_SNAPSHOT);
	this->Bind(wxEVT_BUTTON, (wxObjectEventFunction)&SnapshotWindow::OnRestoreSnapshot, this, SnapshotWindow::Buttons::ID_RESTORE_SNAPSHOT);
	this->Bind(wxEVT_BUTTON, (wxObjectEventFunction)&SnapshotWindow::OnTakeSnapshot, this, SnapshotWindow::Buttons::ID_TAKE_SNAPSHOT);
}

void SnapshotWindow::OnResize(wxSizeEvent& WXUNUSED(evt)) {
	this->FitInside();
}

void SnapshotWindow::OnRemoveSnapshot(wxCommandEvent & WXUNUSED(evt)){
	
	int numberRemoved = 0;
	Snapshot * test;
	for(size_t i = 0; i < snapshotList->GetItemCount() + numberRemoved; i++){

		// If row is selected, delete it from the GUI and internal snapshot list
		if(snapshotList->IsRowSelected(i - numberRemoved)){

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
}

void SnapshotWindow::OnRestoreSnapshot(wxCommandEvent & WXUNUSED(evt)){
	if(snapshotList->GetSelectedRow() != wxNOT_FOUND && snapshotList->GetSelectedRow() < snapshots.size()){
	
		editPanel->AddEditWindows(snapshots.at(snapshotList->GetSelectedRow())->editList);
		PhoedixAUIManager::GetPhoedixAUIManager()->LoadPerspective(snapshots.at(snapshotList->GetSelectedRow())->auiPerspective);
		PhoedixAUIManager::GetPhoedixAUIManager()->Update();
	}
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
	snapshotList->AppendItem(snapshotListData);
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
		snapshotList->AppendItem(snapshotListData);
	}
	
}

void SnapshotWindow::DeleteAllSnapshots(){

	int numberRemoved = 0;
	Snapshot * test;
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
// Copyright 2018 Jacob Chesley.
// See LICENSE.TXT in root of project for license information.

#include "EditWindow.h"

wxDEFINE_EVENT(REPROCESS_IMAGE_EVENT, wxCommandEvent);
wxDEFINE_EVENT(REPROCESS_IMAGE_RAW_EVENT, wxCommandEvent);
wxDEFINE_EVENT(REPROCESS_UNPACK_IMAGE_RAW_EVENT, wxCommandEvent);
wxDEFINE_EVENT(SAVE_NO_REPROCESS, wxCommandEvent);
wxDEFINE_EVENT(WATCHDOG_TERMINATED_EVENT, wxThreadEvent);

EditWindow::EditWindow(wxWindow * parent, wxString editName, Processor * processor, ZoomImagePanel * zoomImgPanel) : wxScrolledWindow(parent) {
	editNme = editName;
	watchdog = NULL;
	watchdogRealtime = NULL;
	isDisabled = false;
	parWindow = parent;
	activated = false;
	proc = processor;
	imgPanel = zoomImgPanel;
	index = -1;

	this->Bind(REPROCESS_IMAGE_EVENT, (wxObjectEventFunction)&EditWindow::Process, this, EditWindow::ID_PROCESS_EDITS);
	this->Bind(WATCHDOG_TERMINATED_EVENT, (wxObjectEventFunction)& EditWindow::OnWatchdogTerminated, this);
}

void EditWindow::DestroyEditWindow() {
	this->StopWatchdog();
}

void EditWindow::DoProcess() {

	if (activated) {
		wxCommandEvent evt(REPROCESS_IMAGE_EVENT, ID_REPROCESS_IMAGE);
		wxPostEvent(parWindow, evt);
		this->SetUpdated(false);
	}
}

void EditWindow::Process(wxCommandEvent& WXUNUSED(event)) {

	if(activated){
		wxCommandEvent evt(REPROCESS_IMAGE_EVENT, ID_REPROCESS_IMAGE);
		wxPostEvent(parWindow, evt);
		this->SetUpdated(false);
	}
}

void EditWindow::SaveNoReprocess() {

	wxCommandEvent evt(SAVE_NO_REPROCESS, ID_SAVE_NO_REPROCESS);
	wxPostEvent(parWindow, evt);
}

wxString EditWindow::GetName() {
	return editNme;
}

void EditWindow::Activate(){
	activated = true;
	this->StartWatchdog();
}

void EditWindow::Deactivate(){
	activated = false;
	this->StopWatchdog();
}

bool EditWindow::GetActivated() {
	return activated;
}

void EditWindow::SetName(wxString editName) {
	editNme = editName;
}

void EditWindow::SetUpdated(bool update){
	updated = update;
}

bool EditWindow::GetUpdated(){
	return updated;
}

void EditWindow::SetDisabled(bool disable) {
	isDisabled = disable;
}

bool EditWindow::GetDisabled() {
	return isDisabled;
}

void EditWindow::AddEditToProcessor() {

	if (!activated) { return; }
	ProcessorEdit * edit = this->GetParamsAndFlags();
	if(edit != NULL){
		proc->AddEdit(edit);
	}
}

void EditWindow::SetParamsAndFlags(ProcessorEdit* WXUNUSED(edit)) {

}

void EditWindow::SetParamsAndFlags(ProcessorEdit * WXUNUSED(edit), bool WXUNUSED(reprocess)) {

}

ProcessorEdit * EditWindow::GetParamsAndFlags() {
	return NULL;
}

bool EditWindow::CheckCopiedParamsAndFlags(){
	return false;
}

void EditWindow::OnUpdate(wxCommandEvent& WXUNUSED(event)){
	updated = true;
}

void EditWindow::StartWatchdog(){
	if (watchdog != NULL) { return; }
	watchdog = new WatchForUpdateThread(this, 50);
	watchdog->Run();
}

void EditWindow::StopWatchdog(){
	if(watchdog != NULL){
		activated = false;
		watchdog->Stop();
		while (watchdog != NULL) {
			wxYield();
		}
	}
}

void EditWindow::OnWatchdogTerminated(wxThreadEvent& WXUNUSED(event)) {
	watchdog = NULL;
	this->GetEventHandler()->DeletePendingEvents();
}

ZoomImagePanel * EditWindow::GetZoomImagePanel() {
	return imgPanel;
}

Processor * EditWindow::GetProcessor() {
	return proc;
}

void EditWindow::SetPreviousEdits(wxVector<ProcessorEdit*> prevEdits) {

	this->DestroyPreviousEdits();

	// Copy edits to previous edits
	for (size_t i = 0; i < prevEdits.size(); i++) {
		previousEdits.push_back(new ProcessorEdit(*prevEdits.at(i)));
	}
}
wxVector<ProcessorEdit*> EditWindow::GetPreviousEdits() {
	return previousEdits;
}

void EditWindow::DestroyPreviousEdits() {

	// Delete previous edits, then clear vector
	for (size_t i = 0; i < previousEdits.size(); i++) {
		delete previousEdits.at(i);
	}
	previousEdits.clear();
}

EditWindow::WatchForUpdateThread::WatchForUpdateThread(EditWindow * windowPar, int sleeptime){
	window = windowPar;
	contin = true;
	slp = sleeptime;
}

wxThread::ExitCode EditWindow::WatchForUpdateThread::Entry(){

	wxString test = window->GetName();

	while(contin){

		if(window->GetUpdated() && window->GetActivated()){

			// Set window to not updated, wait to see if it is updated again
			window->SetUpdated(false);
			this->Sleep(slp);

			// If window still has not been updated, go ahead and and fire the process edits
			// This will prevent the window from processing several similar edits when the
			// sliders change quickly during movement.
			if(!window->GetUpdated() && window != NULL){
				wxCommandEvent evt(REPROCESS_IMAGE_EVENT, EditWindow::ID_PROCESS_EDITS);
				wxPostEvent(window, evt);
			}
		}
		this->Sleep(20);
	}
	
	wxThreadEvent endEvt(WATCHDOG_TERMINATED_EVENT, ID_WATCHDOG_TERMINATED);
	wxPostEvent(window, endEvt);
	return (wxThread::ExitCode)0;
}

void EditWindow::WatchForUpdateThread::Stop(){
	contin = false;
}

void EditWindow::SetIndex(int i) {
	index = i;
}

int EditWindow::GetIndex() {
	return index;
}
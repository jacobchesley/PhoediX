#include "EditWindow.h"

wxDEFINE_EVENT(REPROCESS_IMAGE_EVENT, wxCommandEvent);

EditWindow::EditWindow(wxWindow * parent, wxString editName) : wxScrolledWindow(parent) {
	editNme = editName;
	watchdog = NULL;
	watchdogRealtime = NULL;
	isDisabled = false;
}

wxString EditWindow::GetName() {
	return editNme;
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

void EditWindow::AddEditToProcessor() {}

void EditWindow::SetParamsAndFlags(ProcessorEdit * WXUNUSED(edit)) {}

void EditWindow::OnUpdate(wxCommandEvent& WXUNUSED(event)){
	updated = true;
}


void EditWindow::StartWatchdog(){
	watchdog = new WatchForUpdateThread(this, 250);
	watchdog->Run();
	watchdog->SetPriority(2);
}

void EditWindow::StopWatchdog(){
	if(watchdog != NULL){
		watchdog->Stop();
	}

	if(watchdogRealtime != NULL){
		watchdogRealtime->Stop();
	}
}


EditWindow::WatchForUpdateThread::WatchForUpdateThread(EditWindow * windowPar, int sleeptime){
	window = windowPar;
	contin = true;
	slp = sleeptime;
}

wxThread::ExitCode EditWindow::WatchForUpdateThread::Entry(){
	while(contin){
		if(window->GetUpdated()){

			// Set window to not updated, wait to see if it is updated again
			window->SetUpdated(false);
			this->Sleep(slp);

			// If window still has not been updated, go ahead and and fire the process edits
			// This will prevent the window from processing several similar edits when the
			// sliders change quickly during movement.
			if(!window->GetUpdated()){
				wxCommandEvent evt(wxEVT_BUTTON, EditWindow::ID_PROCESS_EDITS);
				wxPostEvent(window, evt);
			}
		}
		this->Sleep(20);
	}
	return (wxThread::ExitCode)0;
}

void EditWindow::WatchForUpdateThread::Stop(){
	contin = false;
}
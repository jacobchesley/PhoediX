#include "EditWindow.h"

wxDEFINE_EVENT(REPROCESS_IMAGE_EVENT, wxCommandEvent);

EditWindow::EditWindow(wxWindow * parent, wxString editName) : wxScrolledWindow(parent) {
	editNme = editName;
}

wxString EditWindow::GetName() {
	return editNme;
}

void EditWindow::SetName(wxString editName) {
	editNme = editName;
}

void EditWindow::AddEditToProcessor() {}
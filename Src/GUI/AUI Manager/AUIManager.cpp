#include "AUIManager.h"

wxAuiManager* PhoedixAUIManager::auiManager = NULL;

wxAuiManager* PhoedixAUIManager::GetPhoedixAUIManager() {
	return auiManager;
}

void PhoedixAUIManager::SetPhoedixAUIManager(wxAuiManager * manager) {
	auiManager = manager;
}
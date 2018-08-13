// Copyright 2018 Jacob Chesley.
// See LICENSE.TXT in root of project for license information.

#include "AUIManager.h"

wxAuiManager* PhoedixAUIManager::auiManager = NULL;
wxFrame* PhoedixAUIManager::mainWindow = NULL;

wxAuiManager* PhoedixAUIManager::GetPhoedixAUIManager() {
	return auiManager;
}

void PhoedixAUIManager::SetPhoedixAUIManager(wxAuiManager * manager) {
	auiManager = manager;
}

wxFrame * PhoedixAUIManager::GetMainWindow() {
	return mainWindow;
}

void PhoedixAUIManager::SetMainWindow(wxFrame * main) {
	mainWindow = main;
}
// Copyright 2016 Jacob Chesley

#include "AUIManager.h"

wxAuiManager* PhoedixAUIManager::auiManager = NULL;
wxWindow* PhoedixAUIManager::mainWindow = NULL;

wxAuiManager* PhoedixAUIManager::GetPhoedixAUIManager() {
	return auiManager;
}

void PhoedixAUIManager::SetPhoedixAUIManager(wxAuiManager * manager) {
	auiManager = manager;
}

wxWindow * PhoedixAUIManager::GetMainWindow() {
	return mainWindow;
}

void PhoedixAUIManager::SetMainWindow(wxWindow * main) {
	mainWindow = main;
}
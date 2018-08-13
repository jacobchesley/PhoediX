// Copyright 2018 Jacob Chesley.
// See LICENSE.TXT in root of project for license information.

#ifndef PHOEDIX_AUI_MANAGER_H
#define PHOEDIX_AUI_MANAGER_H

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

 // for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/aui/aui.h"

#include "Debugging/MemoryLeakCheck.h"

class PhoedixAUIManager {

	public:
		static wxAuiManager * GetPhoedixAUIManager();
		static void SetPhoedixAUIManager(wxAuiManager * manager);
		static wxFrame * GetMainWindow();
		static void SetMainWindow(wxFrame * window);

	private:
		static wxAuiManager * auiManager;
		static wxFrame * mainWindow;
};
#endif
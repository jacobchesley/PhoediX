#ifndef PHOEDIX_AUI_MANAGER_H
#define PHOEDIX_AUI_MANAGER_H

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

 // for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx\aui\aui.h"

#include "Debugging\MemoryLeakCheck.h"

class PhoedixAUIManager {

	public:
		static wxAuiManager* GetPhoedixAUIManager();
		static void SetPhoedixAUIManager(wxAuiManager * manager);

	private:
		static wxAuiManager* auiManager;
};
#endif
#ifndef PHOEDIX_SESSION_H
#define PHOEDIX_SESSION_H

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/xml/xml.h"

#include "SessionEditList\SessionEditList.h"

class PhoediXSession{

public:
	PhoediXSession();

	void LoadSessionFromFile(wxString filePath);
	void SaveSessionToFile(wxString filePath);
	
	wxString GetImageFilePath();
	void SetImageFilePath(wxString imageFilePath);

	PhoediXSessionEditList *GetEditList();

	float GetImageZoomLevel();
	void SetImageZoomLevel();

	wxString GetPerspective();
	void SetPerspective(wxString newPerspective);

private:

	wxString imgFile;
	PhoediXSessionEditList *editList;
	float imgZoom;
	wxString auiPerspective;
};
#endif
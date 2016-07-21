#ifndef PHOEDIX_SESSION_EDIT_LIST
#define PHOEDIX_SESSION_EDIT_LIST

#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/xml/xml.h"
#include "wx/numformatter.h"
#include "wx/tokenzr.h"

#include "Processing\ProcessorEdit\ProcessorEdit.h"

class PhoediXSessionEditList {

public:
	PhoediXSessionEditList();
	void SetSessionEditList(wxVector<ProcessorEdit*> newEditList);
	wxVector<ProcessorEdit*> GetSessionEditList();
	void LoadSessionEditList(wxXmlNode * editListNode);
	void SaveSessionEditList(wxXmlNode * phoedixProjectNode);

private:
	wxVector<ProcessorEdit*> editList;

};
#endif
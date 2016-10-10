#ifndef EXPORT_WINDOW_H
#define EXPORT_WINDOW_H

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif
#include "wx\progdlg.h"
#include "wx\filename.h"

#include "GUI\Colors\Colors.h"
#include "GUI\Controls\DoubleSlider\DoubleSlider.h"
#include "GUI\EditList\EditListPanel\EditListPanel.h"
#include "Processing\ImageHandler\ImageHandler.h"
#include "Processing\Processor\Processor.h"
#include "Debugging\MemoryLeakCheck.h"

enum {
	ID_UPDATE_EXPORT_PROGRESS_NUM_EVENT,
	ID_UPDATE_EXPORT_PROGRESS_MSG_EVENT,
	ID_UPDATE_EXPORT_PROGRESS_CLOSE
};

wxDECLARE_EVENT(UPDATE_EXPORT_PROGRESS_NUM_EVENT, wxCommandEvent);
wxDECLARE_EVENT(UPDATE_EXPORT_PROGRESS_MSG_EVENT, wxCommandEvent);
wxDECLARE_EVENT(UPDATE_EXPORT_PROGRESS_CLOSE, wxCommandEvent);

class ExportWindow : public wxScrolledWindow {

public:
	ExportWindow(wxWindow * parent, Processor * processor, EditListPanel * panel);
	void ProcessingComplete();
	void SetEditNum(int editNum);
	void SetMessage(wxString message);
	void RawImageLoaded(bool isThereRawImage);

private:

	int progressSize;
	void SetProgressEditNum(wxCommandEvent & evt);
	void SetProgressEditMsg(wxCommandEvent & evt);
	void CloseProgress(wxCommandEvent & WXUNUSED(evt));
	bool rawImageLoaded;
	void OnExport(wxCommandEvent & WXUNUSED(event));
	void OnCombo(wxCommandEvent & WXUNUSED(event));
	void OnBrowse(wxCommandEvent & WXUNUSED(event));
	void OnKeepSize(wxCommandEvent & WXUNUSED(event));

	bool exportStarted;
	EditListPanel * editList;
	Processor * proc;

	wxFlexGridSizer * mainSizer;

	wxStaticText * fileTypeLabel;
	wxComboBox * fileTypeControl;

	wxStaticText * jpegQualityLabel;
	DoubleSlider * jpegQualitySlide;

	wxStaticText * outputFolderLabel;
	wxTextCtrl * outputFolderText;
	wxButton * outputFolderButton;

	wxStaticText * outputNameLabel;
	wxTextCtrl * outputNameText;

	wxButton * exportButton;

	wxProgressDialog * progress;
	wxCriticalSection locker;
	enum ExportActions{
		ID_BROWSE,
		ID_EXPORT
	};
};
#endif
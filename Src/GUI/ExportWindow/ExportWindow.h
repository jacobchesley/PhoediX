// Copyright 2018 Jacob Chesley.
// See LICENSE.TXT in root of project for license information.

#ifndef EXPORT_WINDOW_H
#define EXPORT_WINDOW_H

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif
#include "wx/progdlg.h"
#include "wx/filename.h"

#include "GUI/Controls/PhoediXButton/PhoediXButton.h"
#include "GUI/Colors/Colors.h"
#include "GUI/Icons/Icons.h"
#include "GUI/Controls/DoubleSlider/DoubleSlider.h"
#include "GUI/Controls//PhoediXComboBox/PhoediXComboBox.h"
#include "GUI/EditList/EditListPanel/EditListPanel.h"
#include "Processing/ImageHandler/ImageHandler.h"
#include "Processing/Processor/Processor.h"
#include "Debugging/MemoryLeakCheck.h"

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

	int GetImageType();
	void SetImageType(int type);

	int GetJPEGQuality();
	void SetJPEGQuality(int quality);
	
	wxString GetExportFolder();
	void SetExportFolder(wxString folder);

	wxString GetExportName();
	void SetExportName(wxString name);

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
	bool fastEditFlag;

	wxFlexGridSizer * mainSizer;

	wxStaticText * fileTypeLabel;
	PhoediXComboBox * fileTypeControl;

	wxStaticText * jpegQualityLabel;
	DoubleSlider * jpegQualitySlide;

	wxStaticText * outputFolderLabel;
	wxTextCtrl * outputFolderText;
	PhoediXButton * outputFolderButton;

	wxStaticText * outputNameLabel;
	wxTextCtrl * outputNameText;

	PhoediXButton * exportButton;

	wxGenericProgressDialog * progress;
	wxCriticalSection locker;
	enum ExportActions{
		ID_BROWSE,
		ID_EXPORT
	};
};
#endif
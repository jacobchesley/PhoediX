// Copyright 2018 Jacob Chesley.
// See LICENSE.TXT in root of project for license information.

#ifndef EDIT_WINDOW_H
#define EDIT_WINDOW_H

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/thread.h"

#include "GUI/ImageDisplay/ZoomImagePanel/ZoomImagePanel.h"
#include "Processing/ProcessorEdit/ProcessorEdit.h"
#include "Processing/Processor/Processor.h"
#include "wx/event.h"

enum {
	ID_REPROCESS_IMAGE,
	ID_REPROCESS_IMAGE_RAW,
	ID_REPROCESS_UNPACK_IMAGE_RAW,
	ID_SAVE_NO_REPROCESS,
	ID_WATCHDOG_TERMINATED
};

wxDECLARE_EVENT(REPROCESS_IMAGE_EVENT, wxCommandEvent);
wxDECLARE_EVENT(REPROCESS_IMAGE_RAW_EVENT, wxCommandEvent);
wxDECLARE_EVENT(REPROCESS_UNPACK_IMAGE_RAW_EVENT, wxCommandEvent);
wxDECLARE_EVENT(SAVE_NO_REPROCESS_EVENT, wxCommandEvent);
wxDECLARE_EVENT(WATCHDOG_TERMINATED_EVENT, wxThreadEvent);

class EditWindow : public wxScrolledWindow{
	public:
		EditWindow(wxWindow * parent, wxString editName, Processor * processor, ZoomImagePanel * zoomImgPanel = NULL);
		virtual void DestroyEditWindow();

		void DoProcess();
		void Process(wxCommandEvent& WXUNUSED(event));
		void AddEditToProcessor();
		wxString GetName();
		void SetName(wxString editName);

		virtual void SetParamsAndFlags(ProcessorEdit* edit);
		virtual void SetParamsAndFlags(ProcessorEdit * edit, bool reprocess);
		virtual ProcessorEdit * GetParamsAndFlags();
		virtual bool CheckCopiedParamsAndFlags();
		virtual int GetEditType();

		void OnUpdate(wxCommandEvent& WXUNUSED(event));
		void StartWatchdog();
		void StopWatchdog();

		void SetDisabled(bool disable);
		bool GetDisabled();

		void Activate();
		void Deactivate();
		bool GetActivated();

		void SetPreviousEdits(wxVector<ProcessorEdit*> prevEdits);
		wxVector<ProcessorEdit*> GetPreviousEdits();
		void DestroyPreviousEdits();

		void SetIndex(int i);
		int GetIndex();

		void SaveNoReprocess();
		enum {
			ID_PROCESS_EDITS
		};

	protected:
		void SetUpdated(bool update);
		bool GetUpdated();
		ZoomImagePanel * GetZoomImagePanel();
		Processor * GetProcessor();
		bool isDisabled;

		
	private:

		void OnWatchdogTerminated(wxThreadEvent& WXUNUSED(event));

		wxWindow * parWindow;
		bool updated;
		wxString editNme;
		bool activated;
		Processor * proc;
		ZoomImagePanel * imgPanel;
		wxVector<ProcessorEdit*> previousEdits;
		int index;

		class WatchForUpdateThread : public wxThread {

		public:
			WatchForUpdateThread(EditWindow * windowPar, int sleeptime);
			void Stop();

		protected:
			virtual ExitCode Entry();

		private:
			bool contin;
			EditWindow * window;
			int slp;
		};

	WatchForUpdateThread * watchdog;
	WatchForUpdateThread * watchdogRealtime;
		
};

#endif
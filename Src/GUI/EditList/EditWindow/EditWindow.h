#ifndef EDIT_WINDOW_H
#define EDIT_WINDOW_H

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx\thread.h"

#include "Processing\ProcessorEdit\ProcessorEdit.h"
#include "Processing\Processor\Processor.h"
#include "wx\event.h"

enum {
	ID_REPROCESS_IMAGE,
	ID_REPROCESS_IMAGE_RAW,
	ID_REPROCESS_UNPACK_IMAGE_RAW
};

wxDECLARE_EVENT(REPROCESS_IMAGE_EVENT, wxCommandEvent);
wxDECLARE_EVENT(REPROCESS_IMAGE_RAW_EVENT, wxCommandEvent);
wxDECLARE_EVENT(REPROCESS_UNPACK_IMAGE_RAW_EVENT, wxCommandEvent);

class EditWindow : public wxScrolledWindow{
	public:
		EditWindow(wxWindow * parent, wxString editName, Processor * processor);

		void DoProcess();
		void Process(wxCommandEvent& WXUNUSED(event));
		void AddEditToProcessor();
		wxString GetName();
		void SetName(wxString editName);

		virtual void SetParamsAndFlags(ProcessorEdit * edit);
		virtual ProcessorEdit * GetParamsAndFlags();
		virtual bool CheckCopiedParamsAndFlags();

		void OnUpdate(wxCommandEvent& WXUNUSED(event));
		void StartWatchdog();
		void StopWatchdog();

		void SetDisabled(bool disable);
		bool GetDisabled();

		void Activate();
		void Deactivate();

		enum {
			ID_PROCESS_EDITS
		};

	protected:
		void SetUpdated(bool update);
		bool GetUpdated();
		bool isDisabled;
		
	private:

		wxWindow * parWindow;
		bool updated;
		wxString editNme;
		bool activated;
		Processor * proc;

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
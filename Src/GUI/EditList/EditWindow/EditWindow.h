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
#include "wx\event.h"

enum {
	ID_REPROCESS_IMAGE
};

wxDECLARE_EVENT(REPROCESS_IMAGE_EVENT, wxCommandEvent);

class EditWindow : public wxScrolledWindow{
	public:
		EditWindow(wxWindow * parent, wxString editName);
		virtual void AddEditToProcessor();
		wxString GetName();
		void SetName(wxString editName);

		virtual void SetParamsAndFlags(ProcessorEdit * edit);
		void OnUpdate(wxCommandEvent& WXUNUSED(event));
		void StartWatchdog();
		void StopWatchdog();

		void SetDisabled(bool disable);
		bool GetDisabled();

		enum {
			ID_PROCESS_EDITS
		};

	protected:
		void SetUpdated(bool update);
		bool GetUpdated();
		bool isDisabled;
		
	private:
		bool updated;
		wxString editNme;

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
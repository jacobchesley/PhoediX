#ifndef PROCESSOR_H
#define PROCESSOR_H

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx\thread.h"

#include "Processing\Image\Image.h"
#include "Debugging\MemoryLeakCheck.h"
#include "Processing\ProcessorEdit\ProcessorEdit.h"

enum {
	ID_PROCESSOR_MESSAGE
};

wxDECLARE_EVENT(PROCESSOR_MESSAGE_EVENT, wxCommandEvent);

class Processor {
public:

	Processor(wxWindow * parent = NULL);
	~Processor();

	Image * GetImage();
	
	void ProcessEdits(wxVector<ProcessorEdit*> editList);
	void ProcessEdit(ProcessorEdit * edit);

	void AddEdit(ProcessorEdit * edit);
	void ProcessEdits();
	void DeleteEdits();

	void Enable16Bit();
	void Disable16Bit();

	void SetOriginalImage(Image * newOriginalImage);
	Image* GetOriginalImage();
	void RevertToOriginalImage();

	void SetUpdated(bool updated);
	bool GetUpdated();

	void Lock();
	void Unlock();
	bool GetLocked();

	void Get8BitHistrogram(uint32_t * outputHistogramRed, uint32_t * outputHistogramGreen, uint32_t * outputHistogramBlue, uint32_t * outputHistogramGrey);
	void Get16BitHistrogram(uint32_t * outputHistogramRed, uint32_t * outputHistogramGreen, uint32_t * outputHistogramBlue, uint32_t * outputHistogramGrey);

	enum RotationCropping{
		KEEP_SIZE,
		FIT,
		EXPAND
	};

private:

	Image * img;
	Image * originalImg;

	bool didUpdate;

	wxCriticalSection lock;
	bool locked;
	static double pi;

	wxWindow * parWindow;

	void ShiftBrightness(double all, double red, double green, double blue, int dataStart = -1, int dataEnd = -1);
	void ScaleBrightness(double all, double red, double green, double blue, int dataStart = -1, int dataEnd = -1);
	void AdjustContrast(double all, double red, double green, double blue, int dataStart = -1, int dataEnd = -1);
	void ConvertGreyscale(double redScale, double greenScale, double blueScale, int dataStart = -1, int dataEnd = -1);
	void ChannelScale(double redRedScale, double redGreenScale, double redBlueScale,
		double greenRedScale, double greenGreenScale, double greenBlueScale,
		double blueRedScale, double blueGreenScale, double blueBlueScale, 
		int dataStart = -1, int dataEnd = -1);

	void Rotate90CW();
	void Rotate180();
	void RotateCustom(double angleDegrees, int crop = RotationCropping::KEEP_SIZE);
	void RotateCustomBilinear(double angleDegrees, int crop = RotationCropping::KEEP_SIZE);
	void RotateCustomBicubic(double angleDegrees, int crop = RotationCropping::KEEP_SIZE);
	int GetExpandedRotationWidth(double angleDegrees, int originalWidth, int originalHeight);
	int GetExpandedRotationHeight(double angleDegrees, int originalWidth, int originalHeight);
	int GetFittedRotationWidth(double angleDegrees, int originalWidth, int originalHeight);
	int GetFittedRotationHeight(double angleDegrees, int originalWidth, int originalHeight);

	void MirrorHorizontal();
	void MirrorVertical();

	void SendMessageToParent(wxString message);

	wxVector<ProcessorEdit*> editListInternal;

	class ProcessThread : public wxThread {

		public:
			ProcessThread(Processor * processor, ProcessorEdit * edit);
			ProcessThread(Processor * processor, wxVector<ProcessorEdit*> edits);

		protected:
			virtual ExitCode Entry();

		private:
			Processor * procParent;
			wxVector<ProcessorEdit*> editVec;
	};

};

#endif 
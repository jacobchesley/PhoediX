// Copyright 2016 Jacob Chesley

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
#include "Processing\ImageHandler\ImageHandler.h"
#include "Debugging\MemoryLeakCheck.h"
#include "Processing\ProcessorEdit\ProcessorEdit.h"
#include "libraw.h"
#include <omp.h>

enum {
	ID_PROCESSOR_MESSAGE,
	ID_PROCESSOR_NUM
};

wxDECLARE_EVENT(PROCESSOR_MESSAGE_EVENT, wxCommandEvent);
wxDECLARE_EVENT(PROCESSOR_NUM_EVENT, wxCommandEvent);

enum ColorSpaceENUM {
	ADOBE_RGB,
	PROPHOTO_RGB,
	sRGB,
	WIDE_GAMUT_RGB
};

struct RGB {
	float R;
	float G;
	float B;
};

struct XYZ {
	float X;
	float Y;
	float Z;
};

struct LAB {
	float L;
	float A;
	float B;
};

class Processor {
public:

	Processor(wxWindow * parent = NULL);
	~Processor();

	Image * GetImage();
	Image * GetTempImage();
	
	void ProcessEdits(wxVector<ProcessorEdit*> editList);
	void ProcessEdit(ProcessorEdit * edit);

	void AddEdit(ProcessorEdit * edit);
	void ProcessEdits();
	void DeleteEdits();
	wxVector<ProcessorEdit*> GetEditVector();

	void StoreEditForCopyPaste(ProcessorEdit * edit);
	ProcessorEdit * GetEditForCopyPaste();
	void StoreEditListForCopyPaste(wxVector<ProcessorEdit*>);
	wxVector<ProcessorEdit*> GetEditListForCopyPaste();

	void Enable16Bit();
	void Disable16Bit();

	void SetOriginalImage(Image * newOriginalImage);
	Image* GetOriginalImage();
	void RevertToOriginalImage(bool skipUpdate = false);

	void SetUpdated(bool updated);
	bool GetUpdated();
	bool RawImageGood();

	void LockRaw();
	void UnlockRaw();
	bool GetLockedRaw();

	void SetMultithread(bool doMultithread);
	bool GetMultithread();

	void SetNumThreads(int numThreads);
	int GetNumThreads();

	wxString GetFilePath();
	void SetFilePath(wxString path);

	wxString GetFileName();
	void SetFileName(wxString path);

	void UnpackAndProcessRaw();

	void Lock();
	void Unlock();
	bool GetLocked();

	void Get8BitHistrogram(uint32_t * outputHistogramRed, uint32_t * outputHistogramGreen, uint32_t * outputHistogramBlue, uint32_t * outputHistogramGrey);
	void Get16BitHistrogram(uint32_t * outputHistogramRed, uint32_t * outputHistogramGreen, uint32_t * outputHistogramBlue, uint32_t * outputHistogramGrey);

	void ProcessRaw();
	void DeleteRawImage();
	int GetRawError();

	void SetDoFitImage(bool fit);
	bool GetDoFitImage();
	
	void KillCurrentProcessing();

	int GetLastNumEdits();

	void SetHasEdits(bool doesHaveEdits);
	bool GetHasEdits();

	inline double FastTanH(double x);

	enum RotationCropping{
		KEEP_SIZE,
		FIT,
		EXPAND
	};

	enum BrightnessPreservation{
		SHADOWS,
		HIGHLIGHTS,
		SHADOWS_AND_HIGHLIGHTS
	};

	LibRaw rawPrcoessor;
	libraw_processed_image_t * rawImage;

protected:
	bool forceStop;

private:

	wxCriticalSection lockStopCritical;

	wxCriticalSection processingCritical;
	bool isProcessing;

	int numEdits;
	int numStartedThreads;

	bool rawImageGood;
	int rawErrorCode;
	wxString filePath;
	wxString fileName;

	Image * img;
	Image * originalImg;
	Image * tempImage;

	bool didUpdate;
	bool doFitImage;

	wxCriticalSection lockCritical;
	wxCriticalSection lockRawCritical;
	bool locked;
	bool lockedRaw;
	bool hasEdits;

	bool multiThread;
	int numThreadsToUse;

	ProcessorEdit * copiedEdit;
	wxVector<ProcessorEdit*> copiedEditList;

	static double pi;

	wxWindow * parWindow;

	void ShiftRGB(double all, double red, double green, double blue, int dataStart = -1, int dataEnd = -1);
	void AdjustHSL(double hShift, double sScale, double lScale, int dataStart = -1, int dataEnd = -1);
	void AdjustBrightness(double brightAdjust, double detailsPreserve, double toneSetting, int tonePreservation, int dataStart = -1, int dataEnd = -1);
	void AdjustContrast(double allContrast, double redContrast, double greenContrast, double blueContrast, 
		double allCenter, double redCenter, double greenCenter, double blueCenter, int dataStart = -1, int dataEnd = -1);
	void AdjustContrastCurve(double allContrast, double redContrast, double greenContrast, double blueContrast, 
		double allCenter, double redCenter, double greenCenter, double blueCenter, int dataStart = -1, int dataEnd = -1);
	void ConvertGreyscale(double redScale, double greenScale, double blueScale, int dataStart = -1, int dataEnd = -1);
	void ChannelScale(double redRedScale, double redGreenScale, double redBlueScale,
		double greenRedScale, double greenGreenScale, double greenBlueScale,
		double blueRedScale, double blueGreenScale, double blueBlueScale, 
		int dataStart = -1, int dataEnd = -1);

	void RGBCurves(int * brightCurve8, int * redCurve8, int * greenCurve8, int * blueCurve8,
		int * brightCurve16, int * redCurve16, int * greenCurve16, int * blueCurve16, int dataStart = -1, int dataEnd = -1);
	void LABCurves(int * lChannel16, int * aChannel16, int * bChannel16, int colorSpace, int dataStart = -1, int dataEnd = -1);
	void HSLCurves(int * hChannel16, int * sChannel16, int * lChannel16, int dataStart = -1, int dataEnd = -1);

	void SetupRotation(int editID, double angleDegrees, int crop);
	void CleanupRotation(int editID);
	void Rotate90CW(int dataStart = -1, int dataEnd = -1);
	void Rotate180(int dataStart = -1, int dataEnd = -1);
	void RotateCustom(double angleDegrees, int dataStart = -1, int dataEnd = -1);
	void RotateCustomBilinear(double angleDegrees, int dataStart = -1, int dataEnd = -1);
	void RotateCustomBicubic(double angleDegrees, int dataStart = -1, int dataEnd = -1);
	int GetExpandedRotationWidth(double angleDegrees, int originalWidth, int originalHeight);
	int GetExpandedRotationHeight(double angleDegrees, int originalWidth, int originalHeight);
	int GetFittedRotationWidth(double angleDegrees, int originalWidth, int originalHeight);
	int GetFittedRotationHeight(double angleDegrees, int originalWidth, int originalHeight);

	void SetupScale(int newWidth, int newHeight);
	void CleanupScale();
	void ScaleNearest(int dataStart = -1, int dataEnd = -1);
	void ScaleBilinear(int dataStart = -1, int dataEnd = -1);
	void ScaleBicubic(int dataStart = -1, int dataEnd = -1);

	void SetupCrop(int newWidth, int newHeight);
	void CleanupCrop();
	void Crop(int startPointX, int startPointY, int dataStart = -1, int dataEnd = -1);

	void MirrorHorizontal(int dataStart = -1, int dataEnd = -1);
	void MirrorVertical(int dataStart = -1, int dataEnd = -1);

	void RGBtoXYZ(RGB * rgb, XYZ * xyz, int colorSpace = sRGB);
	void XYZtoRGB(XYZ * xyz, RGB * rgb, int colorSpace = sRGB);
	void XYZtoLAB(XYZ * xyz, LAB * lab);
	void LABtoXYZ(LAB * lab, XYZ * xyz);

	void SendMessageToParent(wxString message);
	void SendProcessorEditNumToParent(int num);

	wxVector<ProcessorEdit*> editListInternal;
	
	class ProcessThread : public wxThread {

		public:
			ProcessThread(Processor * processor, ProcessorEdit * edit);
			ProcessThread(Processor * processor, wxVector<ProcessorEdit*> edits);
			void Terminate();

		protected:
			virtual ExitCode Entry();

		private:
			Processor * procParent;
			wxVector<ProcessorEdit*> editVec;
			bool terminated;
	};

	class RawProcessThread : public wxThread {

		public:
			RawProcessThread(Processor * processorPar, bool unpackAndProcess = false);

		protected:
			virtual ExitCode Entry();

		private:
			Processor * processor;
			bool unpackProcess;
	};

	ProcessThread* processThread;

};

#endif 
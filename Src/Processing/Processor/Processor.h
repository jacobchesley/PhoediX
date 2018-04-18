// Copyright 2016 Jacob Chesley

#ifndef PROCESSOR_H
#define PROCESSOR_H

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/thread.h"

#include "Processing/Image/Image.h"
#include "Processing/ImageHandler/ImageHandler.h"
#include "Debugging/MemoryLeakCheck.h"
#include "Processing/ProcessorEdit/ProcessorEdit.h"
#include "Processing/RawErrorCodes/RawError.h"
#include "libraw.h"

enum {
	ID_PROCESSOR_MESSAGE,
	ID_PROCESSOR_NUM,
	ID_RAW_COMPLETE,
	ID_UPDATE_IMAGE,
};

wxDECLARE_EVENT(PROCESSOR_MESSAGE_EVENT, wxCommandEvent);
wxDECLARE_EVENT(PROCESSOR_NUM_EVENT, wxCommandEvent);
wxDECLARE_EVENT(PROCESSOR_RAW_COMPLETE_EVENT, wxCommandEvent);
wxDECLARE_EVENT(UPDATE_IMAGE_EVENT, wxCommandEvent);

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
	int ProcessEdits();
	void DeleteEdits();
	wxVector<ProcessorEdit*> GetEditVector();

	void StoreEditForCopyPaste(ProcessorEdit * edit);
	ProcessorEdit * GetEditForCopyPaste();

	void StoreEditListForCopyPaste(wxVector<ProcessorEdit*>);
	wxVector<ProcessorEdit*> GetEditListForCopyPaste();

	void Enable16Bit();
	void Disable16Bit();

	void SetColorSpace(int newColorSpace);
	int GetColorSpace();

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

	void Lock();
	void Unlock();
	bool GetLocked();

	void EnableFastEdit();
	void DisableFastEdit();
	bool GetFastEdit();

	void Get8BitHistrogram(uint32_t * outputHistogramRed, uint32_t * outputHistogramGreen, uint32_t * outputHistogramBlue, uint32_t * outputHistogramGrey);
	void Get16BitHistrogram(uint32_t * outputHistogramRed, uint32_t * outputHistogramGreen, uint32_t * outputHistogramBlue, uint32_t * outputHistogramGrey);

	int ProcessRaw();
	int UnpackAndProcessRaw();
	void DeleteRawImage();
	int GetRawError();

	void SetDoFitImage(bool fit);
	bool GetDoFitImage();
	
	void KillCurrentProcessing();
	void KillRawProcessing();

	int GetLastNumEdits();

	void SetHasEdits(bool doesHaveEdits);
	bool GetHasEdits();

	void CalculateWidthHeightRotation(ProcessorEdit * rotationEdit, int origWidth, int origHeight, int * width, int * height);
	void CalcualteWidthHeightEdits(wxVector<ProcessorEdit*> edits, int * width, int * height);

	inline double FastTanH(double x);

	void SetParentWindow(wxWindow * parent);
	wxWindow * GetParentWindow();

	void ResetForceStop();
	void DestroyAll();

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
	static void * rawProcessorData;
	static enum LibRaw_progress rawProcessorProgress;
	static int rawIteration;
	static int rawExpected;

	libraw_processed_image_t * rawImage;
	
protected:

	static int RawCallback(void *data, enum LibRaw_progress p, int iteration, int expected);
	bool forceStop;
	wxCriticalSection forceStopCritical;
	static bool forceStopRaw;
	bool restartRaw;

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

	bool fastEdit; 

	static double pi;

	wxWindow * parWindow;
	int colorSpace;

	void AdjustRGB(double all, double red, double green, double blue, int dataStart = -1, int dataEnd = -1);
	void AdjustHSL(double hShift, double sScale, double lScale, double rScale, double gScale, double bScale, int dataStart = -1, int dataEnd = -1);
	void AdjustLAB(double lScale, double aShift, double bShift, double rScale, double gScale, double bScale, int dataStart = -1, int dataEnd = -1);
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
	void LABCurves(int * lChannel16, int * aChannel16, int * bChannel16, double redScale, double greenScale, double blueScale, int dataStart = -1, int dataEnd = -1);
	void HSLCurves(int * hChannel16, int * sChannel16, int * lChannel16, double rScale, double gScale, double bScale, int dataStart = -1, int dataEnd = -1);

	bool SetupRotation(int editID, double angleDegrees, int crop);
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

	bool SetupScale(int newWidth, int newHeight);
	void CleanupScale();
	void ScaleNearest(int dataStart = -1, int dataEnd = -1);
	void ScaleBilinear(int dataStart = -1, int dataEnd = -1);
	void ScaleBicubic(int dataStart = -1, int dataEnd = -1);

	void SetupCrop(double newWidth, double newHeight);
	void CleanupCrop();
	void Crop(double startPointX, double startPointY, int dataStart = -1, int dataEnd = -1);

	void MirrorHorizontal(int dataStart = -1, int dataEnd = -1);
	void MirrorVertical(int dataStart = -1, int dataEnd = -1);

	void SetupBlur();
	void CleanupBlur();
	void BoxBlurHorizontal(int pixelBlurSize, int dataStart = -1, int dataEnd = -1);
	void BoxBlurVertical(int pixelBlurSize, int dataStart = -1, int dataEnd = -1);

	void RGBtoXYZ(RGB * rgb, XYZ * xyz, int colorSpaceToUse);
	void XYZtoRGB(XYZ * xyz, RGB * rgb, int colorSpaceToUse);
	void XYZtoLAB(XYZ * xyz, LAB * lab);
	void LABtoXYZ(LAB * lab, XYZ * xyz);

	void SendMessageToParent(wxString message);
	void SendProcessorEditNumToParent(int num);
	void SendRawComplete();

	wxVector<ProcessorEdit*> editListInternal;
	wxCriticalSection editListCritical;

	class ProcessThread : public wxThread {

		public:
			ProcessThread(Processor * processor, ProcessorEdit * edit);
			ProcessThread(Processor * processor, wxVector<ProcessorEdit*> edits);
			void Terminate();

		protected:
			virtual ExitCode Entry();

		private:

			void DeleteEditVector();
			void Multithread(ProcessorEdit * edit, int maxDataSize = -1);
			Processor * processor;
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

	class EditThread : public wxThread {
		public:
			EditThread(Processor * processor, ProcessorEdit * edit, int dataStart, int dataEnd, wxMutex * mutLockIn, wxCondition * condition, int numThreads, int * threadsComplete);

		protected:
			virtual ExitCode Entry();

	private:
		Processor * processor;
		ProcessorEdit * procEdit;
		int start;
		int end;
		wxMutex * mutLock;
		wxCondition * cond;
		int threads;
		int * complete;
	};

	ProcessThread* processThread;
};

#endif 
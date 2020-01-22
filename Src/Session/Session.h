// Copyright 2018 Jacob Chesley.
// See LICENSE.TXT in root of project for license information.

#ifndef PHOEDIX_SESSION_H
#define PHOEDIX_SESSION_H

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/datetime.h"
#include "wx/wfstream.h"
#include "wx/xml/xml.h"

#include "SessionEditList/SessionEditList.h"
#include "Processing/Snapshot/Snapshot.h"

class PhoediXSession{

public:
	PhoediXSession();
	~PhoediXSession();
	void Destroy();

	void LoadSessionFromFile(wxString filePath);
	void SaveSessionToFile(wxString filePath);
	
	PhoediXSessionEditList *GetEditList();

	wxVector<Snapshot*> GetSnapshots();
	void SetSnapshots(wxVector<Snapshot*> snapshots);

	float GetImageZoomLevel();
	void SetImageZoomLevel(float zoom);

	int GetImageScrollX();
	void SetImageScrollX(int x);

	int GetImageScrollY();
	void SetImageScrollY(int y);

	void SetImageScrollWidth(int width);
	int GetImageScrollWidth();
	void SetImageScrollHeight(int height);
	int GetImageScrollHeight();

	void SetFitImage(bool fitImage);
	bool GetFitImage();

	int GetHistogramDisplaySelect();
	void SetHistogramDisplaySelect(int selection);

	wxString GetPerspective();
	void SetPerspective(wxString newPerspective);

	void SetFastProcess(bool fastProcess);
	bool GetFastProcess();

	void GenerateID();
	int GetID();

	void SetName(wxString newName);
	wxString GetName();

	void SetProjectPath(wxString path);
	wxString GetProjectPath();

	wxString GetExportFolder();
	void SetExportFolder(wxString exFolder);

	wxString GetExportName();
	void SetExportName(wxString exName);

	int GetExportImageType();
	void SetExportImageType(int type);

	int GetExportJPEGQuality();
	void SetExportJPEGQuality(int quality);

	static bool CompareSessions(PhoediXSession * sessionOne, PhoediXSession * sessionTwo);
	static bool CheckIfSession(wxString filePath);
	static wxString GetImagePathFromProjectPath(wxString projectFile);

	wxColour GetGuidelineColor();
	void SetGuidelineColor(wxColour color);

	wxColour GetCropgridColor1();
	void SetCropgridColor1(wxColour color);

	wxColour GetCropgridColor2();
	void SetCropgridColor2(wxColour color);

	int GetGuidelineType();
	void SetGuidelineType(int type);

	bool GetGuidelinesShown();
	void SetGuidelinesShown(bool guidelinesShown);

private:

	wxString projectPath;
	wxString imgFile;
	PhoediXSessionEditList *editList;
	wxVector <Snapshot*>  snapshotsList;
	float imgZoom;
	int imgScrollX;
	int imgScrollY;
	int imgWidth;
	int imgHeight;
	bool fitImage;
	bool processFast;
	wxString auiPerspective;
	int histogramDisplaySelect;
	int id;
	wxString exportFolder;
	wxString exportName;
	int exportImageType;
	int exportJPEGQuality;
	wxColour guideColor;
	wxColour gridColor1;
	wxColour gridColor2;
	int guideType;
	bool showGuidelines;

	wxString name;
};
#endif
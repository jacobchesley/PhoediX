// Copyright 2018 Jacob Chesley.
// See LICENSE.TXT in root of project for license information.

#include "SessionEditList.h"

#define EDIT_PRECISSION 6

PhoediXSessionEditList::PhoediXSessionEditList() {

}

PhoediXSessionEditList::~PhoediXSessionEditList() {

	for (size_t i = 0; i < editList.size(); i++) {
		delete editList[i];
	}
	editList.clear();
}

void PhoediXSessionEditList::SetSessionEditList(wxVector<ProcessorEdit*> newEditList) {

	for (size_t i = 0; i < editList.size(); i++) {
		delete editList[i];
	}

	editList.clear();
	for (size_t i = 0; i < newEditList.size(); i++) {
		editList.push_back(new ProcessorEdit(*newEditList.at(i)));
	}
}

wxVector<ProcessorEdit*> PhoediXSessionEditList::GetSessionEditList() {
	return editList;
}

void PhoediXSessionEditList::LoadSessionEditList(wxXmlNode * editListNode) {

	// Delete all edits in the internal vector
	for (size_t i = 0; i < editList.size(); i++) {
		editList.at(i)->ClearIntArray();
		editList.at(i)->ClearDoubleArray();
		editList.at(i)->ClearFlags();
		editList.at(i)->ClearParams();
		delete editList.at(i);
	}

	// Clear the vector
	editList.clear();

	wxXmlNode * editNode = editListNode->GetChildren();
	size_t numEdits = 0;
	while (editNode) {
		numEdits += 1;
		editNode = editNode->GetNext();
	}

	editList.resize(numEdits);

	editNode = editListNode->GetChildren();
	while (editNode) {

		
		wxXmlNode * editNodeChildren = editNode->GetChildren();

		ProcessorEdit * newEdit = new ProcessorEdit();
		size_t editIdx = 0;

		while (editNodeChildren) {

			
			// Get edit name and set edit type
			if (editNodeChildren->GetName() == "EditName") {
				if(editNodeChildren->GetChildren() != NULL){
					newEdit->SetEditTypeFromTag(editNodeChildren->GetChildren()[0].GetContent());
				}
			}

			// Get edit name and set edit type
			if (editNodeChildren->GetName() == "EditIndex") {
				if(editNodeChildren->GetChildren() != NULL){
					editIdx = wxAtoi(editNodeChildren->GetChildren()[0].GetContent());
				}
			}

			// Get enable / disable 
			if (editNodeChildren->GetName() == "Disabled") {
				if(editNodeChildren->GetChildren() != NULL){
					int dis = wxAtoi(editNodeChildren->GetChildren()[0].GetContent());
					if(dis > 0){ newEdit->SetDisabled(true); }
				}
			}

			// Get all parameters for the edit
			if (editNodeChildren->GetName() == "Parameters") {

				if(editNodeChildren->GetChildren() != NULL){
					wxXmlNode * editNodeParameters = editNodeChildren->GetChildren();
		
					// Get all parameters
					editNodeParameters = editNodeChildren->GetChildren();
					wxString paramName;
					double param;

					while (editNodeParameters) {

						// Get param name
						paramName = editNodeParameters->GetName();
						
						// Get param value
						param = wxAtof(editNodeParameters->GetChildren()[0].GetContent());

						//Add parameter to new edit
						newEdit->AddParam(paramName, param);

						// Get next parameter
						editNodeParameters = editNodeParameters->GetNext();
					}
				}
			}

			// Get all flags for the edit
			if (editNodeChildren->GetName() == "Flags") {

				if(editNodeChildren->GetChildren() != NULL){
					wxXmlNode * editNodeFlags = editNodeChildren->GetChildren();

					// Get all flags
					editNodeFlags = editNodeChildren->GetChildren();
					wxString flagName;
					int flag;

					while (editNodeFlags) {

						// Get flag name
						flagName = editNodeFlags->GetName();
	
						// Get flag value
						flag = wxAtoi(editNodeFlags->GetChildren()[0].GetContent());

						// Add flag to edit
						newEdit->AddFlag(flagName, flag);
						
						// Get next parameter
						editNodeFlags = editNodeFlags->GetNext();
					}
				}
			}
			
			// Get all double arrays for the edit
			if (editNodeChildren->GetName() == "DoubleArrays") {

				if(editNodeChildren->GetChildren() != NULL){
					wxXmlNode * editNodeDoubleArrays = editNodeChildren->GetChildren();
		
					// Get all double arrays
					editNodeDoubleArrays = editNodeChildren->GetChildren();
					wxString doubleArrayName;

					while (editNodeDoubleArrays) {

						// Get array name
						doubleArrayName = editNodeDoubleArrays->GetName();
						
						// Count number of elements in array
						if(!editNodeDoubleArrays->GetContent() != NULL && editNodeDoubleArrays->GetChildren() != NULL){
							wxString arrayStr = editNodeDoubleArrays->GetChildren()[0].GetContent();
							wxStringTokenizer arrayTokens(arrayStr, ",");
							size_t numElements = arrayTokens.CountTokens();

							// Create and populate double array
							double * newDoubleArray = new double[numElements];

							for (size_t i = 0; i < numElements; i++) {
								arrayTokens.GetNextToken().ToDouble(&newDoubleArray[i]);
							}

							// Add double array to edit
							newEdit->AddDoubleArray(doubleArrayName, newDoubleArray, numElements);

						}
						editNodeDoubleArrays = editNodeDoubleArrays->GetNext();
					}
				}
			}

			editNodeChildren = editNodeChildren->GetNext();
		}

		editList[editIdx] = newEdit;
		
		editNode = editNode->GetNext();
	}
}

void PhoediXSessionEditList::SaveSessionEditList(wxXmlNode * phoedixProjectNode){

	wxXmlNode * editListNode = new wxXmlNode(phoedixProjectNode, wxXML_ELEMENT_NODE, "EditList");

	wxString editIndexStr;
	// Go through each edit one by one.  Each of these will invoke at least 1 child thread for the edit itself
	for (size_t editIndex = 0; editIndex < editList.size(); editIndex++) {

		editIndexStr = "";
		editIndexStr << editIndex;
		// Get the next edit to take place
		ProcessorEdit * curEdit = editList.at(editIndex);

		// Add edit XML info
		wxXmlNode * editNode = new wxXmlNode(editListNode, wxXML_ELEMENT_NODE, "Edit");

		// Add index at which edit is located
		wxXmlNode * indexNode = new wxXmlNode(editNode, wxXML_ELEMENT_NODE, "EditIndex");
		indexNode->AddChild(new wxXmlNode(wxXML_TEXT_NODE, "", editIndexStr));

		// Add edit name to edit node
		wxXmlNode * nameNode = new wxXmlNode(editNode, wxXML_ELEMENT_NODE, "EditName");
		nameNode->AddChild(new wxXmlNode(wxXML_TEXT_NODE, "", curEdit->GetEditTag()));

		// Add enable / disable flag
		wxXmlNode * disableNode = new wxXmlNode(editNode, wxXML_ELEMENT_NODE, "Disabled");
		int dis = 0;
		if(curEdit->GetDisabled()) { dis = 1; }
		wxString disString;
		disString << dis;
		disableNode->AddChild(new wxXmlNode(wxXML_TEXT_NODE, "", disString));

		// Add parameters to edit node
		wxXmlNode * paramsNode = new wxXmlNode(editNode, wxXML_ELEMENT_NODE, "Parameters");
		wxString paramStr;

		wxVector<wxString> parameterNames = curEdit->GetParamterNames();
		for (size_t paramIndex = 0; paramIndex < parameterNames.size(); paramIndex++) {

			paramStr = wxNumberFormatter::ToString(curEdit->GetParam(parameterNames[paramIndex]), EDIT_PRECISSION, wxNumberFormatter::Style_None);
			wxXmlNode * paramNode = new wxXmlNode(paramsNode, wxXML_ELEMENT_NODE, parameterNames[paramIndex]);
			paramNode->AddChild(new wxXmlNode(wxXML_TEXT_NODE, "", paramStr));
		}

		// Add Flags to edit node
		wxXmlNode * flagsNode = new wxXmlNode(editNode, wxXML_ELEMENT_NODE, "Flags");
		wxString flagStr;

		wxVector<wxString> flagNames = curEdit->GetFlagNames();
		for (size_t flagIndex = 0; flagIndex < flagNames.size(); flagIndex++) {

			flagStr = "";
			flagStr << curEdit->GetFlag(flagNames[flagIndex]);

			wxXmlNode * flagNode = new wxXmlNode(flagsNode, wxXML_ELEMENT_NODE, flagNames[flagIndex]);
			flagNode->AddChild(new wxXmlNode(wxXML_TEXT_NODE, "", flagStr));
		}

		// Add double array to edit node
		wxXmlNode * doubleArrayNode = new wxXmlNode(editNode, wxXML_ELEMENT_NODE, "DoubleArrays");
		wxString doubleStrTemp;
		wxString doubleStr;

		wxVector<wxString> doubleArrayNames = curEdit->GetDoubleArrayNames();
		for (size_t doubleIndex = 0; doubleIndex < doubleArrayNames.size(); doubleIndex++) {

			doubleStr = "";

			for (size_t doubleNum = 0; doubleNum < curEdit->GetDoubleArraySize(doubleArrayNames[doubleIndex]); doubleNum++) {
				doubleStrTemp = "";
				doubleStrTemp = wxNumberFormatter::ToString(curEdit->GetDoubleArray(doubleArrayNames[doubleIndex])[doubleNum], EDIT_PRECISSION, wxNumberFormatter::Style_None);
				doubleStr += doubleStrTemp + ",";
			}
			
			// remove trailing comma
			doubleStr = doubleStr.substr(0, doubleStr.size() - 1);

			wxXmlNode * doubleNode = new wxXmlNode(doubleArrayNode, wxXML_ELEMENT_NODE, doubleArrayNames[doubleIndex]);
			doubleNode->AddChild(new wxXmlNode(wxXML_TEXT_NODE, "", doubleStr));
		}
	}
}
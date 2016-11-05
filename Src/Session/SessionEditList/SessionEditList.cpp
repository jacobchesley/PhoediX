// Copyright 2016 Jacob Chesley

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

					// Count number of parameters
					size_t numParams = 0;
					wxVector<double> params;
					while (editNodeParameters) {
						numParams += 1;
						editNodeParameters = editNodeParameters->GetNext();
					}

					// Resize the parameters vector to account for all parameters
					params.resize(numParams);

					// Get all parameters and place in correct order
					editNodeParameters = editNodeChildren->GetChildren();
					wxString editParamNumStr;
					size_t paramIdx = 0;
					double param;
					while (editNodeParameters) {

						// Get param number
						editParamNumStr = editNodeParameters->GetName();
						editParamNumStr = editParamNumStr.SubString(5, editParamNumStr.length());
						paramIdx = wxAtoi(editParamNumStr);

						// Get param value
						param = wxAtof(editNodeParameters->GetChildren()[0].GetContent());

						// Add to param vector
						params[paramIdx] = param;

						// Get next parameter
						editNodeParameters = editNodeParameters->GetNext();
					}

					// Add parameters to new edit
					for (size_t i = 0; i < numParams; i++) {
						newEdit->AddParam(params.at(i));
					}
				}
			}

			// Get all flags for the edit
			if (editNodeChildren->GetName() == "Flags") {

				if(editNodeChildren->GetChildren() != NULL){
					wxXmlNode * editNodeFlags = editNodeChildren->GetChildren();

					// Count number of flags
					size_t numFlags = 0;
					wxVector<double> flags;
					while (editNodeFlags) {
						numFlags += 1;
						editNodeFlags = editNodeFlags->GetNext();
					}

					// Resize the flags vector to account for all parameters
					flags.resize(numFlags);

					// Get all flags and place in correct order
					editNodeFlags = editNodeChildren->GetChildren();
					wxString editFlagNumStr;
					size_t flagIdx = 0;
					int flag;
					while (editNodeFlags) {

						// Get flag number
						editFlagNumStr = editNodeFlags->GetName();
						editFlagNumStr = editFlagNumStr.SubString(4, editFlagNumStr.length());
						flagIdx = wxAtoi(editFlagNumStr);

						// Get param value
						flag = wxAtoi(editNodeFlags->GetChildren()[0].GetContent());

						// Add to param vector
						flags[flagIdx] = flag;

						// Get next parameter
						editNodeFlags = editNodeFlags->GetNext();
					}

					// Add flags to new edit
					for (size_t i = 0; i < numFlags; i++) {
						newEdit->AddFlag(flags.at(i));
					}
				}
			}
			
			// Get all double arrays for the edit
			if (editNodeChildren->GetName() == "DoubleArrays") {

				if(editNodeChildren->GetChildren() != NULL){
					wxXmlNode * editNodeDoubleArrays = editNodeChildren->GetChildren();
					wxVector<double*> doubleArrays;
					wxVector<int> doubleArraySizes;

					// Count number of double arrays
					size_t numDoubleArrays = 0;
					while (editNodeDoubleArrays) {
						numDoubleArrays += 1;
						editNodeDoubleArrays = editNodeDoubleArrays->GetNext();
					}

					// Resize the double arrays vector to account for all arrays
					doubleArrays.resize(numDoubleArrays);
					doubleArraySizes.resize(numDoubleArrays);

					// Get all double arrays and place in correct order
					editNodeDoubleArrays = editNodeChildren->GetChildren();
					wxString editDoubleArrayNumStr;
					size_t arrayIdx = 0;
					while (editNodeDoubleArrays) {

						// Get array number
						editDoubleArrayNumStr = editNodeDoubleArrays->GetName();
						editDoubleArrayNumStr = editDoubleArrayNumStr.SubString(11, editDoubleArrayNumStr.length());
						arrayIdx = wxAtoi(editDoubleArrayNumStr);

						// Count number of elements in array
						if(!editNodeDoubleArrays->GetContent() != NULL){
							wxString arrayStr = editNodeDoubleArrays->GetChildren()[0].GetContent();
							wxStringTokenizer arrayTokens(arrayStr, ",");
							size_t numElements = arrayTokens.CountTokens();

							// Create and populate double array
							double * newDoubleArray = new double[numElements];
							doubleArraySizes[arrayIdx] = numElements;
							for (size_t i = 0; i < numElements; i++) {
								arrayTokens.GetNextToken().ToDouble(&newDoubleArray[i]);
							}

							// Set double array element in vector
							doubleArrays[arrayIdx] = newDoubleArray;

						}
						editNodeDoubleArrays = editNodeDoubleArrays->GetNext();
					}

					// Add double arrays to new edit
					for (size_t i = 0; i < numDoubleArrays; i++) {
						newEdit->AddDoubleArray(doubleArrays[i], doubleArraySizes[i]);
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
		wxString paramIdx;
		wxString paramStr;
		for (int paramIndex = 0; paramIndex < curEdit->GetParamsSize(); paramIndex++) {

			paramIdx = "";
			paramStr = "";
			paramIdx << paramIndex;
			paramStr = wxNumberFormatter::ToString(curEdit->GetParam(paramIndex), EDIT_PRECISSION, wxNumberFormatter::Style_None);

			wxXmlNode * paramNode = new wxXmlNode(paramsNode, wxXML_ELEMENT_NODE, "Param" + paramIdx);
			paramNode->AddChild(new wxXmlNode(wxXML_TEXT_NODE, "", paramStr));
		}

		// Add Flags to edit node
		wxXmlNode * flagsNode = new wxXmlNode(editNode, wxXML_ELEMENT_NODE, "Flags");
		wxString flagIdx;
		wxString flagStr;
		for (int flagIndex = 0; flagIndex < curEdit->GetFlagsSize(); flagIndex++) {

			flagIdx = "";
			flagStr = "";
			flagIdx << flagIndex;
			flagStr << curEdit->GetFlag(flagIndex);

			wxXmlNode * flagNode = new wxXmlNode(flagsNode, wxXML_ELEMENT_NODE, "Flag" + flagIdx);
			flagNode->AddChild(new wxXmlNode(wxXML_TEXT_NODE, "", flagStr));
		}

		// Add double array to edit node
		wxXmlNode * doubleArrayNode = new wxXmlNode(editNode, wxXML_ELEMENT_NODE, "DoubleArrays");
		wxString doubleIdx;
		wxString doubleStrTemp;
		wxString doubleStr;
		for (int doubleIndex = 0; doubleIndex < curEdit->GetNumDoubleArrays(); doubleIndex++) {

			doubleIdx = "";
			doubleStr = "";
			doubleIdx << doubleIndex;

			for (int doubleNum = 0; doubleNum < curEdit->GetDoubleArraySize(doubleIndex); doubleNum++) {
				doubleStrTemp = "";
				doubleStrTemp = wxNumberFormatter::ToString(curEdit->GetDoubleArray(doubleIndex)[doubleNum], EDIT_PRECISSION, wxNumberFormatter::Style_None);
				doubleStr += doubleStrTemp + ",";
			}
			
			// remove trailing comma
			doubleStr = doubleStr.substr(0, doubleStr.size() - 1);

			wxXmlNode * doubleNode = new wxXmlNode(doubleArrayNode, wxXML_ELEMENT_NODE, "DoubleArray" + doubleIdx);
			doubleNode->AddChild(new wxXmlNode(wxXML_TEXT_NODE, "", doubleStr));
		}
	}
}
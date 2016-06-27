#include "SessionEditList.h"

PhoediXSessionEditList::PhoediXSessionEditList() {

}

void PhoediXSessionEditList::SetSessionEditList(wxVector<ProcessorEdit*> newEditList) {

	editList.clear();
	for (size_t i = 0; i < newEditList.size(); i++) {
		editList.push_back(newEditList.at(i));
	}
}

wxVector<ProcessorEdit*> PhoediXSessionEditList::GetSessionEditList() {
	return editList;
}

void PhoediXSessionEditList::LoadSessionEditList(wxXmlNode * editListNode) {

	// Delete all edits in the internal vector
	for (size_t i = 0; i < editList.size(); i++) {
		editList.at(i)->ClearIntArray();
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
				newEdit->SetEditTypeFromTag(editNodeChildren->GetChildren()[0].GetContent());
			}

			// Get edit name and set edit type
			if (editNodeChildren->GetName() == "EditIndex") {
				editIdx = wxAtoi(editNodeChildren->GetChildren()[0].GetContent());
			}

			// Get all parameters for the edit
			if (editNodeChildren->GetName() == "Parameters") {

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

			// Get all flags for the edit
			if (editNodeChildren->GetName() == "Flags") {

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

				// Get all parameters and place in correct order
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

				// Add parameters to new edit
				for (size_t i = 0; i < numFlags; i++) {
					newEdit->AddFlag(flags.at(i));
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

	int size = editList.size();
	int test = 0;
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

		// Add parameters to edit node
		wxXmlNode * paramsNode = new wxXmlNode(editNode, wxXML_ELEMENT_NODE, "Parameters");
		wxString paramIdx;
		wxString paramStr;
		for (size_t paramIndex = 0; paramIndex < curEdit->GetParamsSize(); paramIndex++) {

			paramIdx = "";
			paramStr = "";
			paramIdx << paramIndex;
			paramStr << curEdit->GetParam(paramIndex);

			wxXmlNode * paramNode = new wxXmlNode(paramsNode, wxXML_ELEMENT_NODE, "Param" + paramIdx);
			paramNode->AddChild(new wxXmlNode(wxXML_TEXT_NODE, "", paramStr));
		}

		// Add Flags to edit node
		wxXmlNode * flagsNode = new wxXmlNode(editNode, wxXML_ELEMENT_NODE, "Flags");
		wxString flagIdx;
		wxString flagStr;
		for (size_t flagIndex = 0; flagIndex < curEdit->GetFlagsSize(); flagIndex++) {

			flagIdx = "";
			flagStr = "";
			flagIdx << flagIndex;
			flagStr << curEdit->GetFlag(flagIndex);

			wxXmlNode * flagNode = new wxXmlNode(flagsNode, wxXML_ELEMENT_NODE, "Flag" + flagIdx);
			flagNode->AddChild(new wxXmlNode(wxXML_TEXT_NODE, "", flagStr));
		}
	}
}
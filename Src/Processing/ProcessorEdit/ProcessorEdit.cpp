#include "ProcessorEdit.h"

ProcessorEdit::ProcessorEdit() {
	doMultithread = false;
	numThread = 0;
}

ProcessorEdit::ProcessorEdit(int editType, bool multiThead, int numThreads) {
	edit = editType;
	doMultithread = multiThead;
	numThread = numThreads;
}

void ProcessorEdit::AddParam(double param) {
	params.push_back(param);
}

void ProcessorEdit::AddIntArray(int * newArray, int arraySize) {
	intArrays.push_back(newArray);
	intArraySizes.push_back(arraySize);
}

void ProcessorEdit::ClearParams() {
	params.clear();
}

void ProcessorEdit::ClearIntArray() {
	for (size_t i = 0; i < intArrays.size(); i++) {
		delete[] intArrays[i];
	}
	intArrays.clear();
}

int ProcessorEdit::GetParamsSize() {
	return params.size();
}

int ProcessorEdit::GetIntArraySize() {
	return intArrays.size();
}

double ProcessorEdit::GetParam(size_t index) {
	return params.at(index);
}

int * ProcessorEdit::GetIntArray(size_t index) {
	return intArrays.at(index);
}

int ProcessorEdit::GetIntArraySize(size_t index) {
	return intArraySizes.at(index);
}

int ProcessorEdit::GetNumIntArrays() {
	return intArraySizes.size();
}


void ProcessorEdit::AddFlag(int flag) {
	flags.push_back(flag);
}

void ProcessorEdit::ClearFlags() {
	flags.clear();
}

int ProcessorEdit::GetFlagsSize() {
	return flags.size();
}

double ProcessorEdit::GetFlag(size_t index) {
	return flags.at(index);
}

int ProcessorEdit::GetEditType() {
	return edit;
}

void ProcessorEdit::SetEditType(int editType) {
	edit = editType;
}

bool ProcessorEdit::GetMultithread() {
	return doMultithread;
}

void ProcessorEdit::SetMultithread(bool multiThread) {
	doMultithread = multiThread;
}

int ProcessorEdit::GetNumThreads(){
	return numThread;
}

void ProcessorEdit::SetNumThreads(int numThreads) {
	numThread = numThreads;
}
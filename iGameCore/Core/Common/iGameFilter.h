#ifndef iGameFilter_h
#define iGameFilter_h

#include "iGameObject.h"
#include "iGameDataObject.h"

IGAME_NAMESPACE_BEGIN
class Filter : public Object {
public:
	I_OBJECT(Filter);
	static Pointer New() { return new Filter; }
	using DataObjectArray = ElementArray<DataObject::Pointer>;
	
	void Update();

	void SetNumberOfInputs(int n);
	void SetNumberOfOutputs(int n);
	int GetNumberOfInputs() const;
	int GetNumberOfOutputs() const;

	bool SetInput(int index, DataObject::Pointer data);
	bool SetInput(DataObject::Pointer data);
	DataObject::Pointer GetInput(int index);
	DataObject::Pointer GetNamedInput(const std::string& name);


	bool SetOutput(int index, DataObject::Pointer data);
	bool SetOutput(DataObject::Pointer data);
	DataObject::Pointer GetOutput(int index);
	DataObject::Pointer GetOutput();
	DataObject::Pointer GetNamedOutput(const std::string& name);

	void UpdateProgress(double amount);
	void ResetProgressShiftScale();
	
protected:
	Filter();
	~Filter() override = default;

	virtual bool Execute() { return true; }

	DataObjectArray::Pointer m_Inputs;
	DataObjectArray::Pointer m_Outputs;

	double Progress{ 0.0 };
	double ProgressShift{ 0.0 };
	double ProgressScale{ 1.0 };
};
IGAME_NAMESPACE_END
#endif
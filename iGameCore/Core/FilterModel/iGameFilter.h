#ifndef iGameFilter_h
#define iGameFilter_h

#include "iGameObject.h"
#include "iGameDataObject.h"

IGAME_NAMESPACE_BEGIN
class Filter : public Object {
public:
	I_OBJECT(Filter);
	static Pointer New() { return new Filter; }

	struct Input {
		int Port{};
		std::string Name{};
		DataObjectType Type{};
		DataObject::Pointer Pointer{};
	};

	struct Output {
		int Port{};
		std::string Name{};
		DataObjectType Type{};
		DataObject::Pointer Pointer{};
	};

	void Update();

	int GetNumberOfInputPort() const;
	int GetNumberOfOutputPort() const;


	bool SetInput(int port, DataObject::Pointer data);
	bool SetInput(DataObject::Pointer data);
	DataObject::Pointer GetInput(int port);
	DataObject::Pointer GetInputByName(const std::string& name);



	bool SetOutput(int port, DataObject::Pointer data);
	bool SetOutput(DataObject::Pointer data);
	DataObject::Pointer GetOutput(int port);
	DataObject::Pointer GetOutput();
	DataObject::Pointer GetOutputByName(const std::string& name);

	
	Output& GetOutputPort(int port);
	const Output& GetOutputPort(int port) const;

	void UpdateProgress(double amount);
	void ResetProgressShiftScale();
	
protected:
	Filter() = default;
	~Filter() = default;

	virtual bool Execute() {}

	void SetNumberOfInputPort(int n);
	void SetNumberOfOutputPort(int n);

	bool SetInputPort(int port, DataObjectType type);
	bool SetOutputPort(int port, DataObjectType type);

	bool CheckInputPort(int port);
	bool CheckOutputPort(int port);

	std::vector<Input> InputPort{};
	std::vector<Output> OutputPort{};

	double Progress{ 0.0 };
	double ProgressShift{ 0.0 };
	double ProgressScale{ 1.0 };
};
IGAME_NAMESPACE_END
#endif
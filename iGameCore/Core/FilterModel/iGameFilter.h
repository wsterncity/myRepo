#ifndef iGameFilter_h
#define iGameFilter_h

#include "iGameObject.h"
#include "iGameDataObject.h"
#include "iGameController.h"
#include "iGameStandardController.h"

IGAME_NAMESPACE_BEGIN
//class Controller;
class Filter : public Object {
public:
	I_OBJECT(Filter);
	using ControllerPointer = SmartPointer<Controller>;
	using DataObjectPointer = DataObject::Pointer;
	using DataObjectType = IGenum;

	static Pointer New() { return new Filter; }

	struct Input {
		int Port{};
		std::string Name{};
		DataObjectType Type{};
		DataObjectPointer Pointer{};
	};

	struct Output {
		int Port{};
		std::string Name{};
		DataObjectType Type{};
		DataObjectPointer Pointer{};
	};

	bool HasController();
	void SetController(ControllerPointer newController);
	ControllerPointer GetController();
	ControllerPointer CreateDefaultController();

	virtual bool ProcessSignal(unsigned long);

	void Update();

	int GetNumberOfInputPort() const;
	int GetNumberOfOutputPort() const;


	bool SetInput(int port, DataObjectPointer data);
	bool SetInput(DataObjectPointer data);
	DataObjectPointer GetInput(int port);
	DataObjectPointer GetInputByName(const std::string& name);



	bool SetOutput(int port, DataObjectPointer data);
	bool SetOutput(DataObjectPointer data);
	DataObjectPointer GetOutput(int port);
	DataObjectPointer GetOutput();
	DataObjectPointer GetOutputByName(const std::string& name);

	
	Output& GetOutputPort(int port);
	const Output& GetOutputPort(int port) const;

	void UpdateProgress(double amount);
	void ResetProgressShiftScale();
	
protected:
	Filter() = default;
	~Filter() = default;

	void SetNumberOfInputPort(int n);
	void SetNumberOfOutputPort(int n);

	bool SetInputPort(int port, DataObjectType type);
	bool SetOutputPort(int port, DataObjectType type);

	bool CheckInputPort(int port);
	bool CheckOutputPort(int port);

	ControllerPointer Controller{};

	std::vector<Input> InputPort{};
	std::vector<Output> OutputPort{};

	double Progress{ 0.0 };
	double ProgressShift{ 0.0 };
	double ProgressScale{ 1.0 };

	friend class Controller;
};
IGAME_NAMESPACE_END
#endif
#ifndef iGameController_h
#define iGameController_h

#include "iGameObject.h"
#include "iGameDataObject.h"

IGAME_NAMESPACE_BEGIN
class Filter;
class Controller : public Object {
public:
	I_OBJECT(Controller);
	using FilterPointer = SmartPointer<Filter>;
	using DataObjectPointer = DataObject::Pointer;

	static Pointer New() { return new Controller; }

	void SetFilter(FilterPointer newFilter);
	FilterPointer GetFilter();
	
	enum {
		SIGNAL_DATA = 0,
		SIGNAL_DATA_OBJECT,
	};

	virtual bool EmitSignal(unsigned long);

	virtual bool Update();

	int GetNumberOfInputPorts();
	int GetNumberOfOutputPorts();
	
protected:
	Controller() = default;
	~Controller() = default;

	DataObjectPointer GetOuput(int port);
	void SetOuput(int port, DataObjectPointer data);
	bool ResizeNumberOfInputPorts(int size);

	FilterPointer m_Filter;
	int InProcessing{ 1 };
};
IGAME_NAMESPACE_END
#endif
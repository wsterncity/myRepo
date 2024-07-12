#include "MyFilter.h"
#include "iGameStandardController.h"

IGAME_NAMESPACE_BEGIN
bool MyFilter::ProcessSignal(unsigned long event)
{
	if (event == Controller::SIGNAL_DATA_OBJECT)
	{
		this->ProcessDataObject();
	}
	if (event == Controller::SIGNAL_DATA)
	{
		this->ProcessData();
	}
	
	return this->Filter::ProcessSignal(event);
}

MyFilter::MyFilter()
{
	this->SetNumberOfInputPort(0);
	this->SetNumberOfOutputPort(1);
}

bool MyFilter::ProcessDataObject()
{
	//this->SetOutputPort(0, DataObject::SOURCE_DATA);

	std::cout << "Process Data Object" << std::endl;

	return true;
}

bool MyFilter::ProcessData()
{
	DataObject::Pointer output = this->GetOutput(0);

	std::cout << "Process Data" << std::endl;

	return true;
}
IGAME_NAMESPACE_END
#include "iGameController.h"
#include "iGameFilter.h"

IGAME_NAMESPACE_BEGIN
void Controller::SetFilter(FilterPointer newFilter)
{
    FilterPointer oldController = this->m_Filter;
    if (oldController != newFilter)
    {
        this->m_Filter = newFilter;
    }
}

Filter::Pointer Controller::GetFilter()
{
	return this->m_Filter;
}

bool Controller::EmitSignal(unsigned long e)
{
    return false;
}

bool Controller::Update()
{
    return false;
}

int Controller::GetNumberOfInputPorts()
{
    return this->GetFilter()->GetNumberOfInputPort();
}

int Controller::GetNumberOfOutputPorts()
{
    return this->GetFilter()->GetNumberOfOutputPort();
}

DataObject::Pointer Controller::GetOuput(int port)
{
    return this->GetFilter()->GetOutput(port);
}

void Controller::SetOuput(int port, DataObjectPointer data)
{
    this->GetFilter()->SetOutput(port, data);
}

bool Controller::ResizeNumberOfInputPorts(int size)
{
    int newSize = size + this->GetNumberOfInputPorts();
    this->GetFilter()->SetNumberOfInputPort(newSize);
    return true;
}
IGAME_NAMESPACE_END
#include "iGameFilter.h"
#include <cassert>

IGAME_NAMESPACE_BEGIN
void Filter::Update()
{
    this->Execute();
}

int Filter::GetNumberOfInputPort() const
{
    return this->InputPort.size();
}

int Filter::GetNumberOfOutputPort() const
{
    return this->OutputPort.size();
}

bool Filter::SetInput(int port, DataObject::Pointer data)
{
    if (this->GetNumberOfInputPort() <= port)
    {
        return false;
    }
    this->InputPort[port].Pointer = data;
    return true;
}

bool Filter::SetInput(DataObject::Pointer data)
{
    return this->SetInput(0, data);
}

DataObject::Pointer Filter::GetInput(int port)
{
    if (!this->CheckInputPort(port)) 
    {
        return nullptr;
    }
    
    return this->InputPort[port].Pointer;
}

DataObject::Pointer Filter::GetInputByName(const std::string& name)
{
    for (int i = 0; i < this->GetNumberOfInputPort(); i++) {
        if (name == this->InputPort[i].Name) {
            return this->InputPort[i].Pointer;
        }
    }
    return nullptr;
}

bool Filter::SetOutput(int port, DataObject::Pointer data)
{
    if (this->GetNumberOfOutputPort() <= port)
    {
        return false;
    }
    this->OutputPort[port].Pointer = data;
    return true;
}

bool Filter::SetOutput(DataObject::Pointer data)
{
    return this->SetOutput(0, data);
}

DataObject::Pointer Filter::GetOutput(int port)
{
    if (!this->CheckOutputPort(port)) 
    {
        return nullptr;
    }

    return this->OutputPort[port].Pointer;
}

DataObject::Pointer Filter::GetOutput()
{
    return this->GetOutput(0);
}

DataObject::Pointer Filter::GetOutputByName(const std::string& name)
{
    for (int i = 0; i < this->GetNumberOfOutputPort(); i++) {
        if (name == this->OutputPort[i].Name) {
            return this->OutputPort[i].Pointer;
        }
    }
    return nullptr;
}

Filter::Output& Filter::GetOutputPort(int port)
{
    assert(port < this->GetNumberOfOutputPort());
    return this->OutputPort[port];
}

const Filter::Output& Filter::GetOutputPort(int port) const
{
    assert(port < this->GetNumberOfOutputPort());
    return this->OutputPort[port];
}

void Filter::UpdateProgress(double amount)
{
    amount = this->ProgressShift + this->ProgressScale * amount;

    amount = std::min(1.0, amount);
    amount = std::max(0.0, amount);

    this->Progress = amount;
    //if (this->ProgressObserver)
    //{
    //    this->ProgressObserver->UpdateProgress(amount);
    //}
}

void Filter::ResetProgressShiftScale()
{
    this->ProgressShift = this->Progress;
    this->ProgressScale = 1 - this->Progress;
}

void Filter::SetNumberOfInputPort(int n)
{
    this->InputPort.resize(n);
}

void Filter::SetNumberOfOutputPort(int n)
{
    this->OutputPort.resize(n);
}

bool Filter::SetInputPort(int port, DataObjectType type)
{
    if (this->GetNumberOfInputPort() <= port)
    {
        return false;
    }
    this->InputPort[port].Port = port;
    this->InputPort[port].Type = type;
    return true;
};

bool Filter::SetOutputPort(int port, DataObjectType type)
{
    if (this->GetNumberOfOutputPort() <= port)
    {
        return false;
    }
    this->OutputPort[port].Port = port;
    this->OutputPort[port].Type = type;
    return true;
}
bool Filter::CheckInputPort(int port)
{
    return port < GetNumberOfInputPort() && this->InputPort[port].Pointer != nullptr;
}
bool Filter::CheckOutputPort(int port)
{
    return port < GetNumberOfOutputPort() && this->OutputPort[port].Pointer != nullptr;
}
IGAME_NAMESPACE_END
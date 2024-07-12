#include "iGameStreamingController.h"
#include "iGameFilter.h"
IGAME_NAMESPACE_BEGIN
bool StreamingController::EmitSignal(unsigned long e)
{
	this->InProcessing = 1;
	int result = this->m_Filter->ProcessSignal(e);
	this->InProcessing = 0;

	return result;
}

bool StreamingController::Update()
{
	if (!this->UpdateDataObject())
	{
		return false;
	}
	return this->UpdateData();
}

bool StreamingController::UpdateDataObject()
{
	bool result = this->EmitSignal(SIGNAL_DATA_OBJECT);
	for (int i = 0; result && i < this->GetNumberOfOutputPorts(); ++i)
	{
		auto& output = this->GetFilter()->GetOutputPort(i);
		if (!output.Pointer)
		{
			DataObjectPointer data = DataObject::CreateDataObject(output.Type);
			if (!data)
			{
				result = false;
			}
			output.Pointer = data;
		}
		
		if (output.Pointer->GetDataObjectType() != output.Type)
		{
			result = false;
		}
	}
	return result;
}

bool StreamingController::UpdateData()
{
	return this->EmitSignal(SIGNAL_DATA);
}
IGAME_NAMESPACE_END
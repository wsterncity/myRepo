#ifndef iGameProgressObserver_h
#define iGameProgressObserver_h

#include "iGameObject.h"
#include "iGameCommand.h"

IGAME_NAMESPACE_BEGIN
class ProgressObserver : public Object {
public:
    I_OBJECT(ProgressObserver)
    static ProgressObserver* Instance() { 
		static ProgressObserver ins;
        return &ins;
	}

	void UpdateProgress(double amount) {
        this->m_Progress = amount;
		this->InvokeEvent(Command::ProgressEvent, static_cast<void*>(&amount));
	}

protected:
    ProgressObserver(){};
    ~ProgressObserver() override = default;

	double m_Progress{ 0.0 };
     
};
IGAME_NAMESPACE_END
#endif
#ifndef iGameAbstractObject_h
#define iGameAbstractObject_h

#include "iGameSmartPointer.h"
#include "iGameMacro.h"
#include "iGameType.h"

IGAME_NAMESPACE_BEGIN
class AbstractObject {
public:
	I_OBJECT(AbstractObject);

	virtual void Delete() const
	{
		m_ReferenceCount = 0;
		delete this;
	}

	virtual void Register() const
	{
		++m_ReferenceCount;
	}

	virtual void UnRegister() const noexcept
	{
		if (--m_ReferenceCount <= 0)
		{
			delete this;
		}
	}

protected:
	AbstractObject() {}
	virtual ~AbstractObject() {};

	mutable std::atomic<int> m_ReferenceCount{};
};
IGAME_NAMESPACE_END
#endif
#include "iGameObject.h"
#include "iGameCommand.h"

IGAME_NAMESPACE_BEGIN

class Observer {
public:
	Observer() {}
	~Observer() {}

	Command::Pointer Command{};
	unsigned long Event{ 0 };
	unsigned long Tag{ 0 };
	Observer* Next{ nullptr };
	float Priority{ 0.0 };
};

class ObserverInternal {
public:
	ObserverInternal() {}
	~ObserverInternal() { this->RemoveAllObservers(); }

	unsigned long AddObserver(unsigned long event, Command::Pointer cmd, float p) {
		Observer* elem = new Observer;
		elem->Priority = p;
		elem->Next = nullptr;
		elem->Event = event;
		elem->Command = cmd;
		elem->Tag = this->Count;
		this->Count++;


		if (!this->Start)
		{
			this->Start = elem;
		}
		else
		{
			// Insert sort, from highest to lowest priority
			Observer* prev = nullptr;
			Observer* pos = this->Start;
			while (pos->Priority >= elem->Priority && pos->Next)
			{
				prev = pos;
				pos = pos->Next;
			}

			if (pos->Priority > elem->Priority)
			{
				pos->Next = elem;
			}
			else
			{
				if (prev)
				{
					prev->Next = elem;
				}
				elem->Next = pos;

				if (pos == this->Start)
				{
					this->Start = elem;
				}
			}
		}
		return elem->Tag;
	}

	void RemoveObserver(unsigned long tag)
	{
		Observer* elem;
		Observer* prev;
		Observer* next;

		elem = this->Start;
		prev = nullptr;
		while (elem)
		{
			if (elem->Tag == tag)
			{
				if (prev)
				{
					prev->Next = elem->Next;
					next = prev->Next;
				}
				else
				{
					this->Start = elem->Next;
					next = this->Start;
				}
				delete elem;
				elem = next;
			}
			else
			{
				prev = elem;
				elem = elem->Next;
			}
		}
	}
	void RemoveObservers(unsigned long event)
	{
		Observer* elem;
		Observer* prev;
		Observer* next;

		elem = this->Start;
		prev = nullptr;
		while (elem)
		{
			if (elem->Event == event)
			{
				if (prev)
				{
					prev->Next = elem->Next;
					next = prev->Next;
				}
				else
				{
					this->Start = elem->Next;
					next = this->Start;
				}
				delete elem;
				elem = next;
			}
			else
			{
				prev = elem;
				elem = elem->Next;
			}
		}
	}
	void RemoveObservers(unsigned long event, Command::Pointer cmd)
	{
		Observer* elem;
		Observer* prev;
		Observer* next;

		elem = this->Start;
		prev = nullptr;
		while (elem)
		{
			if (elem->Event == event && elem->Command == cmd)
			{
				if (prev)
				{
					prev->Next = elem->Next;
					next = prev->Next;
				}
				else
				{
					this->Start = elem->Next;
					next = this->Start;
				}
				delete elem;
				elem = next;
			}
			else
			{
				prev = elem;
				elem = elem->Next;
			}
		}
	}
	void RemoveAllObservers()
	{
		Observer* elem = this->Start;
		Observer* next;
		while (elem)
		{
			next = elem->Next;
			delete elem;
			elem = next;
		}
		this->Start = nullptr;
	}

	bool InvokeEvent(unsigned long event, void* callData, Object* self)
	{
		Observer* elem = this->Start;
		Observer* next;

		while (elem)
		{
			next = elem->Next;
			if (elem->Event == event && elem->Tag < this->Count)
			{
				Command::Pointer command = elem->Command;
				elem->Command->Execute(self, event, callData);
			}

			elem = next;
		}

		return 0;
	}

	Command::Pointer GetCommand(unsigned long tag)
	{
		Observer* elem = this->Start;
		while (elem)
		{
			if (elem->Tag == tag)
			{
				return elem->Command;
			}
			elem = elem->Next;
		}
		return nullptr;
	}

	unsigned long GetTag(Command::Pointer cmd)
	{
		Observer* elem = this->Start;
		while (elem)
		{
			if (elem->Command == cmd)
			{
				return elem->Tag;
			}
			elem = elem->Next;
		}
		return 0;
	}

	bool HasObserver(unsigned long event)
	{
		Observer* elem = this->Start;
		while (elem)
		{
			if (elem->Event == event)
			{
				return true;
			}
			elem = elem->Next;
		}
		return false;
	}

	bool HasObserver(unsigned long event, Command::Pointer cmd) {
		Observer* elem = this->Start;
		while (elem)
		{
			if (elem->Event == event && elem->Command == cmd)
			{
				return true;
			}
			elem = elem->Next;
		}
		return false;
	}

protected:
	Observer* Start{ nullptr };
	unsigned long Count{ 0 };
};

class CallbackBase {
public:
	virtual bool operator()(Object*, unsigned long, void*) = 0;
};

class LambdaCallback : public CallbackBase {
public:
	std::function<void()> Method1;
	std::function<void(Object*, unsigned long, void*)> Method2;
	//std::function<bool(Object*, unsigned long, void*)> Method3;

	LambdaCallback(std::function<void()> method)
	{
		this->Method1 = method;
		this->Method2 = nullptr;
		//this->Method3 = nullptr;
	}

	LambdaCallback(std::function<void(Object*, unsigned long, void*)> method)
	{
		this->Method1 = nullptr;
		this->Method2 = method;
		//this->Method3 = nullptr;
	}

	//iGameLambdaCallback(std::function<bool(Object*, unsigned long, void*)> method)
	//{
	//    this->Method1 = nullptr;
	//    this->Method2 = nullptr;
	//    this->Method3 = method;
	//}

	bool operator()(Object* caller, unsigned long event, void* calldata) override
	{
		if (this->Method1)
		{
			Method1();
		}
		else if (this->Method2)
		{
			Method2(caller, event, calldata);
		}
		//else if (this->Method3)
		//{
		//    return Method3(caller, event, calldata);
		//}
		return false;
	}
};

class LambdaCommand : public Command {
public:
	I_OBJECT(LambdaCommand);
	static Pointer New() { return new LambdaCommand; }

	virtual void Execute(Object* caller, unsigned long eventId, void* callData) {
		if (this->Callable)
		{
			(*this->Callable)(caller, eventId, callData);
		}
	};

	void SetCallable(CallbackBase* callable)
	{
		delete this->Callable;
		this->Callable = callable;
	}

private:
	LambdaCommand() {}
	~LambdaCommand() override {
		delete this->Callable;
	}

	CallbackBase* Callable{ nullptr };
};

unsigned long Object::AddObserver(unsigned long event, Command::Pointer cmd, float priority)
{
	if (!this->m_Internal)
	{
		this->m_Internal = new ObserverInternal;
	}
	return this->m_Internal->AddObserver(event, cmd, priority);
}

unsigned long Object::AddObserver(const char* event, Command::Pointer cmd, float priority)
{
	return this->AddObserver(Command::GetEventIdFromString(event), cmd, priority);
}

unsigned long Object::AddObserver(unsigned long event, std::function<void()> callback, float priority)
{
	LambdaCallback* callable = new LambdaCallback(callback);
	LambdaCommand::Pointer command = LambdaCommand::New();
	command->SetCallable(callable);
	return this->AddObserver(event, command, priority);
}

unsigned long Object::AddObserver(unsigned long event, std::function<void(Object*, unsigned long, void*)> callback, float priority)
{
	LambdaCallback* callable = new LambdaCallback(callback);
	LambdaCommand::Pointer command = LambdaCommand::New();
	command->SetCallable(callable);
	return this->AddObserver(event, command, priority);
}

void Object::RemoveObserver(unsigned long tag)
{
	if (this->m_Internal)
	{
		this->m_Internal->RemoveObserver(tag);
	}
}

void Object::RemoveObservers(unsigned long event)
{
	if (this->m_Internal)
	{
		this->m_Internal->RemoveObserver(event);
	}
}

void Object::RemoveObservers(const char* event)
{
	this->RemoveObservers(Command::GetEventIdFromString(event));
}

void Object::RemoveAllObservers()
{
	if (this->m_Internal)
	{
		this->m_Internal->RemoveAllObservers();
	}
}

bool Object::HasObserver(unsigned long event)
{
	if (this->m_Internal)
	{
		return this->m_Internal->HasObserver(event);
	}
	return false;
}

bool Object::HasObserver(const char* event)
{
	return this->HasObserver(Command::GetEventIdFromString(event));
}

Command::Pointer Object::GetCommand(unsigned long tag)
{
	if (this->m_Internal)
	{
		return this->m_Internal->GetCommand(tag);
	}
	return nullptr;
}

void Object::RemoveObserver(Command::Pointer cmd)
{
	if (this->m_Internal)
	{
		unsigned long tag = this->m_Internal->GetTag(cmd);
		while (tag)
		{
			this->m_Internal->RemoveObserver(tag);
			tag = this->m_Internal->GetTag(cmd);
		}
	}
}

void Object::RemoveObservers(unsigned long event, Command::Pointer cmd)
{
	if (this->m_Internal)
	{
		this->m_Internal->RemoveObservers(event, cmd);
	}
}

void Object::RemoveObservers(const char* event, Command::Pointer cmd)
{
	this->RemoveObservers(Command::GetEventIdFromString(event), cmd);
}

bool Object::HasObserver(unsigned long event, Command::Pointer cmd)
{
	if (this->m_Internal)
	{
		return this->m_Internal->HasObserver(event, cmd);
	}
	return false;
}

bool Object::HasObserver(const char* event, Command::Pointer cmd)
{
	return this->HasObserver(Command::GetEventIdFromString(event), cmd);
}

bool Object::InvokeEvent(unsigned long event, void* callData)
{
	if (this->m_Internal)
	{
		return this->m_Internal->InvokeEvent(event, callData, this);
	}
	return false;
}

bool Object::InvokeEvent(const char* event, void* callData)
{
	return this->InvokeEvent(Command::GetEventIdFromString(event), callData);
}

void Object::Modified()
{
	m_MTime.Modified();
	//this->InvokeEvent(Command::ModifiedEvent);
}


IGAME_NAMESPACE_END

#ifndef MyFilter_h
#define MyFilter_h

#include "iGameFilter.h"

IGAME_NAMESPACE_BEGIN

class MyFilter : public Filter {
public:
	I_OBJECT(MyFilter);
	static Pointer New() { return new MyFilter; }

	bool ProcessSignal(unsigned long event) override;

protected:
	MyFilter();
	~MyFilter() = default;

	bool ProcessDataObject();
	bool ProcessData();

};
IGAME_NAMESPACE_END
#endif
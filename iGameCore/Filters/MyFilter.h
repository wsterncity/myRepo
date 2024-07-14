#ifndef MyFilter_h
#define MyFilter_h

#include "iGameFilter.h"

IGAME_NAMESPACE_BEGIN

class MyFilter : public Filter {
public:
	I_OBJECT(MyFilter);
	static Pointer New() { return new MyFilter; }


protected:
	MyFilter() = default;
	~MyFilter() override = default;


};
IGAME_NAMESPACE_END
#endif
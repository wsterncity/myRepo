#ifndef iGameStandardController_h
#define iGameStandardController_h

#include "iGameController.h"

IGAME_NAMESPACE_BEGIN
class Filter;

class StandardController : public Controller {
public:
	I_OBJECT(StandardController);
	static Pointer New() { return new StandardController; }

	bool EmitSignal(unsigned long e) override;

	bool Update() override;

protected:
	StandardController() = default;
	~StandardController() = default;

	bool UpdateDataObject();
	bool UpdateData();

};
IGAME_NAMESPACE_END
#endif
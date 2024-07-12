#ifndef iGameStreamingController_h
#define iGameStreamingController_h

#include "iGameController.h"

IGAME_NAMESPACE_BEGIN
class StreamingController : public Controller {
public:
	I_OBJECT(StreamingController);
	static Pointer New() { return new StreamingController; }

	bool EmitSignal(unsigned long e) override;

	bool Update() override;

protected:
	StreamingController() = default;
	~StreamingController() = default;

	bool UpdateDataObject();
	bool UpdateData();

};
IGAME_NAMESPACE_END
#endif
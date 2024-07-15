#ifndef iGameVTSReader_h
#define iGameVTSReader_h

#include "iGameFileReader.h"

IGAME_NAMESPACE_BEGIN

class VTSReader : public FileReader {
public:
	I_OBJECT(VTSReader);
	static Pointer New() { return new VTSReader; }

	bool Parsing() override;



protected:
	VTSReader() = default;
	~VTSReader() override = default;
};

IGAME_NAMESPACE_END
#endif
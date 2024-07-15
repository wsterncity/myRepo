#ifndef iGameINPReader_h
#define iGameINPReader_h

#include "iGameFileReader.h"

IGAME_NAMESPACE_BEGIN

class INPReader : public FileReader {
public:
	I_OBJECT(INPReader);
	static Pointer New() { return new INPReader; }

	bool Parsing() override;

protected:
	INPReader() = default;
	~INPReader() override = default;
};

IGAME_NAMESPACE_END
#endif
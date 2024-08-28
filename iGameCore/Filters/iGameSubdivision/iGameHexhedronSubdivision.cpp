#include "iGameHexhedronSubdivision.h"
IGAME_NAMESPACE_BEGIN

bool HexhedronSubdivision::Execute()
{
	this->mesh = DynamicCast<VolumeMesh>(GetInput(0));
	if (mesh == nullptr) { return false; }



}


IGAME_NAMESPACE_END
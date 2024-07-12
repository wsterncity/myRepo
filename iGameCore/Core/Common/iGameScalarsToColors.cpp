﻿#include "iGameScalarsToColors.h"
#include <time.h>

IGAME_NAMESPACE_BEGIN
//------------------------------------------------------------------------------
ScalarsToColors::ScalarsToColors()
{
	this->Alpha = 1.0;
	this->VectorComponent = 0;
	this->VectorSize = -1;
	this->VectorMode = ScalarsToColors::COMPONENT;

	// only used in this class, not used in subclasses
	this->InputRange[0] = 0.0;
	this->InputRange[1] = 255.0;
}

//------------------------------------------------------------------------------
ScalarsToColors::~ScalarsToColors()
{

}


//------------------------------------------------------------------------------
void ScalarsToColors::SetVectorModeToComponent()
{
	this->SetVectorMode(ScalarsToColors::COMPONENT);
}

//------------------------------------------------------------------------------
void ScalarsToColors::SetVectorModeToMagnitude()
{
	this->SetVectorMode(ScalarsToColors::MAGNITUDE);
}

//------------------------------------------------------------------------------
void ScalarsToColors::SetVectorModeToRGBColors()
{
	this->SetVectorMode(ScalarsToColors::RGBCOLORS);
}


//------------------------------------------------------------------------------
// do not use SetMacro() because we do not want the table to rebuild.
void ScalarsToColors::SetAlpha(float alpha)
{
	this->Alpha = (alpha < 0.0 ? 0.0 : (alpha > 1.0 ? 1.0 : alpha));
}

//------------------------------------------------------------------------------
void ScalarsToColors::SetRange(float minval, float maxval)
{
	if (this->InputRange[0] != minval || this->InputRange[1] != maxval)
	{
		this->InputRange[0] = minval;
		this->InputRange[1] = maxval;
	}
}
//------------------------------------------------------------------------------
float* ScalarsToColors::GetRange()
{
	return this->InputRange;
}
//------------------------------------------------------------------------------
void ScalarsToColors::ComputeShiftScale(float& shift, float& scale)
{
	constexpr float minscale = -1e17;
	constexpr float maxscale = 1e17;
	const float* range = this->GetRange();
	shift = -range[0];
	scale = range[1] - range[0];
	if (scale * scale > 1e-30) {
		scale = 1.0 / scale;
	}
	else {
		scale = (scale < 0.0 ? minscale : maxscale);
	}
}
//------------------------------------------------------------------------------
void ScalarsToColors::GetColor(float v, float rgb[3], float& shift, float& scale)
{
	constexpr float minval = 0.0;
	constexpr float maxval = 0.999999;
	float val = (v + shift) * scale;
	val = (val > minval ? val : minval);
	val = (val < maxval ? val : maxval);
	MapColor(val, rgb);
}


//------------------------------------------------------------------------------
const unsigned char* ScalarsToColors::MapValue(float v, float& shift, float& scale)
{
	float rgb[3];
	this->GetColor(v, rgb, shift, scale);
	float alpha = this->GetOpacity(v);
	this->RGBABytes[0] = ColorToUChar(rgb[0]);
	this->RGBABytes[1] = ColorToUChar(rgb[1]);
	this->RGBABytes[2] = ColorToUChar(rgb[2]);
	this->RGBABytes[3] = ColorToUChar(alpha);
	return this->RGBABytes;
}
const float* ScalarsToColors::MapValueToRGB(float v, float& shift, float& scale)
{
	this->GetColor(v, this->RGB, shift, scale);
	return this->RGB;
}


//------------------------------------------------------------------------------

void ScalarsToColors::InitRange(DataArray::Pointer input, int component, int size)
{
	float minv = 1e9;
	float maxv = -1e9;
	int vectorMode = this->GetVectorMode();
	int inComponent = input->GetNumberOfComponents();
	if (vectorMode == COMPONENT)
	{
		if (component == -1) {
			// if set to -1, use default value provided by table
			component = this->GetVectorComponent();
		}
		if (component < 0) {
			component = 0;
		}
		if (component >= inComponent) {
			component = inComponent - 1;
		}
	}
	if (vectorMode == MAGNITUDE)
	{
		// make sure vectorSize is within allowed range
		if (size == -1) {
			// if set to -1, use default value provided by table
			size = this->GetVectorSize();
		}
		if (size <= 0) {
			component = 0;
			size = inComponent;
		}
		else {
			if (component < 0) {
				component = 0;
			}
			if (component >= inComponent) {
				component = inComponent - 1;
			}
			if (component + size > inComponent) {
				size = inComponent - component;
			}
		}
		if (size == 1) {
			vectorMode = COMPONENT;
		}
	}
	switch (vectorMode)
	{
	case COMPONENT:
	{
		float data[16];
		for (int i = 0; i < input->GetNumberOfTuples(); i++)
		{
			input->GetTuple(i, data);
			minv = std::min(minv, data[component]);
			maxv = std::max(maxv, data[component]);
		}
	}
	break;
	case MAGNITUDE:
	{
		float data[16];
		for (int i = 0; i < input->GetNumberOfTuples(); i++)
		{
			input->GetTuple(i, data);
			float sum = 0.0;
			for (int j = component; j < component + size; j++) {
				sum += data[j] * data[j];
			}
			sum = sqrt(sum);
			minv = std::min(minv, sum);
			maxv = std::max(maxv, sum);
		}
	}
	break;
	default:
		break;
	}
	//std::cout << minv << " " << maxv << std::endl;
	this->SetRange(minv, maxv);
}
FloatArray::Pointer ScalarsToColors::MapScalars(
	DataArray::Pointer scalars, int component, int outputFormat)
{
	//component::渲染第几个维度
	int numberOfComponents = scalars->GetNumberOfComponents();
	FloatArray::Pointer newColors = FloatArray::New();
	newColors->SetNumberOfComponents(outputFormat);
	newColors->Resize(scalars->GetNumberOfTuples());
	if (component < 0 && numberOfComponents>1) {
		this->SetVectorModeToMagnitude();
		this->MapVectorsThroughTable(scalars, newColors, outputFormat);
	}
	else {
		if (component < 0) {
			component = 0;
		}
		if (component >= numberOfComponents) {
			component = numberOfComponents - 1;
		}
		// Map the scalars to colors
		this->MapVectorsThroughTable(scalars, newColors, outputFormat, component, 1);
	}
	return newColors;
}
//------------------------------------------------------------------------------
// Map a set of vector values through the table
void ScalarsToColors::MapVectorsThroughTable(DataArray::Pointer input, FloatArray::Pointer output,
	int outputFormat, int vectorComponent, int vectorSize)
{
	int inComponents = input->GetNumberOfComponents();
	int vectorMode = this->GetVectorMode();
	if (vectorMode == COMPONENT) {
		if (vectorComponent == -1) {
			// if set to -1, use default value provided by table
			vectorComponent = this->GetVectorComponent();
		}
		if (vectorComponent < 0) {
			vectorComponent = 0;
		}
		if (vectorComponent >= inComponents) {
			vectorComponent = inComponents - 1;
		}
	}
	else {
		// make sure vectorSize is within allowed range
		if (vectorSize == -1) {
			// if set to -1, use default value provided by table
			vectorSize = this->GetVectorSize();
		}
		if (vectorSize <= 0) {
			vectorComponent = 0;
			vectorSize = inComponents;
		}
		else {
			if (vectorComponent < 0) {
				vectorComponent = 0;
			}
			if (vectorComponent >= inComponents) {
				vectorComponent = inComponents - 1;
			}
			if (vectorComponent + vectorSize > inComponents) {
				vectorSize = inComponents - vectorComponent;
			}
		}
		if (vectorMode == MAGNITUDE && (inComponents == 1 || vectorSize == 1)) {
			vectorMode = COMPONENT;
		}
	}
	// map according to the current vector mode
	switch (vectorMode)
	{
	case ScalarsToColors::COMPONENT:
	{
		int index = vectorComponent;
		float shift, scale;
		ComputeShiftScale(shift, scale);
		float data[16];
		for (int i = 0; i < input->GetNumberOfTuples(); i++)
		{
			input->GetTuple(i, data);
			const float* rgb = MapValueToRGB(data[index], shift, scale);
			//const unsigned char* rgb = MapValue(data[index], shift, scale);
			//std::array<unsigned char, 3>tmp = { rgb[0], rgb[1], rgb[2] };
			output->InsertNextTuple3(rgb[0], rgb[1], rgb[2]);
		}
	}
	break;
	case ScalarsToColors::MAGNITUDE:
	{
		int index = vectorComponent;
		float shift, scale;
		ComputeShiftScale(shift, scale);
		float data[16];
		DataArray::Pointer tmp;
		for (int i = 0; i < input->GetNumberOfTuples(); i++) {
			input->GetTuple(i, data);
			float value = 0.0;
			for (int j = index; j < index + vectorSize; j++) {
				value += data[j] * data[j];
			}
			value = sqrt(value);
			const float* rgb = MapValueToRGB(value, shift, scale);
			output->InsertNextTuple3(rgb[0], rgb[1], rgb[2]);
		}
	}
	break;
	case ScalarsToColors::RGBCOLORS:
	{
		if (inComponents < 3)return;
		float data[16];
		std::array<unsigned char, 3> rgb;
		for (int i = 0; i < input->GetNumberOfTuples(); i++)
		{
			input->GetTuple(i, data);
			output->InsertNextTuple3(data[0], data[1], data[2]);
		}
	}
	break;
	}
}

IGAME_NAMESPACE_END
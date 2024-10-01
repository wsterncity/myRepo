#ifndef iGameColorMap_h
#define iGameColorMap_h

#include "iGameFlatArray.h"

IGAME_NAMESPACE_BEGIN
class ColorMap :public Object {
public:
	I_OBJECT(ColorMap);
	static Pointer NewInstance() { return new ColorMap; }

	/*map color to rgb,the value is rescaled to 0.0-1.0*/
	void MapColor(float value, float rgb[3]);
	/*change the color in index position*/
	void SetColorBar(int index, float r, float g, float b);
	void SetColorBar(int index, float rgb[3]) {
		SetColorBar(index, rgb[0], rgb[1], rgb[2]);
	}

	/*change the range in index position*/
	void SetColorRange(int index, float& x);

	/*delete the color in index position*/
	void DeleteIndexColor(int index);

	/*add the color in index position,
	and the colorbar's size will be add when range is added together*/
	void AddColorBar(int index, float r, float g, float b);
	void AddColorBar(int index, float rgb[3]) {
		AddColorBar(index, rgb[0], rgb[1], rgb[2]);
	}
	/*add the range in index position,
      and the colorbar's size will be add when color is added together*/
	void AddColorRange(int index, float x);
	/*init colorbar with default mode,such as white-black,blue-red*/
	void InitColorBarByOneSegmentType();
	void InitColorBarByTwoSegmentType();
	void InitColorBarByFourSegmentType();
	void InitColorBarByFiveSegmentType();

	/*get colorbar*/
	FloatArray::Pointer GetColorBar() { return this->ColorBar; };
	/*get color range*/
	FloatArray::Pointer GetColorRange() { return this->ColorRange; };
	/*init colorbar with default mode,use size to control*/
	void ResetColorBar(int size) {
		switch (size)
		{
		case 1:
			InitColorBarByOneSegmentType();
			break;

		case 2:
			InitColorBarByTwoSegmentType();
			break;
		case 4:
			InitColorBarByFourSegmentType();
			break;
		case 5:
			InitColorBarByFiveSegmentType();
			break;
		default:
			break;
		}
	}
	/*update full colorbar,will update the data address rather than deeply copy*/
	void UpdateColorMap(FloatArray::Pointer colorbar, FloatArray::Pointer colorrange) {
		int colorbarsize = colorbar->GetNumberOfElements();
		int colorrangesize = colorrange->GetNumberOfElements();
		assert(colorbarsize == colorrangesize);
		assert(colorbar->GetDimension() == 3);
		assert(colorrange->GetDimension() == 1);
		this->ColorBarSize = colorbarsize - 1;
		UpdateColorBar(colorbar);
		UpdateColorRange(colorrange);
	}
	void UpdateColorBar(FloatArray::Pointer colorbar) {
		this->ColorBar = colorbar;
	};
	void UpdateColorRange(FloatArray::Pointer colorrange) {
		this->ColorRange = colorrange;
	};
	// get the colorbar draw info,use to draw colorbar
	FloatArray::Pointer GetColorBarDrawInfo();
	/*return the colorbar's size,Note that the number of segments is returned,
	not the size of the array*/
	int GetColorBarSize() { return this->ColorBarSize; }
protected:
	ColorMap()
	{
		this->ColorBar = FloatArray::New();
		this->ColorRange = FloatArray::New();
//		InitColorBarByFourSegmentType();
        InitColorBarByTwoSegmentType();
	}
	~ColorMap() override = default;

	FloatArray::Pointer ColorBar;
	FloatArray::Pointer ColorRange;
	int ColorBarSize;
};
IGAME_NAMESPACE_END
#endif
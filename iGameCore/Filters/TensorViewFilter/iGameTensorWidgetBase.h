#ifndef iGameTensorWidgetBase_h
#define iGameTensorWidgetBase_h
/**
 * @class   iGameTensorWidgetBase
 * @brief   iGameTensorWidgetBase's brief
 */

#include "iGameTensorRepresentation.h" 
#include "iGameSurfaceMesh.h"
#include "iGameDrawObject.h"

IGAME_NAMESPACE_BEGIN
class Scene;
class iGameTensorWidgetBase : public Filter, public DrawObject {
public:
	I_OBJECT(iGameTensorWidgetBase);
	static iGameTensorWidgetBase* New() { return new iGameTensorWidgetBase; }
	~iGameTensorWidgetBase();

	void SetPoints(Points::Pointer);

	void SetTensorAttributes(ArrayObject::Pointer);

	void ShowTensorField();

	void UpdateGlyphDrawData();
	void UpdateGlyphDrawColor();
	void UpdateGlyphScale(double s);

	void SetPositionColors(FloatArray::Pointer);

	void GenerateVectorField();

	Points::Pointer GetDrawGlyphPoints() { return this->m_DrawGlyphPoints; };

	UnsignedIntArray::Pointer GetDrawGlyphPointOrders() { return this->m_DrawGlyphPointOrders; }

	FloatArray::Pointer GetDrawGlyphColors() { return this->m_DrawGlyphColors; }
protected:
	iGameTensorWidgetBase();
private:
	//张量所在点数组
	Points::Pointer m_Points;
	//张量数组
	ArrayObject::Pointer m_TensorAttributes;

	iGameTensorRepresentation::Pointer m_TensorManager;

	//图元的坐标的颜色数组，根据图元所在顶点的标量或其他方式生成
	FloatArray::Pointer m_PositionColors;


	//绘制椭球图元的顶点坐标
	Points::Pointer m_DrawGlyphPoints;
	//椭球面片的邻接顺序
	UnsignedIntArray::Pointer m_DrawGlyphPointOrders;
	//椭球面片的颜色
	FloatArray::Pointer m_DrawGlyphColors;

public:
	void Draw(Scene*) override;
	void ConvertToDrawableData() override;
};
IGAME_NAMESPACE_END
#endif
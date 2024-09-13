#include "iGameTensorWidgetBase.h"

IGAME_NAMESPACE_BEGIN
iGameTensorWidgetBase::iGameTensorWidgetBase()
{
	this->m_TensorManager = iGameTensorRepresentation::New();
	this->m_TensorManager->SetSliceNum(5);
	this->m_DrawGlyphPoints = Points::New();
	this->m_DrawGlyphPointOrders = IntArray::New();
	this->m_DrawGlyphColors = FloatArray::New();
	this->m_DrawGlyphColors->SetElementSize(3);
}
iGameTensorWidgetBase::~iGameTensorWidgetBase()
{

}
void iGameTensorWidgetBase::SetPoints(Points::Pointer points)
{
	this->m_Points = points;
}
void iGameTensorWidgetBase::SetTensorAttributes(ArrayObject::Pointer attributes)
{
	this->m_TensorAttributes = attributes;
}
void iGameTensorWidgetBase::ShowTensorField()
{
	UpdateGlyphDrawData();
	UpdateGlyphDrawColor();
}

void iGameTensorWidgetBase::UpdateGlyphDrawData()
{
	clock_t time1 = clock();
	int PointNum = this->m_Points ? this->m_Points->GetNumberOfPoints() : 0;
	Point p;
	if (PointNum == 0 || !this->m_TensorAttributes ||
		this->m_TensorAttributes->GetNumberOfValues() != (9 * PointNum)) {
		return;
	}
	auto GlyphPointIndexOrders = m_TensorManager->GetEllipsoidDrawPointIndexOrders();
	int GlyphPointNum = m_TensorManager->GetNumberOfDrawPoints();
	m_DrawGlyphPoints->Resize(PointNum * GlyphPointNum);
	m_DrawGlyphPointOrders->Resize(PointNum * GlyphPointIndexOrders->GetNumberOfValues());
	auto GlyphPoints = m_DrawGlyphPoints->RawPointer();
	auto GlyphPointOrders = m_DrawGlyphPointOrders->RawPointer();
	double t[9];
	for (int i = 0; i < PointNum; i++) {
		p = this->m_Points->GetPoint(i);
		m_TensorManager->SetPosition(p);
		for (int j = 0; j < 9; j++) {
			t[j] = this->m_TensorAttributes->GetValue(9 * i + j);
		}
		m_TensorManager->SetTensor(t);
		auto EllipsoidDrawPoints = m_TensorManager->GetEllipsoidDrawPoints()->RawPointer();
		IGsize st = i * 3 * GlyphPointNum;
		std::copy(EllipsoidDrawPoints,
			EllipsoidDrawPoints + 3 * GlyphPointNum,
			GlyphPoints + st);

		st = i * GlyphPointIndexOrders->GetNumberOfValues();
		IGsize offset = i * GlyphPointNum;
		for (int j = 0; j < GlyphPointIndexOrders->GetNumberOfValues(); j++) {
			//这里不太好用copy，不能保证两个类型相同
			m_DrawGlyphPointOrders->SetValue(st + j, offset + GlyphPointIndexOrders->GetValue(j));
			//m_DrawGlyphPointOrders->AddValue(st + GlyphPointIndexOrders->GetValue(j));
		}
	}
	clock_t time2 = clock();
	std::cout << time2 - time1 << "ms\n";
}
void iGameTensorWidgetBase::UpdateGlyphDrawColor()
{
	int PointNum = this->m_Points ? this->m_Points->GetNumberOfPoints() : 0;
	if (PointNum == 0 || this->m_PositionColors == nullptr ||
		this->m_PositionColors->GetNumberOfElements() != PointNum) {
		return;
	}
	int GlyphPointNum = m_TensorManager->GetNumberOfDrawPoints();
	float rgb[16] = { .0 };
	m_DrawGlyphColors->Resize(PointNum * GlyphPointNum);
	auto GlyphColors = m_DrawGlyphColors->RawPointer();
	IGsize offset = 0;
	for (int i = 0; i < PointNum; i++) {
		m_PositionColors->GetElement(i, rgb);
		for (int j = 0; j < GlyphPointNum; j++) {
			offset += 3;
			std::copy(GlyphColors + offset, GlyphColors + offset + 3, rgb);
			//m_DrawGlyphColors->AddValue(rgb[0]);
			//m_DrawGlyphColors->AddValue(rgb[1]);
			//m_DrawGlyphColors->AddValue(rgb[2]);
		}
	}

}
void iGameTensorWidgetBase::UpdateGlyphScale(double s)
{

}

void iGameTensorWidgetBase::SetPositionColors(FloatArray::Pointer colors)
{
	this->m_PositionColors = colors;
}

void iGameTensorWidgetBase::GenerateVectorField()
{
	int PointNum = this->m_Points ? this->m_Points->GetNumberOfPoints() : 0;
	if (PointNum == 0 || !this->m_TensorAttributes ||
		this->m_TensorAttributes->GetNumberOfValues() != (9 * PointNum)) {
		return;
	}
	double vector[3];
	DoubleArray::Pointer vectorData = DoubleArray::New();
	vectorData->SetElementSize(3);
	vectorData->SetName(this->m_TensorAttributes->GetName() + "_PrimaryFeature");
	double t[9];
	for (int i = 0; i < PointNum; i++) {
		for (int j = 0; j < 9; j++) {
			t[j] = this->m_TensorAttributes->GetValue(9 * i + j);
		}
		m_TensorManager->InitTensorEigenData(t);
		m_TensorManager->GetEigenVector(0, vector);
		vectorData->SetElement(i, vector);
	}
}

IGAME_NAMESPACE_END
#ifndef iGameDataObject_h
#define iGameDataObject_h

#include "iGameObject.h"
#include "iGameAttributeData.h"
#include "iGameScalarsToColors.h"
#include "iGameMetadata.h"

#include "OpenGL/GLBuffer.h"
#include "OpenGL/GLShader.h"
#include "OpenGL/GLTexture2d.h"
#include "OpenGL/GLVertexArray.h"

IGAME_NAMESPACE_BEGIN
class Scene;
class DataObject : public Object {
public:
	I_OBJECT(DataObject);
	static Pointer New() { return new DataObject; }

	static Pointer CreateDataObject(IGenum type);

	virtual IGenum GetDataObjectType() const {
		return IG_DATA_OBJECT;
	}

	virtual bool ShallowCopy(Pointer o) {
		return true;
	}

	virtual bool DeepCopy(Pointer o) {
		return true;
	}

	AttributeData* GetAttributes() { return m_Attributes.get(); }
	Metadata* GetMetadata() { return m_Metadata.get(); }

protected:
	DataObject()
	{
		m_Attributes = AttributeData::New();
		m_Metadata = Metadata::New();
	}
	~DataObject() override = default;

	AttributeData::Pointer m_Attributes{};
	Metadata::Pointer m_Metadata{};

public:
	virtual void Draw(Scene*) {};
	virtual void ConvertToDrawableData() {};
	virtual bool IsDrawable() { return false; }
	virtual void ViewCloudPicture(int index, int demension = -1) {} // ø… ”ªØ‘∆Õº
	virtual void SetViewStyle(IGenum mode) { m_ViewStyle = mode; }
	IGenum GetViewStyle() { return m_ViewStyle; }
	void SetVisibility(bool f) { m_Visibility = f; }
	bool GetVisibility() { return m_Visibility; }

protected:
	IGenum m_ViewStyle{ IG_NONE };
	bool m_Visibility{ true };
};

IGAME_NAMESPACE_END
#endif
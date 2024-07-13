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

	void SetUniqueDataObjectId() { m_UniqueId = GetIncrementDataObjectId(); }
	DataObjectId GetDataObjectId() { return m_UniqueId; }

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

	class SubDataObjectsHelper : public Object {
	public:
		I_OBJECT(SubDataObjectsHelper);
		static Pointer New() { return new SubDataObjectsHelper; };
		using SubDataObjectMap = std::map<DataObjectId, DataObject::Pointer>;
		using Iterator = SubDataObjectMap::iterator;
		using ConstIterator = SubDataObjectMap::const_iterator;

		DataObject::Pointer GetSubDataObject(DataObjectId id) {
			auto it = m_SubDataObjects.find(id);
			if (it == End()) {
				return nullptr;
			}
			return it->second;
		}

		DataObjectId AddSubDataObject(DataObject::Pointer obj) {
			for (auto& data : m_SubDataObjects) {
				if (data.second == obj)
				{
					return -1;
				}
			}
			m_SubDataObjects.insert(std::make_pair<>(obj->GetDataObjectId(), obj));
			this->Modified();
			return obj->GetDataObjectId();
		}

		void RemoveSubDataObject(DataObjectId id) {
			auto it = m_SubDataObjects.find(id);
			if (it == End()) {
				return;
			}
			m_SubDataObjects.erase(id);
			this->Modified();
		}

		bool HasSubDataObject() noexcept {
			return m_SubDataObjects.size() != 0;
		}

		int GetNumberOfSubDataObjects() noexcept {
			return m_SubDataObjects.size();
		}

		Iterator Begin() noexcept {
			return m_SubDataObjects.begin();
		}
		ConstIterator Begin() const noexcept {
			return m_SubDataObjects.begin();
		}

		Iterator End() noexcept {
			return m_SubDataObjects.end();
		}
		ConstIterator End() const noexcept {
			return m_SubDataObjects.end();
		}
	private:
		SubDataObjectsHelper() {}
		~SubDataObjectsHelper() override {}

		SubDataObjectMap m_SubDataObjects;
	};

	using SubIterator = typename SubDataObjectsHelper::Iterator;
	using SubConstIterator = typename SubDataObjectsHelper::ConstIterator;

	DataObject::Pointer GetSubDataObject(DataObjectId id);
	DataObjectId AddSubDataObject(DataObject::Pointer obj); 
	void RemoveSubDataObject(DataObjectId id);
	bool HasSubDataObject() noexcept;
	int GetNumberOfSubDataObjects() noexcept;
	SubIterator SubBegin();
	SubConstIterator SubBegin() const;
	SubIterator SubEnd();
	SubConstIterator SubEnd() const;

	DataObject* FindParent();

protected:
	DataObject()
	{
		m_Attributes = AttributeData::New();
		m_Metadata = Metadata::New();
		m_UniqueId = GetIncrementDataObjectId();
	}
	~DataObject() override = default;

	DataObjectId m_UniqueId{};
	AttributeData::Pointer m_Attributes{};
	Metadata::Pointer m_Metadata{};

	friend class SubDataObjectsHelper;
	SmartPointer<SubDataObjectsHelper> m_SubDataObjectsHelper{};
	DataObject* m_Parent{ nullptr };

	template<typename Functor, typename... Args>
	void ProcessSubDataObjects(Functor&& functor, Args&&... args);

private:
	DataObjectId GetIncrementDataObjectId();
	void SetParent(DataObject* parent);

public:
	virtual void Draw(Scene*);
	virtual void ConvertToDrawableData();
	virtual void MakeDrawable() { m_Drawable = true; }
	virtual bool IsDrawable() { return m_Drawable; }
	virtual void ViewCloudPicture(int index, int demension = -1);
	void ViewCloudPictureOfModel(int index, int demension = -1);
	virtual void SetViewStyle(IGenum mode);
	void SetViewStyleOfModel(IGenum mode);
	IGenum GetViewStyle();
	IGenum GetViewStyleOfModel();
	void SetVisibility(bool f);
	bool GetVisibility() { return m_Visibility; }

protected:
	IGenum m_ViewStyle{ IG_NONE };
	bool m_Visibility{ true };
	bool m_Drawable{ false };
};

template<typename Functor, typename... Args>
inline void DataObject::ProcessSubDataObjects(Functor&& functor, Args&&... args)
{
	if (HasSubDataObject())
	{
		for (auto it = m_SubDataObjectsHelper->Begin(); it != m_SubDataObjectsHelper->End(); ++it)
		{
			(it->second->*functor)(std::forward<Args>(args)...);
		}
	}
}

IGAME_NAMESPACE_END
#endif
//
// Created by Sumzeek on 7/4/2024.
//

#ifndef OPENIGAME_SCENCE_H
#define OPENIGAME_SCENCE_H

#include "OpenGL/GLShader.h"
#include "iGameCamera.h"
#include "iGameFontSet.h"
#include "iGameLight.h"
#include "iGameSurfaceMesh.h"
#include "iGameAxes.h"

IGAME_NAMESPACE_BEGIN
class Scene : public Object {
public:
    I_OBJECT(Scene);
    static Pointer New() { return new Scene; }

    struct MVPMatrix {
        alignas(16) igm::mat4 model;
        alignas(16) igm::mat4 normal;
        alignas(16) igm::mat4 mvp;
    };
    struct UniformBufferObject {
        alignas(16) igm::vec3 viewPos;
        alignas(4) int useColor{ 0 };
    };

    enum ShaderType {
        PATCH = 0,
        NOLIGHT,
        PURECOLOR,
        AXES,
        FONT,
        CULLING,
        CULLINGCOMPUTE,
        SHADERTYPE_COUNT
    };

    // 给Actor以及Mapper使用的接口
    MVPMatrix& MVP() { return this->m_MVP; }
    UniformBufferObject& UBO() { return this->m_UBO; }
    void UpdateUniformBuffer();

    void SetShader(IGenum type, GLShaderProgram*);
    GLShaderProgram* GenShader(IGenum type);
    GLShaderProgram* GetShaderWithType(IGenum type);
    GLShaderProgram* GetShader(IGenum type);
    bool HasShader(IGenum type);

    // 给Qt使用的接口
    void ChangeViewStyle(IGenum mode);

    // 渲染使用的接口
    void Draw();
    void Resize(int width, int height, int pixelRatio);
    void Update();

    void AddDataObject(DataObject::Pointer obj)
    {
        m_Models.insert(std::make_pair<>(obj->GetDataObjectId(), obj));
        m_CurrentObjectId = obj->GetDataObjectId();
        m_CurrentObject = obj.get();
    }

    bool UpdateCurrentDataObject(int index)
    {
        for (auto& model : m_Models) {
            auto& id = model.first;
            auto& obj = model.second;
            if (id == index)
            {
                m_CurrentObjectId = id;
                m_CurrentObject = obj.get();
                return true;
            }
            if (obj->HasSubDataObject())
            {
                auto subObj = obj->GetSubDataObject(index);
                if (subObj != nullptr)
                {
                    m_CurrentObjectId = index;
                    m_CurrentObject = subObj.get();
                    return true;
                }
            }
        }
        return false;
    }

    DataObject* GetDataObject(int index)
    {
        for (auto& model : m_Models) {
            auto& id = model.first;
            auto& obj = model.second;
            if (id == index)
            {
                return obj.get();
            }
            if (obj->HasSubDataObject())
            {
                auto subObj = obj->GetSubDataObject(index);
                if (subObj != nullptr)
                {
                    return subObj.get();
                }
            }
        }
        return false;
    }

    DataObject* GetCurrentObject() {
        return m_CurrentObject;
    }

    void DrawModels() 
    {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glViewport(0, 0, m_Camera->GetViewPort().x, m_Camera->GetViewPort().y);

        for (auto& model : m_Models) {
            model.second->ConvertToDrawableData();
            model.second->Draw(this);
        }
    }

    std::map<DataObjectId, DataObject::Pointer>& GetModelList() { return m_Models; }

protected:
    Scene();
    ~Scene() override;

    void InitOpenGL();
    void InitFont();
    void InitAxes();
    
    void UpdateUniformData();
    void DrawFrame();
    void DrawAxes();

    std::map<DataObjectId, DataObject::Pointer> m_Models;
    DataObjectId m_CurrentObjectId{0};
    DataObject* m_CurrentObject{nullptr};

    Camera::Pointer m_Camera{};
    Light::Pointer m_Light{};
    Axes::Pointer m_Axes{};

    MVPMatrix m_MVP;
    UniformBufferObject m_UBO;
    igm::mat4 m_ModelRotate{};
    igm::vec3 m_BackgroundColor{};
    igm::vec4 m_FirstActorCenter{0.0f, 0.0f, 0.0f, 0.707f};

    unsigned int m_MVPBlock, m_UBOBlock;
    std::map<IGenum, std::unique_ptr<GLShaderProgram>> m_ShaderPrograms;

    friend class Interactor;
};

IGAME_NAMESPACE_END
#endif // OPENIGAME_SCENCE_H

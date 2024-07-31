//
// Created by Sumzeek on 7/4/2024.
//

#ifndef OPENIGAME_SCENCE_H
#define OPENIGAME_SCENCE_H

#include "OpenGL/GLFramebuffer.h"
#include "OpenGL/GLIndirectCommand.h"
#include "OpenGL/GLShader.h"
#include "iGameAxes.h"
#include "iGameCamera.h"
#include "iGameFontSet.h"
#include "iGameLight.h"
#include "iGameSurfaceMesh.h"

IGAME_NAMESPACE_BEGIN
class Scene : public Object {
public:
    I_OBJECT(Scene);
    static Pointer New() { return new Scene; }

    struct MVPMatrix {
        alignas(16) igm::mat4 model;
        alignas(16) igm::mat4 normal;
        alignas(16) igm::mat4 viewporj;
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
        DEPTHREDUCE,
        MESHLETCULL,
        SCREEN,
        SHADERTYPE_COUNT
    };

    MVPMatrix& MVP() { return this->m_MVP; }
    UniformBufferObject& UBO() { return this->m_UBO; }
    void UpdateUniformBuffer();

    void SetShader(IGenum type, GLShaderProgram*);
    GLShaderProgram* GenShader(IGenum type);
    GLShaderProgram* GetShaderWithType(IGenum type);
    GLShaderProgram* GetShader(IGenum type);
    bool HasShader(IGenum type);

    void ChangeViewStyle(IGenum mode);

    void Draw();
    void Resize(int width, int height, int pixelRatio);
    void Update();

    void AddDataObject(DataObject::Pointer obj) {
        m_Models.insert(std::make_pair<>(obj->GetDataObjectId(), obj));
        m_CurrentObjectId = obj->GetDataObjectId();
        m_CurrentObject = obj.get();
        if (auto visibility = m_CurrentObject->GetVisibility()) {
            ChangeDataObjectVisibility(m_CurrentObjectId, visibility);
        }
    }

    void RemoveCurrentDataObject() {
        m_Models.erase(m_CurrentObjectId);
        if (m_Models.empty()) return;
        m_CurrentObjectId = m_Models.begin()->first;
        m_CurrentObject = m_Models.begin()->second;
    }

    void RemoveDataObject(DataObject::Pointer obj) {
        for (auto it = m_Models.begin(); it != m_Models.end(); ++it) {
            if (it->second == obj) {
                m_Models.erase(it);
                m_CurrentObjectId = m_Models.begin()->first;
                m_CurrentObject = m_Models.begin()->second;
                break;
            }
        }
    }

    bool UpdateCurrentDataObject(int index) {
        for (auto& [id, obj] : m_Models) {
            if (id == index) {
                m_CurrentObjectId = id;
                m_CurrentObject = obj.get();
                return true;
            }
            if (obj->HasSubDataObject()) {
                auto subObj = obj->GetSubDataObject(index);
                if (subObj != nullptr) {
                    m_CurrentObjectId = index;
                    m_CurrentObject = subObj.get();
                    return true;
                }
            }
        }
        return false;
    }

    DataObject* GetDataObject(int index) {
        for (auto& [id, obj] : m_Models) {
            if (id == index) { return obj.get(); }
            if (obj->HasSubDataObject()) {
                auto subObj = obj->GetSubDataObject(index);
                if (subObj != nullptr) { return subObj.get(); }
            }
        }
        return nullptr;
    }

    DataObject* GetCurrentObject() { return m_CurrentObject; }

    void ChangeDataObjectVisibility(int index, bool visibility) {
        auto* obj = GetDataObject(index);
        obj->SetVisibility(visibility);

        if (visibility) {
            m_VisibleModelsCount++;
            if (m_VisibleModelsCount == 1) {
                Vector3f center = obj->GetBoundingBox().center();
                float radius = obj->GetBoundingBox().diag() / 2;

                m_FirstModelCenter = igm::vec4{ center[0],center[1],center[2], radius };
                m_Camera->SetCamaraPos(center[0], center[1],
                    center[2] + 2.0f * radius);
            }
        }
        else {
            m_VisibleModelsCount--;
        }
    }

    void DrawModels() {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glViewport(0, 0, m_Camera->GetViewPort().x, m_Camera->GetViewPort().y);

        for (auto& [id, obj] : m_Models) {
            obj->ConvertToDrawableData();
            obj->Draw(this);
        }
    }

    std::map<DataObjectId, DataObject::Pointer>& GetModelList() {
        return m_Models;
    }

    void RefreshHZBTexture() {
        //GetShader(Scene::DEPTHREDUCE)->use();
        //for (int level = 0; level < m_HZBLevels - 1; ++level) {
        //    glBindImageTexture(0, level == 0 ? depthTexture : hzbTexture,
        //                       level == 0 ? 0 : level - 1, GL_FALSE, 0,
        //                       GL_READ_ONLY, GL_R32F);
        //    glBindImageTexture(1, hzbTexture, level, GL_FALSE, 0, GL_WRITE_ONLY,
        //                       GL_R32F);
        //
        //    int w = width >> (level + 1);
        //    int h = height >> (level + 1);
        //    glDispatchCompute((w + 7) / 8, (h + 7) / 8, 1);
        //    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        //}

        //for (int i = 0; i < levels; i++) {
        //    texture.subImage(i, 0, 0, width >> i, height >> i,
        //                     GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        //}
    };

protected:
    Scene();
    ~Scene() override;

    void InitOpenGL();
    void InitFont();
    void InitAxes();

    void ResizeFrameBuffer();
    void ResizeHZBTexture();

    void UpdateUniformData();
    void DrawFrame();
    void DrawAxes();

    std::map<DataObjectId, DataObject::Pointer> m_Models;
    DataObjectId m_CurrentObjectId{ 0 };
    DataObject* m_CurrentObject{ nullptr };

    Camera::Pointer m_Camera{};
    Light::Pointer m_Light{};
    Axes::Pointer m_Axes{};

    MVPMatrix m_MVP;
    UniformBufferObject m_UBO;
    igm::mat4 m_ModelRotate{};
    igm::vec3 m_BackgroundColor{};

    uint32_t m_VisibleModelsCount = 0;
    igm::vec4 m_FirstModelCenter{ 0.0f, 0.0f, 0.0f, 1.0f };

    GLBuffer m_MVPBlock, m_UBOBlock;
    std::map<IGenum, std::unique_ptr<GLShaderProgram>> m_ShaderPrograms;

    GLVertexArray m_ScreenQuadVAO;
    GLBuffer m_ScreenQuadVBO;
    GLFramebuffer m_Framebuffer, m_FramebufferMultisampled;
    GLTexture2d m_ColorTexture, m_DepthTexture;
    GLTexture2d m_HZBTexture;
    uint32_t m_HZBLevels;

    friend class Interactor;
};

IGAME_NAMESPACE_END
#endif // OPENIGAME_SCENCE_H

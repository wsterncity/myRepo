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
        alignas(4) int useColor{0};
    };

    struct DrawCullData {
        igm::mat4 view;
        float P00, P11, znear, zfar; // symmetric projection parameters
        float frustum[4]; // data for left/right/top/bottom frustum planes
        float pyramidWidth, pyramidHeight; // depth pyramid size in texels
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
        if (auto visibility = m_CurrentObject->GetVisibility()) {
            m_VisibleModelsCount--;
        }

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
        for (auto& [id, obj]: m_Models) {
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
        for (auto& [id, obj]: m_Models) {
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

                m_FirstModelCenter =
                        igm::vec4{center[0], center[1], center[2], radius};
                m_Camera->SetCamaraPos(center[0], center[1],
                                       center[2] + 2.0f * radius);
            }
        } else {
            m_VisibleModelsCount--;
        }
    }

    void DrawModels() {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glViewport(0, 0, m_Camera->GetViewPort().x, m_Camera->GetViewPort().y);

        for (auto& [id, obj]: m_Models) {
            obj->ConvertToDrawableData();
            GLCheckError();
            obj->Draw(this);
            GLCheckError();
        }
    }

    std::map<DataObjectId, DataObject::Pointer>& GetModelList() {
        return m_Models;
    }
    
    void RefreshHZBTexture() {
        //uint32_t width = m_Camera->GetViewPort().x;
        //uint32_t height = m_Camera->GetViewPort().y;
        //
        //int level = 2;
        //
        //GLTexture2d::copyImageSubData(m_DepthTexture, GL_TEXTURE_2D, 0, 0, 0, 0,
        //                              m_HZBTexture, GL_TEXTURE_2D, level, 0, 0,
        //                              0, width >> level, height >> level, 1);
        //
        //glBindTexture(GL_TEXTURE_2D, m_HZBTexture);
        //
        //std::vector<float> mipLevelData((width) * (height));
        //glGetTexImage(GL_TEXTURE_2D, level, GL_RED, GL_FLOAT,
        //              mipLevelData.data());
        //
        //int cnt = 0;
        //std::cout << (width >> level) * (height >> level) << std::endl;
        //for (int i = 0; i < (width >> level) * (height >> level); ++i) {
        //    if (mipLevelData[i] < 1.0f) {
        //        cnt++;
        //        //std::cout << mipLevelData[i] << std::endl;
        //        //std::cout << i << std::endl;
        //    }
        //}
        //std::cout << cnt << std::endl;
        //glBindTexture(GL_TEXTURE_2D, 0);

        // copy level 0
        GLTexture2d::copyImageSubData(m_DepthTexture, GL_TEXTURE_2D, 0, 0, 0, 0,
                                      m_DepthPyramid, GL_TEXTURE_2D, 0, 0, 0, 0,
                                      m_DepthPyramidWidth, m_DepthPyramidHeight,
                                      1);

        // generate other level
        for (unsigned int level = 1; level < m_DepthPyramidLevels; ++level) {
            uint32_t width = m_DepthPyramidWidth >> (level - 1);
            uint32_t height = m_DepthPyramidHeight >> (level - 1);
            if (width < 1) width = 1;
            if (height < 1) height = 1;

            uint32_t levelWidth = width >> 1;
            uint32_t levelHeight = height >> 1;
            if (levelWidth < 1) levelWidth = 1;
            if (levelHeight < 1) levelHeight = 1;

            auto shader = GetShader(DEPTHREDUCE);
            shader->use();
            shader->setUniform(shader->getUniformLocation("inImage"),
                               m_DepthPyramid.getTextureHandle());
            shader->setUniform(shader->getUniformLocation("inImageSize"),
                               igm::uvec2{width, height});
            shader->setUniform(shader->getUniformLocation("level"), level);
            m_DepthPyramid.bindImage(0, level, GL_FALSE, 0, GL_WRITE_ONLY,
                                     GL_R32F);

            glDispatchCompute((levelWidth + 15) / 16, (levelHeight + 15) / 16,
                              1);
            glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        }

        //unsigned int level = 0;
        //
        //uint32_t width = m_DepthPyramidWidth >> level;
        //uint32_t height = m_DepthPyramidHeight >> level;
        //if (width < 1) width = 1;
        //if (height < 1) height = 1;
        //
        //glBindTexture(GL_TEXTURE_2D, m_DepthPyramid);
        //std::vector<float> mipLevelData(width * height);
        //glGetTexImage(GL_TEXTURE_2D, level, GL_RED, GL_FLOAT,
        //              mipLevelData.data());
        //
        //int cnt = 0;
        //std::cout << width << std::endl;
        //std::cout << height << std::endl;
        //std::cout << width * height << std::endl;
        //for (int i = 0; i < width * height; ++i) {
        //    if (mipLevelData[i] < 1.0f) {
        //        cnt++;
        //        if (cnt == 1) std::cout << mipLevelData[i] << std::endl;
        //        //std::cout << i << std::endl;
        //    }
        //}
        //std::cout << cnt << std::endl;
        //std::cout << "\n\n\n\n" << std::endl;
        //glBindTexture(GL_TEXTURE_2D, 0);

        //uint32_t width = m_Camera->GetViewPort().x;
        //uint32_t height = m_Camera->GetViewPort().y;
        //
        //unsigned int levels = 1 + static_cast<unsigned int>(std::floor(
        //                                  std::log2(std::max(width, height))));
        //
        //levels = 2;
        //unsigned int level = 0;
        //
        //auto shader = GetShader(DEPTHREDUCE);
        //shader->use();
        //shader->setUniform(shader->getUniformLocation("inImage"),
        //                   m_DepthTexture.getTextureHandle());
        //shader->setUniform(shader->getUniformLocation("inImageSize"),
        //                   igm::uvec2{width, height});
        ////shader->setUniform(shader->getUniformLocation("level"), level);
        //m_HZBTexture.bindImage(0, level, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
        //
        //width = width << 1;
        //height = height << 1;
        //glDispatchCompute((width + 15) / 16, (height + 15) / 16, 1);
        //glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        //for (unsigned int level = 1; level < levels; ++level) {
        //    auto shader = GetShader(DEPTHREDUCE);
        //    shader->use();
        //    shader->setUniform(shader->getUniformLocation("inImage"),
        //                       m_DepthTexture.getTextureHandle());
        //    shader->setUniform(shader->getUniformLocation("imageSize"),
        //                       igm::uvec2{width, height});
        //
        //    int groupX = (width >> (level + 1) + 15) / 16;
        //    int groupY = (height >> (level + 1) + 15) / 16;
        //
        //    glDispatchCompute(groupX, groupY, 1);
        //    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        //}
    };

    GLTexture2d& DepthPyramidTexture() { return m_DepthPyramid; }

    GLBuffer& GetDrawCullDataBuffer() {
        igm::mat4 projection = m_Camera->GetProjectionMatrix();
        igm::mat4 projectionT = projection.transpose();

        auto normalizePlane = [](igm::vec4 p) {
            return p / igm::vec3(p).length();
        };
        igm::vec4 frustumX =
                normalizePlane(projectionT[3] + projectionT[0]); // x + w < 0
        igm::vec4 frustumY =
                normalizePlane(projectionT[3] + projectionT[1]); // y + w < 0

        DrawCullData cullData = {};
        cullData.view = m_Camera->GetViewMatrix();
        cullData.P00 = projection[0][0];
        cullData.P11 = projection[1][1];
        cullData.znear = m_Camera->GetNearPlane();
        cullData.zfar = m_Camera->GetFarPlane();
        cullData.frustum[0] = frustumX.x;
        cullData.frustum[1] = frustumX.z;
        cullData.frustum[2] = frustumY.y;
        cullData.frustum[3] = frustumY.z;
        cullData.pyramidWidth = static_cast<float>(m_DepthPyramidWidth);
        cullData.pyramidHeight = static_cast<float>(m_DepthPyramidHeight);

        m_DrawCullData.subData(0, sizeof(DrawCullData), &cullData);

        return m_DrawCullData;
    }

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
    DataObjectId m_CurrentObjectId{0};
    DataObject* m_CurrentObject{nullptr};

    Camera::Pointer m_Camera{};
    Light::Pointer m_Light{};
    Axes::Pointer m_Axes{};

    MVPMatrix m_MVP;
    UniformBufferObject m_UBO;
    igm::mat4 m_ModelRotate{};
    igm::vec3 m_BackgroundColor{};

    uint32_t m_VisibleModelsCount = 0;
    igm::vec4 m_FirstModelCenter{0.0f, 0.0f, 0.0f, 1.0f};

    GLBuffer m_MVPBlock, m_UBOBlock;
    std::map<IGenum, std::unique_ptr<GLShaderProgram>> m_ShaderPrograms;

    GLVertexArray m_ScreenQuadVAO;
    GLBuffer m_ScreenQuadVBO;
    GLFramebuffer m_Framebuffer, m_FramebufferMultisampled;
    GLTexture2d m_ColorTexture, m_DepthTexture;

    GLBuffer m_DrawCullData;
    int m_DepthPyramidWidth, m_DepthPyramidHeight, m_DepthPyramidLevels;
    GLTexture2d m_DepthPyramid;

    friend class Interactor;
};

IGAME_NAMESPACE_END
#endif // OPENIGAME_SCENCE_H

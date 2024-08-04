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
        igm::mat4 modelview;
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
    void ChangeDataObjectVisibility(int index, bool visibility);

    void Draw();
    void Resize(int width, int height, int pixelRatio);
    void Update();

    void AddDataObject(DataObject::Pointer obj);
    void RemoveDataObject(DataObject::Pointer obj);
    void RemoveCurrentDataObject();
    bool UpdateCurrentDataObject(int index);
    DataObject* GetDataObject(int index);
    DataObject* GetCurrentObject();
    std::map<DataObjectId, DataObject::Pointer>& GetModelList();

    GLTexture2d& DepthPyramidTexture() { return m_DepthPyramid; }

    // TODO: slove z-buffer Accuracy issues
    GLBuffer& GetDrawCullDataBuffer();

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
    void DrawModels();
    void DrawAxes();
    void RefreshHizTexture();

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

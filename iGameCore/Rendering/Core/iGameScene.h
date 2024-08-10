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
#include "iGameModel.h"

IGAME_NAMESPACE_BEGIN
class Scene : public Object {
public:
    I_OBJECT(Scene);
    static Pointer New() { return new Scene; }

    struct CameraDataBuffer {
        alignas(16) igm::mat4 view;
        alignas(16) igm::mat4 proj;
        alignas(16) igm::mat4 projview;
    };
    struct ObjectDataBuffer {
        alignas(16) igm::mat4 model;
        alignas(16) igm::mat4 normal;
        alignas(16) igm::vec4 spherebounds;
    };
    struct UniformBufferObjectBuffer {
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

    CameraDataBuffer& CameraData() { return this->m_CameraData; }
    ObjectDataBuffer& ObjectData() { return this->m_ObjectData; }
    UniformBufferObjectBuffer& UBO() { return this->m_UBO; }
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
    template<typename Functor, typename... Args>
    void SetUpdateFunctor(Functor&& functor, Args&&... args) {
        m_UpdateFunctor = std::bind(functor, args...);
    }

    void AddDataObject(DataObject::Pointer obj);
    void RemoveDataObject(DataObject::Pointer obj);
    void RemoveCurrentDataObject();
    bool UpdateCurrentDataObject(int index);
    DataObject* GetDataObject(int index);
    DataObject* GetCurrentObject();
    Model* GetCurrentModel();
    std::map<int, Model::Pointer>& GetModelList();

    GLTexture2d& HizTexture() { return m_DepthPyramid; }

    // TODO: slove z-buffer Accuracy issues
    GLBuffer& GetDrawCullDataBuffer();

protected:
    Scene();
    ~Scene() override;

    void UpdateModelsBoundingSphere();

    void InitOpenGL();
    void InitFont();
    void InitAxes();

    void ResizeFrameBuffer();
    void ResizeHizTexture();
    void RefreshHizTexture();

    void UpdateUniformData();
    void DrawFrame();
    void DrawModels();
    void DrawAxes();

    std::function<void()> m_UpdateFunctor;

    std::map<int, Model::Pointer> m_Models;
    int m_CurrentModelId{0};
    Model* m_CurrentModel{nullptr};
    DataObject* m_CurrentObject{nullptr};

    Camera::Pointer m_Camera{};
    Light::Pointer m_Light{};
    Axes::Pointer m_Axes{};

    CameraDataBuffer m_CameraData;
    ObjectDataBuffer m_ObjectData;
    UniformBufferObjectBuffer m_UBO;

    igm::mat4 m_ModelRotate{};
    igm::vec3 m_BackgroundColor{};

    uint32_t m_VisibleModelsCount = 0;
    igm::vec4 m_ModelsBoundingSphere{0.0f, 0.0f, 0.0f, 1.0f};

    GLBuffer m_CameraDataBlock, m_ObjectDataBlock, m_UBOBlock;
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

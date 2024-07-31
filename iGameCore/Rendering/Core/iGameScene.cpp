#include "iGameScene.h"
#include "iGameCommand.h"

IGAME_NAMESPACE_BEGIN
Scene::Scene() {
    m_Camera = Camera::New();
    m_Camera->Initialize(igm::vec3{0.0f, 0.0f, 1.0f});

    m_ModelRotate = igm::mat4(1.0f);
    m_BackgroundColor = {0.5f, 0.5f, 0.5f};

    InitOpenGL();
    InitFont();
    InitAxes();
}
Scene::~Scene() {}

void Scene::SetShader(IGenum type, GLShaderProgram* sp) {
    if (sp == nullptr) { return; }
    m_ShaderPrograms[type] = std::unique_ptr<GLShaderProgram>(sp);
}

GLShaderProgram* Scene::GenShader(IGenum type) {
    GLShaderProgram* sp;
    switch (type) {
        case PATCH: {
            GLShader shader_vert = GLShader{
                    (std::string(ASSEST_DIR) + "/Shaders/shader.vert").c_str(),
                    GL_VERTEX_SHADER};
            GLShader shader_frag = GLShader{
                    (std::string(ASSEST_DIR) + "/Shaders/shader.frag").c_str(),
                    GL_FRAGMENT_SHADER};
            sp = new GLShaderProgram;
            sp->addShaders({shader_vert, shader_frag});
        } break;
        case NOLIGHT: {
            GLShader shader_vert = GLShader{
                    (std::string(ASSEST_DIR) + "/Shaders/shader.vert").c_str(),
                    GL_VERTEX_SHADER};
            GLShader shader_frag = GLShader{
                    (std::string(ASSEST_DIR) + "/Shaders/shaderNoLight.frag")
                            .c_str(),
                    GL_FRAGMENT_SHADER};
            sp = new GLShaderProgram;
            sp->addShaders({shader_vert, shader_frag});
        } break;
        case PURECOLOR: {
            GLShader shader_vert = GLShader{
                    (std::string(ASSEST_DIR) + "/Shaders/shader.vert").c_str(),
                    GL_VERTEX_SHADER};
            GLShader pureColor_frag = GLShader{
                    (std::string(ASSEST_DIR) + "/Shaders/pureColor.frag")
                            .c_str(),
                    GL_FRAGMENT_SHADER};
            sp = new GLShaderProgram;
            sp->addShaders({shader_vert, pureColor_frag});
        } break;
        case AXES: {
            GLShader Axis_vert = GLShader{
                    (std::string(ASSEST_DIR) + "/Shaders/axis.vert").c_str(),
                    GL_VERTEX_SHADER};
            GLShader Axis_frag = GLShader{
                    (std::string(ASSEST_DIR) + "/Shaders/axis.frag").c_str(),
                    GL_FRAGMENT_SHADER};
            sp = new GLShaderProgram;
            sp->addShaders({Axis_vert, Axis_frag});
        } break;
        case FONT: {
            GLShader font_vert = GLShader{
                    (std::string(ASSEST_DIR) + "/Shaders/font.vert").c_str(),
                    GL_VERTEX_SHADER};
            GLShader font_frag = GLShader{
                    (std::string(ASSEST_DIR) + "/Shaders/font.frag").c_str(),
                    GL_FRAGMENT_SHADER};
            sp = new GLShaderProgram;
            sp->addShaders({font_vert, font_frag});
        } break;
        case DEPTHREDUCE: {
            GLShader depthReduce_comp = GLShader{
                    (std::string(ASSEST_DIR) + "/Shaders/depthReduce.comp")
                            .c_str(),
                    GL_COMPUTE_SHADER};
            sp = new GLShaderProgram;
            sp->addShaders({depthReduce_comp});
        } break;
        case MESHLETCULL: {
            GLShader meshletCull_comp = GLShader{
                    (std::string(ASSEST_DIR) + "/Shaders/meshletCull.comp")
                            .c_str(),
                    GL_COMPUTE_SHADER};
            sp = new GLShaderProgram;
            sp->addShaders({meshletCull_comp});
        } break;
        case SCREEN: {
            GLShader screen_vert = GLShader{
                    (std::string(ASSEST_DIR) + "/Shaders/screenShader.vert")
                            .c_str(),
                    GL_VERTEX_SHADER};
            GLShader screen_frag = GLShader{
                    (std::string(ASSEST_DIR) + "/Shaders/screenShader.frag")
                            .c_str(),
                    GL_FRAGMENT_SHADER};
            sp = new GLShaderProgram;
            sp->addShaders({screen_vert, screen_frag});
        } break;
        default:
            break;
    }
    return sp;
}

GLShaderProgram* Scene::GetShaderWithType(IGenum type) {
    auto it = m_ShaderPrograms.find(type);
    if (it == m_ShaderPrograms.end()) { return nullptr; }
    return it->second.get();
}

GLShaderProgram* Scene::GetShader(IGenum type) {
    GLShaderProgram* sp = this->GetShaderWithType(type);
    if (sp != nullptr) { return sp; }

    sp = this->GenShader(type);
    if (sp == nullptr) {
        // std::cout << "Error for GenShader\n";
    }
    this->SetShader(type, sp);
    return sp;
}

bool Scene::HasShader(IGenum type) {
    return this->GetShaderWithType(type) != nullptr;
}

void Scene::InitOpenGL() {
    if (!gladLoadGL()) {
        throw std::runtime_error("Failed to initialize GLAD");
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // allocate memory
    m_MVPBlock.create();
    m_MVPBlock.allocate(sizeof(MVPMatrix), nullptr, GL_STATIC_DRAW);
    m_UBOBlock.create();
    m_UBOBlock.allocate(sizeof(UniformBufferObject), nullptr, GL_STATIC_DRAW);

    auto patchShader = this->GetShader(PATCH);
    patchShader->mapUniformBlock("MVPMatrix", 0, m_MVPBlock);
    patchShader->mapUniformBlock("UniformBufferObject", 1, m_UBOBlock);

    auto noLightShader = this->GetShader(NOLIGHT);
    noLightShader->mapUniformBlock("MVPMatrix", 0, m_MVPBlock);
    noLightShader->mapUniformBlock("UniformBufferObject", 1, m_UBOBlock);

    auto cullComputeShader = this->GetShader(MESHLETCULL);
    cullComputeShader->mapUniformBlock("MVPMatrix", 0, m_MVPBlock);

    m_UBO.useColor = false;

    // init framebuffer
    ResizeFrameBuffer();

    // init screen quad VAO
    m_ScreenQuadVAO.create();
    m_ScreenQuadVBO.create();

    float quadVertices[] = {// positions   // texCoords
                            -1.0f, 1.0f,  0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f,
                            1.0f,  -1.0f, 1.0f, 0.0f, -1.0f, 1.0f,  0.0f, 1.0f,
                            1.0f,  -1.0f, 1.0f, 0.0f, 1.0f,  1.0f,  1.0f, 1.0f};
    m_ScreenQuadVBO.allocate(sizeof(quadVertices), quadVertices,
                             GL_STATIC_DRAW);

    // bind VBO to VAO
    m_ScreenQuadVAO.vertexBuffer(GL_VBO_IDX_0, m_ScreenQuadVBO, 0,
                                 4 * sizeof(float));
    // position
    GLSetVertexAttrib(m_ScreenQuadVAO, GL_LOCATION_IDX_0, GL_VBO_IDX_0, 2,
                      GL_FLOAT, GL_FALSE, 0);
    // texture coord
    GLSetVertexAttrib(m_ScreenQuadVAO, GL_LOCATION_IDX_1, GL_VBO_IDX_0, 2,
                      GL_FLOAT, GL_FALSE, 2 * sizeof(float));
}
void Scene::InitFont() {
    const wchar_t* text = L"XYZ";
    FontSet::Instance().RegisterWords(text);
}
void Scene::InitAxes() {
    auto axesShader = this->GetShader(AXES);
    GLUniform viewLocation = axesShader->getUniformLocation("view");
    GLUniform projLocation = axesShader->getUniformLocation("proj");
    GLUniform viewPosLocation = axesShader->getUniformLocation("viewPos");

    axesShader->use();
    axesShader->setUniform(viewLocation, Axes::ViewMatrix());
    axesShader->setUniform(projLocation, Axes::ProjMatrix());
    axesShader->setUniform(viewPosLocation, Axes::CameraPos());

    m_Axes = Axes::New();
}

void Scene::ResizeFrameBuffer() {
    uint32_t width = m_Camera->GetViewPort().x;
    uint32_t height = m_Camera->GetViewPort().y;

    // multisample framebuffer
    GLint samples = 4;
    //glGetIntegerv(GL_MAX_SAMPLES, &samples);

    {
        GLFramebuffer fbo;
        fbo.create();
        fbo.bind(GL_FRAMEBUFFER);

        GLTexture2dMultisample colorTexture;
        colorTexture.create();
        colorTexture.bind();
        colorTexture.storage(samples, GL_RGB8, width, height, GL_TRUE);
        colorTexture.parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        colorTexture.parameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        colorTexture.getTextureHandle().makeResident();
        fbo.texture(GL_COLOR_ATTACHMENT0, colorTexture, 0);

        GLTexture2dMultisample depthTexture;
        depthTexture.create();
        depthTexture.bind();
        depthTexture.storage(samples, GL_DEPTH_COMPONENT32F, width, height,
                             GL_TRUE);
        depthTexture.parameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        depthTexture.parameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        depthTexture.getTextureHandle().makeResident();
        fbo.texture(GL_DEPTH_ATTACHMENT, depthTexture, 0);

        fbo.release(GL_FRAMEBUFFER);

        m_FramebufferMultisampled = std::move(fbo);

        if (m_FramebufferMultisampled.checkStatus(GL_FRAMEBUFFER) !=
            GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!"
                      << std::endl;
    }

    // second post-processing framebuffer(resolve)
    {
        GLFramebuffer fbo;
        fbo.create();
        fbo.bind(GL_FRAMEBUFFER);

        GLTexture2d colorTexture;
        colorTexture.create();
        colorTexture.bind();
        colorTexture.storage(1, GL_RGB8, width, height);
        colorTexture.parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        colorTexture.parameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        colorTexture.getTextureHandle().makeResident();
        fbo.texture(GL_COLOR_ATTACHMENT0, colorTexture, 0);

        GLTexture2d depthTexture;
        depthTexture.create();
        depthTexture.bind();
        depthTexture.storage(1, GL_DEPTH_COMPONENT32F, width, height);
        depthTexture.parameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        depthTexture.parameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        depthTexture.getTextureHandle().makeResident();
        fbo.texture(GL_DEPTH_ATTACHMENT, depthTexture, 0);

        fbo.release(GL_FRAMEBUFFER);

        m_Framebuffer = std::move(fbo);
        m_ColorTexture = std::move(colorTexture);
        m_DepthTexture = std::move(depthTexture);

        if (m_Framebuffer.checkStatus(GL_FRAMEBUFFER) !=
            GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!"
                      << std::endl;
    }

    ResizeHZBTexture();
}
void Scene::ResizeHZBTexture() {
    uint32_t width = m_Camera->GetViewPort().x;
    uint32_t height = m_Camera->GetViewPort().y;

    unsigned int levels = 1 + static_cast<unsigned int>(std::floor(
                                      std::log2(std::max(width, height))));
    m_HZBLevels = levels;

    GLTexture2d texture;
    texture.create();
    texture.bind();
    texture.storage(levels, GL_DEPTH_COMPONENT32F, width, height);
    texture.parameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    texture.parameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    texture.parameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    texture.parameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    texture.release();

    m_HZBTexture = std::move(texture);
}

void Scene::Draw() {
    // save default framebuffer, because it is not 0 in Qt
    GLint defaultFramebuffer = 0;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &defaultFramebuffer);

    auto width = m_Camera->GetViewPort().x;
    auto height = m_Camera->GetViewPort().y;

    // render to framebuffer
    {
        m_FramebufferMultisampled.bind(GL_FRAMEBUFFER);
        glClearColor(m_BackgroundColor.r, m_BackgroundColor.g,
                     m_BackgroundColor.b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        DrawFrame();
        m_FramebufferMultisampled.release(GL_FRAMEBUFFER);
        //return;
    }

    // blit multisampled buffer(s) to normal colorbuffer of intermediate FBO.
    {
        m_FramebufferMultisampled.bind(GL_READ_FRAMEBUFFER);
        m_Framebuffer.bind(GL_DRAW_FRAMEBUFFER);
        glBlitFramebuffer(0, 0, width, height, 0, 0, width, height,
                          GL_COLOR_BUFFER_BIT, GL_NEAREST);
        glBlitFramebuffer(0, 0, width, height, 0, 0, width, height,
                          GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    // render to screen
    {
        glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
        glViewport(0, 0, width, height);
        glClearColor(m_BackgroundColor.r, m_BackgroundColor.g,
                     m_BackgroundColor.b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);

        auto shader = GetShader(Scene::SCREEN);
        shader->use();

        GLUniform textureUniform = shader->getUniformLocation("screenTexture");
        shader->setUniform(textureUniform, m_ColorTexture.getTextureHandle());
        //shader->setUniform(textureUniform, m_DepthTexture.getTextureHandle());
        m_ScreenQuadVAO.bind();
        glDrawArrays(GL_TRIANGLES, 0, 6);
        m_ScreenQuadVAO.release();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

void Scene::Update() { this->Draw(); }

void Scene::Resize(int width, int height, int pixelRatio) {
    m_Camera->SetViewPort(width, height);
    m_Camera->SetDevicePixelRatio(pixelRatio);
    ResizeFrameBuffer();
}

void Scene::DrawFrame() {
    // update ubo data in CPU
    UpdateUniformData();

    DrawModels();
    DrawAxes();
}

void Scene::UpdateUniformData() {
    // update mvp matrix
    igm::mat4 translateToOrigin =
            igm::translate(igm::mat4(1.0f), -igm::vec3{0.0f, 0.0f, 0.0f});
    igm::mat4 translateBack =
            igm::translate(igm::mat4(1.0f), igm::vec3{0.0f, 0.0f, 0.0f});

    m_MVP.model = translateBack * m_ModelRotate * translateToOrigin;
    m_MVP.normal = m_MVP.model.invert().transpose();
    m_MVP.viewporj =
            m_Camera->GetProjectionMatrix() * m_Camera->GetViewMatrix();

    // update other ubo
    m_UBO.viewPos = m_Camera->GetCamaraPos();
}

void Scene::UpdateUniformBuffer() {
    // update mvp matrix
    m_MVPBlock.bind(GL_UNIFORM_BUFFER);
    m_MVPBlock.subData(0, sizeof(MVPMatrix), &m_MVP);

    // update other ubo
    m_UBOBlock.bind(GL_UNIFORM_BUFFER);
    m_UBOBlock.subData(0, sizeof(UniformBufferObject), &m_UBO);
}

void Scene::DrawAxes() {
    //glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glViewport(0, 0, 200, 200);

    //draw Axes
    auto axesShader = this->GetShader(Scene::AXES);
    GLUniform modelLocation = axesShader->getUniformLocation("model");

    axesShader->use();
    axesShader->setUniform(modelLocation, m_ModelRotate);

    m_Axes->DrawAxes();

    // draw Axes Font
    auto fontShader = this->GetShader(Scene::FONT);
    GLUniform projLocation = fontShader->getUniformLocation("proj");
    auto mvp = Axes::ProjMatrix() * Axes::ViewMatrix() * m_ModelRotate;

    fontShader->use();
    fontShader->setUniform(projLocation, mvp);

    m_Axes->Update(mvp, {0, 0, 200, 200});

    GLUniform textureUniform = fontShader->getUniformLocation("textureHandle");
    GLUniform colorUniform = fontShader->getUniformLocation("textColor");
    m_Axes->DrawXYZ(fontShader, textureUniform, colorUniform);
}

void Scene::ChangeViewStyle(IGenum mode) {
    m_CurrentObject->SetViewStyle(mode);
}

IGAME_NAMESPACE_END
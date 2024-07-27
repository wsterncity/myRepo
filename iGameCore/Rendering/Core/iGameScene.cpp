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
        case CULLING: {
            GLShader culling_vert = GLShader{
                    (std::string(ASSEST_DIR) + "/Shaders/culling.vert").c_str(),
                    GL_VERTEX_SHADER};
            GLShader culling_frag = GLShader{
                    (std::string(ASSEST_DIR) + "/Shaders/culling.frag").c_str(),
                    GL_FRAGMENT_SHADER};
            sp = new GLShaderProgram;
            sp->addShaders({culling_vert, culling_frag});
        } break;
        case CULLINGCOMPUTE: {

            sp = new GLShaderProgram;
            sp->addShaders({});
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
    glGenBuffers(1, &m_MVPBlock);
    glBindBuffer(GL_UNIFORM_BUFFER, m_MVPBlock);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(MVPMatrix), NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glGenBuffers(1, &m_UBOBlock);
    glBindBuffer(GL_UNIFORM_BUFFER, m_UBOBlock);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(UniformBufferObject), NULL,
                 GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    auto mapUniformBlock = [&](unsigned int programId,
                               const char* uniformBlockName,
                               uint32_t uniformBlockBinding,
                               unsigned int m_UBOBlock) {
        // do not forget generate and allocate memory before map uniform block
        GLuint blockIndex = glGetUniformBlockIndex(programId, uniformBlockName);
        glUniformBlockBinding(programId, blockIndex, uniformBlockBinding);
        glBindBufferBase(GL_UNIFORM_BUFFER, uniformBlockBinding, m_UBOBlock);
    };

    auto patchShader = this->GetShader(PATCH);
    mapUniformBlock(patchShader->programID(), "MVPMatrix", 0, m_MVPBlock);
    mapUniformBlock(patchShader->programID(), "UniformBufferObject", 1,
                    m_UBOBlock);

    auto noLightShader = this->GetShader(NOLIGHT);
    mapUniformBlock(noLightShader->programID(), "MVPMatrix", 0, m_MVPBlock);
    mapUniformBlock(noLightShader->programID(), "UniformBufferObject", 1,
                    m_UBOBlock);

    auto cullingShader = this->GetShader(CULLING);
    mapUniformBlock(cullingShader->programID(), "MVPMatrix", 0, m_MVPBlock);

    m_UBO.useColor = false;
}
void Scene::InitFont() {
    const wchar_t* text = L"xyz";
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

void Scene::Draw() {
    glClearColor(m_BackgroundColor.r, m_BackgroundColor.g, m_BackgroundColor.b,
                 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    DrawFrame();
}

void Scene::Update() { this->Draw(); }

void Scene::Resize(int width, int height, int pixelRatio) {
    m_Camera->SetViewPort(width, height);
    m_Camera->SetDevicePixelRatio(pixelRatio);
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
    m_MVP.mvp = m_Camera->GetProjectionMatrix() * m_Camera->GetViewMatrix() *
                m_MVP.model;

    // update other ubo
    m_UBO.viewPos = m_Camera->GetCamaraPos();
}

void Scene::UpdateUniformBuffer() {
    // update mvp matrix
    glBindBuffer(GL_UNIFORM_BUFFER, m_MVPBlock);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(MVPMatrix), &m_MVP, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // update other ubo
    glBindBuffer(GL_UNIFORM_BUFFER, m_UBOBlock);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(UniformBufferObject), &m_UBO,
                 GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Scene::DrawAxes() {
    glClear(GL_DEPTH_BUFFER_BIT);
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
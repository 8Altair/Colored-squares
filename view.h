#ifndef COLORED_SQUARES_VIEW_H
#define COLORED_SQUARES_VIEW_H

#include <QOpenGLWidget>    // QOpenGLWidget base class (custom GL widget)
#include <QOpenGLFunctions_4_5_Core>    // Provides GL 4.5 core functions after initializeOpenGLFunctions()

#include <glm/glm.hpp>  // GLM core types (mat4, vec4, vec3, etc.)
#include <glm/gtc/matrix_transform.hpp> // GLM transformations (translate, rotate, scale, ortho)
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr for sending matrices to shader


using namespace std;


QT_BEGIN_NAMESPACE

namespace Ui { class View; }    // Forward declaration for uic-generated UI class (if a .ui exists)

QT_END_NAMESPACE

class QOpenGLShaderProgram; // Forward declare shader program (a unique_ptr is kept to it)

class View final : public QOpenGLWidget, protected QOpenGLFunctions_4_5_Core // Class View inherits QOpenGLWidget and QOpenGLFunctions_4_5_Core
{
    Q_OBJECT

    Ui::View *ui{}; // Pointer to generated UI (only if .ui is kept for this widget)

    GLuint shader_program_id = 0;   // OpenGL shader program ID (compiled+linked GLSL program); it identifies the linked vertex + fragment shader pair used for rendering
    GLint uniform_location_mvp = -1;    // Uniform location for MVP matrix (cached after link)
    GLint uniform_location_color = -1;  // Uniform location for per-draw color (cached after link)

    // Raw GL objects
    GLuint vertex_array_object = 0;   // Vertex Array Object handle
    GLuint vertex_buffer_object = 0;   // Vertex Buffer Object handle

    glm::mat4 projection{};  // Projection matrix (orthographic in this 2D scene)
    glm::mat4 view_matrix{}; // View matrix (camera), here identity for 2D

    void setup_shaders();   // Create, compile, link shaders; fetch uniform locations
    void setup_geometry();  // Create VAO/VBO and upload unit-square vertex data
    void draw_square(const glm::mat4 &model, const glm::vec4 &color);  // Set uniforms and draw 6 vertices for one square

protected:
    void initializeGL() override;   // Called once: load GL functions, create buffers/shaders, states
    void resizeGL(int w, int h) override;   // Called on resize: update viewport/projection
    void paintGL() override;    // Called to render a frame: bind VAO, set uniforms, draw

public:
    explicit View(QWidget *parent = nullptr);   // Constructor
    ~View() override;   // Destructor
};


#endif //COLORED_SQUARES_VIEW_H
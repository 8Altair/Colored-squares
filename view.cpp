#include "view.h"


View::View(QWidget *parent) : QOpenGLWidget(parent)
{
    setMinimumSize(400, 300);
}

View::~View()
{
    makeCurrent();    // Make this widget's OpenGL context current; required so GL calls below operate on the right context

    /* If a vertex buffer (VBO) was created (non-zero ID), delete it from GPU memory to free VRAM
       Then reset its handle to 0 (the “no buffer” default value). */
    if (vertex_buffer_object) glDeleteBuffers(1, &vertex_buffer_object); vertex_buffer_object = 0;
    /* If a vertex array object (VAO) exists, delete it to release GPU state resources.
       Reset the handle to 0 to mark it invalid/unused. */
    if (vertex_array_object) glDeleteVertexArrays(1, &vertex_array_object); vertex_array_object = 0;
    /* If the shader program was successfully created, delete it from the GPU.
       Reset to 0 to indicate no active program is bound to this object anymore. */
    if (shader_program_id) glDeleteProgram(shader_program_id); shader_program_id = 0;

    doneCurrent();    // Release the current OpenGL context; Qt’s cleanup convention after finishing GL operations
}

void View::initializeGL()
{
    initializeOpenGLFunctions();    // Enables 4.5 core entry points via QOpenGLFunctions_4_5_Core

    glDisable(GL_DEPTH_TEST);   // Disable depth test
    glClearColor(0.10f, 0.10f, 0.12f, 1.0f);    // Set the background color for the next frame (dark blue-gray)

    setup_shaders();
    setup_geometry();

    // Static camera/view matrix
    view_matrix = glm::mat4(1.0f);
}

void View::resizeGL(const int w, const int h)
{
    glViewport(0, 0, w, h);    // Define the drawable region of the window: start at (0,0) and cover the full width and height in pixels
    const float aspect = h > 0 ? static_cast<float>(w) / static_cast<float>(h) : 1.0f;
    projection = glm::mat4(1.0f);    // Reset projection matrix
    constexpr float half_height = 3.0f;
    const float half_width = aspect * half_height;
    projection = glm::ortho(-half_width, half_width,   // Left, right
                 -half_height, half_height, // Bottom, top
                 -1.0f, 1.0f);  // Near, far
}

void View::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the framebuffer at the start of each frame

    glUseProgram(shader_program_id);    // Activate the shader program
    glBindVertexArray(vertex_array_object); // Bind the VAO (Vertex Array Object) that defines how to interpret vertex data in the VBO

    constexpr float half_height = 3.0f;   // Vertical half-size of the ortho box
    constexpr float ground_height = 0.35f;  // Thickness of the ground strip
    constexpr float square_size = 1.0f;    // Unit square height in XY after rotation
    constexpr float ground_gap = 0.32f; // Gap between ground top and base-row bottom
    constexpr float inter_row_gap = 0.0f;  // Vertical gap between rows
    constexpr float pyramid_offset_y = 0.0f;    // Shift the whole pyramid up/down together

    constexpr float ground_top_y = -half_height + ground_height;
    constexpr float base_row_center_y = ground_top_y + square_size * 0.5f + ground_gap + pyramid_offset_y;
    constexpr float second_row_center_y = base_row_center_y + square_size + inter_row_gap;
    constexpr float top_row_center_y = second_row_center_y + square_size + inter_row_gap;
    constexpr float zero = 0.0f;

    // Ground (reused from the same unit square geometry)
    {
        constexpr float ground_width = 10.0f;  // How wide the strip is
        auto model = glm::mat4(1.0f);   // Reset to identity: start with no transformations

        model = glm::translate(model, glm::vec3(zero, -half_height + ground_height * 0.5f, zero));   // Translate by (-half_height + ground_height/2 ) because it is scaled from the center
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, zero, zero)); // Rotate -90° around X so the unit square (originally in XZ) becomes a rectangle in XY
        model = glm::scale(model, glm::vec3(ground_width, ground_height, 1.0f)); // Scale to a long, thin rectangle in XY
        draw_square(model, glm::vec4(0.55f, 0.55f, 0.55f, 1.0f)); // Draw gray ground
    }

    // Pyramid of colored squares (4-3-1 formation)
    // Base row (4 squares along the bottom of the wall)
    {
        constexpr float y = base_row_center_y;  // Vertical position (Y) of the entire bottom row
        auto m = glm::mat4(1.0f);   // A local transformation matrix for each square

        m = glm::translate(m, glm::vec3(-1.5f, y, zero));    // Move left (X = -1.5) and down to Y = -2.5
        m = glm::rotate(m, glm::radians(-90.0f), glm::vec3(1.0f, zero, zero)); // Rotate -90° around X axis so the square faces forward (toward camera)
        draw_square(m, glm::vec4(1.0f, 0.2f, 0.2f, 1.0f));  // Draw red square (RGBA = bright red)

        m = glm::mat4(1.0f);
        m = glm::translate(m, glm::vec3(-0.5f, y, zero));
        m = glm::rotate(m, glm::radians(-90.0f), glm::vec3(1.0f, zero, zero));
        draw_square(m, glm::vec4(0.2f, 1.0f, 0.2f, 1.0f));  // Draw green square

        m = glm::mat4(1.0f);
        m = glm::translate(m, glm::vec3(0.5f, y, zero));
        m = glm::rotate(m, glm::radians(-90.0f), glm::vec3(1.0f, zero, zero));
        draw_square(m, glm::vec4(0.2f, 0.6f, 1.0f, 1.0f));  /// Draw light blue square

        m = glm::mat4(1.0f);
        m = glm::translate(m, glm::vec3(1.5f, y, zero));
        m = glm::rotate(m, glm::radians(-90.0f), glm::vec3(1.0f, zero, zero));
        draw_square(m, glm::vec4(1.0f, 0.6f, 0.2f, 1.0f));  // Draw orange square
    }

    // Second row (3 squares centered above base row)
    {
        constexpr float y = second_row_center_y;
        auto m = glm::mat4(1.0f);

        m = glm::translate(m, glm::vec3(-1.0f, y, zero));
        m = glm::rotate(m, glm::radians(-90.0f), glm::vec3(1.0f, zero, zero));
        draw_square(m, glm::vec4(0.8f, 0.2f, 1.0f, 1.0f));  // Draw bright violet square

        m = glm::mat4(1.0f);
        m = glm::translate(m, glm::vec3(0.0f, y, 0.0f));
        m = glm::rotate(m, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        draw_square(m, glm::vec4(1.0f, 0.9f, 0.2f, 1.0f));  // Draw gold square

        m = glm::mat4(1.0f);
        m = glm::translate(m, glm::vec3(1.0f, y, 0.0f));
        m = glm::rotate(m, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        draw_square(m, glm::vec4(0.2f, 1.0f, 0.8f, 1.0f));  // Draw turquoise square
    }

    // Top row (1 square centered above second row)
    {
        constexpr float y = top_row_center_y;
        auto m = glm::mat4(1.0f);

        m = glm::translate(m, glm::vec3(zero, y, zero));
        m = glm::rotate(m, glm::radians(-90.0f), glm::vec3(1.0f, zero, zero));
        draw_square(m, glm::vec4(0.9f, 0.3f, 0.4f, 1.0f));  // Draw reddish-pink square
    }

    glBindVertexArray(0);   // Unbind VAO
    glUseProgram(0);    // Unbind the shader program (stop using it)
}

void View::setup_shaders()
{
    /* Vertex shader: transforms vertex position to clip space using MVP matrix */
    static auto vertex_shader_source = R"(#version 450 core
    layout(location = 0) in vec3 position;
    uniform mat4 mvp;
    void main()
    {
        gl_Position = mvp * vec4(position, 1.0);
    }
    )";

    /* Fragment shader: outputs a uniform color for the square */
    static auto fragment_shader_source = R"(#version 450 core
    uniform vec4 color;
    out vec4 FragColor;
    void main()
    {
        FragColor = color;
    }
    )";

    shader_program_id = glCreateProgram();  // Create a new OpenGL shader program
    const GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, nullptr);
    glCompileShader(vertex_shader); // Compile vertex shader
    glAttachShader(shader_program_id, vertex_shader);   // Attach compiled vertex shader
    const GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, nullptr);
    glCompileShader(fragment_shader);   // Compile fragment shader
    glAttachShader(shader_program_id, fragment_shader); // Attach compiled fragment shader
    glLinkProgram(shader_program_id);   // Link vertex + fragment into program
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    // Cache uniform locations
    uniform_location_mvp = glGetUniformLocation(shader_program_id, "mvp");
    uniform_location_color = glGetUniformLocation(shader_program_id, "color");
}

void View::setup_geometry()
{
    // Define a unit square lying in the XZ plane (y=0) made from two triangles
    /* "CCW winding" means the vertices of each triangle are specified in Counter-Clockwise order,
       which OpenGL interprets as the *front face* of the polygon (visible side when face culling is used). */
    constexpr GLfloat unit_square_vertices[6 * 3] =  // 6 vertices × 3 coordinates (x, y, z)
    {
        // Triangle 1 (first half of the square)
        -0.5f, 0.0f, -0.5f, // Bottom-left vertex
         0.5f, 0.0f, -0.5f, // Bottom-right vertex
         0.5f, 0.0f,  0.5f, // Top-right vertex
        // Triangle 2 (second half of the square)
        -0.5f, 0.0f, -0.5f, // Bottom-left vertex (reused)
         0.5f, 0.0f,  0.5f, // Top-right vertex (reused)
        -0.5f, 0.0f,  0.5f  // top-left vertex
    };

    // VAO: remember vertex attribute layout + VBO bindings
    glGenVertexArrays(1, &vertex_array_object);
    glBindVertexArray(vertex_array_object);

    // VBO: upload vertex data once
    glGenBuffers(1, &vertex_buffer_object);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);
    glBufferData(GL_ARRAY_BUFFER, sizeof(unit_square_vertices), unit_square_vertices,GL_STATIC_DRAW);

    // Vertex format: layout(location=0) = vec3 position, tightly packed
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0,                      // Attribute index (matches shader layout(location=0))
        3,                      // 3 floats per vertex (x,y,z)
        GL_FLOAT,               // Data type
        GL_FALSE,               // No normalization
        3 * sizeof(GLfloat),    // Stride: tightly packed vec3
        nullptr                 // Offset into buffer
    );

    // Clean up binds
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void View::draw_square(const glm::mat4 &model, const glm::vec4 &color) // Draw one square with a given transform and RGBA color
{
    const glm::mat4 mvp = projection * view_matrix * model;    // Build the final transform: Model→View→Projection (rightmost applied first)

    // Send uniforms with raw GL calls
    glUniformMatrix4fv(uniform_location_mvp, 1, GL_FALSE, glm::value_ptr(mvp)); // Upload a 4x4 float matrix uniform to the current shader program
    /* Location of "mvp" uniform, number of matrices being sent (just one), do NOT transpose
       (QMatrix4x4 is already column-major for OpenGL), pointer to the 16 floats (contiguous) inside QMatrix4x4 */
    glUniform4f(uniform_location_color, color.r, color.g, color.b, color.a);    // Upload a vec4 uniform to the fragment shader (the color)
    // Location of "color" uniform, RGBA components as floats in [0,1]

    glDrawArrays(GL_TRIANGLES, 0, 6);   // Issue the actual draw call using the currently bound VAO/VBO
    /* Interpret vertex stream as independent triangles, start at vertex 0 in the bound vertex buffer, draw 6 vertices
       (2 triangles) that form one square */
}

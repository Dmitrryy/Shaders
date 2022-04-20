
#include <Sample.hpp>

#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui-filebrowser/imfilebrowser.h>

#include <iostream>
#include <fstream>

static const char *vertexShaderSource = "#version 330 core\n"
                                        "layout (location = 0) in vec3 aPos;\n"
                                        "void main()\n"
                                        "{\n"
                                        "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                        "}\0";

namespace sh
{
    void framebuffer_size_callback(GLFWwindow *window, int width, int height);
    std::vector<GLchar> readFile(const std::string &filename);

    Sample::Sample(unsigned width, unsigned height, const std::string &shaderRootPath)
        : m_rootPath(shaderRootPath)
    {
        // glfw: initialize and configure
        // ------------------------------
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        m_window = glfwCreateWindow(width, height, "Shading", NULL, NULL);
        if (m_window == NULL)
            throw std::runtime_error("Failed to create GLFW window");

        glfwMakeContextCurrent(m_window);
        glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);

        // enable vsync
        glfwSwapInterval(1);

        // glad: load all OpenGL function pointers
        // ---------------------------------------
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
            throw std::runtime_error("Failed to initialize GLAD");

        // imgui: setup
        // ---------------------------------------
        ImGui::CreateContext();
        ImGui::StyleColorsDark();

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(m_window, true);
        ImGui_ImplOpenGL3_Init("#version 130");

        // set up vertex data (and buffer(s)) and configure vertex attributes
        // ------------------------------------------------------------------
        float vertices[] = {
            1.0f, 1.f, 0.0f,    // top right
            1.0f, -1.0f, 0.0f,  // bottom right
            -1.0f, -1.0f, 0.0f, // bottom left
            -1.0f, 1.0f, 0.0f   // top left
        };
        unsigned int indices[] = {
            // note that we start from 0!
            0, 1, 3, // first triangle
            1, 2, 3  // second triangle
        };
        glGenBuffers(1, &m_EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);
        // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
        glBindVertexArray(m_VAO);

        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
    }

    Sample::~Sample()
    {
        glDeleteVertexArrays(1, &m_VAO);
        glDeleteBuffers(1, &m_VBO);
        glDeleteProgram(m_shaderProgram);

        glfwTerminate();
    }

    void Sample::compile(const std::filesystem::path &fragShaderPath)
    {
        m_changeTime = std::filesystem::last_write_time(fragShaderPath);
        if (m_isCompiled)
        {
            glDeleteProgram(m_shaderProgram);
            m_isCompiled = false;
        }
        // vertex shader
        unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
        glCompileShader(vertexShader);
        // check for shader compile errors
        int success;
        char infoLog[512];
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
            m_errorLog = infoLog;

            glDeleteShader(vertexShader);

            return;
        }
        // fragment shader
        unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        auto &&fragmentShaderSource = readFile(fragShaderPath.string());
        auto *tmp = fragmentShaderSource.data();
        glShaderSource(fragmentShader, 1, &tmp, NULL);
        glCompileShader(fragmentShader);
        // check for shader compile errors
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
            m_errorLog = infoLog;

            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
            return;
        }
        // link shaders
        m_shaderProgram = glCreateProgram();
        glAttachShader(m_shaderProgram, vertexShader);
        glAttachShader(m_shaderProgram, fragmentShader);
        glLinkProgram(m_shaderProgram);
        // check for linking errors
        glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(m_shaderProgram, 512, NULL, infoLog);
            m_errorLog = infoLog;

            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
            glDeleteProgram(m_shaderProgram);
        }
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        m_isCompiled = true;
        m_errorLog.clear();
    }

    void Sample::run()
    {
        // create a file browser instance
        ImGui::FileBrowser fileDialog;

        // (optional) set browser properties
        fileDialog.SetTitle("Select frag shader");
        fileDialog.SetTypeFilters({".frag"});

        Timer timer;
        // render loop
        // -----------
        while (!glfwWindowShouldClose(m_window))
        {
            if (!m_isSleep)
                m_time += timer.elapsed();
            timer.reset();

            if (!m_currentShader.empty()) {
                auto&& curTime = std::filesystem::last_write_time(m_currentShader);
                if (curTime > m_changeTime) {
                    compile(m_currentShader);
                }
            }

            // input
            // -----
            processInput();

            // Start the Dear ImGui frame
            // -----
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            ImGui::Begin("Info"); // Create a window called "Hello, world!" and append into it.

            ImGui::Text("Current shader:");
            ImGui::SameLine();
            ImGui::Text(m_currentShader.filename().string().data());
            ImGui::Text("Compiled: %s", (m_isCompiled) ? "yes" : "no");
            ImGui::InputDouble("Time", &m_time, 0.2);
            ImGui::Checkbox("Sleep", &m_isSleep);
            double xpos, ypos;
            glfwGetCursorPos(m_window, &xpos, &ypos);
            ImGui::Text("Cursor position (%lf, %lf)", xpos, ypos);
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

            // open file dialog when user clicks this button
            if (ImGui::Button("open file dialog"))
            {
                if (!m_rootPath.empty())
                    fileDialog.SetPwd(m_rootPath);
                fileDialog.Open();
            }
            ImGui::SameLine();
            if (ImGui::Button("Recompile") && !m_currentShader.empty())
            {
                compile(m_currentShader);
            }

            fileDialog.Display();
            if (fileDialog.HasSelected())
            {
                compile(fileDialog.GetSelected());
                m_currentShader = fileDialog.GetSelected();
                fileDialog.ClearSelected();
            }

            ImGui::Text("Compile Log:");
            if (!m_errorLog.empty())
            {
                ImGui::Text(m_errorLog.data());
            }
            else
            {
                ImGui::Text("success");
            }

            ImGui::End();
            // -----
            // End the Dear ImGui frame
            if (m_isCompiled)
            {
                glUseProgram(m_shaderProgram);

                updateUniforms();

                glBindVertexArray(m_VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            }

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
            // -------------------------------------------------------------------------------
            glfwSwapBuffers(m_window);
            glfwPollEvents();
        }
    }

    void Sample::updateUniforms() const
    {
        int timeLocation = glGetUniformLocation(m_shaderProgram, "u_time");
        glUniform1f(timeLocation, m_time);

        int resolutionLocation = glGetUniformLocation(m_shaderProgram, "u_resolution");
        GLint vSize[4] = {};
        glGetIntegerv(GL_VIEWPORT, vSize);
        glUniform2f(resolutionLocation, vSize[2], vSize[3]);

        int mouseLocation = glGetUniformLocation(m_shaderProgram, "u_mouse");
        double xpos, ypos;
        glfwGetCursorPos(m_window, &xpos, &ypos);
        glUniform2f(mouseLocation, xpos, ypos);
    }

    void Sample::processInput()
    {
        if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(m_window, true);
    }

    // glfw: whenever the window size changed (by OS or user resize) this callback function executes
    // ---------------------------------------------------------------------------------------------
    void framebuffer_size_callback(GLFWwindow *window, int width, int height)
    {
        // make sure the viewport matches the new window dimensions; note that width and
        // height will be significantly larger than specified on retina displays.
        glViewport(0, 0, width, height);
    }

    std::vector<GLchar> readFile(const std::string &filename)
    {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);
        if (!file.is_open())
        {
            throw std::runtime_error("failed to open file!");
        }
        auto fileSize = file.tellg();
        std::vector<char> buffer(fileSize);

        file.seekg(0);

        file.read(buffer.data(), fileSize);
        buffer.push_back('\0');

        file.close();
        return buffer;
    }
} // namespace sh

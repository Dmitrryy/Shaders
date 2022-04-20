#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <string>
#include <filesystem>
#include <chrono>

namespace sh
{

    class Sample final
    {
        GLFWwindow *m_window = {};
        std::string m_rootPath;

        std::filesystem::path m_currentShader;
        std::filesystem::file_time_type m_changeTime;

        double m_time = {};
        bool m_isSleep = false;

        std::string m_errorLog;
        bool m_isCompiled = false;
        unsigned m_shaderProgram = {};
        unsigned m_EBO = {};
        unsigned m_VBO = {};
        unsigned m_VAO = {};

    public:
        Sample(unsigned width, unsigned height, const std::string &shaderRootPath = {});

        ~Sample();

    public:
        void run();

    private:
        void compile(const std::filesystem::path &fragShaderPath);

        void processInput();
        void updateUniforms() const;
    }; // class Sample

    class Timer
    {
    private:
        using clock_t = std::chrono::high_resolution_clock;
        using second_t = std::chrono::duration<double, std::ratio<1>>;

        std::chrono::time_point<clock_t> m_beg;

    public:
        Timer() : m_beg(clock_t::now())
        {
        }

        void reset()
        {
            m_beg = clock_t::now();
        }

        double elapsed() const
        {
            return std::chrono::duration_cast<second_t>(clock_t::now() - m_beg).count();
        }
    };

} // namespace sh
#ifndef SHADER_H
#define SHADER_H


#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

class Shader {
public:
    Shader(const std::string vsPath, const std::string fsPath);
    ~Shader();
    void Bind() const;
    void Unbind() const;

    //Set uniforms
    void SetUniform1i(const std::string& name, int value);
    void SetUniform1f(const std::string& name, float value);
    void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3 );
    void SetUniformMat4f(const std::string& name,const glm::mat4&  matrix);

private:
    std::string parseShader (const std::string& filepath);
    unsigned int compileShader (unsigned int type, const std::string& source);
    unsigned int createShader(const std::string& vertexShader, const std::string& fragmentShader);
    int GetUniformLocation (const std::string& name);
private:
    unsigned int rendererID;
    std::unordered_map<std::string, int> uniformLocationCache;
};
#endif // SHADER_H

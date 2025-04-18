/*
 * Copyright 2023 Unionman Technology Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SHADER_H
#define SHADER_H

#include <GLES3/gl32.h>
#include <string>
class Shader {
public:
    GLuint shaderProgram;
    Shader(const char* vertexSource, const char* fragmentSource);
    ~Shader();
    void use() const;
    void setMat4fv(const std::string& name, const GLfloat* mat) const;
    void setVec3(const std::string& name, float x, float y, float z) const;

protected:
    GLuint loadShader(GLuint shaderType, const char* source);
};
#endif

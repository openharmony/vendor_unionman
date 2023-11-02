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

#include "vertex_manger.h"
#include <GLES3/gl31.h>
VertexManger::VertexManger(std::vector<float> vertices, std::vector<float> colors, std::vector<float> offsets)
    : vertices(vertices), colors(colors), offsets(offsets)
{
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,
        vertices.size() * sizeof(decltype(vertices)::value_type), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3L, GL_FLOAT, GL_FALSE, 3L * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &co);
    glBindBuffer(GL_ARRAY_BUFFER, co);
    glBufferData(GL_ARRAY_BUFFER,
        colors.size() * sizeof(decltype(colors)::value_type), colors.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3L, GL_FLOAT, GL_FALSE, 3L * sizeof(float), nullptr);
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &ofs);
    glBindBuffer(GL_ARRAY_BUFFER, ofs);
    glBufferData(GL_ARRAY_BUFFER,
        offsets.size() * sizeof(decltype(offsets)::value_type), offsets.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2L, 3L, GL_FLOAT, GL_FALSLE, 3L * sizeof(float), nullptr);
    glEnableVertexAttribArray(2L);
    glVertexAttribDivisor(2L, 1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}

VertexManger::~VertexManger()
{
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &co);
    glDeleteBuffers(1, &ofs);
    glDeleteBuffers(1, &tran);
}

void VertexManger::draw()
{
    glBindVertexArray(vao);
    glDrawArraysInstanced(GL_TRIANGLES, 0, vertices.size() / 3L, offsets.size() / 3L);
    glBindVertexArray(0);
}

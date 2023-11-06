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

#ifndef OpenglDraw_H
#define OpenglDraw_H

#include <cstdint>
#include <string>
#include <GLES3/gl3.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include "vertex_manger.h"
#include "shader.h"
class OpenglDraw {
public:
    int32_t Init(EGLNativeWindowType windowHandle, int windowWidth, int windowHeight);
    void Update(void);
    int32_t Quit(void);
    void twist(Axis axis, Direction dir);
    float angleX = -45.0f;
    float angleY = 30.0f;
protected:
    EGLNativeWindowType mEglWindow;
    EGLDisplay mEGLDisplay = EGL_NO_DISPLAY;
    EGLConfig mEGLConfig = nullptr;
    EGLContext mEGLContext = EGL_NO_CONTEXT;
    EGLContext mSharedEGLContext = EGL_NO_CONTEXT;
    EGLSurface mEGLSurface = nullptr;
    GLuint mProgramHandle;
    GLint mRotationLocation;
    GLint mTranslationLocation;
    GLint mMoveOriginLocation;
    
    VertexManger* vertexManger = nullptr;
    Shader* shader = nullptr;
};

#endif // OpenglDraw_H

#pragma once

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_OpenHarmony.h>

#include "opensles_common.h"

class IRecorderSink
{
public:
    ~IRecorderSink(){}

public:

    virtual void OnRecordData(SLuint8 *buffer, SLuint32 size) = 0;
};

class OpenSLESRecorder
{
public:
    OpenSLESRecorder(SLObjectItf engine_object, IRecorderSink *sink);
    ~OpenSLESRecorder();
    
public:
    int Terminate();
    int InitRecording();
    int StartRecording();
    int StopRecording();
    
private:

    bool ObtainEngineInterface();
    bool CreateAudioRecorder();
    void DestroyAudioRecorder();
    void AllocateDataBuffers();
    
    static void SimpleBufferQueueCallback(SLOHBufferQueueItf caller, void* context, SLuint32 size);
    void ReadBufferQueue();
    SLuint32 GetRecordState() const;
    SLOHBufferQueueState GetBufferQueueState() const;
    SLuint32 GetBufferCount();
    void LogBufferState() const;
    
private:
    SLObjectItf engine_object_;
    IRecorderSink *m_pSink;
    bool initialized_;
    bool recording_;
    SLDataFormat_PCM pcm_format_;
    
    SLEngineItf engine_;
    ScopedSLObjectItf recorder_object_;
    SLRecordItf recorder_;
    SLOHBufferQueueItf simple_buffer_queue_;
};
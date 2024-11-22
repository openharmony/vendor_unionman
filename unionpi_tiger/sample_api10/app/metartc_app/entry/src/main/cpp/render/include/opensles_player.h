#pragma once

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_OpenHarmony.h>

#include "opensles_common.h"
#include <yangutil/yangavinfotype.h>
#include <yangutil/buffer/YangAudioPlayBuffer.h>

class IPlayerSink
{
public:
    ~IPlayerSink(){}

public:

    virtual void OnGetPlayoutData(SLuint8 *buffer, SLuint32 size) = 0;
};

class OpenSLESPlayer
{
public:
  OpenSLESPlayer(SLObjectItf engine_object, IPlayerSink *sink, YangContext *pcontext);
  ~OpenSLESPlayer();

public:
    int Terminate();
    int InitPlayout();
    int StartPlayout();
    int StopPlayout();
    
private:

    static void SimpleBufferQueueCallback(SLOHBufferQueueItf caller,void* context, SLuint32 size);
    void FillBufferQueue();
    void EnqueuePlayoutData();
    bool ObtainEngineInterface();
    bool CreateMix();
    void DestroyMix();
    bool CreateAudioPlayer();
    void DestroyAudioPlayer();
    SLuint32 GetPlayState() const;

private:
    YangContext *m_context;
    SLObjectItf engine_object_;
    IPlayerSink *m_pSink;
    bool initialized_;
    bool playing_;
    SLDataFormat_PCM pcm_format_;
    
    SLEngineItf engine_;
    ScopedSLObjectItf output_mix_;
    ScopedSLObjectItf player_object_;
    SLPlayItf player_;
    SLOHBufferQueueItf simple_buffer_queue_;
};

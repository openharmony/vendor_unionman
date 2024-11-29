#pragma once

#include <stdio.h>
#include <pthread.h>
#include "opensles_player.h"
#include "opensles_recorder.h"
#include <yangutil/yangavinfotype.h>

class ICAudioWrapperCallback
{
public:
    ~ICAudioWrapperCallback(){}

public:

    virtual void OnRecordData(SLuint8 *buffer, SLuint32 size) = 0;
};

class CAudioWrapper : public IPlayerSink, public IRecorderSink
{
public:
    CAudioWrapper(ICAudioWrapperCallback *callback);
    CAudioWrapper(YangContext *pcontext);
    ~CAudioWrapper();
    
public:
    
    bool StartPlay(const char *distFile);
    void StopPlay();
    bool StartAudioRecorder(const char *dstRecordPCMFile);
    void StopAudioRecorder();
    
public:
    virtual void OnGetPlayoutData(SLuint8 *buffer, SLuint32 size);
    virtual void OnRecordData(SLuint8 *buffer, SLuint32 size);
    
private:
    
	bool StartRecord(const char *dstFileName);
    bool StartPlayer(const char *dstFileName);
    void DestroyPlayResources();
    void DestroyRecordResources();
    
private:
    YangContext *m_context; 
    ScopedSLObjectItf engine_object_;
    OpenSLESPlayer *m_pPlayer;
    OpenSLESRecorder *m_pRecorder;
    
    FILE *m_pSrcPCMFile4Play;
    long m_nFileSize;
    long m_nRemainSize;
    
    FILE *m_pDstPlayFile;
    FILE *m_pDstRecordFile;
    pthread_mutex_t m_mutex;
    ICAudioWrapperCallback *audioRecordDataCallback_;
};
#include "AudioWrapper.h"
#include <memory>
#include "hilog/log.h"
#define LOGI(...) ((void)OH_LOG_Print(LOG_APP, LOG_INFO, LOG_DOMAIN, "CAudioWrapper", __VA_ARGS__))
#define LOGD(...) ((void)OH_LOG_Print(LOG_APP, LOG_INFO, LOG_DOMAIN, "CAudioWrapper", __VA_ARGS__))
#define LOGW(...) ((void)OH_LOG_Print(LOG_APP, LOG_WARN, LOG_DOMAIN, "CAudioWrapper", __VA_ARGS__))
#define LOGE(...) ((void)OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "CAudioWrapper", __VA_ARGS__))

CAudioWrapper::CAudioWrapper(ICAudioWrapperCallback *callback)
: m_pPlayer(nullptr)
, m_pDstPlayFile(nullptr)
, m_pDstRecordFile(nullptr)
, m_pSrcPCMFile4Play(nullptr)
, m_nFileSize(0)
, m_nRemainSize(0)
, audioRecordDataCallback_(callback)
, m_pRecorder(nullptr)
{
    LOGD("CAudioWrapper with param callback");
    
    pthread_mutex_init(&m_mutex, NULL);
    
    const SLEngineOption option[] = {{SL_ENGINEOPTION_THREADSAFE, static_cast<SLuint32>(SL_BOOLEAN_TRUE)}};
    SLresult result = slCreateEngine(engine_object_.Receive(), 1, option, 0, NULL, NULL);
    if (result != SL_RESULT_SUCCESS)
    {
        LOGE("slCreateEngine() failed: %{public}s", GetSLErrorString(result));
        engine_object_.Reset();
        return;
    }
    
    result = engine_object_->Realize(engine_object_.Get(), SL_BOOLEAN_FALSE);
    if (result != SL_RESULT_SUCCESS)
    {
        LOGE("Realize() failed: %{public}s", GetSLErrorString(result));
        engine_object_.Reset();
        return;
    }
}

CAudioWrapper::CAudioWrapper(YangContext *pcontext)
: m_pPlayer(nullptr)
, m_pDstPlayFile(nullptr)
, m_pDstRecordFile(nullptr)
, m_pSrcPCMFile4Play(nullptr)
, m_nFileSize(0)
, m_nRemainSize(0)
{
    LOGD("CAudioWrapper witch param YangContext");
    
    pthread_mutex_init(&m_mutex, NULL);
    
    const SLEngineOption option[] = {{SL_ENGINEOPTION_THREADSAFE, static_cast<SLuint32>(SL_BOOLEAN_TRUE)}};
    SLresult result = slCreateEngine(engine_object_.Receive(), 1, option, 0, NULL, NULL);
    if (result != SL_RESULT_SUCCESS)
    {
        LOGE("slCreateEngine() failed: %{public}s", GetSLErrorString(result));
        engine_object_.Reset();
        return;
    }
    
    result = engine_object_->Realize(engine_object_.Get(), SL_BOOLEAN_FALSE);
    if (result != SL_RESULT_SUCCESS)
    {
        LOGE("Realize() failed: %{public}s", GetSLErrorString(result));
        engine_object_.Reset();
        return;
    }
    m_context = pcontext;
}

CAudioWrapper::~CAudioWrapper()
{
    LOGD("~CAudioWrapper");
    
//    Stop();
    pthread_mutex_destroy(&m_mutex);
}

bool CAudioWrapper::StartPlay(const char *distFile)
{
    LOGI("CAudioWrapper Start");
    
    if(!engine_object_.Get())
    {
        LOGE("Start, invalid engine object");
        return false;
    }
    
    if(m_pPlayer)
    {
        LOGW("CAudioWrapper::Start Start, Already Started");
        return true;   
    }
    
    int64_t startTime = GetTimeMS();

    do
    {
        if(!StartPlayer(distFile))
            break;

        return true;
        
    } while (false);
    
    DestroyPlayResources();
    
    return false;
}

void CAudioWrapper::StopPlay()
{
    DestroyPlayResources();
}

bool CAudioWrapper::StartAudioRecorder(const char *dstRecordPCMFile)
{
    LOGI("Start, dstRecordPCMFile = %{public}s", dstRecordPCMFile);
    
    if(!engine_object_.Get())
    {
        LOGE("Start, invalid engine object");
        return false;
    }
    
    if(m_pRecorder)
    {
        LOGW("StartAudioRecorder Start, Already Started");
        return true;   
    }
    
    int64_t startTime = GetTimeMS();

    do
    {
        int64_t time1 = GetTimeMS();
        
        if(!StartRecord(dstRecordPCMFile))
            break;

        int64_t time2 = GetTimeMS();
        
        LOGI("Start, playerCreateDelta = %{public}ld ms, recordCreateDelta = %{public}ld ms", time1 - startTime, time2 - time1);
        
        return true;
        
    } while (false);
    
    DestroyRecordResources();
    
    return false;
}

void CAudioWrapper::StopAudioRecorder()
{
    DestroyRecordResources();
}

void CAudioWrapper::OnGetPlayoutData(SLuint8 *buffer, SLuint32 size)
{
    if(!buffer || !size)
        return;
    
    LOGI("==========OnGetPlayoutData, size = %{public}lu", size);
    if(m_pDstPlayFile)
    {
        fwrite(buffer, 1, size, m_pDstPlayFile);
        fflush(m_pDstPlayFile);
    }
}

void CAudioWrapper::OnRecordData(SLuint8 *buffer, SLuint32 size)
{
    if(!buffer || !size)
        return;
        
    if(audioRecordDataCallback_) {
        audioRecordDataCallback_->OnRecordData(buffer, size);
    }
     if(m_pDstRecordFile)
     {
         fwrite(buffer, 1, size, m_pDstRecordFile);
         fflush(m_pDstRecordFile);
     }
}

bool CAudioWrapper::StartPlayer(const char *dstFileName)
{
     LOGE("StartPlayer, dstFileName=%{public}lu", strlen(dstFileName));

     if (dstFileName == nullptr || strlen(dstFileName) <= 1) {
         m_pDstPlayFile = nullptr;
    } else {
         m_pDstPlayFile = fopen(dstFileName, "wb");
    }
    
	
    m_pPlayer = new OpenSLESPlayer(engine_object_.Get(), this, m_context);
    if(!m_pPlayer)
    {
        LOGE("StartPlayer, new OpenSLESPlayer Failed");
        return false;      
    }
    
    if(m_pPlayer->InitPlayout() < 0)
    {
        LOGE("StartPlayer, m_pPlayer->InitPlayout Failed");
        return false;          
    }
    
    if(m_pPlayer->StartPlayout())
    {
        LOGE("StartPlayer, m_pPlayer->StartPlayout Failed");
        return false;            
    }
    
    return true;
}

bool CAudioWrapper::StartRecord(const char *dstFileName)
{
    if(dstFileName != nullptr && strlen(dstFileName)>1){
        m_pDstRecordFile = fopen(dstFileName, "wb");
        if (nullptr == m_pDstRecordFile) {
            LOGE("StartRecord, open player dst file failed");
            return false;
        }
    } else {
		m_pDstRecordFile = nullptr;
	}
	
    m_pRecorder = new OpenSLESRecorder(engine_object_.Get(), this);
    if(!m_pRecorder)
    {
        LOGE("StartRecord, new OpenSLESRecorder Failed");
        return false;             
    }
    
    if(m_pRecorder->InitRecording() < 0)
    {
        LOGE("StartRecord, m_pRecorder->InitRecording Failed");
        return false;      
    }
    
    if(m_pRecorder->StartRecording() < 0)
    {
         LOGE("StartRecord, m_pRecorder->StartRecording Failed");
         return false;      
    }
    
    return true;
}

void CAudioWrapper::DestroyPlayResources()
{
    LOGE("DestroyPlayResources start");

    if(m_pPlayer)
    {
        m_pPlayer->StopPlayout();
        delete m_pPlayer;
        m_pPlayer = nullptr;
    }

    if (m_pDstPlayFile) {
        fclose(m_pDstPlayFile);
        m_pDstPlayFile = nullptr;
    }

    LOGE("DestroyPlayResources end");
}

void CAudioWrapper::DestroyRecordResources()
{
    LOGE("DestroyRecordResources start");
    
    if(m_pRecorder)
    {
        m_pRecorder->StopRecording();
        delete m_pRecorder;
        m_pRecorder = nullptr;
    }

    if(m_pDstRecordFile) {
        fclose(m_pDstRecordFile);
        m_pDstRecordFile = nullptr;
    }
	
    LOGE("DestroyRecordResources end");
}

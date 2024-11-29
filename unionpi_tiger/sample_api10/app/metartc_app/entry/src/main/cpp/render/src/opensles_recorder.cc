#include "opensles_recorder.h"

#include "hilog/log.h"
#include <SLES/OpenSLES.h>
#define LOGI(...) ((void)OH_LOG_Print(LOG_APP, LOG_INFO, LOG_DOMAIN, "OpenSLESRecorder", __VA_ARGS__))
#define LOGD(...) ((void)OH_LOG_Print(LOG_APP, LOG_INFO, LOG_DOMAIN, "OpenSLESRecorder", __VA_ARGS__))
#define LOGW(...) ((void)OH_LOG_Print(LOG_APP, LOG_WARN, LOG_DOMAIN, "OpenSLESRecorder", __VA_ARGS__))
#define LOGE(...) ((void)OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "OpenSLESRecorder", __VA_ARGS__))

#define LOG_ON_ERROR(op)                                    \
  [](SLresult err) {                                        \
    if (err != SL_RESULT_SUCCESS) {                         \
      LOGE("%{public}s:%{public}d %{public}s failed: %{public}s", __FILE__, __LINE__, #op, \
            GetSLErrorString(err));                         \
      return true;                                          \
    }                                                       \
    return false;                                           \
  }(op)

OpenSLESRecorder::OpenSLESRecorder(SLObjectItf engine_object, IRecorderSink *sink)
: engine_object_(engine_object)
, m_pSink(sink)
, initialized_(false)
, recording_(false)
, engine_(nullptr)
, recorder_(nullptr)
, simple_buffer_queue_(nullptr)
 {
    LOGD("OpenSLESRecorder constructor");
    pcm_format_ = CreatePCMConfiguration(2, 48000, 16);
}

OpenSLESRecorder::~OpenSLESRecorder()
{
    LOGD("~OpenSLESRecorder");
    Terminate();
    DestroyAudioRecorder();
    engine_ = nullptr;
}

int OpenSLESRecorder::Terminate()
{
    LOGD("Terminate");
    // StopRecording();
    return 0;
}

int OpenSLESRecorder::InitRecording() 
{
    LOGD("InitRecording");
    
    if (!ObtainEngineInterface())
    {
        LOGE("Failed to obtain SL Engine interface");
        return -1;
    }
    
    if(!CreateAudioRecorder())
        return -1;
    
    initialized_ = true;
    
    return 0;
}

int OpenSLESRecorder::StartRecording()
{
    LOGD("StartRecording");

    LogBufferState();

    if (LOG_ON_ERROR((*recorder_)->SetRecordState(recorder_, SL_RECORDSTATE_RECORDING)))
        return -1;
    
    recording_ = (GetRecordState() == SL_RECORDSTATE_RECORDING);

    return 0;
}

int OpenSLESRecorder::StopRecording()
{
    LOGD("StopRecording start");

    if (!initialized_ || !recording_){
        LOGD("stop failed! initialized_:%{public}d, recording_:%{public}d", initialized_, recording_);
        return 0;
    }

    if (LOG_ON_ERROR((*recorder_)->SetRecordState(recorder_, SL_RECORDSTATE_STOPPED))) {
        LOGD("StopRecording SetRecordState stop");
        return -1;
    }
    
    if (LOG_ON_ERROR((*simple_buffer_queue_)->Clear(simple_buffer_queue_))){
        LOGD("StopRecording Clear buffer");
        return -1;
    }
    
    // DestroyAudioRecorder();
    
    initialized_ = false;
    recording_ = false;
    return 0;
}

bool OpenSLESRecorder::ObtainEngineInterface()
{
    LOGD("ObtainEngineInterface");

    if (engine_)
        return true;
    
    if (engine_object_ == nullptr)
    {
        LOGE("Failed to access the global OpenSL engine");
        return false;
    }

    if (LOG_ON_ERROR((*engine_object_)->GetInterface(engine_object_, SL_IID_ENGINE, &engine_)))
        return false;
    
    return true;
}

bool OpenSLESRecorder::CreateAudioRecorder()
{
    LOGD("CreateAudioRecorder");

    if (recorder_object_.Get())
        return true;
    
    SLDataLocator_IODevice mic_locator = {SL_DATALOCATOR_IODEVICE, SL_IODEVICE_AUDIOINPUT, SL_DEFAULTDEVICEID_AUDIOINPUT, NULL};
    SLDataSource audio_source = {&mic_locator, NULL};
    
    SLDataLocator_BufferQueue buffer_queue = {SL_DATALOCATOR_BUFFERQUEUE, 0};
    SLDataSink audio_sink = {&buffer_queue, &pcm_format_};
    
    const SLInterfaceID interface_id[] = {SL_IID_OH_BUFFERQUEUE};
    const SLboolean interface_required[] = {SL_BOOLEAN_TRUE};
    if (LOG_ON_ERROR((*engine_)->CreateAudioRecorder(engine_, recorder_object_.Receive(), &audio_source, &audio_sink, 1, interface_id, interface_required)))
        return false;
    
    if (LOG_ON_ERROR((recorder_object_->Realize(recorder_object_.Get(), SL_BOOLEAN_FALSE))))
        return false;
    
    if (LOG_ON_ERROR((recorder_object_->GetInterface(recorder_object_.Get(), SL_IID_RECORD, &recorder_))))
        return false;
    
    if (LOG_ON_ERROR((recorder_object_->GetInterface(recorder_object_.Get(), SL_IID_OH_BUFFERQUEUE, &simple_buffer_queue_))))
        return false;
    
    if (LOG_ON_ERROR(((*simple_buffer_queue_)->RegisterCallback(simple_buffer_queue_, SimpleBufferQueueCallback, this))))
        return false;
    
    return true;
}

void OpenSLESRecorder::DestroyAudioRecorder()
{
    LOGD("DestroyAudioRecorder");

    if (!recorder_object_.Get())
        return;
    
    if(simple_buffer_queue_)
        (*simple_buffer_queue_)->RegisterCallback(simple_buffer_queue_, nullptr, nullptr);
    
    recorder_object_.Reset();
    
    recorder_ = nullptr;
    
    simple_buffer_queue_ = nullptr;
}

void OpenSLESRecorder::SimpleBufferQueueCallback(SLOHBufferQueueItf buffer_queue,void* context, SLuint32 size)
{
    OpenSLESRecorder* stream = static_cast<OpenSLESRecorder*>(context);
    stream->ReadBufferQueue();
}

void OpenSLESRecorder::ReadBufferQueue()
{
    SLuint32 state = GetRecordState();
    if (state != SL_RECORDSTATE_RECORDING)
    {
        LOGW("Buffer callback in non-recording state!");
        return;
    }
    
    SLuint8 *recData = nullptr;
    SLuint32 recDataSize = 0;
    if(LOG_ON_ERROR((*simple_buffer_queue_)->GetBuffer(simple_buffer_queue_, &recData, &recDataSize)))
    {
        LOGE("OpenSLESRecorder::ReadBufferQueue(), simple_buffer_queue_->GetBuffer Failed");
    }
    else
    {
        //LOGI("OpenSLESRecorder::ReadBufferQueue(), dataptr = %{public}x, size = %{public}d", recData, recDataSize);
        
        if(m_pSink)
            m_pSink->OnRecordData(recData, recDataSize);
        
        SLresult err =(*simple_buffer_queue_)->Enqueue(simple_buffer_queue_, recData, recDataSize);
        if (SL_RESULT_SUCCESS != err)
            LOGE("Enqueue failed: %{public}s", GetSLErrorString(err));
    }
}

SLuint32 OpenSLESRecorder::GetRecordState() const
{
    SLuint32 state;
    SLresult err = (*recorder_)->GetRecordState(recorder_, &state);
    
    if (SL_RESULT_SUCCESS != err)
        LOGE("GetRecordState failed: %{public}s", GetSLErrorString(err));
    
    return state;
}

SLOHBufferQueueState OpenSLESRecorder::GetBufferQueueState() const
{
    SLOHBufferQueueState state;
    SLresult err = (*simple_buffer_queue_)->GetState(simple_buffer_queue_, &state);
    
    if (SL_RESULT_SUCCESS != err)
        LOGE("GetState failed: %{public}s", GetSLErrorString(err));
    
    return state;
}

void OpenSLESRecorder::LogBufferState() const
{
    SLOHBufferQueueState state = GetBufferQueueState();
    LOGD("state.count:%{public}d state.index:%{public}d", state.count, state.index);
}

SLuint32 OpenSLESRecorder::GetBufferCount() {
  SLOHBufferQueueState state = GetBufferQueueState();
  return state.count;
}


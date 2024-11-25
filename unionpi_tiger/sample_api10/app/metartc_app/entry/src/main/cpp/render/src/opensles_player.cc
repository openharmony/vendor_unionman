#include "opensles_player.h"

#include "hilog/log.h"
#define LOGI(...) ((void)OH_LOG_Print(LOG_APP, LOG_INFO, LOG_DOMAIN, "OpenSLESPlayer", __VA_ARGS__))
#define LOGD(...) ((void)OH_LOG_Print(LOG_APP, LOG_INFO, LOG_DOMAIN, "OpenSLESPlayer", __VA_ARGS__))
#define LOGW(...) ((void)OH_LOG_Print(LOG_APP, LOG_WARN, LOG_DOMAIN, "OpenSLESPlayer", __VA_ARGS__))
#define LOGE(...) ((void)OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "OpenSLESPlayer", __VA_ARGS__))

#define RETURN_ON_ERROR(op, ...)                          \
  do {                                                    \
    SLresult err = (op);                                  \
    if (err != SL_RESULT_SUCCESS) {                       \
      LOGE("%{public}s failed: %{public}s", #op, GetSLErrorString(err)); \
      return __VA_ARGS__;                                 \
    }                                                     \
  } while (0)

OpenSLESPlayer::OpenSLESPlayer(SLObjectItf engine_object, IPlayerSink *sink, YangContext *pcontext)
: engine_object_(engine_object)
, m_pSink(sink)
, initialized_(false)
, playing_(false)
, engine_(nullptr)
, player_(nullptr)
, simple_buffer_queue_(nullptr)
{
    LOGD("OpenSLESPlayer");
    pcm_format_ = CreatePCMConfiguration(2, 48000, 16);
    m_context = pcontext;
    m_context->metaAudioPlayBuffer = new YangAudioPlayBuffer();
}

OpenSLESPlayer::~OpenSLESPlayer()
{
    LOGD("~OpenSLESPlayer");
    Terminate();
    DestroyAudioPlayer();
    DestroyMix();
    engine_ = nullptr;
    yang_delete(m_context->metaAudioPlayBuffer);
}

int OpenSLESPlayer::Terminate()
{
    LOGD("Terminate");
    
    StopPlayout();
    
    return 0;
}

int OpenSLESPlayer::InitPlayout()
{
    LOGD("InitPlayout");

    if (!ObtainEngineInterface())
    {
        LOGE("Failed to obtain SL Engine interface");
        return -1;
    }
    
    CreateMix();
    
    initialized_ = true;
    
    return 0;
}

int OpenSLESPlayer::StartPlayout()
{
    LOGD("StartPlayout");
    
    if(!CreateAudioPlayer())
        return -1;
    
    SLOHBufferQueueState buffer_queue_state = {};
    (*simple_buffer_queue_)->GetState(simple_buffer_queue_, &buffer_queue_state);
    
    LOGI("state.count:%{public}lu state.index:%{public}lu", buffer_queue_state.count, buffer_queue_state.index);
    
    RETURN_ON_ERROR((*player_)->SetPlayState(player_, SL_PLAYSTATE_PLAYING), -1);
    
    playing_ = (GetPlayState() == SL_PLAYSTATE_PLAYING);
    return 0;
}

int OpenSLESPlayer::StopPlayout()
{
    LOGD("StopPlayout");
    if (!initialized_ || !playing_)
        return 0;
    
    RETURN_ON_ERROR((*player_)->SetPlayState(player_, SL_PLAYSTATE_STOPPED), -1);

    RETURN_ON_ERROR((*simple_buffer_queue_)->Clear(simple_buffer_queue_), -1);

    DestroyAudioPlayer();

    initialized_ = false;
    
    playing_ = false;
    
    return 0;
}

bool OpenSLESPlayer::ObtainEngineInterface()
{
    LOGD("ObtainEngineInterface");
    
    if (engine_)
        return true;
    
    if (engine_object_ == nullptr)
    {
        LOGE("Invalid engine object");
        return false;
    }
    
    RETURN_ON_ERROR((*engine_object_)->GetInterface(engine_object_, SL_IID_ENGINE, &engine_), false);
    
    return true;
}

bool OpenSLESPlayer::CreateMix()
{
    LOGD("CreateMix");

    if (output_mix_.Get())
        return true;

    RETURN_ON_ERROR((*engine_)->CreateOutputMix(engine_, output_mix_.Receive(), 0, nullptr, nullptr), false);
    
    RETURN_ON_ERROR(output_mix_->Realize(output_mix_.Get(), SL_BOOLEAN_FALSE), false);
    
    return true;
}

void OpenSLESPlayer::DestroyMix()
{
    LOGD("DestroyMix");

    if (!output_mix_.Get())
        return;
    
    output_mix_.Reset();
}

bool OpenSLESPlayer::CreateAudioPlayer()
{
    LOGD("CreateAudioPlayer");

    if (player_object_.Get())
        return true;
    
    SLDataLocator_BufferQueue simple_buffer_queue = {SL_DATALOCATOR_BUFFERQUEUE, 2};
    SLDataSource audio_source = {&simple_buffer_queue, &pcm_format_};
    
    SLDataLocator_OutputMix locator_output_mix = {SL_DATALOCATOR_OUTPUTMIX, output_mix_.Get()};
    SLDataSink audio_sink = {&locator_output_mix, nullptr};
    
    const SLInterfaceID interface_ids[] = {SL_IID_OH_BUFFERQUEUE, SL_IID_VOLUME};
    const SLboolean interface_required[] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};
    
    RETURN_ON_ERROR((*engine_)->CreateAudioPlayer(engine_, player_object_.Receive(), &audio_source, &audio_sink, 2, interface_ids, interface_required), false);
    
    RETURN_ON_ERROR(player_object_->Realize(player_object_.Get(), SL_BOOLEAN_FALSE), false);
    
    RETURN_ON_ERROR(player_object_->GetInterface(player_object_.Get(), SL_IID_PLAY, &player_), false);
    
    RETURN_ON_ERROR(player_object_->GetInterface(player_object_.Get(), SL_IID_OH_BUFFERQUEUE, &simple_buffer_queue_), false);
    
    RETURN_ON_ERROR((*simple_buffer_queue_)->RegisterCallback(simple_buffer_queue_, SimpleBufferQueueCallback, this), false);
    
    return true;
}

void OpenSLESPlayer::DestroyAudioPlayer()
{
    LOGD("DestroyAudioPlayer");

    if (!player_object_.Get())
        return;
    
    (*simple_buffer_queue_)->RegisterCallback(simple_buffer_queue_, nullptr, nullptr);
    
    player_object_.Reset();
    
    player_ = nullptr;
    
    simple_buffer_queue_ = nullptr;
}

void OpenSLESPlayer::SimpleBufferQueueCallback(SLOHBufferQueueItf caller, void* context, SLuint32 size)
{
    OpenSLESPlayer* stream = reinterpret_cast<OpenSLESPlayer*>(context);
    stream->FillBufferQueue();
}

void OpenSLESPlayer::FillBufferQueue()
{
    SLuint32 state = GetPlayState();
    if (state != SL_PLAYSTATE_PLAYING)
    {
        LOGW("Buffer callback in non-playing state!");
        return;
    }
    
    EnqueuePlayoutData();
}

void OpenSLESPlayer::EnqueuePlayoutData()
{
    YangFrame audioFrame;
    SLuint8 *buffer = nullptr;
    SLuint32 size = 0;
    SLresult err = 0;
    
    while(1) {
        if(GetPlayState() != SL_PLAYSTATE_PLAYING){
            LOGD("return EnqueuePlayoutData");
            return;
        }
        if(m_context->metaAudioPlayBuffer && m_context->metaAudioPlayBuffer->size() == 0) {
            usleep(20*1000);
        } else{
            break;
        }
    } 

    err = (*simple_buffer_queue_)->GetBuffer(simple_buffer_queue_, &buffer, &size);
    if(SL_RESULT_SUCCESS != err)
    {
        LOGE("SLES simple Buffer Get buffer Failed", err);
        return;
    }

    memset(&audioFrame,0,sizeof(YangFrame));
    audioFrame.payload = buffer;
    m_context->metaAudioPlayBuffer->getAudio(&audioFrame);

    // LOGI("OpenSLESPlayer::EnqueuePlayoutData(), dataptr = %{public}x, size = %{public}d", buffer, size);

    if (m_pSink)
        m_pSink->OnGetPlayoutData(buffer, audioFrame.nb);
    
    err = (*simple_buffer_queue_)->Enqueue(simple_buffer_queue_, buffer, audioFrame.nb);

    if (SL_RESULT_SUCCESS != err)
        LOGE("Enqueue failed: %{public}s", GetSLErrorString(err));
}

SLuint32 OpenSLESPlayer::GetPlayState() const
{
    SLuint32 state;
    if (player_ == nullptr)
        return -1;
    SLresult err = (*player_)->GetPlayState(player_, &state);
    if (SL_RESULT_SUCCESS != err)
        LOGE("GetPlayState failed: %{public}lu", err);

    return state;
}

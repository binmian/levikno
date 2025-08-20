#include "levikno.h"
#include "levikno_internal.h"

#include "miniaudio.h"

namespace lvn
{

LvnResult initAudioContext(LvnContext* lvnctx)
{
    ma_engine* pEngine = (ma_engine*)LVN_MALLOC(sizeof(ma_engine));

    if (ma_engine_init(nullptr, pEngine) != MA_SUCCESS)
    {
        LVN_CORE_ERROR("failed to initialize audio engine context");
        return Lvn_Result_Failure;
    }

    lvnctx->audioEngineContextPtr = pEngine;

    LVN_CORE_TRACE("audio context initialized");
    return Lvn_Result_Success;
}

void terminateAudioContext(LvnContext* lvnctx)
{
    if (lvnctx->audioEngineContextPtr != nullptr)
    {
        ma_engine_uninit(static_cast<ma_engine*>(lvnctx->audioEngineContextPtr));
        lvn::memFree(lvnctx->audioEngineContextPtr);
    }

    LVN_CORE_TRACE("audio context terminated");
}

float volumeDbToLinear(float db)
{
    return ma_volume_db_to_linear(db);
}

float volumeLineatToDb(float volume)
{
    return ma_volume_linear_to_db(volume);
}

void audioSetGlobalTimeMilliSeconds(uint64_t ms)
{
    ma_engine_set_time_in_milliseconds(static_cast<ma_engine*>(lvn::getContext()->audioEngineContextPtr), ms);
}

void audioSetGlobalTimePcmFrames(uint64_t pcm)
{
    ma_engine_set_time_in_pcm_frames(static_cast<ma_engine*>(lvn::getContext()->audioEngineContextPtr), pcm);
}

void audioSetMasterVolume(float volume)
{
    ma_engine_set_volume(static_cast<ma_engine*>(lvn::getContext()->audioEngineContextPtr), volume);
}

uint32_t audioGetSampleRate()
{
    return ma_engine_get_sample_rate(static_cast<ma_engine*>(lvn::getContext()->audioEngineContextPtr));
}

uint64_t audioGetGlobalTimeMilliseconds()
{
    return ma_engine_get_time_in_milliseconds(static_cast<ma_engine*>(lvn::getContext()->audioEngineContextPtr));
}

uint64_t audioGetGlobalTimePcmFrames()
{
    return ma_engine_get_time_in_pcm_frames(static_cast<ma_engine*>(lvn::getContext()->audioEngineContextPtr));
}


void listenerSetPosition(float x, float y, float z)
{
    ma_engine_listener_set_position(static_cast<ma_engine*>(lvn::getContext()->audioEngineContextPtr), 0, x, y, z);
}

void listenerSetPosition(const LvnVec3& pos)
{
    ma_engine_listener_set_position(static_cast<ma_engine*>(lvn::getContext()->audioEngineContextPtr), 0, pos.x, pos.y, pos.z);
}

void listenerSetDirection(float x, float y, float z)
{
    ma_engine_listener_set_direction(static_cast<ma_engine*>(lvn::getContext()->audioEngineContextPtr), 0, x, y, z);
}

void listenerSetDirection(const LvnVec3 dir)
{
    ma_engine_listener_set_direction(static_cast<ma_engine*>(lvn::getContext()->audioEngineContextPtr), 0, dir.x, dir.y, dir.z);
}

void listenerSetVelocity(float x, float y, float z)
{
    ma_engine_listener_set_velocity(static_cast<ma_engine*>(lvn::getContext()->audioEngineContextPtr), 0, x, y, z);
}

void listenerSetVelocity(const LvnVec3 vel)
{
    ma_engine_listener_set_velocity(static_cast<ma_engine*>(lvn::getContext()->audioEngineContextPtr), 0, vel.x, vel.y, vel.z);
}

void listenerSetWorldUp(float x, float y, float z)
{
    ma_engine_listener_set_world_up(static_cast<ma_engine*>(lvn::getContext()->audioEngineContextPtr), 0, x, y, z);
}

void listenerSetWorldUp(const LvnVec3 up)
{
    ma_engine_listener_set_world_up(static_cast<ma_engine*>(lvn::getContext()->audioEngineContextPtr), 0, up.x, up.y, up.z);
}

void listenerSetCone(float innerAngleRad, float outerAngleRad, float outerGain)
{
    ma_engine_listener_set_cone(static_cast<ma_engine*>(lvn::getContext()->audioEngineContextPtr), 0, innerAngleRad, outerAngleRad, outerGain);
}

LvnVec3 listenerGetPosition()
{
    ma_vec3f pos = ma_engine_listener_get_position(static_cast<ma_engine*>(lvn::getContext()->audioEngineContextPtr), 0);
    return LvnVec3{ pos.x, pos.y, pos.z };
}

LvnVec3 listenerGetDirection()
{
    ma_vec3f dir = ma_engine_listener_get_position(static_cast<ma_engine*>(lvn::getContext()->audioEngineContextPtr), 0);
    return LvnVec3{ dir.x, dir.y, dir.z };
}

LvnVec3 listenerGetWorldUp()
{
    ma_vec3f up = ma_engine_listener_get_position(static_cast<ma_engine*>(lvn::getContext()->audioEngineContextPtr), 0);
    return LvnVec3{ up.x, up.y, up.z };
}

void listenerGetCone(float* innerAngleRad, float* outerAngleRad, float* outerGain)
{
    ma_engine_listener_get_cone(static_cast<ma_engine*>(lvn::getContext()->audioEngineContextPtr), 0, innerAngleRad, outerAngleRad, outerGain);
}


LvnResult createSound(LvnSound** sound, const LvnSoundCreateInfo* createInfo)
{
    LvnContext* lvnctx = lvn::getContext();
    ma_engine* pEngine = static_cast<ma_engine*>(lvnctx->audioEngineContextPtr);

    if (createInfo->filepath.empty())
    {
        LVN_CORE_ERROR("createSound(LvnSound**, LvnSoundCreateInfo*) | createInfo->filepath is nullptr, cannot load sound data without a valid path to the sound file");
        return Lvn_Result_Failure;
    }

    *sound = lvn::createObject<LvnSound>(lvnctx, Lvn_Stype_Sound);

    LvnSound* soundPtr = *sound;
    soundPtr->volume = createInfo->volume;
    soundPtr->pan = createInfo->pan;
    soundPtr->pitch = createInfo->pitch;
    soundPtr->pos = createInfo->pos;
    soundPtr->looping = createInfo->looping;

    ma_sound* maSound = (ma_sound*)LVN_MALLOC(sizeof(ma_sound));
    if (ma_sound_init_from_file(pEngine, createInfo->filepath.c_str(), createInfo->flags, NULL, NULL, maSound) != MA_SUCCESS)
    {
        LVN_CORE_ERROR("createSound(LvnSound**, LvnSoundCreateInfo*) | failed to create sound object");
        return Lvn_Result_Failure;
    }

    ma_sound_set_volume(maSound, createInfo->volume);
    ma_sound_set_pan(maSound, createInfo->pan);
    ma_sound_set_pitch(maSound, createInfo->pitch);
    ma_sound_set_position(maSound, createInfo->pos.x, createInfo->pos.y, createInfo->pos.z);
    ma_sound_set_looping(maSound, createInfo->looping);

    soundPtr->apiData = maSound;

    LVN_CORE_TRACE("created sound: (%p), volume: %.2f, pan: %.2f, pitch: %.2f", *sound, createInfo->volume, createInfo->pan, createInfo->pitch);
    return Lvn_Result_Success;
}

void destroySound(LvnSound* sound)
{
    if (sound == nullptr) { return; }
    LvnContext* lvnctx = lvn::getContext();

    ma_sound* maSound = static_cast<ma_sound*>(sound->apiData);
    ma_sound_uninit(maSound);
    LVN_FREE(maSound);
    lvn::destroyObject(lvnctx, sound, Lvn_Stype_Sound);
}

LvnSoundCreateInfo configSoundInit(const char* filepath)
{
    LvnSoundCreateInfo soundInit{};
    soundInit.pos = { 0.0f, 0.0f, 0.0f };
    soundInit.volume = 1.0f;
    soundInit.pan = 0.0f;
    soundInit.pitch = 1.0f;
    soundInit.looping = false;
    soundInit.filepath = filepath;

    return soundInit;
}

void soundSetVolume(LvnSound* sound, float volume)
{
    ma_sound_set_volume(static_cast<ma_sound*>(sound->apiData), volume);
}

void soundSetPan(LvnSound* sound, float pan)
{
    ma_sound_set_pan(static_cast<ma_sound*>(sound->apiData), pan);
}

void soundSetPitch(LvnSound* sound, float pitch)
{
    ma_sound_set_pitch(static_cast<ma_sound*>(sound->apiData), pitch);
}

void soundSetPositioning(LvnSound* sound, LvnSoundPositioningFlags positioning)
{
    ma_sound_set_positioning(static_cast<ma_sound*>(sound->apiData), static_cast<ma_positioning>(positioning));
}

void soundSetPosition(LvnSound* sound, float x, float y, float z)
{
    ma_sound_set_position(static_cast<ma_sound*>(sound->apiData), x, y, z);
}

void soundSetPosition(LvnSound* sound, const LvnVec3& pos)
{
    ma_sound_set_position(static_cast<ma_sound*>(sound->apiData), pos.x, pos.y, pos.z);
}

void soundSetDirection(LvnSound* sound, float x, float y, float z)
{
    ma_sound_set_direction(static_cast<ma_sound*>(sound->apiData), x, y, z);
}

void soundSetDirection(LvnSound* sound, const LvnVec3& dir)
{
    ma_sound_set_direction(static_cast<ma_sound*>(sound->apiData), dir.x, dir.y, dir.z);
}

void soundSetVelocity(LvnSound* sound, float x, float y, float z)
{
    ma_sound_set_velocity(static_cast<ma_sound*>(sound->apiData), x, y, z);
}

void soundSetVelocity(LvnSound* sound, const LvnVec3& vel)
{
    ma_sound_set_velocity(static_cast<ma_sound*>(sound->apiData), vel.x, vel.y, vel.z);
}

void soundSetCone(LvnSound* sound, float innerAngleRad, float outerAngleRad, float outerGain)
{
    ma_sound_group_set_cone(static_cast<ma_sound*>(sound->apiData), innerAngleRad, outerAngleRad, outerGain);
}

void soundSetAttenuation(LvnSound* sound, LvnSoundAttenuationFlags attenuation)
{
    ma_sound_set_attenuation_model(static_cast<ma_sound*>(sound->apiData), static_cast<ma_attenuation_model>(attenuation));
}

void soundSetRolloff(LvnSound* sound, float rolloff)
{
    ma_sound_set_rolloff(static_cast<ma_sound*>(sound->apiData), rolloff);
}

void soundSetMinGain(LvnSound* sound, float minGain)
{
    ma_sound_set_min_gain(static_cast<ma_sound*>(sound->apiData), minGain);
}

void soundSetMaxGain(LvnSound* sound, float maxGain)
{
    ma_sound_set_max_gain(static_cast<ma_sound*>(sound->apiData), maxGain);
}

void soundSetMinDistance(LvnSound* sound, float minDist)
{
    ma_sound_set_min_distance(static_cast<ma_sound*>(sound->apiData), minDist);
}

void soundSetMaxDistance(LvnSound* sound, float maxDist)
{
    ma_sound_set_max_distance(static_cast<ma_sound*>(sound->apiData), maxDist);
}

void soundSetDopplerFactor(LvnSound* sound, float dopplerFactor)
{
    ma_sound_set_doppler_factor(static_cast<ma_sound*>(sound->apiData), dopplerFactor);
}

void soundSetLooping(LvnSound* sound, bool looping)
{
    ma_sound_set_looping(static_cast<ma_sound*>(sound->apiData), looping);
}

void soundPlayStart(LvnSound* sound)
{
    ma_sound_start(static_cast<ma_sound*>(sound->apiData));
}

void soundPlayStop(LvnSound* sound)
{
    ma_sound_stop(static_cast<ma_sound*>(sound->apiData));
}

void soundTogglePause(LvnSound* sound)
{
    if (ma_sound_is_playing(static_cast<ma_sound*>(sound->apiData)))
        ma_sound_stop(static_cast<ma_sound*>(sound->apiData));
    else
        ma_sound_start(static_cast<ma_sound*>(sound->apiData));
}

LVN_API void soundScheduleStartTimePcmFrames(LvnSound* sound, uint64_t pcm)
{
    ma_sound_set_start_time_in_pcm_frames(static_cast<ma_sound*>(sound->apiData), pcm);
}

void soundScheduleStartTimeMilliseconds(LvnSound* sound, uint64_t ms)
{
    ma_sound_set_start_time_in_milliseconds(static_cast<ma_sound*>(sound->apiData), ms);
}

LVN_API void soundScheduleStopTimePcmFrames(LvnSound* sound, uint64_t pcm)
{
    ma_sound_set_stop_time_in_pcm_frames(static_cast<ma_sound*>(sound->apiData), pcm);
}

void soundScheduleStopTimeMilliseconds(LvnSound* sound, uint64_t ms)
{
    ma_sound_set_stop_time_in_milliseconds(static_cast<ma_sound*>(sound->apiData), ms);
}

void soundSetFadeMilliseconds(LvnSound* sound, float volBegin, float volEnd, uint64_t ms)
{
    ma_sound_set_fade_in_milliseconds(static_cast<ma_sound*>(sound->apiData), volBegin, volEnd, ms);
}

void soundSetFadePcmFrames(LvnSound* sound, float volBegin, float volEnd, uint64_t pcm)
{
    ma_sound_set_fade_in_pcm_frames(static_cast<ma_sound*>(sound->apiData), volBegin, volEnd, pcm);
}

void soundSeekToPcmFrame(LvnSound* sound, uint64_t pcm)
{
    ma_sound_seek_to_pcm_frame(static_cast<ma_sound*>(sound->apiData), pcm);
}

float soundGetVolume(const LvnSound* sound)
{
    return ma_sound_get_volume(static_cast<ma_sound*>(sound->apiData));
}

float soundGetPan(const LvnSound* sound)
{
    return ma_sound_get_pan(static_cast<ma_sound*>(sound->apiData));
}

float soundGetPitch(const LvnSound* sound)
{
    return ma_sound_get_pitch(static_cast<ma_sound*>(sound->apiData));
}

LvnSoundPositioningFlags soundGetPositioning(const LvnSound* sound)
{
    return static_cast<LvnSoundPositioningFlags>(ma_sound_get_positioning(static_cast<ma_sound*>(sound->apiData)));
}

LvnVec3 soundGetPosition(const LvnSound* sound)
{
    ma_vec3f pos = ma_sound_get_position(static_cast<ma_sound*>(sound->apiData));
    return LvnVec3{ pos.x, pos.y, pos.z };
}

LvnVec3 soundGetDirection(const LvnSound* sound)
{
    ma_vec3f dir = ma_sound_get_direction(static_cast<ma_sound*>(sound->apiData));
    return LvnVec3{ dir.x, dir.y, dir.z };
}

void soundGetCone(const LvnSound* sound, float* innerAngleRad, float* outerAngleRad, float* outerGain)
{
    ma_sound_get_cone(static_cast<ma_sound*>(sound->apiData), innerAngleRad, outerAngleRad, outerGain);
}

LvnVec3 soundGetVelocity(const LvnSound* sound)
{
    ma_vec3f vel = ma_sound_get_velocity(static_cast<ma_sound*>(sound->apiData));
    return LvnVec3{ vel.x, vel.y, vel.z };
}

LvnSoundAttenuationFlags soundGetAttenuation(const LvnSound* sound)
{
    return static_cast<LvnSoundAttenuationFlags>(ma_sound_get_attenuation_model(static_cast<ma_sound*>(sound->apiData)));
}

float soundGetRolloff(const LvnSound* sound)
{
    return ma_sound_get_rolloff(static_cast<ma_sound*>(sound->apiData));
}

float soundGetMinGain(const LvnSound* sound)
{
    return ma_sound_get_min_gain(static_cast<ma_sound*>(sound->apiData));
}

float soundGetMaxGain(const LvnSound* sound)
{
    return ma_sound_get_max_gain(static_cast<ma_sound*>(sound->apiData));
}

float soundGetMinDistance(const LvnSound* sound)
{
    return ma_sound_get_min_distance(static_cast<ma_sound*>(sound->apiData));
}

float soundGetMaxDistance(const LvnSound* sound)
{
    return ma_sound_get_max_distance(static_cast<ma_sound*>(sound->apiData));
}

float soundGetDopplerFactor(const LvnSound* sound)
{
    return ma_sound_get_doppler_factor(static_cast<ma_sound*>(sound->apiData));
}

bool soundIsLooping(const LvnSound* sound)
{
    return ma_sound_is_looping(static_cast<ma_sound*>(sound->apiData));
}

bool soundIsPlaying(const LvnSound* sound)
{
    return ma_sound_is_playing(static_cast<ma_sound*>(sound->apiData));
}

bool soundAtEnd(const LvnSound* sound)
{
    return ma_sound_at_end(static_cast<ma_sound*>(sound->apiData));
}

uint64_t soundGetTimeMilliseconds(const LvnSound* sound)
{
    return ma_sound_get_time_in_milliseconds(static_cast<ma_sound*>(sound->apiData));
}

uint64_t soundGetTimePcmFrames(const LvnSound* sound)
{
    return ma_sound_get_time_in_pcm_frames(static_cast<ma_sound*>(sound->apiData));
}

float soundGetLengthSeconds(LvnSound* sound)
{
    float length;
    ma_sound_get_length_in_seconds(static_cast<ma_sound*>(sound->apiData), &length);
    return length;
}

} /* namespace lvn */

#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include <miniaudio.h>
#include <iostream>

class AudioManager
{
private:
    ma_engine engine;
    ma_sound swimSound;
    ma_sound diveSound;
    bool isInitialized = false;

public:
    bool init()
    {
        if (ma_engine_init(NULL, &engine) != MA_SUCCESS)
        {
            std::cerr << "Failed to initialize Miniaudio!" << std::endl;
            return false;
        }
        ma_sound_init_from_file(&engine, "../assets/swim.mp3", 0, NULL, NULL, &swimSound);
        ma_sound_init_from_file(&engine, "../assets/dive.mp3", 0, NULL, NULL, &diveSound);
        ma_sound_set_looping(&swimSound, MA_TRUE);
        ma_sound_set_looping(&diveSound, MA_TRUE);
        isInitialized = true;
        return true;
    }

    void update(bool isDiving, bool isPlaying)
    {
        if (!isInitialized)
            return;

        if (!isPlaying)
        {
            if (ma_sound_is_playing(&swimSound))
                ma_sound_stop(&swimSound);
            if (ma_sound_is_playing(&diveSound))
                ma_sound_stop(&diveSound);
            return;
        }

        if (isDiving)
        {
            if (ma_sound_is_playing(&swimSound))
                ma_sound_stop(&swimSound);
            if (!ma_sound_is_playing(&diveSound))
                ma_sound_start(&diveSound);
        }
        else
        {
            if (ma_sound_is_playing(&diveSound))
                ma_sound_stop(&diveSound);
            if (!ma_sound_is_playing(&swimSound))
                ma_sound_start(&swimSound);
        }
    }

    void playCrash()
    {
        if (isInitialized)
            ma_engine_play_sound(&engine, "../assets/crash.mp3", NULL);
    }

    void playSound(const char *filepath)
    {
        if (isInitialized)
            ma_engine_play_sound(&engine, filepath, NULL);
    }

    void cleanup()
    {
        if (!isInitialized)
            return;
        ma_sound_uninit(&swimSound);
        ma_sound_uninit(&diveSound);
        ma_engine_uninit(&engine);
    }
};

#endif
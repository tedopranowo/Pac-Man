#include "AudioSystem.h"

SFX::SFX(char* filename)
{
    m_sfx = Mix_LoadWAV(filename);
//    assert(m_sfx != nullptr);
}

// To play the sound 'x' amount of times, change the last parameter. (0 = 1x)
void SFX::Play()
{
    Mix_Resume(2);
    Mix_PlayChannel(2, m_sfx, 0);
}

BgMusic::BgMusic(const char* pFilename)
    : m_musicStarted(false)
{
    m_pBgMusic = Mix_LoadWAV(pFilename);
    if (!m_pBgMusic)
        printf("Mix_LoadWAV: %s\n", Mix_GetError());
}

void BgMusic::Play(bool play)
{
    if (play)
    {
        if (m_musicStarted == false)
        {
            Mix_Volume(1, 50);
            Mix_PlayChannel(1, m_pBgMusic, -1);
        }

        Mix_Resume(-1);
    }
    else
        Mix_Pause(-1);
}

void AudioSystem::Play(bool play)
{
    m_bgMusic.Play(play);
}

//
//void AudioSystem::HandleEvent(const Event* pEvent)
//{
//    switch (pEvent->GetID())
//    {
//    case EventIDManager::UpKeyPressedEventID:
//        m_scroll.Play();
//        break;
//    case EventIDManager::DownKeyPressedEventID:
//        m_scroll.Play();
//        break;
//    case EventIDManager::SelectionMadeEventID:
//        m_select.Play();
//        break;
//    case EventIDManager::ExitDialogueBoxEventID:
//        m_bgMusicMellow.Play();
//        break;
//    case EventIDManager::DoorOpenEventID:
//        m_openDoor.Play();
//        break;
//    case EventIDManager::PlayerSetBombEventID:
//        m_fuseBurning.Play();
//        break;
//    default:
//        break;
//    }
//}



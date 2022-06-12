#include "SoundCentral.hpp"
#include "AssetHolder.hpp"

template <typename T>
std::size_t ToSizeT(T value)
{
    return static_cast<size_t>(value);
}

SoundCentral::SoundCentral()
{
    auto& ah = AssetHolder::get();
    // Sfx
    m_soundEffects[ToSizeT(SoundEffectTypes::PlanetCollision)].setBuffer(
        *ah.getSoundBuffer("bin/sounds/planet_collide.wav"));
    m_soundEffects[ToSizeT(SoundEffectTypes::LevelStart)].setBuffer(*ah.getSoundBuffer("bin/sounds/level_reset.wav"));
    m_soundEffects[ToSizeT(SoundEffectTypes::MenuItemHover)].setBuffer(*ah.getSoundBuffer("bin/sounds/menu_hover.wav"));

    // Music
    m_musicStreams[ToSizeT(MusicTypes::MainGameTheme)] = ah.getMusic("bin/sounds/game_theme_music.mp3");
    m_musicStreams[ToSizeT(MusicTypes::MainGameTheme)]->setLoop(true);
}

void SoundCentral::playSoundEffect(SoundEffectTypes type)
{
    assert(type < SoundEffectTypes::MAX_SFX);
    m_soundEffects[ToSizeT(type)].play();
}

void SoundCentral::playMusic(MusicTypes type)
{
    assert(type < MusicTypes::MAX_MUSIC);
    m_musicStreams[ToSizeT(type)]->play();
}

auto SoundCentral::getSoundStatus(SoundEffectTypes type) const -> sf::Sound::Status
{
    assert(type < SoundEffectTypes::MAX_SFX);
    return m_soundEffects[ToSizeT(type)].getStatus();
}

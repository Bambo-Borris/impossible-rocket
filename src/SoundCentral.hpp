#pragma once

#include <SFML/Audio/Music.hpp>
#include <SFML/Audio/Sound.hpp>
#include <array>

class SoundCentral {
public:
    enum class SoundEffectTypes { PlanetCollision = 0, LevelStart, MenuItemHover, MAX_SFX };
    enum class MusicTypes { MainGameTheme = 0, MAX_MUSIC };

    SoundCentral();

    void playSoundEffect(SoundEffectTypes type);
    void playMusic(MusicTypes type);
    void setMasterVolume(float volume);

    auto getSoundStatus(SoundEffectTypes type) const -> sf::Sound::Status;
    auto getMasterVolume() const -> float;

private:
    std::array<sf::Sound, static_cast<std::size_t>(SoundEffectTypes::MAX_SFX)> m_soundEffects;
    std::array<sf::Music*, static_cast<std::size_t>(MusicTypes::MAX_MUSIC)> m_musicStreams;

    float m_masterVolume { 100.0f };
};

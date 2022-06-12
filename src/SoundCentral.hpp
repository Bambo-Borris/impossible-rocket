#pragma once

#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/Music.hpp>
#include <array>

class SoundCentral {
public:
    enum class SoundEffectTypes { PlanetCollision = 0, LevelStart, MenuItemHover, MAX_SFX };
    enum class MusicTypes { MainGameTheme = 0, MAX_MUSIC };

    SoundCentral();

    void playSoundEffect(SoundEffectTypes type);
	void playMusic(MusicTypes type);

    auto getSoundStatus(SoundEffectTypes type) const -> sf::Sound::Status;

private:
    std::array<sf::Sound, static_cast<std::size_t>(SoundEffectTypes::MAX_SFX)> m_soundEffects;
    std::array<sf::Music*, static_cast<std::size_t>(MusicTypes::MAX_MUSIC)> m_musicStreams;
};

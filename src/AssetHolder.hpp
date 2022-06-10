#pragma once

#include <SFML/Audio/Music.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <filesystem>
#include <optional>
#include <unordered_map>

class AssetHolder {
public:
    static AssetHolder& get()
    {
        static AssetHolder& instance = *new AssetHolder();
        return instance;
    }

    sf::Font* getFont(const std::filesystem::path& path);
    sf::Texture* getTexture(const std::filesystem::path& path);
    sf::SoundBuffer* getSoundBuffer(const std::filesystem::path& path);
    sf::Music* getMusic(const std::filesystem::path& path);

private:
    AssetHolder() = default;
    std::unordered_map<std::string, sf::Font> m_fontMap;
    std::unordered_map<std::string, sf::Texture> m_textureMap;
    std::unordered_map<std::string, sf::SoundBuffer> m_soundBufferMap;
    std::unordered_map<std::string, sf::Music> m_musicMap;
};

#pragma once

#include <unordered_map>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Audio/Music.hpp>
#include <filesystem>
#include <optional>

class AssetHolder
{
public:
	static AssetHolder &get()
	{
		static AssetHolder &instance = *new AssetHolder();
		return instance;
	}

	sf::Font *getFont(const std::filesystem::path &path);
	sf::Texture *getTexture(const std::filesystem::path &path);
	sf::SoundBuffer *getSoundBuffer(const std::filesystem::path &path);

private:
	AssetHolder() = default;
	std::unordered_map<std::string, sf::Font> m_fontMap;
	std::unordered_map<std::string, sf::Texture> m_textureMap;
	std::unordered_map<std::string, sf::SoundBuffer> m_soundBufferMap;

	// TODO: add implementations to load music
};

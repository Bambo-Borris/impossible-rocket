#include "AssetHolder.hpp"

#include <fmt/format.h>

sf::Font *AssetHolder::getFont(const std::filesystem::path &path)
{
	const auto result = m_fontMap.find(path);
	if (result != m_fontMap.end())
	{
		return {&(*result).second};
	}

	if (!std::filesystem::exists(path))
		throw std::runtime_error(fmt::format("Font at path {} not found", path.generic_string()));

	if (!m_fontMap[path].loadFromFile(path))
		throw std::runtime_error(fmt::format("Unable to load font {}", path.generic_string()));

	return &m_fontMap[path];
}

sf::Texture *AssetHolder::getTexture(const std::filesystem::path &path)
{
	const auto result = m_textureMap.find(path);
	if (result != m_textureMap.end())
		return {&(*result).second};

	if (!std::filesystem::exists(path))
		throw std::runtime_error(fmt::format("Texture at path {} not found", path.generic_string()));

	if (!m_textureMap[path].loadFromFile(path))
		throw std::runtime_error(fmt::format("Unable to load texture {}", path.generic_string()));

	return &m_textureMap[path];
}

sf::SoundBuffer *AssetHolder::getSoundBuffer(const std::filesystem::path &path)
{
	const auto result = m_soundBufferMap.find(path);
	if (result != m_soundBufferMap.end())
		return {&(*result).second};

	if (!std::filesystem::exists(path))
		throw std::runtime_error(fmt::format("Sound at path {} not found", path.generic_string()));

	if (!m_soundBufferMap[path].loadFromFile(path))
		throw std::runtime_error(fmt::format("Unable to load sound {}", path.generic_string()));

	return &m_soundBufferMap[path];
}

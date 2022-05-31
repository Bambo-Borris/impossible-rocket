#include "AssetHolder.hpp"

#include <spdlog/fmt/fmt.h>

sf::Font *AssetHolder::getFont(const std::filesystem::path &path)
{
	const auto pathString = path.string();
	const auto result = m_fontMap.find(pathString);

	if (result != m_fontMap.end())
	{
		return &(*result).second;
	}

	if (!std::filesystem::exists(path))
		throw std::runtime_error(fmt::format("Font at path {} not found", pathString));

	if (!m_fontMap[pathString].loadFromFile(path))
		throw std::runtime_error(fmt::format("Unable to load font {}", pathString));

	return &m_fontMap[pathString];
}

sf::Texture *AssetHolder::getTexture(const std::filesystem::path &path)
{
	const auto pathString = path.string();
	const auto result = m_textureMap.find(pathString);
	if (result != m_textureMap.end())
		return &(*result).second;

	if (!std::filesystem::exists(path))
		throw std::runtime_error(fmt::format("Texture at path {} not found", pathString));

	if (!m_textureMap[pathString].loadFromFile(path))
		throw std::runtime_error(fmt::format("Unable to load texture {}", pathString));

	return &m_textureMap[pathString];
}

sf::SoundBuffer *AssetHolder::getSoundBuffer(const std::filesystem::path &path)
{
	const auto pathString = path.string();
	const auto result = m_soundBufferMap.find(pathString);
	if (result != m_soundBufferMap.end())
		return &(*result).second;

	if (!std::filesystem::exists(path))
		throw std::runtime_error(fmt::format("Sound at path {} not found", pathString));

	if (!m_soundBufferMap[pathString].loadFromFile(path))
		throw std::runtime_error(fmt::format("Unable to load sound {}", pathString));

	return &m_soundBufferMap[pathString];
}

#pragma once

namespace bb
{
	// Rocket related
	constexpr auto THRUST_FORCE{2.0e7f};
	constexpr auto TORQUE_MAG{8.0e3f};
	constexpr auto TRAIL_PARTICLE_COUNT{50};
	constexpr sf::Vector2f ROCKET_SIZE{32.0f, 32.0f};
	constexpr sf::Vector2f PARTICLE_SIZE{4.0f, 4.0f};

	// Level Related
	constexpr auto BIG_G{6.67e-11f};
	constexpr auto OBJECTIVE_ROTATION_SPEED{50.0f};
	const sf::Vector2f OBJECTIVE_SIZE{24.0f, 24.0f};

	// Menu related
	constexpr auto MENU_ORBIT_RADIUS{200.0f};
	constexpr auto MENU_ORBIT_SPEED{50.0f};
};

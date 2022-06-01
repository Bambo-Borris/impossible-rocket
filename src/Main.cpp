#include "App.hpp"

#include <SFML/GpuPreference.hpp>
#include <spdlog/spdlog.h> // JUST TO REPRO COMPILE ERROR, REMOVE AFTER FIX
SFML_DEFINE_DISCRETE_GPU_PREFERENCE

int main(int argc, char *argv[])
{
	(void)(argc);
	(void)(argv);

	// SPDLOG WILL BUILD FINE WITH THIS LINE SINCE 0 ARGS
	spdlog::debug("Not broken");
	// SPDLOG WILL CAUSE A COMPILE ERROR WITH THIS LINE
	spdlog::debug("Broken {}", 2.0f);

	App app;
	app.run();

	return 0;
}

#include "App.hpp"

#if defined(_MSC_VER)
extern __declspec(dllexport) unsigned long NvOptimusEnablement = 1;
extern __declspec(dllexport) unsigned long AmdPowerXpressRequestHighPerformance = 1;
#endif

int main(int argc, char *argv[])
{
	(void)(argc);
	(void)(argv);

	App app;
	app.run();

	return 0;
}

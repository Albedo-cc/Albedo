#include "startup/startup.h"
using namespace Albedo::APP;

int main(int argc, char* argv[])
{
	try
	{
		AlbedoAPP::StartUp(argc, argv);
		AlbedoAPP::Run();
	}
	catch (const std::exception& err)
	{
		Log::Fatal("Unsolved Error: {}", err.what());
	}

	return AlbedoAPP::Terminate();
}
#include "startup/startup.h"
using namespace Albedo;

int main(int argc, char* argv[])
{
	try
	{
		APP::StartUp(argc, argv);
		APP::Run();
	}
	catch (const std::exception& err)
	{
		Log::Fatal("Unsolved Error: {}", err.what());
	}

	return APP::Terminate();
}
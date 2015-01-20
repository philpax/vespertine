#include <vesp/Main.hpp>

int main()
{
	vesp::StringPtr gameBuildId = "Afterdawn (" __DATE__ " " __TIME__ ")";

	if (!vesp::Initialize(gameBuildId))
		return 1;

	vesp::Log(vesp::LogType::Info, gameBuildId);

	vesp::Loop();

	vesp::Shutdown();

	return 0;
}
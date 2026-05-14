#include "PCH.h"

namespace logger = SKSE::log;

void SetupLog()
{
	auto logsFolder = logger::log_directory();
	if (!logsFolder) {
		SKSE::stl::report_and_fail("SKSE log_directory not provided, logs can't be written");
	}

	auto logPath = *logsFolder / "ExampleMod.log";
	auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logPath.string(), true);
	auto spdlogger = std::make_shared<spdlog::logger>("global", std::move(fileSink));
	spdlog::set_default_logger(std::move(spdlogger));
	spdlog::set_level(spdlog::level::info);
	spdlog::flush_on(spdlog::level::info);
}

void OnDataLoaded()
{
	logger::info("OnDataLoaded hook fired");

	if (auto* const console = RE::ConsoleLog::GetSingleton()) {
		console->Print("[ExampleMod] Loaded successfully!");
	}
}

SKSEPluginLoad(const SKSE::LoadInterface* a_skse)
{
	SKSE::Init(a_skse);
	SetupLog();

	const auto* plugin = SKSE::PluginDeclaration::GetSingleton();
	if (!plugin) {
		logger::error("Failed to get plugin declaration");
		return false;
	}
	logger::info("{} v{} loaded", plugin->GetName(), plugin->GetVersion());

	const auto* messaging = SKSE::GetMessagingInterface();
	if (!messaging) {
		logger::error("Failed to get SKSE messaging interface");
		return false;
	}

	if (!messaging->RegisterListener([](SKSE::MessagingInterface::Message* a_msg) {
			switch (a_msg->type) {
			case SKSE::MessagingInterface::kDataLoaded:
				OnDataLoaded();
				break;
			default:
				break;
			}
		})) {
		logger::error("Failed to register messaging listener");
		return false;
	}

	return true;
}

#include <juce_audio_utils/juce_audio_utils.h>
#include "LightHostController.h"

LightHostController::LightHostController(bool startInSafeMode)
	: engine(startInSafeMode, true)
{
}

LightHostController::~LightHostController()
{
	saveAndFlush();
}

std::vector<ActivePluginView> LightHostController::getActivePlugins() const
{
	std::vector<ActivePluginView> result;
	const auto plugins = engine.getActivePluginsSorted();
	result.reserve(plugins.size());

	for (int i = 0; i < (int) plugins.size(); ++i)
	{
		const auto& plugin = plugins[(size_t) i];
		ActivePluginView view;
		view.name = plugin.name;
		view.manufacturer = plugin.manufacturerName;
		view.format = plugin.pluginFormatName;
		view.bypassed = engine.isPluginBypassed(i);
		view.order = i + 1;
		result.push_back(std::move(view));
	}

	return result;
}

DiagnosticsView LightHostController::getDiagnostics() const
{
	const DiagnosticsSnapshot snapshot = engine.getDiagnosticsSnapshot();
	DiagnosticsView view;
	view.backend = snapshot.backend;
	view.deviceName = snapshot.deviceName;
	view.cpuUsagePercent = snapshot.cpuUsagePercent;
	view.xRunCount = snapshot.xRunCount;
	view.sampleRate = snapshot.sampleRate;
	view.bufferSize = snapshot.bufferSize;
	view.inputLatency = snapshot.inputLatency;
	view.outputLatency = snapshot.outputLatency;
	view.activePlugins = snapshot.activePlugins;
	view.loadedPlugins = snapshot.loadedPlugins;
	view.chainLatencySamples = snapshot.chainLatencySamples;
	view.processFailures = snapshot.processFailures;
	return view;
}

void LightHostController::addKnownPluginByMenuId(int menuId)
{
	engine.addPluginFromMenuId(menuId);
}

void LightHostController::removeActivePlugin(int index)
{
	engine.removePlugin(index);
}

void LightHostController::moveActivePluginUp(int index)
{
	engine.movePluginUp(index);
}

void LightHostController::moveActivePluginDown(int index)
{
	engine.movePluginDown(index);
}

void LightHostController::setActivePluginBypassed(int index, bool shouldBypass)
{
	engine.setPluginBypassed(index, shouldBypass);
}

void LightHostController::showActivePluginEditor(int index)
{
	engine.showPluginEditor(index);
}

void LightHostController::deletePluginStates()
{
	engine.deletePluginStates();
	engine.loadActivePlugins();
}

void LightHostController::saveAndFlush()
{
	engine.savePluginStates();
	engine.flushPendingSaves();
}

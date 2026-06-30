#ifndef LightHostController_h
#define LightHostController_h

#include "AudioEngine.h"

struct ActivePluginView
{
	String name;
	String manufacturer;
	String format;
	bool bypassed = false;
	int order = 0;
};

struct DiagnosticsView
{
	String backend;
	String deviceName;
	double cpuUsagePercent = 0.0;
	int xRunCount = 0;
	double sampleRate = 0.0;
	int bufferSize = 0;
	int inputLatency = 0;
	int outputLatency = 0;
	int activePlugins = 0;
	int loadedPlugins = 0;
	int chainLatencySamples = 0;
	uint64 processFailures = 0;
};

class LightHostController
{
public:
	explicit LightHostController(bool startInSafeMode = false);
	~LightHostController();

	AudioEngine& getEngine() noexcept { return engine; }
	const AudioEngine& getEngine() const noexcept { return engine; }

	std::vector<ActivePluginView> getActivePlugins() const;
	DiagnosticsView getDiagnostics() const;

	void addKnownPluginByMenuId(int menuId);
	void removeActivePlugin(int index);
	void moveActivePluginUp(int index);
	void moveActivePluginDown(int index);
	void setActivePluginBypassed(int index, bool shouldBypass);
	void showActivePluginEditor(int index);
	void deletePluginStates();
	void saveAndFlush();

private:
	AudioEngine engine;
};

#endif /* LightHostController_h */

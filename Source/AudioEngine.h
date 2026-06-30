#ifndef AudioEngine_h
#define AudioEngine_h

#include "RealtimeHostProcessor.h"

ApplicationProperties& getAppProperties();

struct DiagnosticsSnapshot
{
	String backend;
	String deviceName;
	double cpuUsagePercent = 0.0;
	int xRunCount = 0;
	double sampleRate = 0.0;
	int bufferSize = 0;
	int inputLatency = 0;
	int outputLatency = 0;
	int inputChannels = 0;
	int outputChannels = 0;
	float inputLevel = 0.0f;
	float outputLevel = 0.0f;
	int activePlugins = 0;
	int loadedPlugins = 0;
	int chainLatencySamples = 0;
	uint64 chainReloads = 0;
	uint64 bypassToggles = 0;
	uint64 pluginStateSaves = 0;
	uint64 settingsFlushes = 0;
	uint64 processFailures = 0;
	uint64 reusedSlots = 0;
	uint64 rebuiltSlots = 0;
};

struct AudioDeviceConfiguration
{
	std::vector<String> backendNames;
	std::vector<String> inputDeviceNames;
	std::vector<String> outputDeviceNames;
	std::vector<String> inputChannelNames;
	std::vector<String> outputChannelNames;
	std::vector<bool> activeInputChannels;
	std::vector<bool> activeOutputChannels;
	std::vector<double> sampleRates;
	std::vector<int> bufferSizes;
	int currentBackendIndex = -1;
	int currentInputDeviceIndex = -1;
	int currentOutputDeviceIndex = -1;
	int currentInputChannels = 0;
	int currentOutputChannels = 0;
	int maxInputChannels = 0;
	int maxOutputChannels = 0;
};

class PluginStateStore
{
public:
	static String getKey(String type, const PluginDescription& plugin);
	static String getLegacyKey(String type, const PluginDescription& plugin);

	String getValue(String type, const PluginDescription& plugin, const String& defaultValue = String()) const;
	void setValue(String type, const PluginDescription& plugin, const var& value);
	void removeValue(String type, const PluginDescription& plugin);
	void markDirty();
	void flushIfDirty();
	bool isDirty() const noexcept { return dirty; }

private:
	bool dirty = false;
};

class DeviceController
{
public:
	String initialise(AudioDeviceManager& deviceManager, const XmlElement* savedAudioState);
	void recoverIfNeeded(AudioDeviceManager& deviceManager, int& failedAudioRecoveryAttempts);
	DiagnosticsSnapshot createDiagnosticsSnapshot(AudioDeviceManager& deviceManager) const;
};

class AudioEngine : private ChangeListener, private MultiTimer
{
public:
	explicit AudioEngine(bool startInSafeMode, bool restoreActivePluginsOnStartup);
	~AudioEngine() override;

	AudioDeviceManager& getDeviceManager() noexcept { return deviceManager; }
	AudioPluginFormatManager& getFormatManager() noexcept { return formatManager; }
	KnownPluginList& getKnownPluginList() noexcept { return knownPluginList; }

	std::vector<PluginDescription> getActivePluginsSorted() const;
	std::vector<PluginDescription> getKnownPluginsSorted() const;
	AudioDeviceConfiguration getAudioDeviceConfiguration();
	bool isVst2FormatActive() const;
	bool isPluginBypassed(int sortedIndex) const;
	bool isKnownPluginMenuId(int menuId) const;
	void addKnownPluginsToMenu(PopupMenu& menu) const;

	bool setAudioBackendByIndex(int backendIndex);
	bool setAudioInputDeviceByIndex(int deviceIndex);
	bool setAudioOutputDeviceByIndex(int deviceIndex);
	String getLastAudioConfigurationError() const { return lastAudioConfigurationError; }
	bool setAudioSampleRate(double sampleRate);
	bool setAudioBufferSize(int bufferSize);
	bool setAudioInputChannelCount(int channelCount);
	bool setAudioOutputChannelCount(int channelCount);
	bool setAudioInputChannelEnabled(int channelIndex, bool enabled);
	bool setAudioOutputChannelEnabled(int channelIndex, bool enabled);

	void scanDefaultPluginLocations(bool scanVst, bool scanVst3);
	void scanPluginPath(const String& path, bool scanVst, bool scanVst3);

	void addPluginFromMenuId(int menuId);
	bool addKnownPluginByIndex(int sortedIndex);
	void duplicatePlugin(int sortedIndex);
	int removeKnownPluginByIndex(int sortedIndex);
	int clearKnownPlugins();
	void openKnownPluginLocation(int sortedIndex) const;
	void removePlugin(int sortedIndex);
	void movePluginUp(int sortedIndex);
	void movePluginDown(int sortedIndex);
	void movePluginToIndex(int fromSortedIndex, int toSortedIndex);
	void setPluginBypassed(int sortedIndex, bool shouldBypass);
	void deletePluginStates();
	void savePluginStates();
	void saveAudioDeviceState();
	void flushPendingSaves();
	void removePluginsLackingInputOutput();
	void removeMissingKnownPlugins();
	void loadActivePlugins();
	void showPluginEditor(int sortedIndex);

	DiagnosticsSnapshot getDiagnosticsSnapshot() const;
	uint64 getChainVersion() const noexcept { return chainVersion; }
	uint64 getPluginDatabaseVersion() const noexcept { return pluginDatabaseVersion; }
	uint64 getAudioConfigVersion() const noexcept { return audioConfigVersion; }

private:
	enum TimerIds
	{
		audioWatchdogTimerId = 1,
		persistenceTimerId = 2,
		diagnosticsTimerId = 3
	};

	void timerCallback(int timerId) override;
	void changeListenerCallback(ChangeBroadcaster* changed) override;
	void markSettingsDirty();
	PluginSlot* findActiveSlotFor(const PluginDescription& plugin) const;
	void recordProcessFailures();
	void logDiagnosticsSnapshot();
	std::unique_ptr<XmlElement> getXmlValueOrClear(const String& key);

	bool safeMode = false;
	bool restoreActivePluginsOnStartup = false;
	bool settingsDirty = false;
	bool audioDeviceStateDirty = false;
	int failedAudioRecoveryAttempts = 0;
	uint64 chainReloadCount = 0;
	uint64 bypassToggleCount = 0;
	uint64 settingsFlushCount = 0;
	uint64 pluginStateSaveCount = 0;
	uint64 chainVersion = 0;
	uint64 pluginDatabaseVersion = 0;
	uint64 audioConfigVersion = 0;
	String lastAudioConfigurationError;

	PluginStateStore pluginStateStore;
	DeviceController deviceController;
	AudioDeviceManager deviceManager;
	AudioPluginFormatManager formatManager;
	KnownPluginList knownPluginList;
	KnownPluginList activePluginList;
	KnownPluginList::SortMethod pluginSortMethod = KnownPluginList::sortByManufacturer;
	RealtimeHostProcessor hostProcessor;
	AudioProcessorPlayer player;
};

#endif /* AudioEngine_h */

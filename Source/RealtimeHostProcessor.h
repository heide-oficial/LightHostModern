#ifndef RealtimeHostProcessor_h
#define RealtimeHostProcessor_h

#include <juce_audio_utils/juce_audio_utils.h>
#include <atomic>
#include <memory>
#include <vector>

using namespace juce;

struct RealtimeHostStats
{
	int loadedSlots = 0;
	int chainLatencySamples = 0;
	uint64 processFailures = 0;
	uint64 reusedSlots = 0;
	uint64 rebuiltSlots = 0;
	float inputLevel = 0.0f;
	float outputLevel = 0.0f;
};

struct PluginSlot
{
	explicit PluginSlot(PluginDescription descriptionIn, std::unique_ptr<AudioPluginInstance> processorIn);
	~PluginSlot();

	void prepare(double sampleRateIn, int blockSizeIn);
	void release();
	void processBypass(AudioBuffer<float>& buffer);
	int getLatencySamples() const noexcept { return latencySamples; }

	PluginDescription description;
	std::unique_ptr<AudioPluginInstance> processor;
	NamedValueSet windowProperties;
	std::atomic<bool> bypassed { false };
	std::atomic<bool> processDisabled { false };
	std::atomic<bool> processFailed { false };
	int inputChannels = 0;
	int outputChannels = 0;
	bool prepared = false;
	double preparedSampleRate = 0.0;
	int preparedBlockSize = 0;
	int preparedChannels = 0;

private:
	void prepareBypassDelay(int channels, int blockSizeIn);

	int latencySamples = 0;
	int bypassDelayWritePosition = 0;
	AudioBuffer<float> bypassDelayBuffer;
};

struct ChainSnapshot
{
	double sampleRate = 44100.0;
	int blockSize = 512;
	int inputChannels = 2;
	int outputChannels = 2;
	int maxPluginChannels = 2;
	int totalLatencySamples = 0;
	uint64 reusedSlots = 0;
	uint64 rebuiltSlots = 0;
	std::vector<std::shared_ptr<PluginSlot>> slots;
};

class RealtimeHostProcessor final : public AudioProcessor
{
public:
	RealtimeHostProcessor();
	~RealtimeHostProcessor() override;

	void publishSnapshot(std::shared_ptr<ChainSnapshot> snapshot);
	std::shared_ptr<ChainSnapshot> getActiveSnapshot() const;
	void collectRetiredSnapshots();
	RealtimeHostStats getStats() const;

	double getCurrentSampleRateForPlugins() const noexcept { return currentSampleRate; }
	int getCurrentBlockSizeForPlugins() const noexcept { return currentBlockSize; }

	const String getName() const override { return "Light Host Modern Serial Chain"; }
	void prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock) override;
	void releaseResources() override;
	bool isBusesLayoutSupported(const BusesLayout&) const override { return true; }
	void processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages) override;

	bool acceptsMidi() const override { return true; }
	bool producesMidi() const override { return true; }
	bool isMidiEffect() const override { return false; }
	double getTailLengthSeconds() const override { return 0.0; }

	int getNumPrograms() override { return 1; }
	int getCurrentProgram() override { return 0; }
	void setCurrentProgram(int) override { }
	const String getProgramName(int) override { return {}; }
	void changeProgramName(int, const String&) override { }

	bool hasEditor() const override { return false; }
	AudioProcessorEditor* createEditor() override { return nullptr; }

	void getStateInformation(MemoryBlock&) override { }
	void setStateInformation(const void*, int) override { }

private:
	static constexpr int maxScratchChannels = 64;

	void prepareSnapshot(ChainSnapshot& snapshot);
	void processSlot(PluginSlot& slot, AudioBuffer<float>& buffer, MidiBuffer& midiMessages);
	static float calculatePeakLevel(const AudioBuffer<float>& buffer) noexcept;

	mutable std::shared_ptr<ChainSnapshot> activeSnapshot;
	std::vector<std::shared_ptr<ChainSnapshot>> retiredSnapshots;
	std::atomic<uint64> processFailureCount { 0 };
	std::atomic<float> lastInputLevel { 0.0f };
	std::atomic<float> lastOutputLevel { 0.0f };
	double currentSampleRate = 44100.0;
	int currentBlockSize = 512;
	AudioBuffer<float> scratchBuffer;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RealtimeHostProcessor)
};

#endif /* RealtimeHostProcessor_h */

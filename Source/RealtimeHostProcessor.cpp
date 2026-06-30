#include "RealtimeHostProcessor.h"

#include <algorithm>

void lightHostLog(const String& message);
void setLightHostCrashContext(const String& context);

PluginSlot::PluginSlot(PluginDescription descriptionIn, std::unique_ptr<AudioPluginInstance> processorIn)
	: description(std::move(descriptionIn)),
	  processor(std::move(processorIn))
{
	if (processor != nullptr)
	{
		inputChannels = jmax(1, jmax(processor->getTotalNumInputChannels(), description.numInputChannels));
		outputChannels = jmax(1, jmax(processor->getTotalNumOutputChannels(), description.numOutputChannels));
	}
}

PluginSlot::~PluginSlot()
{
	release();
}

void PluginSlot::prepare(double sampleRateIn, int blockSizeIn)
{
	if (processor == nullptr)
		return;

	const int inputs = jmax(1, inputChannels);
	const int outputs = jmax(1, outputChannels);

	const int channels = jmax(inputs, outputs);
	if (prepared && preparedSampleRate == sampleRateIn && preparedBlockSize == blockSizeIn && preparedChannels == channels)
		return;

	if (prepared)
		processor->releaseResources();

	processor->setPlayConfigDetails(inputs, outputs, sampleRateIn, blockSizeIn);
	processor->prepareToPlay(sampleRateIn, blockSizeIn);
	latencySamples = jmax(0, processor->getLatencySamples());
	prepareBypassDelay(channels, blockSizeIn);
	prepared = true;
	preparedSampleRate = sampleRateIn;
	preparedBlockSize = blockSizeIn;
	preparedChannels = channels;
}

void PluginSlot::release()
{
	if (processor != nullptr && prepared)
	{
		processor->releaseResources();
		prepared = false;
		preparedSampleRate = 0.0;
		preparedBlockSize = 0;
		preparedChannels = 0;
	}
}

void PluginSlot::processBypass(AudioBuffer<float>& buffer)
{
	const int numSamples = buffer.getNumSamples();
	if (latencySamples <= 0 || numSamples <= 0)
		return;

	if (bypassDelayBuffer.getNumChannels() < buffer.getNumChannels()
		|| bypassDelayBuffer.getNumSamples() < latencySamples)
		return;

	for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
	{
		float* const channelData = buffer.getWritePointer(channel);
		float* const delayData = bypassDelayBuffer.getWritePointer(channel);
		int writePosition = bypassDelayWritePosition;

		for (int sample = 0; sample < numSamples; ++sample)
		{
			const float delayed = delayData[writePosition];
			delayData[writePosition] = channelData[sample];
			channelData[sample] = delayed;

			if (++writePosition >= latencySamples)
				writePosition = 0;
		}
	}

	bypassDelayWritePosition += numSamples;
	bypassDelayWritePosition %= latencySamples;
}

void PluginSlot::prepareBypassDelay(int channels, int blockSizeIn)
{
	const int delaySamples = jmax(0, latencySamples);
	const int delayChannels = jmax(1, channels);

	bypassDelayWritePosition = 0;

	if (delaySamples == 0)
	{
		bypassDelayBuffer.setSize(delayChannels, 1, false, false, true);
		bypassDelayBuffer.clear();
		return;
	}

	bypassDelayBuffer.setSize(delayChannels, delaySamples + jmax(1, blockSizeIn), false, false, true);
	bypassDelayBuffer.clear();
}

RealtimeHostProcessor::RealtimeHostProcessor()
	: AudioProcessor(BusesProperties()
		.withInput("Input", AudioChannelSet::stereo(), true)
		.withOutput("Output", AudioChannelSet::stereo(), true))
{
	scratchBuffer.setSize(maxScratchChannels, currentBlockSize, false, false, true);
}

RealtimeHostProcessor::~RealtimeHostProcessor()
{
	std::atomic_store_explicit(&activeSnapshot, std::shared_ptr<ChainSnapshot>(), std::memory_order_release);
	retiredSnapshots.clear();
}

void RealtimeHostProcessor::publishSnapshot(std::shared_ptr<ChainSnapshot> snapshot)
{
	if (snapshot != nullptr)
		prepareSnapshot(*snapshot);

	if (auto previous = std::atomic_load_explicit(&activeSnapshot, std::memory_order_acquire))
		retiredSnapshots.push_back(std::move(previous));

	std::atomic_store_explicit(&activeSnapshot, std::move(snapshot), std::memory_order_release);
	collectRetiredSnapshots();
}

std::shared_ptr<ChainSnapshot> RealtimeHostProcessor::getActiveSnapshot() const
{
	return std::atomic_load_explicit(&activeSnapshot, std::memory_order_acquire);
}

RealtimeHostStats RealtimeHostProcessor::getStats() const
{
	RealtimeHostStats stats;
	stats.processFailures = processFailureCount.load(std::memory_order_relaxed);

	if (auto snapshot = getActiveSnapshot())
	{
		stats.loadedSlots = (int) snapshot->slots.size();
		stats.chainLatencySamples = snapshot->totalLatencySamples;
		stats.reusedSlots = snapshot->reusedSlots;
		stats.rebuiltSlots = snapshot->rebuiltSlots;
	}

	stats.inputLevel = lastInputLevel.load(std::memory_order_relaxed);
	stats.outputLevel = lastOutputLevel.load(std::memory_order_relaxed);
	return stats;
}

void RealtimeHostProcessor::collectRetiredSnapshots()
{
	retiredSnapshots.erase(std::remove_if(retiredSnapshots.begin(), retiredSnapshots.end(),
		[] (const std::shared_ptr<ChainSnapshot>& snapshot)
		{
			return snapshot == nullptr || snapshot.use_count() == 1;
		}),
		retiredSnapshots.end());
}

void RealtimeHostProcessor::prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock)
{
	currentSampleRate = sampleRate > 0.0 ? sampleRate : 44100.0;
	currentBlockSize = jmax(1, maximumExpectedSamplesPerBlock);
	scratchBuffer.setSize(maxScratchChannels, currentBlockSize, false, false, true);

	if (auto snapshot = getActiveSnapshot())
		prepareSnapshot(*snapshot);
}

void RealtimeHostProcessor::releaseResources()
{
	if (auto snapshot = getActiveSnapshot())
		for (auto& slot : snapshot->slots)
			if (slot != nullptr)
				slot->release();
}

void RealtimeHostProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
	ScopedNoDenormals noDenormals;
	lastInputLevel.store(calculatePeakLevel(buffer), std::memory_order_relaxed);

	if (auto snapshot = getActiveSnapshot())
		for (auto& slot : snapshot->slots)
		{
			if (slot == nullptr || slot->processor == nullptr)
				continue;

			if (slot->bypassed.load(std::memory_order_relaxed))
				slot->processBypass(buffer);
			else
				processSlot(*slot, buffer, midiMessages);
		}

	lastOutputLevel.store(calculatePeakLevel(buffer), std::memory_order_relaxed);
}

void RealtimeHostProcessor::prepareSnapshot(ChainSnapshot& snapshot)
{
	lightHostLog("RealtimeHostProcessor prepareSnapshot begin slots=" + String((int) snapshot.slots.size()));
	snapshot.sampleRate = currentSampleRate;
	snapshot.blockSize = currentBlockSize;
	snapshot.maxPluginChannels = jmax(snapshot.inputChannels, snapshot.outputChannels);
	snapshot.totalLatencySamples = 0;

	for (auto& slot : snapshot.slots)
	{
		if (slot == nullptr)
			continue;

		snapshot.maxPluginChannels = jmax(snapshot.maxPluginChannels, jmax(slot->inputChannels, slot->outputChannels));

		try
		{
			setLightHostCrashContext("RealtimeHostProcessor::prepareSnapshot prepare '" + slot->description.name
				+ "' sampleRate=" + String(currentSampleRate)
				+ " blockSize=" + String(currentBlockSize)
				+ " inputs=" + String(slot->inputChannels)
				+ " outputs=" + String(slot->outputChannels));
			lightHostLog("RealtimeHostProcessor prepare slot begin '" + slot->description.name + "'");
			slot->prepare(currentSampleRate, currentBlockSize);
			lightHostLog("RealtimeHostProcessor prepare slot completed '" + slot->description.name + "'");
		}
		catch (const std::exception& e)
		{
			lightHostLog("RealtimeHostProcessor prepare slot C++ exception '" + slot->description.name + "': " + String(e.what()));
			slot->processDisabled.store(true, std::memory_order_release);
			slot->processFailed.store(true, std::memory_order_release);
			processFailureCount.fetch_add(1, std::memory_order_relaxed);
		}
		catch (...)
		{
			lightHostLog("RealtimeHostProcessor prepare slot unknown exception '" + slot->description.name + "'");
			slot->processDisabled.store(true, std::memory_order_release);
			slot->processFailed.store(true, std::memory_order_release);
			processFailureCount.fetch_add(1, std::memory_order_relaxed);
		}

		snapshot.totalLatencySamples += slot->getLatencySamples();
	}

	setLatencySamples(snapshot.totalLatencySamples);
	lightHostLog("RealtimeHostProcessor prepareSnapshot completed latencySamples=" + String(snapshot.totalLatencySamples));
}

void RealtimeHostProcessor::processSlot(PluginSlot& slot, AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
	const int numSamples = buffer.getNumSamples();
	if (numSamples <= 0)
		return;

	if (slot.processDisabled.load(std::memory_order_acquire))
	{
		slot.processBypass(buffer);
		return;
	}

	const int neededChannels = jmax(buffer.getNumChannels(), jmax(slot.inputChannels, slot.outputChannels));
	if (neededChannels > maxScratchChannels)
		return;

	if (neededChannels <= buffer.getNumChannels())
	{
		try
		{
			slot.processor->processBlock(buffer, midiMessages);
		}
		catch (...)
		{
			slot.processDisabled.store(true, std::memory_order_release);
			slot.processFailed.store(true, std::memory_order_release);
			processFailureCount.fetch_add(1, std::memory_order_relaxed);
			slot.processBypass(buffer);
		}

		for (int channel = slot.outputChannels; channel < buffer.getNumChannels(); ++channel)
			buffer.clear(channel, 0, numSamples);

		return;
	}

	if (numSamples > scratchBuffer.getNumSamples())
		return;

	scratchBuffer.clear(0, numSamples);
	for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
		scratchBuffer.copyFrom(channel, 0, buffer, channel, 0, numSamples);

	AudioBuffer<float> expandedBuffer(scratchBuffer.getArrayOfWritePointers(), neededChannels, numSamples);
	try
	{
		slot.processor->processBlock(expandedBuffer, midiMessages);
	}
	catch (...)
	{
		slot.processDisabled.store(true, std::memory_order_release);
		slot.processFailed.store(true, std::memory_order_release);
		processFailureCount.fetch_add(1, std::memory_order_relaxed);
		slot.processBypass(buffer);
		return;
	}

	const int channelsToCopy = jmin(buffer.getNumChannels(), slot.outputChannels);
	for (int channel = 0; channel < channelsToCopy; ++channel)
		buffer.copyFrom(channel, 0, scratchBuffer, channel, 0, numSamples);

	for (int channel = channelsToCopy; channel < buffer.getNumChannels(); ++channel)
		buffer.clear(channel, 0, numSamples);
}

float RealtimeHostProcessor::calculatePeakLevel(const AudioBuffer<float>& buffer) noexcept
{
	float peak = 0.0f;
	for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
		peak = jmax(peak, buffer.getMagnitude(channel, 0, buffer.getNumSamples()));

	return jlimit(0.0f, 1.0f, peak);
}

#ifndef HostIpcServer_h
#define HostIpcServer_h

#include "AudioEngine.h"
#include <atomic>
#include <functional>
#include <thread>

class HostIpcServer
{
public:
	explicit HostIpcServer(AudioEngine& engineToExpose);
	HostIpcServer(AudioEngine& engineToExpose, std::function<void()> trayIconChangedCallback);
	~HostIpcServer();

	String getPipeName() const { return pipeName; }

private:
	void run();
	void wakeServer();
	String processRequestOnMessageThread(const String& request);
	String processRequest(const String& request);
	String buildTelemetry();
	String buildSnapshot();
	String commandOk();
	String commandResult(bool success);

	static String escapeJson(const String& value);
	static String quote(const String& value);
	static String stringArrayJson(const std::vector<String>& values);
	static String numberArrayJson(const std::vector<int>& values);
	static String numberArrayJson(const std::vector<double>& values);
	static String boolArrayJson(const std::vector<bool>& values);

	AudioEngine& engine;
	std::function<void()> trayIconChanged;
	String pipeName;
	std::atomic<bool> stopping { false };
	std::thread worker;
};

#endif /* HostIpcServer_h */

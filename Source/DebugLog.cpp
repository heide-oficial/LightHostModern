#include "DebugLog.h"

#if JUCE_WINDOWS
 #ifndef NOMINMAX
  #define NOMINMAX
 #endif
 #include <Windows.h>
 #include <cstdio>
 #include <iostream>
#endif

#include <atomic>
#include <cstdlib>
#include <exception>

namespace
{
	bool debugEnabled = false;
	bool consoleOpened = false;
	bool crashDiagnosticsInstalled = false;
	CriticalSection crashContextLock;
	String crashContext;
	std::atomic<bool> fatalCrashLogged { false };

	void writeFatalCrashLog(const String& reason, void* address = nullptr, uint32 code = 0)
	{
		if (!debugEnabled || fatalCrashLogged.exchange(true))
			return;

		const ScopedLock lock(crashContextLock);
		std::cout << "[LightHost] Fatal crash diagnostic: " << reason << std::endl;

		if (code != 0)
			std::cout << "[LightHost] Fatal crash exception code: 0x" << std::hex << code << std::dec << std::endl;

		if (address != nullptr)
			std::cout << "[LightHost] Fatal crash address: " << address << std::endl;

		if (crashContext.isNotEmpty())
			std::cout << "[LightHost] Fatal crash context: " << crashContext << std::endl;

		std::cout.flush();
	}

#if JUCE_WINDOWS
	LONG WINAPI lightHostUnhandledExceptionFilter(EXCEPTION_POINTERS* exceptionInfo)
	{
		uint32 code = 0;
		void* address = nullptr;

		if (exceptionInfo != nullptr && exceptionInfo->ExceptionRecord != nullptr)
		{
			code = (uint32) exceptionInfo->ExceptionRecord->ExceptionCode;
			address = exceptionInfo->ExceptionRecord->ExceptionAddress;
		}

		writeFatalCrashLog("Unhandled native exception", address, code);
		return EXCEPTION_CONTINUE_SEARCH;
	}
#endif

	void lightHostTerminateHandler()
	{
		writeFatalCrashLog("std::terminate called");
		std::abort();
	}
}

void setLightHostDebugEnabled(bool enabled)
{
	debugEnabled = enabled;
}

bool isLightHostDebugEnabled()
{
	return debugEnabled;
}

void openLightHostDebugConsoleIfNeeded()
{
#if JUCE_WINDOWS
	if (!debugEnabled || consoleOpened)
		return;

	if (!AllocConsole())
		return;

	consoleOpened = true;
	SetConsoleTitleW(L"Light Host Modern Debug Console");

	FILE* stream = nullptr;
	freopen_s(&stream, "CONOUT$", "w", stdout);
	freopen_s(&stream, "CONOUT$", "w", stderr);
	freopen_s(&stream, "CONIN$", "r", stdin);

	std::ios::sync_with_stdio(true);
	std::cout.clear();
	std::cerr.clear();
	std::clog.clear();

	lightHostLog("Debug console opened.");
#endif
}

void installLightHostCrashDiagnostics()
{
	if (!debugEnabled || crashDiagnosticsInstalled)
		return;

	crashDiagnosticsInstalled = true;
	std::set_terminate(lightHostTerminateHandler);

#if JUCE_WINDOWS
	SetUnhandledExceptionFilter(lightHostUnhandledExceptionFilter);
#endif

	lightHostLog("Crash diagnostics installed.");
}

void lightHostLog(const String& message)
{
	if (!debugEnabled)
		return;

	std::cout << "[LightHost] " << message << std::endl;
}

void setLightHostCrashContext(const String& context)
{
	if (!debugEnabled)
		return;

	const ScopedLock lock(crashContextLock);
	crashContext = context;
}

void clearLightHostCrashContext()
{
	if (!debugEnabled)
		return;

	const ScopedLock lock(crashContextLock);
	crashContext.clear();
}

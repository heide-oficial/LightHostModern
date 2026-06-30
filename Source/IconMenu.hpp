#ifndef IconMenu_hpp
#define IconMenu_hpp

#include "AudioEngine.h"
#include "HostIpcServer.h"

class IconMenu : public SystemTrayIconComponent, private MultiTimer
{
public:
    IconMenu(bool startInSafeMode = false, bool debugEnabled = false, bool restoreActivePluginsOnStartup = false);
    ~IconMenu() override;

    void mouseDown(const MouseEvent&) override;
    static void menuInvocationCallback(int id, IconMenu*);

	const int INDEX_OPEN_WINUI, INDEX_QUIT;

private:
	enum TimerIds
	{
		menuTimerId = 1
	};

	void timerCallback(int timerId) override;
	void showNativeContextMenu();
	void openWinUI();
	bool openPackagedWinUI(const String& parameters);
	String resolvePackagedWinUIAumid();
	void setIcon();

    std::unique_ptr<AudioEngine> engine;
	std::unique_ptr<HostIpcServer> ipcServer;
    PopupMenu menu;
	bool debugMode = false;
	int x = 0, y = 0;

};

#endif /* IconMenu_hpp */

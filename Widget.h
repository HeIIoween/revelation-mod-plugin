#pragma once

#define WINDOW_TITLE_MAX_LEN 256
#define WINDOW_CLASS_MAX_LEN 256

#define WINDOWID_FLSERVER_MENUIDSUMVIEW 0x800c
#define WINDOWID_FLSERVER_STATUSFRAME 0xe900

#define WIDGET_WM_INIT_WIDGETS 0xeeff

#include <string>
#include <memory>

#include "../flhookplugin_sdk/headers/FLHook.h"

using namespace std;

namespace raincious
{
	namespace FLHookPlugin
	{
		namespace Revelation
		{
			namespace Widget
			{

				class WidgetBase
				{
				public:
					WidgetBase();
					virtual ~WidgetBase();

					virtual void create(HWND parent, uint top, uint left, uint width, uint height);

					// Automatically call in every second for reloading data
					virtual void tick();

					// Self event proc
					virtual LRESULT CALLBACK proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

					// Automatically call when window event been fired (You need to check if thats your event)
					virtual void onCreate(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
					virtual void onClose(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
					virtual void onDestory(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
					virtual void onSize(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, uint top, uint left, uint width, uint height);
					virtual void onMove(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, uint top, uint left, uint width, uint height);
					virtual void onCommand(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
					virtual void onUpdate(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
					virtual void onEvent(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
					virtual void onPaint(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, HDC windowClientArea);
					virtual void onDisable();
					virtual void onRemove();

					// Status set & get
					void setDisable(bool dis);
					bool isDisabled();

					void setPause(bool pus);
					bool isPaused();

				protected:
					typedef struct WidgetSubClassInfo
					{
						WidgetBase* Instance = NULL;
						WNDPROC OldProc = NULL;
					};

					bool disabled = false;
					bool paused = false;

					HWND widget = NULL;

					static UINT_PTR SubClassIDs;
					static map <UINT_PTR, WidgetSubClassInfo> InstanceSubClassIDMap;

					static LRESULT CALLBACK WidgetProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

					static WidgetBase* GetInstanceBySubClassID(UINT_PTR uIdSubclass);
					static bool SetWidgetSubClassProc(WidgetBase* instance);
					static bool RemoveWidgetSubClass(WidgetBase* instance);
					static LRESULT WINAPI CallWidgetDefaultProc(UINT_PTR uIdSubclass, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
				};

				// Mounted widgets
				typedef map <const char*, unique_ptr<WidgetBase>> MountedWidgetList;

				// Window weights for searching FLServer window
				typedef map <HWND, uint> WindowWeights;

				// Information for window searching and binding
				typedef struct ProcessSearchInfo
				{
					DWORD processID = 0;
					WindowWeights rootWindows;
				} processSearchInfo;

				class Main
				{
				public:
					static uint Width;
					static uint Height;

					static Main* Get();
					static void Free();

					// Mount and unmount a widget
					bool mount(const char* name, unique_ptr<WidgetBase> widget);
					bool demount(const char* name);

					void run();

				protected:
					static Main* Instance;

					bool paused = false;

					MountedWidgetList list;
					RECT windowOriginalRect, windowExpectingRect, widgetTargetOriginalRect;
					uint windowExpectingWidth, windowExpectingHeight, windowOriginalWidth, windowOriginalHeight;
					uint widgetTargetOriginalWidth, widgetTargetOriginalHeight;
					uint widgetCurrentTop, widgetCurrentLeft, widgetCurrentWidth, widgetCurrentHeight;
					HANDLE tickThreadHandle, uiThreadHandle;
					HWND window, widgetWindow;
					HHOOK windowEventHook;
					WNDPROC oldWindowProc = NULL;

					bool threadStopSignal = false, tickThreadRunning = false, uiThreadRunning = false;

					static BOOL CALLBACK EnumServerWindow(HWND hwnd, LPARAM lParam);
					static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
					static void UIDataUpdateThread();
					static void UIMessagingThread();

					Main();
					~Main();

					// Call when adding widgets to the widget area
					void create();
					void tick();

					// Total windows event hooks
					void onCreate(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
					void onDestory(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
					void onClose(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
					void onSize(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
					void onMove(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
					void onCommand(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
					void onUpdate(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
					void onPaint(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, HDC windowClientArea);
					void onEvent(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
					
					// Window size related functions
					void fixWindowSize();
					void fixWidgetSize();
				};

			}
		}
	}
}
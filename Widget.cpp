#include "Widget.h"
#include "Print.h"

#include <commctrl.h>

namespace raincious
{
	namespace FLHookPlugin
	{
		namespace Revelation
		{
			namespace Widget
			{
				map <UINT_PTR, WidgetBase::WidgetSubClassInfo> WidgetBase::InstanceSubClassIDMap;
				UINT_PTR WidgetBase::SubClassIDs = 0;

				WidgetBase* WidgetBase::GetInstanceBySubClassID(UINT_PTR uIdSubclass)
				{
					if (InstanceSubClassIDMap.find(uIdSubclass) == InstanceSubClassIDMap.end())
					{
						return nullptr;
					}

					return InstanceSubClassIDMap[uIdSubclass].Instance;
				}

				bool WidgetBase::SetWidgetSubClassProc(WidgetBase* instance)
				{
					UINT_PTR classIDs;
					WNDPROC oldProc;

					if (instance->widget == NULL)
					{
						return false;
					}

					RemoveWidgetSubClass(instance);

					oldProc = (WNDPROC)GetWindowLongPtr(instance->widget, GWLP_WNDPROC);

					if (oldProc == 0)
					{
						return false;
					}

					classIDs = SubClassIDs++;

					if (!SetWindowSubclass(instance->widget, (SUBCLASSPROC)WidgetProc, classIDs, NULL))
					{
						return false;
					}

					InstanceSubClassIDMap[classIDs].Instance = instance;
					InstanceSubClassIDMap[classIDs].OldProc = oldProc;

					return true;
				}

				bool WidgetBase::RemoveWidgetSubClass(WidgetBase* instance)
				{
					map <UINT_PTR, WidgetSubClassInfo>::iterator iter;

					for (iter = InstanceSubClassIDMap.begin(); iter != InstanceSubClassIDMap.end(); ++iter)
					{
						if (iter->second.Instance == instance)
						{
							InstanceSubClassIDMap.erase(iter->first);

							return true;
						}
					}

					return false;
				}

				LRESULT CALLBACK WidgetBase::WidgetProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
				{
					WidgetBase* instance = GetInstanceBySubClassID(uIdSubclass);

					if (instance == nullptr)
					{
						return DefWindowProc(hWnd, uMsg, wParam, lParam);
					}

					return instance->proc(hWnd, uMsg, wParam, lParam, uIdSubclass, dwRefData);
				}
				
				LRESULT WINAPI WidgetBase::CallWidgetDefaultProc(UINT_PTR uIdSubclass, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
				{
					if (InstanceSubClassIDMap.find(uIdSubclass) == InstanceSubClassIDMap.end())
					{
						return false;
					}

					return CallWindowProc(InstanceSubClassIDMap[uIdSubclass].OldProc, hWnd, uMsg, wParam, lParam);
				}

				WidgetBase::WidgetBase()
				{

				}

				WidgetBase::~WidgetBase()
				{

				}

				// WidgetObject
				void WidgetBase::create(HWND parent, uint top, uint left, uint width, uint height)
				{
				}

				void WidgetBase::tick()
				{
				}

				LRESULT CALLBACK WidgetBase::proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
				{
					return DefWindowProc(hWnd, uMsg, wParam, lParam);
				}

				void WidgetBase::onCreate(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
				{
				}

				void WidgetBase::onClose(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
				{
				}

				void WidgetBase::onDestory(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
				{
				}

				void WidgetBase::onSize(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, uint top, uint left, uint width, uint height)
				{
					SetWindowPos(
						widget,
						NULL,
						left,
						top,
						width,
						height,
						SWP_NOZORDER | SWP_NOACTIVATE
						);
				}

				void WidgetBase::onMove(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, uint top, uint left, uint width, uint height)
				{
				}

				void WidgetBase::onCommand(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
				{
				}

				void WidgetBase::onUpdate(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
				{
					UpdateWindow(widget);
				}

				void WidgetBase::onEvent(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
				{
				}

				void WidgetBase::onPaint(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, HDC windowClientArea)
				{
				}

				void WidgetBase::onDisable()
				{
				}

				void WidgetBase::onRemove()
				{
				}

				// Status
				void WidgetBase::setDisable(bool dis)
				{
					disabled = dis;
				}

				bool WidgetBase::isDisabled()
				{
					return disabled;
				}

				void WidgetBase::setPause(bool pus)
				{
					paused = pus;
				}

				bool WidgetBase::isPaused()
				{
					return paused;
				}

				Main* Main::Instance = nullptr;
				uint Main::Width = 0;
				uint Main::Height = 0;

				// Widget container
				Main* Main::Get()
				{
					if (Instance == nullptr)
					{
						Instance = new Main();
					}

					return Instance;
				}

				void Main::Free()
				{
					if (Instance == nullptr)
					{
						return;
					}

					delete Instance;
					Instance = nullptr;
				}

				BOOL CALLBACK Main::EnumServerWindow(HWND hwnd, LPARAM lParam)
				{
					DWORD cProID = 0;
					char windowTitle[WINDOW_TITLE_MAX_LEN];

					GetWindowThreadProcessId(hwnd, &cProID);

					ProcessSearchInfo* pEPI = (ProcessSearchInfo*)lParam;

					if (pEPI->processID != cProID)
					{
						return true;
					}

					HWND testWindow = GetAncestor(hwnd, GA_ROOTOWNER);

					GetWindowText(testWindow, windowTitle, WINDOW_TITLE_MAX_LEN);

					if (strstr(windowTitle, "FLServer") == nullptr)
					{
						return true;
					}

					// Count the weight
					pEPI->rootWindows[testWindow]++;

					return true;
				}

				LRESULT CALLBACK Main::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
				{
					HDC windowClientArea;
					PAINTSTRUCT ps;
					Main* main = Get();
					LRESULT callResult = 0;

					switch (uMsg)
					{
					// Add ID to the list to avoid call been pass to CallWindowProc
					case WIDGET_WM_INIT_WIDGETS:
						callResult = uMsg;
						break;

					default:
						callResult = CallWindowProc(main->oldWindowProc, hwnd, uMsg, wParam, lParam);
						break;
					}

					if (main->paused)
					{
						return callResult;
					}
					
					switch (uMsg)
					{
					case WIDGET_WM_INIT_WIDGETS:
						main->fixWindowSize();
						main->fixWidgetSize();

						main->create();
						
						UpdateWindow(hwnd);
						break;
						
					case WM_PAINT:
						windowClientArea = BeginPaint(hwnd, &ps);
						
						try
						{
							main->onPaint(hwnd, uMsg, wParam, lParam, windowClientArea);
						} 
						catch (...)
						{
							// Do nothing
						}

						EndPaint(hwnd, &ps);

						UpdateWindow(hwnd);
						break;

					case WM_CREATE:
						main->onCreate(hwnd, uMsg, wParam, lParam);
						main->onUpdate(hwnd, uMsg, wParam, lParam);

						UpdateWindow(hwnd);
						break;

					case WM_CLOSE:
						main->onClose(hwnd, uMsg, wParam, lParam);
						main->onUpdate(hwnd, uMsg, wParam, lParam);

						UpdateWindow(hwnd);
						break;

					case WM_DESTROY:
						main->onDestory(hwnd, uMsg, wParam, lParam);
						main->onUpdate(hwnd, uMsg, wParam, lParam);

						UpdateWindow(hwnd);
						break;

					case WM_MOVE:
						main->fixWidgetSize();

						main->onMove(hwnd, uMsg, wParam, lParam);
						main->onUpdate(hwnd, uMsg, wParam, lParam);

						UpdateWindow(hwnd);
						break;

					case WM_SIZING:
					case WM_SIZE:
						main->fixWidgetSize();
						main->onSize(hwnd, uMsg, wParam, lParam);
						main->onUpdate(hwnd, uMsg, wParam, lParam);

						UpdateWindow(hwnd);
						break;

					case WM_COMMAND:
						switch (LOWORD(wParam))
						{
						case WINDOWID_FLSERVER_MENUIDSUMVIEW:
							main->fixWindowSize();
							main->fixWidgetSize();
							break;

						default:
							break;
						}

						main->onCommand(hwnd, uMsg, wParam, lParam);
						main->onUpdate(hwnd, uMsg, wParam, lParam);

						UpdateWindow(hwnd);
						break;
						
					default:
						main->onEvent(hwnd, uMsg, wParam, lParam);
						break;
					}

					return callResult;
				}

				void Main::UIMessagingThread()
				{
					Main* main = Get();
					DWORD waitStatus = 0;
					short threadReturn = 0;
					MSG eventMessage;

					if (!GetWindowRect(main->window, &main->windowOriginalRect))
					{
						Print::Error("Can't get size of FLServer window. Binding aborted.", "");

						return;
					}
					else
					{
						// Get window original size
						main->windowOriginalWidth = main->windowOriginalRect.right - main->windowOriginalRect.left;
						main->windowOriginalHeight = main->windowOriginalRect.bottom - main->windowOriginalRect.top;

						// Set expecting rect based on expand setting
						main->windowExpectingRect.top = main->windowOriginalRect.top;
						main->windowExpectingRect.left = main->windowOriginalRect.left;

						main->windowExpectingRect.right = main->windowOriginalRect.right + Width;
						main->windowExpectingRect.bottom = main->windowOriginalRect.bottom + Height;

						main->windowExpectingWidth = main->windowExpectingRect.right - main->windowExpectingRect.left;
						main->windowExpectingHeight = main->windowExpectingRect.bottom - main->windowExpectingRect.top;
					}

					if (!GetClientRect(main->widgetWindow, &main->widgetTargetOriginalRect))
					{
						Print::Error("Can't get size of FLServer window. Binding aborted.", "");

						return;
					}
					else
					{
						main->widgetTargetOriginalWidth = main->widgetTargetOriginalRect.right - main->widgetTargetOriginalRect.left;
						main->widgetTargetOriginalHeight = main->widgetTargetOriginalRect.bottom - main->widgetTargetOriginalRect.top;

						if (Height > 0)
						{
							main->widgetCurrentTop = main->widgetTargetOriginalHeight;
						}
						else
						{
							main->widgetCurrentTop = 0;
						}

						if (Width > 0)
						{
							main->widgetCurrentLeft = main->widgetTargetOriginalWidth;
						}
						else
						{
							main->widgetCurrentLeft = 0;
						}
					}

					// Point WindowProc to our's
					main->oldWindowProc = (WNDPROC)SetWindowLongPtr(main->window, GWLP_WNDPROC, (LONG_PTR)WindowProc);

					if (main->oldWindowProc == NULL)
					{
						Print::Error("Can't re-point WindowProc handler, widget will not be enabled.", "");

						main->threadStopSignal = false;

						ExitThread(1); return;
					}

					// Send message, tell window proc to init widgets
					if (!CallWindowProc(WindowProc, main->window, WIDGET_WM_INIT_WIDGETS, NULL, NULL))
					{
						main->threadStopSignal = false;

						ExitThread(1); return;
					}

					main->uiThreadRunning = true;

					try
					{
						while (!main->threadStopSignal)
						{
							waitStatus = MsgWaitForMultipleObjects(0, NULL, FALSE, 1000, QS_ALLINPUT);

							if (waitStatus == WAIT_TIMEOUT)
							{
								continue;
							}
							else if (waitStatus == WAIT_FAILED)
							{
								break;
							}

							if (!IsWindow(main->window))
							{
								break;
							}

							while (PeekMessage(&eventMessage, main->window, 0, 0, PM_REMOVE))
							{
								TranslateMessage(&eventMessage);
								DispatchMessage(&eventMessage);
							}
						}
					}
					catch (exception &e)
					{
						threadReturn = 1;

						main->threadStopSignal = false;

						Print::Error(e.what(), "");
					}

					main->uiThreadRunning = false;

					ExitThread(threadReturn);
				}

				void Main::UIDataUpdateThread()
				{
					Main* main = Get();

					main->tickThreadRunning = true;

					while (!main->threadStopSignal)
					{
						Sleep(1000);

						main->tick();
					}

					main->tickThreadRunning = false;

					ExitThread(0);
				}

				Main::Main()
				{
					uint currentWeight = 0;
					HWND currentWindow = NULL;
					ProcessSearchInfo pInfo;
					WindowWeights::iterator wwIter;

					if (Width == 0 && Height == 0)
					{
						Print::Error("Sorry, widget area must larger than 0*0 pixels to continue. Binding aborted.", "");

						return;
					}

					pInfo.processID = GetCurrentProcessId();

					EnumWindows(EnumServerWindow, (LPARAM)&pInfo);

					for (wwIter = pInfo.rootWindows.begin(); wwIter != pInfo.rootWindows.end(); ++wwIter)
					{
						if (wwIter->second > currentWeight)
						{
							currentWindow = wwIter->first;
							currentWeight = wwIter->second;
						}
					}

					// Check if that's truly a window
					if (currentWindow == NULL || !IsWindow(currentWindow))
					{
						Print::Error("Can't found the FLServer window. Binding aborted.", "");

						return;
					}
					else
					{
						window = currentWindow;
					}

					// Disable the maximize button, re-enable the code after making sure the auto resize will work
					/*SetWindowLong(
						currentWindow,
						GWL_STYLE,
						GetWindowLong(currentWindow, GWL_STYLE) & ~WS_MAXIMIZEBOX
						);*/

					// Get the window which we will put widget on
					widgetWindow = GetDlgItem(window, WINDOWID_FLSERVER_STATUSFRAME);

					if (widgetWindow == NULL)
					{
						Print::Error("Can't get server status frame. Binding aborted", "");

						return;
					}

					Print::Debug("The FLServer window has been found and binded.", "");
				}

				Main::~Main()
				{
					MountedWidgetList::iterator iter;

					threadStopSignal = true;

					Print::Debug("Exiting widget UI managing module ...", "");

					SetWindowPos(
						window,
						NULL,
						0,
						0,
						windowOriginalWidth,
						windowOriginalHeight,
						SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE
						);

					UpdateWindow(window);

					while (true)
					{
						Sleep(300);

						if (tickThreadRunning)
						{
							continue;
						}

						if (uiThreadRunning)
						{
							continue;
						}

						break;
					}

					threadStopSignal = false;

					CloseHandle(tickThreadHandle);

					Print::Debug("UI threads has been ended.", "");

					for (iter = list.begin(); iter != list.end(); ++iter)
					{
						demount(iter->first);
					}

					if (oldWindowProc != NULL)
					{
						SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)oldWindowProc);
					}

					Print::Debug("Widget has been stopped and released.", "");
				}

				// Mount & Demount
				bool Main::mount(const char* name, unique_ptr<WidgetBase> widget)
				{
					if (list.find(name) != list.end())
					{
						return false;
					}

					list[name] = move(widget);

					return true;
				}

				bool Main::demount(const char* name)
				{
					if (list.find(name) == list.end())
					{
						return false;
					}

					unique_ptr<WidgetBase> widget = move(list[name]);

					list.erase(name);

					widget->setDisable(true);

					widget->onRemove();

					return true;
				}

				void Main::run()
				{
					Print::Debug("Starting widgets ...", "");

					// Start UI response thread
					tickThreadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)UIDataUpdateThread, NULL, 0, NULL);
					uiThreadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)UIMessagingThread, NULL, 0, NULL);
				}

				void Main::onCreate(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
				{
					MountedWidgetList::iterator iter;

					for (iter = list.begin(); iter != list.end(); ++iter)
					{
						iter->second->onCreate(hwnd, uMsg, wParam, lParam);
					}
				}

				void Main::onClose(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
				{
					MountedWidgetList::iterator iter;

					for (iter = list.begin(); iter != list.end(); ++iter)
					{
						iter->second->onClose(hwnd, uMsg, wParam, lParam);
					}
				}

				void Main::onDestory(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
				{
					MountedWidgetList::iterator iter;

					for (iter = list.begin(); iter != list.end(); ++iter)
					{
						iter->second->onDestory(hwnd, uMsg, wParam, lParam);
					}
				}

				void Main::onMove(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
				{
					MountedWidgetList::iterator iter;

					for (iter = list.begin(); iter != list.end(); ++iter)
					{
						iter->second->onMove(
							hwnd,
							uMsg,
							wParam,
							lParam,
							widgetCurrentTop,
							widgetCurrentLeft,
							widgetCurrentWidth,
							widgetCurrentHeight
							);
					}
				}

				// Window hooks one by one call
				void Main::onSize(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
				{
					MountedWidgetList::iterator iter;

					for (iter = list.begin(); iter != list.end(); ++iter)
					{
						iter->second->onSize(
							hwnd, 
							uMsg, 
							wParam, 
							lParam,
							widgetCurrentTop, 
							widgetCurrentLeft, 
							widgetCurrentWidth, 
							widgetCurrentHeight
							);
					}
				}

				void Main::onCommand(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
				{
					MountedWidgetList::iterator iter;

					for (iter = list.begin(); iter != list.end(); ++iter)
					{
						iter->second->onCommand(hwnd, uMsg, wParam, lParam);
					}
				}

				void Main::onEvent(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
				{
					MountedWidgetList::iterator iter;

					for (iter = list.begin(); iter != list.end(); ++iter)
					{
						iter->second->onEvent(hwnd, uMsg, wParam, lParam);
					}
				}

				void Main::onUpdate(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
				{
					MountedWidgetList::iterator iter;

					for (iter = list.begin(); iter != list.end(); ++iter)
					{
						iter->second->onUpdate(hwnd, uMsg, wParam, lParam);
					}
				}

				void Main::onPaint(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, HDC windowClientArea)
				{
					MountedWidgetList::iterator iter;

					for (iter = list.begin(); iter != list.end(); ++iter)
					{
						iter->second->onPaint(hwnd, uMsg, wParam, lParam, windowClientArea);
					}
				}

				// Widget adding
				void Main::create()
				{
					MountedWidgetList::iterator iter;

					for (iter = list.begin(); iter != list.end(); ++iter)
					{
						iter->second->create(
							widgetWindow, 
							widgetCurrentTop, 
							widgetCurrentLeft, 
							widgetCurrentWidth, 
							widgetCurrentHeight
							);
					}
				}
				
				void Main::tick()
				{
					MountedWidgetList::iterator iter;

					for (iter = list.begin(); iter != list.end(); ++iter)
					{
						iter->second->tick();
					}
				}

				// Window size releated functions
				void Main::fixWindowSize()
				{
					RECT windowSize;
					uint currentWidth, currentHeight, resizingToWidth, resizingToHeight;
					
					if (threadStopSignal)
					{
						return;
					}

					if (!GetWindowRect(window, &windowSize))
					{
						return;
					}
					else
					{
						currentWidth = windowSize.right - windowSize.left;
						currentHeight = windowSize.bottom - windowSize.top;
					}

					resizingToWidth = currentWidth >= windowExpectingWidth ? currentWidth : windowExpectingWidth;
					resizingToHeight = currentHeight >= windowExpectingHeight ? currentHeight : windowExpectingHeight;

					SetWindowPos(
						window,
						NULL,
						0,
						0,
						Width > 0 ? resizingToWidth : currentWidth,
						Height > 0 ? resizingToHeight : currentHeight,
						SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE
						);
				}

				void Main::fixWidgetSize()
				{
					RECT widgetSize;
					uint widgetAreaWidth = 0, widgetAreaHeight = 0;
					
					if (!GetWindowRect(widgetWindow, &widgetSize))
					{
						return;
					}
					else
					{
						if (Width > 0)
						{
							widgetAreaWidth = (widgetSize.right - widgetSize.left) - widgetTargetOriginalWidth;
						}
						else
						{
							widgetAreaWidth = widgetSize.right - widgetSize.left;
						}

						if (Height > 0)
						{
							widgetAreaHeight = (widgetSize.bottom - widgetSize.top) - widgetTargetOriginalHeight;
						}
						else
						{
							widgetAreaHeight = widgetSize.bottom - widgetSize.top;
						}
					}

					// If user can't see any control, no need to update them
					if (widgetAreaWidth <= 0 && widgetAreaHeight <= 0)
					{
						paused = true;

						return;
					}
					else
					{
						paused = false;
					}
					
					widgetCurrentWidth = widgetAreaWidth;
					widgetCurrentHeight = widgetAreaHeight;

					UpdateWindow(widgetWindow);
				}
			}
		}
	}
}
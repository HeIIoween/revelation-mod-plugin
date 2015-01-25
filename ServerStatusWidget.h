#pragma once

#include <windows.h>
#include <commctrl.h>

#include "Widget.h"
#include "Print.h"

#include "../flhookplugin_sdk/headers/FLHook.h"

namespace raincious
{
	namespace FLHookPlugin
	{
		namespace Revelation
		{
			namespace Widget
			{

				class ServerStatus : public WidgetBase
				{
				public:
					virtual ~ServerStatus();

					void create(HWND parent, uint top, uint left, uint width, uint height);
					void onSize(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, uint top, uint left, uint width, uint height);
					void onPaint(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, HDC windowClientArea);
					void tick();

				protected:
					static uint Margin, MaxPlayer, MaxServerLoad;

					HDC hDC = NULL, drawDC = NULL;
					HBITMAP drawBitmap = NULL;
					HPEN whiteSpacePen, statusChangePen, refreshLinePen, serverStatusPen, playerCounterPen;
					uint windowTop = 0, windowLeft = 0, windowWidth = 0, windowHeight = 0, windowHalfHeight = 0;
					uint drawPosition = 0;
					double playerCountNumberMappingFactor = 0.0, serverLoadNumberMappingFactor = 0.0;
					
					static double getNumberMappingFactor(uint fromMin, uint fromMax, uint toMin, uint toMax);

					uint getNextDrawPosition();
					uint moveToNextDrawPosition();

					void drawLine(HPEN pen, uint leftPosition, uint topPosition, uint height);

					void drawStatus();
					void pushDrawToDisplay();
					void resizeDraw(uint oldWidth, uint oldHeight);
				};

			}
		}
	}
}
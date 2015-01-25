#include "ServerStatusWidget.h"

namespace raincious
{
	namespace FLHookPlugin
	{
		namespace Revelation
		{
			namespace Widget
			{
				uint ServerStatus::Margin = 1;
				uint ServerStatus::MaxPlayer = 0;
				uint ServerStatus::MaxServerLoad = 0;

				ServerStatus::~ServerStatus()
				{
					if (drawDC != NULL)
					{
						DeleteDC(drawDC);
					}

					if (widget != NULL)
					{
						DeleteObject(whiteSpacePen);
						DeleteObject(statusChangePen);
						DeleteObject(refreshLinePen);
						DeleteObject(serverStatusPen);
						DeleteObject(playerCounterPen);

						ReleaseDC(widget, hDC);
					}

					if (drawBitmap != NULL)
					{
						DeleteObject(drawBitmap);
					}
				}

				void ServerStatus::create(HWND parent, uint top, uint left, uint width, uint height)
				{
					InitCommonControls();

					windowTop = (top + height) - 80;
					windowLeft = left + 10;
					windowWidth = width - 20;
					windowHeight = 70;
					windowHalfHeight = windowHeight / 2;

					widget = CreateWindowEx(
						WS_EX_TRANSPARENT,
						WC_BUTTON,
						TEXT(""),
						WS_CHILD | WS_DISABLED | BS_FLAT,
						windowLeft,
						windowTop,
						windowWidth,
						windowHeight,
						parent,
						NULL,
						(HINSTANCE)GetModuleHandle(NULL),
						NULL
						);

					if (widget == NULL)
					{
						return;
					}
					
					hDC = GetDC(widget);
					drawDC = CreateCompatibleDC(hDC);
					drawBitmap = CreateCompatibleBitmap(hDC, windowWidth, windowHeight);

					if (hDC == NULL || drawDC == NULL || drawBitmap == NULL)
					{
						return;
					}

					SelectObject(drawDC, drawBitmap);

					RECT rc;
					rc.left = 0;
					rc.top = 0;
					rc.right = windowWidth;
					rc.bottom = windowHeight;

					HBRUSH brush = CreateSolidBrush(RGB(30, 50, 80));

					if (brush != NULL)
					{
						FillRect(drawDC, &rc, brush);

						DeleteObject(brush);
					}
					
					whiteSpacePen = CreatePen(PS_SOLID, 1, RGB(30, 50, 80));
					statusChangePen = CreatePen(PS_SOLID, 1, RGB(90, 120, 150));
					refreshLinePen = CreatePen(PS_SOLID, 1, RGB(180, 50, 30));
					serverStatusPen = CreatePen(PS_SOLID, 1, RGB(70, 200, 200));
					playerCounterPen = CreatePen(PS_SOLID, 1, RGB(50, 200, 140));

					ShowWindow(widget, SW_SHOW);
				}

				void ServerStatus::onSize(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, uint top, uint left, uint width, uint height)
				{
					uint oldWidth = windowWidth, oldHeight = windowHeight;

					// Get new size data
					windowTop = (top + height) - 80;
					windowLeft = left + 10;
					windowWidth = width - 20;
					// windowHeight = 70;
					// windowHalfHeight = windowHeight / 2;

					if (!SetWindowPos(
						widget,
						NULL,
						windowLeft,
						windowTop,
						windowWidth,
						windowHeight,
						SWP_NOZORDER | SWP_NOACTIVATE
						))
					{
						return;
					}

					resizeDraw(oldWidth, oldHeight);
				}

				void ServerStatus::resizeDraw(uint oldWidth, uint oldHeight)
				{
					HDC tempDC = NULL;
					HBITMAP newDrawBitmap = NULL, tempBMap = NULL;
					HBRUSH brush = NULL;
					RECT rc;

					if (hDC == NULL || drawDC == NULL || drawBitmap == NULL)
					{
						return;
					}

					if (!IsWindowVisible(widget))
					{
						return;
					}

					rc.left = 0;
					rc.top = 0;
					rc.right = windowWidth;
					rc.bottom = windowHeight;

					newDrawBitmap = CreateCompatibleBitmap(hDC, windowWidth, windowHeight);

					if (newDrawBitmap == NULL)
					{
						return;
					}

					brush = CreateSolidBrush(RGB(30, 50, 80));

					if (brush == NULL)
					{
						DeleteObject(newDrawBitmap);

						return;
					}

					tempDC = CreateCompatibleDC(hDC);

					if (tempDC == NULL)
					{
						DeleteObject(newDrawBitmap);
						DeleteObject(brush);

						return;
					}

					tempBMap = CreateCompatibleBitmap(hDC, windowWidth, windowHeight);

					if (tempBMap == NULL)
					{
						DeleteObject(newDrawBitmap);
						DeleteObject(brush);
						DeleteDC(tempDC);

						return;
					}

					SelectObject(tempDC, tempBMap);

					FillRect(tempDC, &rc, brush);

					if (!BitBlt(tempDC, 0, 0, oldWidth, oldHeight, drawDC, 0, 0, SRCCOPY))
					{
						DeleteDC(tempDC);
						DeleteObject(tempBMap);
						DeleteObject(newDrawBitmap);
						DeleteObject(brush);

						return;
					}

					DeleteObject(drawBitmap); // Delete old DC Bitmap, give space to the new one

					drawBitmap = newDrawBitmap;
					SelectObject(drawDC, drawBitmap);

					FillRect(drawDC, &rc, brush);

					if (!BitBlt(drawDC, 0, 0, windowWidth, windowHeight, tempDC, 0, 0, SRCCOPY))
					{
						DeleteDC(tempDC);
						DeleteDC(drawDC);
						DeleteObject(drawBitmap);
						DeleteObject(tempBMap);
						DeleteObject(brush);

						drawDC = NULL;
						drawBitmap = NULL;

						return;
					}

					DeleteDC(tempDC);
					DeleteObject(brush);
					DeleteObject(tempBMap);
				}

				void ServerStatus::onPaint(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, HDC windowClientArea)
				{
					if (hDC == NULL || drawDC == NULL || drawBitmap == NULL)
					{
						return;
					}

					pushDrawToDisplay();
				}

				uint ServerStatus::getNextDrawPosition()
				{
					return (drawPosition + 1) % windowWidth;
				}

				uint ServerStatus::moveToNextDrawPosition()
				{
					uint result = drawPosition++;

					if (drawPosition >= windowWidth)
					{
						drawPosition = 0;

						return 0;
					}

					return result;
				}

				void ServerStatus::tick()
				{
					if (hDC == NULL || drawDC == NULL || drawBitmap == NULL)
					{
						return;
					}

					drawStatus();

					UpdateWindow(widget);
				}

				void ServerStatus::drawLine(HPEN pen, uint leftPosition, uint topPosition, uint height)
				{
					uint lineHeight = height > 0 ? (topPosition + height) : (topPosition + 1);

					SelectObject(drawDC, pen);

					MoveToEx(drawDC, leftPosition, topPosition, NULL);
					LineTo(drawDC, leftPosition, lineHeight);
				}

				double ServerStatus::getNumberMappingFactor(uint fromMin, uint fromMax, uint toMin, uint toMax)
				{
					return (double)(fromMax - fromMin) / (double)(toMax - toMin);
				}

				void ServerStatus::pushDrawToDisplay()
				{
					BitBlt(hDC, 0, 0, windowWidth, windowHeight, drawDC, 0, 0, SRCCOPY);
				}

				void ServerStatus::drawStatus()
				{
					uint serverLoad, playerCount;
					bool drawBaselineChange = false;

					// Check and update max counter if needed

					if (g_iPlayerCount > MaxPlayer)
					{
						MaxPlayer = g_iPlayerCount;

						playerCountNumberMappingFactor = getNumberMappingFactor(0, windowHalfHeight - Margin, 0, MaxPlayer);

						drawBaselineChange = true;
					}

					if (g_iServerLoad > MaxServerLoad)
					{
						MaxServerLoad = g_iServerLoad;

						serverLoadNumberMappingFactor = getNumberMappingFactor(0, windowHalfHeight - Margin, 0, MaxServerLoad);

						drawBaselineChange = true;
					}

					serverLoad = (uint)(g_iServerLoad * serverLoadNumberMappingFactor);
					playerCount = (uint)(g_iPlayerCount * playerCountNumberMappingFactor);

					// Draw a refresh line in the next line
					drawLine(refreshLinePen, getNextDrawPosition(), 0, windowHeight);

					// Draw whiteSpacePen in the current line
					if (drawBaselineChange)
					{
						drawLine(statusChangePen, drawPosition, 0, windowHeight);
					}
					else
					{
						drawLine(whiteSpacePen, drawPosition, 0, windowHeight);
					}

					// Top one
					drawLine(playerCounterPen, drawPosition, windowHalfHeight - playerCount, playerCount);

					// Buttom one
					drawLine(serverStatusPen, drawPosition, windowHalfHeight, serverLoad);

					pushDrawToDisplay();

					moveToNextDrawPosition();
				}
			}
		}
	}
}
#pragma once
#include <string>

class CGameWorld;

class CGame
{
public:
	CGame();
	~CGame();
	bool Init(const std::wstring& aVersion = L"", const bool aHasAuthority = false, HWND aHWND = nullptr);

	class Client *myClient = nullptr;
	class Server *myServer = nullptr;

private:
	void InitCallBack();
	void UpdateCallBack();
	LRESULT WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	CGameWorld* myGameWorld = nullptr;

};

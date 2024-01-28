#pragma once
#include <Windows.h>
#include <string>
#include <strsafe.h>
enum ConsoleOutputLevel
{
	ConsoleModeQuiet = 0x0,
	ConsoleModeVerbose = 0x1,
};
enum OutputLevel
{
	OutputSilent = 0x0,
	OutputFailure = 0x1,
	OutputWarning = 0x2,
	OutputInformation = 0x3,
	OutputDebug = 0x4,
};

class CConsoleWriter
{
public:
	HANDLE m_hConsole;
	ConsoleOutputLevel m_OutputLevel;
	BOOL FileIsConsole();
	HRESULT WriteStringToConsole(
		const wchar_t* format,
		...);
	HRESULT Internal_WriteToConsole(
		const wchar_t* lpBuffer,
		DWORD cchBuffer);
	HRESULT InitiateProgressBar(const wchar_t* action, const wchar_t* prep);
	HRESULT PrintProgressBar(
		unsigned int progress,
		unsigned int total,
		wchar_t* message);
	HRESULT TerminateProgressBar(const wchar_t* message);
	HRESULT WriteMessage(
		OutputLevel level,
		const wchar_t* pszMessage);
	HRESULT PromptMessageYesNo(
		OutputLevel level,
		int bDefault,
		int* pbResult);
};
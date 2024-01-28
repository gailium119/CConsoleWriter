//User Defined
#define LIB_NAME CConsoleWriter.STL // Change to ATL if you want to test ATL
#define INCLUDE_NAME CConsoleWriter.h
// Tricky definitions
#include "msvc_include.h"

int main()
{
    CConsoleWriter consoleWriter;
    consoleWriter.m_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    consoleWriter.m_OutputLevel = ConsoleModeVerbose;
    consoleWriter.InitiateProgressBar(L"Progress",0);
    consoleWriter.PrintProgressBar(100, 100, 0);
    consoleWriter.TerminateProgressBar(0);
}

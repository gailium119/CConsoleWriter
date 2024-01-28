
#include "CConsoleWriter.h"
HRESULT NormalizeStringEols(const wchar_t* pwszOriginalString, wchar_t** ppwszNormalizedString)
{
    int index = 0;
    int additionalNewlines = 0;
    unsigned int totalChars = 0;
    const wchar_t* currentChar = nullptr;
    unsigned int totalCharsPlusNewlines = 0;
    wchar_t* normalizedString = nullptr;
    wchar_t* normalizedStringPtr = nullptr;
    wchar_t currentCharValue;

    if (!pwszOriginalString || !ppwszNormalizedString)
        return 0x80070057;

    currentChar = pwszOriginalString;
    if (*pwszOriginalString == 10 || *pwszOriginalString == 13)
    {
        additionalNewlines = 1;
        currentChar = ++pwszOriginalString;
        totalCharsPlusNewlines = 1;
    }

    while (*currentChar)
    {
        index = 0;
        if (totalCharsPlusNewlines + 1 < totalCharsPlusNewlines)
            return 0x80070216;
        ++totalCharsPlusNewlines;
        if (*currentChar == 10 && *(currentChar - 1) != 13)
            ++additionalNewlines;
        ++currentChar;
    }

    totalChars = totalCharsPlusNewlines + additionalNewlines;
    if (totalCharsPlusNewlines < totalCharsPlusNewlines || totalChars + 1 < totalChars)
        return 0x80070216;

    normalizedString = new wchar_t[(totalChars + 1)];
    normalizedStringPtr = normalizedString;

    if (!normalizedString)
        return 0x8007000E;

    if (*pwszOriginalString == 10)
    {
        *normalizedString = 13;
        normalizedString[1] = 10;
        normalizedString += 2;
        ++pwszOriginalString;
    }

    currentCharValue = *pwszOriginalString;

    if (*pwszOriginalString)
    {
        do
        {
            if (currentCharValue != 10 || *(pwszOriginalString - 1) == 13)
            {
                *normalizedString = currentCharValue;
            }
            else
            {
                *normalizedString++ = 13;
                *normalizedString = 10;
            }
            ++pwszOriginalString;
            ++normalizedString;
            currentCharValue = *pwszOriginalString;
        } while (*pwszOriginalString);

        index = 0;
    }

    *normalizedString = 0;
    *ppwszNormalizedString = normalizedStringPtr;
    return index;
}
BOOL CConsoleWriter::FileIsConsole()
{
	DWORD dwMode;
	return GetConsoleMode(this->m_hConsole, &dwMode);
}

HRESULT CConsoleWriter::WriteStringToConsole(
    const  wchar_t* format,
    ...)
{
    HRESULT result=S_OK;
    wchar_t* buffer;
    int IsConsole;
    const wchar_t* resultBuffer=NULL; // edx
    size_t resultBufferSize;
    wchar_t* pwszNormalizedStr=NULL;
    va_list va;
    int max_buffersize = 10000;
    va_start(va, format);
    if (!this->m_hConsole)
        return 0;
    while (1)
    {
        buffer = new wchar_t[max_buffersize];
        if (!buffer)
        {
            result = 0x8007000E;
            goto Label_Exit;
        }
        result = StringCchVPrintfW(buffer, max_buffersize-1, format, va);
        if (result >= 0)
            break;
        if (result != 0x8007000E)
            goto Label_Exit;
        delete[] buffer;
        max_buffersize *= 2;
    }
    if (GetFileType(this->m_hConsole) == FILE_TYPE_CHAR)
        IsConsole = this->FileIsConsole();
    else
        IsConsole = 0;
    if (IsConsole)
    {
        resultBuffer = buffer;
    }
    else {
        result = NormalizeStringEols(buffer, &pwszNormalizedStr);
        resultBuffer = L"";
        if (result >= 0)
        {
            resultBuffer = pwszNormalizedStr;
        }
    }
    resultBufferSize = wcslen(resultBuffer);
    result = this->Internal_WriteToConsole(resultBuffer, (DWORD)resultBufferSize);
Label_Exit:
    if (buffer)
        delete[] buffer;
    return result;
}
HRESULT CConsoleWriter::Internal_WriteToConsole(
    const wchar_t* lpBuffer,
    DWORD cchBuffer)
{
    char* tempBuffer;

    if (this->m_hConsole)
    {
        if (GetFileType(this->m_hConsole) == FILE_TYPE_CHAR && CConsoleWriter::FileIsConsole())
        {
            WriteConsoleW(this->m_hConsole, lpBuffer, cchBuffer, &cchBuffer, 0);
        }
        else
        {
            tempBuffer = (char*)LocalAlloc(0, 2 * cchBuffer);
            if (tempBuffer)
            {
                cchBuffer = WideCharToMultiByte(1u, 0, lpBuffer, cchBuffer, tempBuffer, 2 * cchBuffer, 0, 0);
                if (cchBuffer)
                    WriteFile(this->m_hConsole, tempBuffer, cchBuffer, &cchBuffer, 0);
                LocalFree(tempBuffer);
            }
        }
    }
    return 0;
}

HRESULT CConsoleWriter::InitiateProgressBar(const wchar_t* action, const wchar_t* prep)
{
    std::wstring cstrLine(L"");
    if (action)
    {

        cstrLine = action;
        if (prep && prep != L"")
        {
            cstrLine.append(L": ");
            cstrLine.append(prep);
        }
        cstrLine.append(L"\n");
    }
    else if (prep && prep != L"")
    {
        cstrLine.append(prep);
        cstrLine.append(L"\n");
    }
    if (this->m_OutputLevel)
        this->WriteStringToConsole(L"%s", cstrLine.c_str());
    return 0;
}
HRESULT CConsoleWriter::PrintProgressBar(
    unsigned int progress,
    unsigned int total,
    wchar_t* message)
{
    wchar_t* buffer=NULL;
    HRESULT result;
    wchar_t* tmpPwchar, *tmpPwchar2; 
    unsigned int percentcount;
    float percent;
    wchar_t lpszPercent[10]=L"";
    std::wstring cstrUserMsg;

    if (this->m_OutputLevel == ConsoleModeQuiet)
        return 0;
    if (total <= 1)
        total = 1;
    if (progress >= total)
        progress = total;
    percent = (float)progress / (float)total;
    percentcount = (unsigned int)(percent * 58.0);
    buffer = (wchar_t*)HeapAlloc(GetProcessHeap(), 8u, 0x7Au);
    if (buffer)
    {
        result = 0;
        memset(buffer, 0, 0x7Au);
        if(StringCchPrintfW(buffer, 0x3Du, L"[%58s]", L"")>=0 && StringCchPrintfW(lpszPercent, 9, L"%2.1f%%", percent * 100.0) >= 0)
        {
            if (wcslen(lpszPercent) < wcslen(buffer))
            {
                if (percentcount)
                {
                    for (unsigned int i = 1; i <= percentcount; i++) {
                        buffer[i] = L'=';
                    }
                    if ((percentcount & 1) == 1) buffer[2 * (percentcount >> 1) + 1] = L'=';
                }
                tmpPwchar = lpszPercent;
                if (*lpszPercent)
                {
                    tmpPwchar2 = &buffer[30 - (wcslen(lpszPercent) >> 1)];
                    do
                    {
                        *tmpPwchar2++ = *tmpPwchar++;
                    } while (*tmpPwchar);
                }
                if (message)
                {
                    cstrUserMsg.append(message);
                    if (wcslen(message) < 40) {
                        cstrUserMsg.append(40 - wcslen(message), L' ');
                    }
                    cstrUserMsg.erase(std::remove_if(cstrUserMsg.begin(), cstrUserMsg.end(), ::isspace), cstrUserMsg.end());
                }
                if (GetFileType(this->m_hConsole) != FILE_TYPE_CHAR && !this->FileIsConsole())
                    cstrUserMsg.append(L"\n");
                result = this->WriteStringToConsole(
                    L"\r%s %s",
                    buffer,
                    cstrUserMsg.c_str());
            }
        }
        if (buffer)
        {
            HeapFree(GetProcessHeap(), 0, buffer);
        }
    }
    else {
        result = 0x8007000E;
    }
    return result;
}
HRESULT CConsoleWriter::TerminateProgressBar(const wchar_t* message)
{
    std::wstring cstrLine(L""); 

    if (GetFileType(this->m_hConsole) == FILE_TYPE_CHAR && CConsoleWriter::FileIsConsole())
    {
        cstrLine.append(L"\n");
    }
    if (message)
    {
        cstrLine.append(message);
        cstrLine.append(L"\n\n");
    }
    if (this->m_OutputLevel)
        CConsoleWriter::WriteStringToConsole(L"%s", cstrLine.c_str());
    return 0;
}
HRESULT CConsoleWriter::WriteMessage(
    OutputLevel level,
    const wchar_t* pszMessage)
{
    HRESULT result = 0;
    if (this->m_OutputLevel || level == OutputFailure)
        return this->WriteStringToConsole(L"%s", pszMessage);
    return result;
}
HRESULT CConsoleWriter::PromptMessageYesNo(
    OutputLevel level,
    int bDefault,
    int* pbResult)
{
    HRESULT result = 0;
    wint_t ch; // bx
    wint_t chup; // ax
    *pbResult = 1;
    if (this->m_OutputLevel && GetFileType(this->m_hConsole) == FILE_TYPE_CHAR
           && this->FileIsConsole())
    {
        do
        {
            ch = _getwch();
            chup = towupper(ch);
            if (chup == L'Y')
            {
                result = this->WriteStringToConsole(L"%c\n", ch);
                *pbResult = 1;
                goto LExit_9;
            }
        } while (chup != L'N');
        result = this->WriteStringToConsole(L"%c\n", ch);
        *pbResult = 0;
    }
LExit_9:
    return result;
}
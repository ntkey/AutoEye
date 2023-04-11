#pragma once

#include <windows.h>
#include <tchar.h>

class CRegistry
{
public:
    CRegistry(HKEY hRootKey = HKEY_CURRENT_USER);
    virtual ~CRegistry();

public:
    BOOL VerifyKey(LPCTSTR lpszPath);
    BOOL VerifyValue(LPCTSTR lpszValue);

    BOOL CreateKey(LPCTSTR lpszPath);
    void Close();

    BOOL DeleteValue(LPCTSTR lpszValue);
    BOOL DeleteKey(LPCTSTR lpszPath);

    BOOL Write(LPCTSTR pszKey, int iVal);
    BOOL Write(LPCTSTR pszKey, DWORD dwVal);
    BOOL Write(LPCTSTR pszKey, LPCTSTR lpszVal);

    BOOL Read(LPCTSTR pszKey, int& iVal);
    BOOL Read(LPCTSTR pszKey, DWORD& dwVal);
    BOOL Read(LPCTSTR pszKey, CString& strVal);

    BOOL IsEqual(LPCTSTR pszValue, int iVal);
    BOOL IsEqual(LPCTSTR pszValue, DWORD dwVal);
    BOOL IsEqual(LPCTSTR pszValue, LPCTSTR lpszVal);

protected:
    HKEY m_hSubKey;
    HKEY m_hRootKey;
};
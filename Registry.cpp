#include "pch.h"
#include "Registry.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CRegistry::CRegistry(HKEY hRootKey) 
{
    m_hRootKey = hRootKey;
    m_hSubKey = NULL;
}


CRegistry::~CRegistry()
{
    Close();
}


BOOL CRegistry::VerifyKey(LPCTSTR lpszPath)
{
    LSTATUS status = ::RegOpenKeyEx(m_hRootKey, lpszPath, 0, KEY_ALL_ACCESS, &m_hSubKey);
    return (status == ERROR_SUCCESS);
}


BOOL CRegistry::VerifyValue(LPCTSTR lpszValue)
{
    LSTATUS status = ::RegQueryValueEx(m_hSubKey, lpszValue, NULL, NULL, NULL, NULL);
    return (status == ERROR_SUCCESS);
}


BOOL CRegistry::IsEqual(LPCTSTR lpszValue, int iVal)
{
    int nTemp = 0;
    BOOL bRet = Read(lpszValue, nTemp);
    return (bRet ? (iVal == nTemp) : FALSE);
}


BOOL CRegistry::IsEqual(LPCTSTR lpszValue, DWORD dwVal)
{
    DWORD dwTemp = 0;
    BOOL bRet = Read(lpszValue, dwTemp);
    return (bRet ? (dwVal == dwTemp) : FALSE);
}


BOOL CRegistry::IsEqual(LPCTSTR lpszValue, LPCTSTR lpszVal)
{
    CString str;
    BOOL bRet = Read(lpszValue, str);
    return (bRet ? (str.CompareNoCase(lpszVal) == 0) : FALSE);
}


BOOL CRegistry::CreateKey(LPCTSTR lpszPath)
{
    LSTATUS status = ::RegCreateKeyEx(m_hRootKey, lpszPath, 0, NULL,
                            REG_OPTION_VOLATILE, KEY_ALL_ACCESS, NULL, 
                            &m_hSubKey, NULL);

    return (status == ERROR_SUCCESS);
}


BOOL CRegistry::Write(LPCTSTR lpszKeyName, int iVal)
{
    DWORD dwValue = (DWORD)iVal;
    LSTATUS status = ::RegSetValueEx(m_hSubKey, lpszKeyName, 0L, REG_DWORD,
        (const BYTE*)&dwValue, sizeof(DWORD));

    return (status == ERROR_SUCCESS);
}


BOOL CRegistry::Write(LPCTSTR lpszKeyName, DWORD dwVal)
{
    LSTATUS status = ::RegSetValueEx(m_hSubKey, lpszKeyName, 0, REG_DWORD,
        (const BYTE*)&dwVal, sizeof(DWORD));

    return (status == ERROR_SUCCESS);
}


BOOL CRegistry::Write(LPCTSTR lpszKeyName, LPCTSTR lpszData)
{
    LSTATUS status = ::RegSetValueEx(m_hSubKey, lpszKeyName, 0, REG_SZ,
        (const BYTE*)lpszData, sizeof(TCHAR)*(lstrlen(lpszData)));

    return (status == ERROR_SUCCESS);
}


BOOL CRegistry::Read(LPCTSTR lpszKeyName, int& iVal)
{
    DWORD dwType = 0;
    
    int dwDest = 0;
    DWORD dwSize = sizeof(dwDest);

    LSTATUS status = ::RegQueryValueEx(m_hSubKey, lpszKeyName, NULL,
        &dwType, (BYTE *)&dwDest, &dwSize);
    if (status != ERROR_SUCCESS)
    {
        return FALSE;
    }

    iVal = dwDest;
    return TRUE;
}


BOOL CRegistry::Read(LPCTSTR lpszKeyName, DWORD& dwVal)
{
    DWORD dwType = 0;
    
    DWORD dwDest = 0;
    DWORD dwSize = sizeof(dwDest);

    LSTATUS status = ::RegQueryValueEx(m_hSubKey, lpszKeyName, NULL,
        &dwType, (BYTE *)&dwDest, &dwSize);
    if (status != ERROR_SUCCESS)
    {
        return FALSE;
    }

    dwVal = dwDest;
    return TRUE;
}


BOOL CRegistry::Read(LPCTSTR lpszKeyName, CString& strVal)
{
    DWORD dwType = 0;

    TCHAR szString[MAX_PATH] = {0};
    DWORD dwSize = _countof(szString) - 1;

    LSTATUS status = ::RegQueryValueEx(m_hSubKey, lpszKeyName, NULL,
        &dwType, (BYTE *)szString, &dwSize);
    if (status != ERROR_SUCCESS)
    {
        return FALSE;
    }

    strVal = szString;
    return TRUE;
}


BOOL CRegistry::DeleteValue(LPCTSTR lpszValue)
{
    LSTATUS status = ::RegDeleteValue(m_hSubKey, lpszValue);
    return (status == ERROR_SUCCESS);
}


BOOL CRegistry::DeleteKey(LPCTSTR lpszPath)
{
    LSTATUS status = ::RegDeleteKey(m_hRootKey, lpszPath);
    return (status == ERROR_SUCCESS);
}


void CRegistry::Close()
{
    if (m_hSubKey != NULL)
    {
        ::RegCloseKey (m_hSubKey);
        m_hSubKey = NULL;
    }

    m_hRootKey = NULL;
}

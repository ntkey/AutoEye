
// AutoEyeDlg.cpp : 实现文件
//

#include "pch.h"
#include "AutoEye.h"
#include "AutoEyeDlg.h"
#include "Registry.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define FOLDER_SHELL_PATH   _T("Folder\\Shell\\")
#define SHELL_SUB_PATH      _T("\\Shell\\")
#define SHELL_COMMAND       _T("\\command")

BOOL CAutoEyeDlg::WinIntegrated(CString strRegName, CString strMenuName, CString strCmdFile)
{
    BOOL bRet = FALSE;

    do 
    {
        CRegistry regInfo(HKEY_CLASSES_ROOT);

        if (!regInfo.VerifyKey(FOLDER_SHELL_PATH + strRegName))
        {
            if (!regInfo.CreateKey(FOLDER_SHELL_PATH + strRegName))
            {
                break;
            }
        }

        if (!regInfo.Write(_T(""), strMenuName))
        {
            break;
        }

        if (!regInfo.VerifyKey(FOLDER_SHELL_PATH + strRegName + SHELL_COMMAND))
        {
            if (!regInfo.CreateKey(FOLDER_SHELL_PATH + strRegName + SHELL_COMMAND))
            {
                break;
            }
        }

        CString strRegCommand = _T("\"") + strCmdFile + _T("\"") + _T(" \"") + _T("%1") + _T("\"");
        if (!regInfo.Write(_T(""), strRegCommand))
        {
            break;
        }

        bRet = TRUE;
    } while (FALSE);
    
    return bRet;
}


BOOL CAutoEyeDlg::WinIntegrated(CString strRegName, CString strMenuName, CString strCmdFile, CString strFileExt, CString strFileTypeDetail)
{
    BOOL bRet = FALSE;

    do 
    {
        CString strRegKey;
        CRegistry regInfo(HKEY_CLASSES_ROOT);
        
        if (!regInfo.VerifyKey(_T(".") + strFileExt))
        {
            if (!regInfo.CreateKey(_T(".") + strFileExt))
            {
                break;
            }

            if (!regInfo.Write(_T(""), strFileTypeDetail))
            {
                break;
            }

            strRegKey = strFileTypeDetail;
        }
        else
        {
            if (!regInfo.Read(_T(""), strRegKey))
            {
                break;
            }
        }

        if (!regInfo.VerifyKey(strRegKey + SHELL_SUB_PATH + strRegName))
        {
            if (!regInfo.CreateKey(strRegKey + SHELL_SUB_PATH + strRegName))
            {
                break;
            }
        }

        if (!regInfo.Write(_T(""), strMenuName))
        {
            break;
        }

        if (!regInfo.VerifyKey(strRegKey + SHELL_SUB_PATH + strRegName + SHELL_COMMAND))
        {
            if (!regInfo.CreateKey(strRegKey + SHELL_SUB_PATH + strRegName + SHELL_COMMAND))
            {
                break;
            }
        }

        CString strRegCommand = _T("\"") + strCmdFile + _T("\"") + _T(" \"") + _T("%1") + _T("\"");
        if (!regInfo.Write(_T(""), strRegCommand))
        {
            break;
        }

        bRet = TRUE;
    } while (FALSE);
    
    return bRet;
}


CString CAutoEyeDlg::GetAppFullName()
{
    TCHAR szPath[MAX_PATH] = {};
    GetModuleFileName(GetModuleHandle(NULL), szPath, _countof(szPath) - 1);

    return CString(szPath);
}


BOOL CAutoEyeDlg::UnWinIntegrated(CString strRegName)
{
    LSTATUS status = SHDeleteKey(HKEY_CLASSES_ROOT, FOLDER_SHELL_PATH + strRegName);
    return (status == ERROR_SUCCESS);
}


BOOL CAutoEyeDlg::UnWinIntegrated(CString strRegName, CString strFileExt)
{
    BOOL bRet = FALSE;
    CRegistry regInfo(HKEY_CLASSES_ROOT);

    do 
    {
        if (!regInfo.VerifyKey(_T(".") + strFileExt))
        {
            break;
        }

        CString strRegKey;
        if (!regInfo.Read(_T(""), strRegKey))
        {
            break;
        }

        LSTATUS status = SHDeleteKey(HKEY_CLASSES_ROOT, strRegKey + SHELL_SUB_PATH + strRegName);
        if (status != ERROR_SUCCESS)
        {
            break;
        }

        bRet = TRUE;
    } while (FALSE);
    
    return bRet;
}


BOOL CAutoEyeDlg::CheckInterStatus()
{
    BOOL bFolderIntegrated = FALSE;
    BOOL bExeIntegrated = FALSE;
    BOOL bA3XIntegrated = FALSE;

    // check for directory
    CRegistry regInfo(HKEY_CLASSES_ROOT);
    CString strAutoDecPath = _T("Autoit3 PATH Decompiler");
    bFolderIntegrated = regInfo.VerifyKey(FOLDER_SHELL_PATH + strAutoDecPath + SHELL_COMMAND);

    // check for *.exe
    if (regInfo.VerifyKey(_T(".exe")))
    {
        CString strRegKey;
        regInfo.Read(_T(""), strRegKey);

        CString strAutoDecEXE = _T("Autoit3 EXE Decompiler");
        bExeIntegrated = regInfo.VerifyKey(strRegKey + SHELL_SUB_PATH + strAutoDecEXE + SHELL_COMMAND);
        strRegKey.Empty();
    }

    // check for *.a3x
    if (regInfo.VerifyKey(_T(".a3x")))
    {
        CString strRegKey;
        regInfo.Read(_T(""), strRegKey);

        CString strAutoDecA3X = _T("Autoit3 A3X Decompiler");
        bA3XIntegrated = regInfo.VerifyKey(strRegKey + SHELL_SUB_PATH + strAutoDecA3X + SHELL_COMMAND);
    }

    return (bExeIntegrated && bA3XIntegrated && bFolderIntegrated);
}



// AutoEye.cpp: 定义应用程序的类行为。
//

#include "pch.h"
#include "framework.h"
#include "AutoEye.h"
#include "AutoEyeDlg.h"
#include "ProgressDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAutoEyeApp

BEGIN_MESSAGE_MAP(CAutoEyeApp, CWinApp)
    ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CAutoEyeApp 构造

CAutoEyeApp::CAutoEyeApp()
{
    // 支持重新启动管理器
    m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

    // TODO: 在此处添加构造代码，
    // 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的 CAutoEyeApp 对象

CAutoEyeApp theApp;


// CAutoEyeApp 初始化

BOOL CAutoEyeApp::InitInstance()
{
    // 如果一个运行在 Windows XP 上的应用程序清单指定要
    // 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
    //则需要 InitCommonControlsEx()。  否则，将无法创建窗口。
    INITCOMMONCONTROLSEX InitCtrls;
    InitCtrls.dwSize = sizeof(InitCtrls);
    // 将它设置为包括所有要在应用程序中使用的
    // 公共控件类。
    InitCtrls.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&InitCtrls);

    CWinApp::InitInstance();

    if (!AfxSocketInit())
    {
        AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
        return FALSE;
    }


    AfxEnableControlContainer();


    // 标准初始化
    // 如果未使用这些功能并希望减小
    // 最终可执行文件的大小，则应移除下列
    // 不需要的特定初始化例程
    // 更改用于存储设置的注册表项
    // TODO: 应适当修改该字符串，
    // 例如修改为公司或组织名
    SetRegistryKey(_T(""));

    au3engine_init();

    // 命令行处理,处理完了后要退出
    CString strFullName = m_lpCmdLine;

    TCHAR szPath[MAX_PATH] = { 0 };
    lstrcpy(szPath, strFullName);
    int nLen = lstrlen(szPath);
    if (szPath[0] == '"')
    {
        if (szPath[nLen - 1] == '"')
        {
            szPath[nLen - 1] = 0;
        }

        strFullName = szPath + 1;
    }

    if (!strFullName.IsEmpty())
    {
        DWORD dwAttrib = ::GetFileAttributes(strFullName);
        if (dwAttrib != INVALID_FILE_ATTRIBUTES)
        {
            if (dwAttrib & FILE_ATTRIBUTE_DIRECTORY)
            {
                strFullName += _T("\\");
                AddFilesByPath(strFullName, TRUE, NULL);
            }
            else
            {
                BOOL bRet = AddFilesByFileName(strFullName);
                if (!bRet)
                {
                    AfxMessageBox(_T("反编译失败"), MB_ICONSTOP);
                }
            }

            return FALSE;
        }
    }

    CAutoEyeDlg dlg;
    m_pMainWnd = &dlg;
    INT_PTR nResponse = dlg.DoModal();
    if (nResponse == IDOK)
    {
        // TODO: 在此放置处理何时用
        //  “确定”来关闭对话框的代码
    }
    else if (nResponse == IDCANCEL)
    {
        // TODO: 在此放置处理何时用
        //  “取消”来关闭对话框的代码
    }
    else if (nResponse == -1)
    {
        TRACE(traceAppMsg, 0, "警告: 对话框创建失败，应用程序将意外终止。\n");
    }


#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
    ControlBarCleanUp();
#endif

    // 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
    //  而不是启动应用程序的消息泵。
    return FALSE;
}


DWORD CAutoEyeApp::AddFilesByPath(LPCTSTR lpszFileDir, BOOL bIncludeSub, LPCTSTR lpszFileExt)
{
    CString strFilter;
    strFilter.Format(_T("%s\\*.%s"),
        lpszFileDir,
        (lpszFileExt == nullptr) ? _T("*") : lpszFileExt);

    int nFileCount = 0;
    CFileFind finder;
    BOOL bWorking = finder.FindFile(strFilter);

    while (bWorking)
    {
        bWorking = finder.FindNextFile();
        if (finder.IsDots())
        {
            continue;
        }

        if (finder.IsDirectory())
        {
            if (bIncludeSub)
            {
                CString strSubDir = CString(lpszFileDir) + _T("\\") + finder.GetFileName();
                AddFilesByPath(strSubDir, FALSE, lpszFileExt);
            }
        }
        else
        {
            int nExtLength = finder.GetFileName().GetLength() - finder.GetFileTitle().GetLength() - 1;
            CString strExt = finder.GetFileName().Right(nExtLength);

            if (lpszFileExt != nullptr)
            {
                if (strExt != lpszFileExt)
                {
                    continue;
                }
            }

            CString strFileName = finder.GetFilePath();
            nFileCount++;

            AddFilesByFileName(strFileName);
        }
    }

    return nFileCount;
}

BOOL CAutoEyeApp::AddFilesByFileName(LPCTSTR lpszFileName)
{
    int hAutoHandle = -1;
    BasicInformation objBaseInfo = {};
    CProgressDlg dlg(&hAutoHandle, &objBaseInfo, lpszFileName, FALSE, TRUE);
    dlg.DoModal();

    int nRet = dlg.GetDecResult();
    if (FAILED(nRet))
    {
        return FALSE;
    }

    std::vector<ItemInformation> vecItems;

    do
    {
        ItemInformation itemInfo = {};
        nRet = au3engine_next_item(hAutoHandle, &itemInfo);
        if (FAILED(nRet))
        {
            break;
        }

        if (nRet == SuccessNomoreItem)
        {
            break;
        }

        vecItems.push_back(itemInfo);
    } while (TRUE);

    CAutoEyeDlg dlgDec;
    dlgDec.OutputDecFiles(&objBaseInfo, lpszFileName, &vecItems[0], (DWORD)vecItems.size(), FALSE);

    return SUCCEEDED(nRet);
}

int CAutoEyeApp::ExitInstance()
{
    au3engine_uinit();
    CoUninitialize();

    return CWinApp::ExitInstance();
}

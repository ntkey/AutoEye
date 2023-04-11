
// AutoEyeDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "AutoEye.h"
#include "AutoEyeDlg.h"
#include "AboutDlg.h"
#include "ProgressDlg.h"
#include "afxdialogex.h"
#include "CommonVersion.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


void ConvertHex2String(const BYTE* lpInBuffer, int nInSize, CString& strOutput, BOOL bOneLine);
CStringA ws2s(CStringW strUnicode, UINT nCodePage);
CStringW s2ws(CStringA strMBCS, UINT CodePage);


void ConvertHex2String(const BYTE* lpInBuffer, int nInSize, CString& strOutput, BOOL bOneLine)
{
    int nSrcPos = 0;
    strOutput.Empty();

    while (nSrcPos < nInSize)
    {
        TCHAR szTmp[8] = {};
        wsprintf(szTmp, _T("%02X"), *(BYTE*)(lpInBuffer + nSrcPos));
        nSrcPos++;

        if (nSrcPos < nInSize)
        {
            if ((nSrcPos % 16 != 0) || bOneLine)
            {
                lstrcat(szTmp, _T(" "));
            }
        }

        strOutput += szTmp;
        if (!bOneLine)
        {
            if ((nSrcPos != nInSize) && (nSrcPos % 16 == 0))
            {
                strOutput += '\r';
                strOutput += '\n';
            }
        }
    }
}


CStringA ws2s(CStringW strUnicode, UINT nCodePage)
{
    int nLen = strUnicode.GetLength();
    if (nLen == 0)
    {
        return CStringA();
    }

    int nCharCount = WideCharToMultiByte(nCodePage,
        0,
        strUnicode,
        nLen,
        NULL,
        0,
        NULL,
        NULL);
    if (nCharCount == 0)
    {
        return CStringA();
    }

    std::string mb;
    mb.resize(nCharCount);
    WideCharToMultiByte(nCodePage,
        0,
        strUnicode,
        nLen,
        &mb[0],
        nCharCount,
        NULL,
        NULL);

    return CStringA(mb.c_str());
}


CStringW s2ws(CStringA strMBCS, UINT CodePage)
{
    int nLen = strMBCS.GetLength();
    if (nLen == 0)
    {
        return CStringW();
    }

    int nCharCount = MultiByteToWideChar(CodePage,
        0,
        strMBCS,
        nLen,
        NULL,
        0);
    if (nCharCount == 0)
    {
        return CStringW();
    }

    std::wstring wide;
    wide.resize(nCharCount);
    MultiByteToWideChar(CodePage,
        0,
        strMBCS,
        nLen,
        &wide[0],
        nCharCount);

    return CStringW(wide.c_str());
}


// CAutoEyeDlg 对话框

CAutoEyeDlg::CAutoEyeDlg(CWnd* pParent /*=nullptr*/)
    : CDialog(IDD_AUTOEYE_DIALOG, pParent)
    , m_strInputFile(_T("")), m_objBaseInfo()
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    m_hAutoHandle = -1;
}

void CAutoEyeDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_FILEPATH, m_strInputFile);
    DDX_Control(pDX, IDC_LIST_INSTFILE, m_ctlFileList);
    DDX_Control(pDX, IDC_EDIT_PREVIEW, m_ctlFileView);
}

BEGIN_MESSAGE_MAP(CAutoEyeDlg, CDialog)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_WM_DROPFILES()
    ON_WM_DESTROY()
    ON_BN_CLICKED(IDC_BTN_SELFILE, &CAutoEyeDlg::OnBnClickedBtnSelfile)
    ON_BN_CLICKED(IDC_BTN_DECOMPILE, &CAutoEyeDlg::OnBnClickedBtnDecompile)
    ON_BN_CLICKED(IDC_BTN_OUTPUT, &CAutoEyeDlg::OnBnClickedBtnOutput)
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_INSTFILE, &CAutoEyeDlg::OnLvnItemchangedListInstfile)
    ON_NOTIFY(LVN_DELETEALLITEMS, IDC_LIST_INSTFILE, &CAutoEyeDlg::OnLvnDeleteallitemsListInstfile)
    ON_BN_CLICKED(IDC_BTN_MENUINTER, &CAutoEyeDlg::OnBnClickedBtnMenuinter)
    ON_BN_CLICKED(IDC_BTN_ADVDEC, &CAutoEyeDlg::OnBnClickedBtnAdvdec)
    ON_BN_CLICKED(IDC_BTN_ABOUT, &CAutoEyeDlg::OnBnClickedBtnAbout)
END_MESSAGE_MAP()


// CAutoEyeDlg 消息处理程序

BOOL CAutoEyeDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // 将“关于...”菜单项添加到系统菜单中。

    // IDM_ABOUTBOX 必须在系统命令范围内。
    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);

    CMenu* pSysMenu = GetSystemMenu(FALSE);
    if (pSysMenu != nullptr)
    {
        BOOL bNameValid;
        CString strAboutMenu;
        bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
        ASSERT(bNameValid);
        if (!strAboutMenu.IsEmpty())
        {
            pSysMenu->AppendMenu(MF_SEPARATOR);
            pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
        }
    }

    // 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
    //  执行此操作
    SetIcon(m_hIcon, TRUE);			// 设置大图标
    SetIcon(m_hIcon, FALSE);		// 设置小图标

    // TODO: 在此添加额外的初始化代码
    SetWindowText(CStringW(VER_TITLEINFO));
    m_ctlFileList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_FLATSB | LVS_EX_ONECLICKACTIVATE);
    m_ctlFileList.InsertColumn(FILEINFO_PATH, _T("路径"), LVCFMT_LEFT, 410);
    m_ctlFileList.InsertColumn(FILEINFO_INDEX, _T("序号"), LVCFMT_LEFT, 40);
    m_ctlFileList.InsertColumn(FILEINFO_SIZE, _T("字节大小"), LVCFMT_LEFT, 70);

    SHFILEINFO sfi = { 0 };

    TCHAR szSystemDir[MAX_PATH] = { 0 };
    GetWindowsDirectory(szSystemDir, MAX_PATH);

    HIMAGELIST himlSmall = (HIMAGELIST)SHGetFileInfo((LPCTSTR)szSystemDir,
        0,
        &sfi,
        sizeof(SHFILEINFO),
        SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
    HIMAGELIST himlLarge = (HIMAGELIST)SHGetFileInfo((LPCTSTR)szSystemDir,
        0,
        &sfi,
        sizeof(SHFILEINFO),
        SHGFI_SYSICONINDEX | SHGFI_LARGEICON);
    if (himlSmall && himlLarge)
    {
        ::SendMessage(m_ctlFileList.m_hWnd, LVM_SETIMAGELIST,
            (WPARAM)LVSIL_SMALL, (LPARAM)himlSmall);
        ::SendMessage(m_ctlFileList.m_hWnd, LVM_SETIMAGELIST,
            (WPARAM)LVSIL_NORMAL, (LPARAM)himlLarge);
    }

    GetDlgItem(IDC_BTN_MENUINTER)->SetWindowText(CheckInterStatus() ? _T("-") : _T("+"));

    return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CAutoEyeDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    if ((nID & 0xFFF0) == IDM_ABOUTBOX)
    {
        CAboutDlg dlgAbout;
        dlgAbout.DoModal();
    }
    else
    {
        CDialog::OnSysCommand(nID, lParam);
    }
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CAutoEyeDlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this); // 用于绘制的设备上下文

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        // 使图标在工作区矩形中居中
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // 绘制图标
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialog::OnPaint();
    }
}

// 当用户拖动最小化窗口时系统调用此函数取得光标显示。
HCURSOR CAutoEyeDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}


void CAutoEyeDlg::OnBnClickedBtnSelfile()
{
    CString strFilter = _T("可执行文件 (*.exe)|*.exe|Autoit脚本编译文件 (*.a3x)|*.a3x|所有文件 (*.*)|*.*||");
    CFileDialog dlg(TRUE, _T("dll"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, strFilter, this);
    if (IDOK != dlg.DoModal())
    {
        return;
    }

    SetDefaultUI();

    m_strInputFile = dlg.GetPathName();
    UpdateData(FALSE);

    GetDlgItem(IDC_BTN_DECOMPILE)->EnableWindow(TRUE);
    GetDlgItem(IDC_BTN_ADVDEC)->EnableWindow(TRUE);
    ProcessAutoitFile(m_strInputFile);
}


void CAutoEyeDlg::OnBnClickedBtnOutput()
{
    do
    {
        if (m_strInputFile.IsEmpty())
        {
            break;
        }

        int nItemCount = m_ctlFileList.GetItemCount();
        if (nItemCount == 0)
        {
            break;
        }

        std::vector<ItemInformation> vecDecItems;
        for (int nItem = 0; nItem < nItemCount; nItem++)
        {
            const ItemInformation *lpInfo = (const ItemInformation *)m_ctlFileList.GetItemData(nItem);
            if (lpInfo == nullptr)
            {
                continue;
            }

            vecDecItems.push_back(*lpInfo);
        }

        OutputDecFiles(&m_objBaseInfo, m_strInputFile, &vecDecItems[0], (DWORD)vecDecItems.size(), TRUE);
    } while (FALSE);
}


void CAutoEyeDlg::OutputDecFiles(
    BasicInformation *pBaseInfo, 
    CString strSrcPath, 
    ItemInformation *pDecItems, 
    DWORD dwCount, 
    BOOL bExplor
)
{
    do
    {
        if (pDecItems == nullptr || dwCount == 0)
        {
            break;
        }

        TCHAR szDir[_MAX_DIR] = { 0 };
        TCHAR szDrive[_MAX_DRIVE] = { 0 };
        TCHAR szName[_MAX_FNAME] = { 0 };
        TCHAR szExt[_MAX_EXT] = { 0 };
        _tsplitpath(strSrcPath, szDrive, szDir, szName, szExt);

        CString strDstRootPath;
        strDstRootPath.Format(_T("%s%s%s%s_dec\\"), szDrive, szDir, szName, szExt);
        SHCreateDirectory(NULL, strDstRootPath);
        if (!PathFileExists(strDstRootPath))
        {
            break;
        }

        CString strScriptFilePath;
        for (DWORD nItem = 0; nItem < dwCount; nItem++)
        {
            ItemInformation *lpInfo = &pDecItems[nItem];
            if (lpInfo == nullptr)
            {
                continue;
            }

            int len = lstrlenW(lpInfo->compile_path);
            if (len == 0)
            {
                continue;
            }

            if (lpInfo->file_index == 0)
            {
                CString strFileName = CString(szName) + _T(".au3");
                
                strScriptFilePath = strDstRootPath;
                CString strScriptFile = strScriptFilePath + strFileName;

                for (DWORD dwFileIndex = 1; dwFileIndex < dwCount; dwFileIndex++)
                {
                    // 这里的算法可能会导致BUG
                    ItemInformation *lpFileInfo = &pDecItems[dwFileIndex];
                    CString strFileInstallPath = lpFileInfo->install_path;
                    int nFindPos = strFileInstallPath.Find(_T("..\\"));
                    if (nFindPos == 0)
                    {
                        if (dwFileIndex + 1 < dwCount)
                        {
                            continue;
                        }

                        int nLayers = strFileInstallPath.Replace(L"..\\", L"");
                        for (int nId = 0; nId < nLayers; nId++)
                        {
                            int nFirstPos = strFileInstallPath.Find('\\');
                            if (nFirstPos != -1)
                            {
                                CString strNewPath = strFileInstallPath.Mid(nFirstPos + 1);
                                strFileInstallPath = strNewPath;
                            }
                        }
                    }

                    CString strFileCompilePath(lpFileInfo->compile_path);
                    int nEndPos = strFileCompilePath.Find(strFileInstallPath);
                    if (nEndPos != -1)
                    {
                        CString strSubPath = strFileCompilePath.Left(nEndPos);
                        strSubPath.Replace(L":\\", L"\\");
                        strScriptFilePath = strDstRootPath + strSubPath;

                        strScriptFile = strScriptFilePath + strFileName;
                        break;
                    }
                }

                SHCreateDirectory(NULL, strScriptFilePath);
                if (!PathFileExists(strScriptFilePath))
                {
                    break;
                }

                HANDLE hFileOut = CreateFile(strScriptFile,
                    GENERIC_WRITE,
                    FILE_SHARE_WRITE,
                    NULL,
                    CREATE_ALWAYS,
                    FILE_ATTRIBUTE_NORMAL,
                    NULL);
                if (hFileOut != INVALID_HANDLE_VALUE)
                {
                    DWORD dwWrite = 0;
                    BOOL bNeedDelete = FALSE;
                    if (pBaseInfo->unicode_script)
                    {
                        // UNICODE处理为UTF-8 3.3.14.0开始脚本调整为UTF8,遇到UNICODE全部都兼容处理为最新的
                        CStringW strUnicodeScript = (LPCWSTR)lpInfo->file_buffer;

                        BYTE szBom[4] = { 0xEF, 0xBB, 0xBF, 0x00 };
                        CStringA strUTF8Script = CStringA(szBom) + ws2s(strUnicodeScript, CP_UTF8);

                        LPCSTR pWriteBuf = strUTF8Script.GetBuffer();
                        DWORD dwWriteSize = strUTF8Script.GetLength();
                        BOOL bRet = WriteFile(hFileOut, pWriteBuf, dwWriteSize, &dwWrite, NULL);
                        if (!bRet || dwWriteSize != dwWrite)
                        {
                            bNeedDelete = TRUE;
                        }
                    }
                    else
                    {
                        // ANSI
                        BOOL bRet = WriteFile(hFileOut, lpInfo->file_buffer, lpInfo->file_size, &dwWrite, NULL);
                        if (!bRet || lpInfo->file_size != dwWrite)
                        {
                            bNeedDelete = TRUE;
                        }
                    }

                    CloseHandle(hFileOut);
                    hFileOut = INVALID_HANDLE_VALUE;

                    if (bNeedDelete)
                    {
                        DeleteFile(strScriptFile);
                    }
                }
            }
            else
            {
                CString strTmp = lpInfo->compile_path;
                strTmp.Replace(L":\\", L"\\");
                CString strFullFileName = strDstRootPath + strTmp;

                int nPos = strFullFileName.ReverseFind('\\');
                CString strFullDir = strFullFileName.Left(nPos + 1);
                SHCreateDirectory(NULL, strFullDir);
                if (!PathFileExists(strFullDir))
                {
                    break;
                }

                HANDLE hFileOut = CreateFile(strFullFileName,
                    GENERIC_WRITE,
                    FILE_SHARE_WRITE,
                    NULL,
                    CREATE_ALWAYS,
                    FILE_ATTRIBUTE_NORMAL,
                    NULL);
                if (hFileOut != INVALID_HANDLE_VALUE)
                {
                    BOOL bNeedDelete = FALSE;
                    if (lpInfo->file_buffer != nullptr && lpInfo->file_size != 0)
                    {
                        DWORD dwWrite = 0;
                        BOOL bRet = WriteFile(hFileOut, lpInfo->file_buffer, lpInfo->file_size, &dwWrite, NULL);
                        if (!bRet || lpInfo->file_size != dwWrite)
                        {
                            bNeedDelete = TRUE;
                        }
                    }

                    CloseHandle(hFileOut);
                    hFileOut = INVALID_HANDLE_VALUE;

                    if (bNeedDelete)
                    {
                        DeleteFile(strFullFileName);
                    }
                }
            }
        }

        if (bExplor)
        {
            ShellExecute(NULL, _T("explore"), strScriptFilePath, NULL, NULL, SW_SHOWNORMAL);
        }
    } while (FALSE);
}


void CAutoEyeDlg::OnDropFiles(HDROP hDropInfo)
{
    SetDefaultUI();

    int nCharactersLen = ::DragQueryFile(hDropInfo, 0, NULL, 0);

    TCHAR szFilePath[MAX_PATH] = { 0 };
    int nFileNum = ::DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);
    if (nFileNum > 1)
    {
        AfxMessageBox(_T("仅支持处理单个文件"));
        return;
    }

    ::DragQueryFile(hDropInfo, 0, szFilePath, MAX_PATH);
    DWORD dwAttrib = ::GetFileAttributes(szFilePath);
    if (INVALID_FILE_ATTRIBUTES == dwAttrib)
    {
        AfxMessageBox(_T("文件属性错误"));
        return;
    }

    if (dwAttrib & FILE_ATTRIBUTE_DIRECTORY)
    {
        AfxMessageBox(_T("不支持目录"));
        return;
    }

    m_strInputFile = szFilePath;
    UpdateData(FALSE);

    GetDlgItem(IDC_BTN_DECOMPILE)->EnableWindow(TRUE);
    GetDlgItem(IDC_BTN_ADVDEC)->EnableWindow(TRUE);
    ProcessAutoitFile(m_strInputFile);

    CDialog::OnDropFiles(hDropInfo);
}


void CAutoEyeDlg::SetDefaultUI()
{
    m_ctlFileList.DeleteAllItems();
    m_ctlFileView.SetWindowText(_T(""));

    GetDlgItem(IDC_BTN_OUTPUT)->EnableWindow(FALSE);
    GetDlgItem(IDC_EDIT_BASEINFO)->SetWindowText(_T(""));

    m_strInputFile = _T("");
    UpdateData(FALSE);
}


int CAutoEyeDlg::GetFileIconIndex(CString strFilePath)
{
    DWORD dwValue = FILE_ATTRIBUTE_NORMAL;

    TCHAR szFullName[MAX_PATH] = { 0 };
    LPTSTR lpszNamePtr = NULL;
    GetFullPathName(strFilePath, MAX_PATH, szFullName, &lpszNamePtr);

    DWORD dwAttrib = ::GetFileAttributes(lpszNamePtr);
    if (INVALID_FILE_ATTRIBUTES != dwAttrib)
    {
        if (dwAttrib & FILE_ATTRIBUTE_DIRECTORY)
        {
            dwValue = FILE_ATTRIBUTE_DIRECTORY;
        }
    }

    SHFILEINFO sfi = {};
    SHGetFileInfo(lpszNamePtr,
        dwValue,
        &sfi,
        sizeof(sfi),
        SHGFI_LARGEICON | SHGFI_SYSICONINDEX |
        SHGFI_USEFILEATTRIBUTES | SHGFI_OPENICON);

    return sfi.iIcon;
}


void CAutoEyeDlg::ProcessAutoitFile(CString strSourceFile, BOOL bAdvance)
{
        std::memset(m_objBaseInfo.version, 0, sizeof(m_objBaseInfo.version));
        std::memset(m_objBaseInfo.pasword, 0, sizeof(m_objBaseInfo.pasword));
        m_objBaseInfo.type = VersionType::TypeInvalid;
        m_objBaseInfo.compiled_script = FALSE;
        m_objBaseInfo.from_resource = FALSE;
        m_objBaseInfo.unicode_script = FALSE;

    CProgressDlg dlg(&m_hAutoHandle, &m_objBaseInfo, strSourceFile, bAdvance, FALSE);
    dlg.DoModal();

    HRESULT hRet = dlg.GetDecResult();
    if (FAILED(hRet))
    {
        return;
    }

    CString strBaseInfo;
    strBaseInfo.Format(_T("格式:%s  编码:%s  版本:%s"),
        (m_objBaseInfo.compiled_script ? _T("a3x") : _T("exe")),
        (m_objBaseInfo.unicode_script ? _T("Unicode") : _T("Ansi")),
        m_objBaseInfo.version);
    int len = lstrlenW(m_objBaseInfo.pasword);
    if (len > 0)
    {
        strBaseInfo += (_T("密码:") + CString(m_objBaseInfo.pasword));
    }
    GetDlgItem(IDC_EDIT_BASEINFO)->SetWindowText(strBaseInfo);

    do
    {
        ItemInformation itemInfo = {};
        hRet = au3engine_next_item(m_hAutoHandle, &itemInfo);
        if (FAILED(hRet))
        {
            break;
        }

        if (hRet == SuccessNomoreItem)
        {
            break;
        }

        CString strFilePath = itemInfo.compile_path;
        if (itemInfo.file_index == 0)
        {
            TCHAR szDir[_MAX_DIR] = { 0 };
            TCHAR szDrive[_MAX_DRIVE] = { 0 };
            TCHAR szName[_MAX_FNAME] = { 0 };
            TCHAR szExt[_MAX_EXT] = { 0 };
            _tsplitpath(m_strInputFile, szDrive, szDir, szName, szExt);
            strFilePath = CString(szName) + _T(".au3");
        }

        int nIcon = GetFileIconIndex(strFilePath);
        m_ctlFileList.InsertItem(itemInfo.file_index, NULL, nIcon);
        m_ctlFileList.SetItemText(itemInfo.file_index, FILEINFO_PATH, strFilePath);

        CString strIndex;
        strIndex.Format(_T("%u"), itemInfo.file_index);
        m_ctlFileList.SetItemText(itemInfo.file_index, FILEINFO_INDEX, strIndex);

        CString strSizeInfo;
        strSizeInfo.Format(_T("%u"), itemInfo.file_size);
        m_ctlFileList.SetItemText(itemInfo.file_index, FILEINFO_SIZE, strSizeInfo);

        ItemInformation *pItemInfo = new ItemInformation;
        *pItemInfo = itemInfo;

        m_ctlFileList.SetItemData(itemInfo.file_index, (DWORD_PTR)pItemInfo);
    } while (true);

    m_ctlFileList.SetItemState(0, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
    GetDlgItem(IDC_BTN_OUTPUT)->EnableWindow(TRUE);
}


void CAutoEyeDlg::OnLvnItemchangedListInstfile(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    *pResult = 0;
    m_ctlFileView.SetWindowText(_T(""));

    int nItem = pNMLV->iItem;
    if (!(pNMLV->uOldState & LVIS_SELECTED) && (pNMLV->uNewState & LVIS_SELECTED))
    {
        const ItemInformation *pItemInfo = (const ItemInformation *)m_ctlFileList.GetItemData(nItem);
        if (pItemInfo != nullptr)
        {
            if (pItemInfo->file_buffer != nullptr)
            {
                if (pItemInfo->file_index == 0)
                {
                    // 脚本预览处理,有些版本会有2字节多余的文件编码标识
                    const BYTE *lpTextBuf = pItemInfo->file_buffer;
                    if (lpTextBuf != nullptr)
                    {
                        if (m_objBaseInfo.unicode_script)
                        {
                            CString strText((LPCWSTR)lpTextBuf);
                            m_ctlFileView.SetWindowText(strText);
                        }
                        else
                        {
                            CString strText((LPCSTR)lpTextBuf);
                            m_ctlFileView.SetWindowText(strText);
                        }
                    }
                }
                else
                {
                    CString strOutput;
                    ConvertHex2String(pItemInfo->file_buffer, pItemInfo->file_size, strOutput, FALSE);
                    m_ctlFileView.SetWindowText(strOutput);
                }
            }
        }
    }
}


void CAutoEyeDlg::OnLvnDeleteallitemsListInstfile(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    *pResult = 0;

    int nItemCount = m_ctlFileList.GetItemCount();
    for (int nItem = 0; nItem < nItemCount; nItem++)
    {
        ItemInformation *lpInfo = (ItemInformation *)m_ctlFileList.GetItemData(nItem);
        if (lpInfo != nullptr)
        {
            au3engine_close_item(m_hAutoHandle, lpInfo);
            m_ctlFileList.SetItemData(nItem, NULL);

            delete lpInfo;
            lpInfo = nullptr;
        }
    }

    if (m_hAutoHandle != NULL)
    {
        au3engine_close_handle(m_hAutoHandle);
        m_hAutoHandle = NULL;
    }
}


void CAutoEyeDlg::OnDestroy()
{
    CDialog::OnDestroy();

    // TODO: 在此处添加消息处理程序代码
    SetDefaultUI();
}


void CAutoEyeDlg::OnBnClickedBtnMenuinter()
{
    CString strAutoDecA3X = _T("Autoit3 A3X Decompiler");
    CString strAutoDecEXE = _T("Autoit3 EXE Decompiler");
    CString strAutoDecPath = _T("Autoit3 PATH Decompiler");

    if (CheckInterStatus())
    {
        UnWinIntegrated(strAutoDecPath);
        UnWinIntegrated(strAutoDecEXE, _T("exe"));
        UnWinIntegrated(strAutoDecA3X, _T("a3x"));
    }
    else
    {
        CString strMenuName(_T("Autoit 反编译"));
        BOOL bPath = WinIntegrated(strAutoDecPath, strMenuName, GetAppFullName());
        BOOL bExe = WinIntegrated(strAutoDecEXE, strMenuName, GetAppFullName(), _T("exe"), _T("应用程序"));
        BOOL bA3X = WinIntegrated(strAutoDecA3X, strMenuName, GetAppFullName(), _T("a3x"), _T("Autoit3编译数据"));
        if (!bPath || !bExe || !bA3X)
        {
            AfxMessageBox(_T("菜单集成失败，请检查是否具有相关权限！"), MB_OK | MB_ICONSTOP);
        }
    }

    GetDlgItem(IDC_BTN_MENUINTER)->SetWindowText(CheckInterStatus() ? _T("-") : _T("+"));
}


void CAutoEyeDlg::OnBnClickedBtnAdvdec()
{
    CString strTmp = m_strInputFile;
    SetDefaultUI();
    m_strInputFile = strTmp;
    UpdateData(FALSE);

    int nID = AfxMessageBox(_T("透视模式下一些特殊的AutoIT文件可能会被运行起来，存在一定的风险。"
        "如果选择继续后若程序运行弹出任何界面或提示请直接关闭即可，您是否需要继续？"), 
        MB_YESNO | MB_ICONQUESTION);
    if (nID == IDYES)
    {
        ProcessAutoitFile(m_strInputFile, TRUE);
    }
}


void CAutoEyeDlg::OnBnClickedBtnDecompile()
{
    CString strTmp = m_strInputFile;
    SetDefaultUI();

    m_strInputFile = strTmp;
    UpdateData(FALSE);

    ProcessAutoitFile(m_strInputFile, FALSE);
}


void CAutoEyeDlg::OnBnClickedBtnAbout()
{
    CAboutDlg dlg;
    dlg.DoModal();
}

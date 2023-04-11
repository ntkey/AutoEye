// ProgressDlg.cpp: 实现文件
//

#include "pch.h"
#include "AutoEye.h"
#include "afxdialogex.h"
#include "ProgressDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CProgressDlg 对话框
enum
{
    WM_EXITDIALOG = WM_USER + 1
};

LPCTSTR CProgressDlg::m_lpszInfo = _T("正在努力处理中，请耐心等待");

IMPLEMENT_DYNAMIC(CProgressDlg, CDialog)

CProgressDlg::CProgressDlg(int *phAutoHandle,
    BasicInformation *pBaseInfo,
    CString strInputFile,
    BOOL bAdvance,
    BOOL bCmdMode,
    CWnd* pParent /*=nullptr*/)
    : CDialog(IDD_PROGRESS, pParent)
    , m_strStatus(m_lpszInfo)
{
    m_phAutoHandle = phAutoHandle;
    m_pBaseInfo = pBaseInfo;
    m_strInputFile = strInputFile;

    m_nCount = 0;

    m_bAdvance = bAdvance;
    m_bCmdMode = bCmdMode;

    m_nResult = ErrorUnknow;
    m_pThreadDecompile = nullptr;
}


CProgressDlg::~CProgressDlg()
{
    if (m_pThreadDecompile != nullptr)
    {
        m_pThreadDecompile->join();
    }
}


void CProgressDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_STATIC_STATUS, m_strStatus);
}


BEGIN_MESSAGE_MAP(CProgressDlg, CDialog)
    ON_WM_TIMER()
END_MESSAGE_MAP()


// CProgressDlg 消息处理程序


void CProgressDlg::OnTimer(UINT_PTR nIDEvent)
{
    m_nCount++;
    m_nCount = m_nCount % 20;
    CString strDot;
    for (int i=0; i<m_nCount; i++)
    {
        strDot += _T(".");
    }

    m_strStatus = CString(m_lpszInfo) + strDot;
    UpdateData(FALSE);

    CDialog::OnTimer(nIDEvent);
}


BOOL CProgressDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    if (m_bCmdMode && !IsDebuggerPresent())
    {
        AttachThreadInput(GetWindowThreadProcessId(::GetForegroundWindow(), NULL), GetCurrentThreadId(), TRUE);
        SetForegroundWindow();
        SetFocus();
        AttachThreadInput(GetWindowThreadProcessId(::GetForegroundWindow(), NULL), GetCurrentThreadId(), FALSE);
    }	

    SetTimer(0, 100, NULL);
    m_pThreadDecompile = std::unique_ptr<std::thread>(new std::thread([this]()
    {
        if (m_bAdvance)
        {
            m_nResult = au3engine_openfile_advance(m_phAutoHandle, m_pBaseInfo, m_strInputFile, nullptr);
        }
        else
        {
            m_nResult = au3engine_openfile_general(m_phAutoHandle, m_pBaseInfo, m_strInputFile, nullptr);
        }

        PostMessage(WM_EXITDIALOG);
    }));
    if (m_pThreadDecompile == nullptr)
    {
        PostMessage(WM_EXITDIALOG);
    }

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


void CProgressDlg::OnOK()
{
}


void CProgressDlg::OnCancel()
{
}


BOOL CProgressDlg::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->message == WM_EXITDIALOG)
    {
        CDialog::OnOK();
    }

    return CDialog::PreTranslateMessage(pMsg);
}

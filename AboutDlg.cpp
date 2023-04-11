// AboutDlg.cpp: 实现文件
//

#include "pch.h"
#include "AutoEye.h"
#include "afxdialogex.h"
#include "AboutDlg.h"
#include "CommonVersion.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAboutDlg 对话框

IMPLEMENT_DYNAMIC(CAboutDlg, CDialog)

CAboutDlg::CAboutDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_ABOUTBOX, pParent)
{
	CString strAppInfo = _T("程序说明：\r\n"
		"    本程序为AutoIT & AutoHotKey脚本反编译程序，可以将其编译后的文件（包括exe和a3x）"
		"反编译为原始文件，并具备针对混淆过的脚本进行反混淆的功能。\r\n"
		"    支持所有AutoIT版本及任意加密方式"
		"（如VMProtect、Armadillo、Themida、Winlicense、Enigma等各种强壳猛壳），"
		"任何加密的AutoIT文件均无所遁形一眼洞穿，故名为 “乾坤镜”。\r\n");
	
	CString strThankInfo = _T("特别感谢以下热心网友提供的相关帮助和测试：\r\n"
		"    发哥         \tQQ：252951233\r\n"
		"    赛德尼玛黢黑 \tQQ：862228699\r\n"
		"    茶末余香     \tQQ：448119939\r\n"
		"    Rēмēbēr.   \tQQ：420296173");

	m_strAboutInfo = strAppInfo + _T("\r\n\r\n") + strThankInfo;
}

CAboutDlg::~CAboutDlg()
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_INFOS, m_strAboutInfo);
}


BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CAboutDlg 消息处理程序


void CAboutDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
}


BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	SetDlgItemText(IDC_STATIC_NAME, CString(VER_PRUDUCT_NAME) + _T(" v") + CString(VER_FILEVERSION));
	SetDlgItemText(IDC_STATIC_COPYRIGHT, CString(VER_COPYRIGHT));

	CString strVersion = CString(_T("Build: ")) + CString(__DATE__) + _T(" ") + CString(__TIME__);
	SetDlgItemText(IDC_STATIC_COMPTIME, strVersion);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

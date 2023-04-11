#pragma once

#include "afxdialogex.h"

// CProgressDlg 对话框

class CProgressDlg : public CDialog
{
	DECLARE_DYNAMIC(CProgressDlg)

public:
	CProgressDlg(int *phAutoHandle,
		BasicInformation *pBaseInfo,
		CString strInputFile, 
		BOOL bAdvance = FALSE,
		BOOL bCmdMode = FALSE,
		CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CProgressDlg();

	int GetDecResult() {
		return m_nResult;
	}

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROGRESS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
	static LPCTSTR m_lpszInfo;
	CString m_strStatus;
	std::unique_ptr<std::thread> m_pThreadDecompile;
	int *m_phAutoHandle;
	BasicInformation *m_pBaseInfo;
	CString m_strInputFile;
	int m_nResult;
	int m_nCount;
	BOOL m_bAdvance;
	BOOL m_bCmdMode;

public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

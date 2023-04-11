
// AutoEyeDlg.h: 头文件
//

#pragma once


// CAutoEyeDlg 对话框
class CAutoEyeDlg : public CDialog
{
// 构造
public:
	CAutoEyeDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_AUTOEYE_DIALOG };
#endif

	void OutputDecFiles(BasicInformation *pBaseInfo, 
		CString strSrcPath, 
		ItemInformation *pDecItems, 
		DWORD dwCount,
		BOOL bExplor);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

private:
	CString m_strInputFile;
	CListCtrl m_ctlFileList;
	CEdit m_ctlFileView;
	int m_hAutoHandle;
	BasicInformation m_objBaseInfo;

	enum
	{
		FILEINFO_PATH = 0,
		FILEINFO_INDEX,
		FILEINFO_SIZE,
	};

protected:
	void SetDefaultUI();
	void ProcessAutoitFile(CString strSourceFile, BOOL bAdvance = FALSE);
	int GetFileIconIndex(CString strFilePath);

	BOOL CheckInterStatus();
	CString GetAppFullName();

	BOOL UnWinIntegrated(CString strRegName);
	BOOL UnWinIntegrated(CString strRegName, CString strFileExt);

	BOOL WinIntegrated(CString strRegName, CString strMenuName, CString strCmdFile);
	BOOL WinIntegrated(CString strRegName, CString strMenuName, CString strCmdFile,
		CString strFileExt, CString strFileTypeDetail);
	
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnSelfile();
	afx_msg void OnBnClickedBtnOutput();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnLvnItemchangedListInstfile(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnDeleteallitemsListInstfile(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedBtnDecompile();
	afx_msg void OnBnClickedBtnMenuinter();
	afx_msg void OnBnClickedBtnAdvdec();
	afx_msg void OnBnClickedBtnAbout();
};

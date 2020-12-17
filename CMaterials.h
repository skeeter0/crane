#pragma once
#include < Gdiplus.h>  
#pragma comment(lib, "Gdiplus.lib")  
using namespace Gdiplus;


// CMaterials 对话框

class CMaterials : public CDialogEx
{
	DECLARE_DYNAMIC(CMaterials)

public:
	CMaterials(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CMaterials();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROPPAGE_MEDIUM };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);

	bool Save(Bitmap* const pBitMap, LPCTSTR lpFileName);

	DECLARE_MESSAGE_MAP()
};

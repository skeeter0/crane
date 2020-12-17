// CMaterials.cpp: 实现文件
//

#include "pch.h"
#include "ZFNest.h"
#include "CMaterials.h"
#include "afxdialogex.h"

#include "Gdiplusheaders.h"

// CMaterials 对话框

IMPLEMENT_DYNAMIC(CMaterials, CDialogEx)

CMaterials::CMaterials(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PROPPAGE_MEDIUM, pParent)
{

}

CMaterials::~CMaterials()
{
}

void CMaterials::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMaterials, CDialogEx)
END_MESSAGE_MAP()


// CMaterials 消息处理程序
int CMaterials::GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	ImageCodecInfo* pImageCodecInfo = NULL;

	GetImageEncodersSize(&num, &size);
	if (size == 0)
		return -1;  // Failure

	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if (pImageCodecInfo == NULL)
		return -1;  // Failure

	GetImageEncoders(num, size, pImageCodecInfo);

	for (UINT j = 0; j < num; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}
	}
	free(pImageCodecInfo);
	return -1;  // Failure
}
bool CMaterials::Save(Bitmap* const pBitMap, LPCTSTR lpFileName)
{
	CLSID encoderClsid;
	GetEncoderClsid(L"image/bmp", &encoderClsid);
	pBitMap->Save(lpFileName, &encoderClsid, NULL);
	return true;
}
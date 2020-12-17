
// MainFrm.cpp: CMainFrame 类的实现
//

#include "pch.h"
#include "framework.h"
#include "ZFNest.h"

#include "MainFrm.h"

#include "dxflib/dl_dxf.h"
#include "dxflib/dl_creationadapter.h"
#include "test_creationclass.h"
#include "libqrencode/qrencode.h"
#include "QR2BMP.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Prescaler (number of pixels in bmp file for each QRCode pixel, on each dimension)
#define OUT_FILE_PIXEL_PRESCALER    4       
// Color of bmp pixels  
#define PIXEL_COLOR_R               0xff        
#define PIXEL_COLOR_G               0
#define PIXEL_COLOR_B               0
#define BI_RGB                      0L



// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

const int  iMaxUserToolbars = 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_CUSTOMIZE, &CMainFrame::OnViewCustomize)
	ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &CMainFrame::OnToolbarCreateNew)
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnUpdateApplicationLook)
	ON_COMMAND(ID_VIEW_CAPTION_BAR, &CMainFrame::OnViewCaptionBar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_CAPTION_BAR, &CMainFrame::OnUpdateViewCaptionBar)
	ON_COMMAND(ID_TOOLS_OPTIONS, &CMainFrame::OnOptions)
	ON_WM_SETTINGCHANGE()
	ON_COMMAND(ID_FILE_OPEN, &CMainFrame::OnFileOpen)
	ON_COMMAND(ID_ImportPatterns, &CMainFrame::OnImportpatterns)
	ON_COMMAND(ID_MenuExportQRCode, &CMainFrame::OnMenuexportqrcode)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 状态行指示器
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame 构造/析构

CMainFrame::CMainFrame() noexcept
{
	// TODO: 在此添加成员初始化代码
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_VS_2008);
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	BOOL bNameValid;

	if (!m_wndMenuBar.Create(this))
	{
		TRACE0("未能创建菜单栏\n");
		return -1;      // 未能创建
	}

	m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);

	// 防止菜单栏在激活时获得焦点
	CMFCPopupMenu::SetForceMenuFocus(FALSE);

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(theApp.m_bHiColorIcons ? IDR_MAINFRAME_256 : IDR_MAINFRAME))
	{
		TRACE0("未能创建工具栏\n");
		return -1;      // 未能创建
	}

	CString strToolBarName;
	bNameValid = strToolBarName.LoadString(IDS_TOOLBAR_STANDARD);
	ASSERT(bNameValid);
	m_wndToolBar.SetWindowText(strToolBarName);

	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);
	m_wndToolBar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);

	// 允许用户定义的工具栏操作: 
	InitUserToolbars(nullptr, uiFirstUserToolBarId, uiLastUserToolBarId);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("未能创建状态栏\n");
		return -1;      // 未能创建
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	// TODO: 如果您不希望工具栏和菜单栏可停靠，请删除这五行
	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndMenuBar);
	DockPane(&m_wndToolBar);


	// 启用 Visual Studio 2005 样式停靠窗口行为
	CDockingManager::SetDockingMode(DT_SMART);
	// 启用 Visual Studio 2005 样式停靠窗口自动隐藏行为
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// 创建标题栏: 
	if (!CreateCaptionBar())
	{
		TRACE0("未能创建标题栏\n");
		return -1;      // 未能创建
	}

	// 加载菜单项图像(不在任何标准工具栏上): 
	CMFCToolBar::AddToolBarForImageCollection(IDR_MENU_IMAGES, theApp.m_bHiColorIcons ? IDB_MENU_IMAGES_24 : 0);

	// 创建停靠窗口
	if (!CreateDockingWindows())
	{
		TRACE0("未能创建停靠窗口\n");
		return -1;
	}

	m_wndFileView.EnableDocking(CBRS_ALIGN_ANY);
	m_wndClassView.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndFileView);
	CDockablePane* pTabbedBar = nullptr;
	m_wndClassView.AttachToTabWnd(&m_wndFileView, DM_SHOW, TRUE, &pTabbedBar);
	m_wndOutput.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndOutput);
	m_wndProperties.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndProperties);

	// 基于持久值设置视觉管理器和样式
	OnApplicationLook(theApp.m_nAppLook);

	// 启用工具栏和停靠窗口菜单替换
	EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, strCustomize, ID_VIEW_TOOLBAR);

	// 启用快速(按住 Alt 拖动)工具栏自定义
	CMFCToolBar::EnableQuickCustomization();

	if (CMFCToolBar::GetUserImages() == nullptr)
	{
		// 加载用户定义的工具栏图像
		if (m_UserImages.Load(_T(".\\UserImages.bmp")))
		{
			CMFCToolBar::SetUserImages(&m_UserImages);
		}
	}

	// 启用菜单个性化(最近使用的命令)
	// TODO: 定义您自己的基本命令，确保每个下拉菜单至少有一个基本命令。
	CList<UINT, UINT> lstBasicCommands;

	lstBasicCommands.AddTail(ID_FILE_NEW);
	lstBasicCommands.AddTail(ID_FILE_OPEN);
	lstBasicCommands.AddTail(ID_FILE_SAVE);
	lstBasicCommands.AddTail(ID_FILE_PRINT);
	lstBasicCommands.AddTail(ID_APP_EXIT);
	lstBasicCommands.AddTail(ID_EDIT_CUT);
	lstBasicCommands.AddTail(ID_EDIT_PASTE);
	lstBasicCommands.AddTail(ID_EDIT_UNDO);
	lstBasicCommands.AddTail(ID_APP_ABOUT);
	lstBasicCommands.AddTail(ID_VIEW_STATUS_BAR);
	lstBasicCommands.AddTail(ID_VIEW_TOOLBAR);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2003);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_VS_2005);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLUE);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_SILVER);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLACK);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_AQUA);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_WINDOWS_7);
	lstBasicCommands.AddTail(ID_SORTING_SORTALPHABETIC);
	lstBasicCommands.AddTail(ID_SORTING_SORTBYTYPE);
	lstBasicCommands.AddTail(ID_SORTING_SORTBYACCESS);
	lstBasicCommands.AddTail(ID_SORTING_GROUPBYTYPE);

	CMFCToolBar::SetBasicCommands(lstBasicCommands);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE
		 | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_MAXIMIZE | WS_SYSMENU;

	return TRUE;
}

BOOL CMainFrame::CreateDockingWindows()
{
	BOOL bNameValid;

	// 创建类视图
	CString strClassView;
	bNameValid = strClassView.LoadString(IDS_CLASS_VIEW);
	ASSERT(bNameValid);
	if (!m_wndClassView.Create(strClassView, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_CLASSVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建“类视图”窗口\n");
		return FALSE; // 未能创建
	}

	// 创建文件视图
	CString strFileView;
	bNameValid = strFileView.LoadString(IDS_FILE_VIEW);
	ASSERT(bNameValid);
	if (!m_wndFileView.Create(strFileView, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_FILEVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT| CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建“文件视图”窗口\n");
		return FALSE; // 未能创建
	}

	// 创建输出窗口
	CString strOutputWnd;
	bNameValid = strOutputWnd.LoadString(IDS_OUTPUT_WND);
	ASSERT(bNameValid);
	if (!m_wndOutput.Create(strOutputWnd, this, CRect(0, 0, 100, 100), TRUE, ID_VIEW_OUTPUTWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建输出窗口\n");
		return FALSE; // 未能创建
	}

	// 创建属性窗口
	CString strPropertiesWnd;
	bNameValid = strPropertiesWnd.LoadString(IDS_PROPERTIES_WND);
	ASSERT(bNameValid);
	if (!m_wndProperties.Create(strPropertiesWnd, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_PROPERTIESWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建“属性”窗口\n");
		return FALSE; // 未能创建
	}

	SetDockingWindowIcons(theApp.m_bHiColorIcons);
	return TRUE;
}

void CMainFrame::SetDockingWindowIcons(BOOL bHiColorIcons)
{
	HICON hFileViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_FILE_VIEW_HC : IDI_FILE_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndFileView.SetIcon(hFileViewIcon, FALSE);

	HICON hClassViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_CLASS_VIEW_HC : IDI_CLASS_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndClassView.SetIcon(hClassViewIcon, FALSE);

	HICON hOutputBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_OUTPUT_WND_HC : IDI_OUTPUT_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndOutput.SetIcon(hOutputBarIcon, FALSE);

	HICON hPropertiesBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_PROPERTIES_WND_HC : IDI_PROPERTIES_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndProperties.SetIcon(hPropertiesBarIcon, FALSE);

}

BOOL CMainFrame::CreateCaptionBar()
{
	if (!m_wndCaptionBar.Create(WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, this, ID_VIEW_CAPTION_BAR, -1, TRUE))
	{
		TRACE0("未能创建标题栏\n");
		return FALSE;
	}

	BOOL bNameValid;

	CString strTemp, strTemp2;
	bNameValid = strTemp.LoadString(IDS_CAPTION_BUTTON);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetButton(strTemp, ID_TOOLS_OPTIONS, CMFCCaptionBar::ALIGN_LEFT, FALSE);
	bNameValid = strTemp.LoadString(IDS_CAPTION_BUTTON_TIP);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetButtonToolTip(strTemp);

	bNameValid = strTemp.LoadString(IDS_CAPTION_TEXT);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetText(strTemp, CMFCCaptionBar::ALIGN_LEFT);

	m_wndCaptionBar.SetBitmap(IDB_INFO, RGB(255, 255, 255), FALSE, CMFCCaptionBar::ALIGN_LEFT);
	bNameValid = strTemp.LoadString(IDS_CAPTION_IMAGE_TIP);
	ASSERT(bNameValid);
	bNameValid = strTemp2.LoadString(IDS_CAPTION_IMAGE_TEXT);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetImageToolTip(strTemp, strTemp2);

	return TRUE;
}

// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 消息处理程序

void CMainFrame::OnViewCustomize()
{
	CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* 扫描菜单*/);
	pDlgCust->EnableUserDefinedToolbars();
	pDlgCust->Create();
}

LRESULT CMainFrame::OnToolbarCreateNew(WPARAM wp,LPARAM lp)
{
	LRESULT lres = CFrameWndEx::OnToolbarCreateNew(wp,lp);
	if (lres == 0)
	{
		return 0;
	}

	CMFCToolBar* pUserToolbar = (CMFCToolBar*)lres;
	ASSERT_VALID(pUserToolbar);

	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
	return lres;
}

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2008:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_WINDOWS_7:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
	}

	m_wndOutput.UpdateFonts();
	RedrawWindow(nullptr, nullptr, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}

void CMainFrame::OnViewCaptionBar()
{
	m_wndCaptionBar.ShowWindow(m_wndCaptionBar.IsVisible() ? SW_HIDE : SW_SHOW);
	RecalcLayout(FALSE);
}

void CMainFrame::OnUpdateViewCaptionBar(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_wndCaptionBar.IsVisible());
}

void CMainFrame::OnOptions()
{
}


BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext)
{
	// 基类将执行真正的工作

	if (!CFrameWndEx::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
	{
		return FALSE;
	}


	// 为所有用户工具栏启用自定义按钮
	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	for (int i = 0; i < iMaxUserToolbars; i ++)
	{
		CMFCToolBar* pUserToolbar = GetUserToolBarByIndex(i);
		if (pUserToolbar != nullptr)
		{
			pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
		}
	}

	return TRUE;
}


void CMainFrame::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CFrameWndEx::OnSettingChange(uFlags, lpszSection);
	m_wndOutput.UpdateFonts();
}


void CMainFrame::OnFileOpen()
{
	// TODO: 在此添加命令处理程序代码
	CString filter;
	CString fileFullStr, fileStr;
	filter = "DXF document(*.dxf)|ISO document(*.iso)";
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, filter);
	dlg.m_ofn.lpstrTitle = _T("ZG Open desgine file");
	if (dlg.DoModal() == IDOK)
	{
		fileFullStr = dlg.GetPathName();
		fileStr = dlg.GetFileName();
#ifdef __DEBUG
		MessageBoxA(NULL, fileFullStr, "Open", MB_OK);
#endif
	}
	else
		return;

	if (fileFullStr.GetLength() < 0)
		return;
	fileFullStr.MakeUpper();
	char* tmp = new char[MAX_PATH];
	sprintf(tmp, "%S", fileFullStr);

	//std::string fileName = CStringToSTDStr(fileFullStr);
	testReading(tmp);
	testWriting();
	delete tmp;
}

void CMainFrame::testReading(char* file) {
	// Load DXF file into memory:
//	std::cout << "Reading file " << file << "...\n";
	Test_CreationClass* creationClass = new Test_CreationClass();
	DL_Dxf* dxf = new DL_Dxf();
	if (!dxf->in(file, creationClass)) { // if file open failed
		MessageBoxA(NULL,"could not be opened.\n", "ZGNest",MB_OK);
		
	}

	else
		MessageBoxA(NULL, "reading finish....\n", "ZGNest ", MB_OK);
	//dxf->addLayer(creationClass);
	delete dxf;
	delete creationClass;
}



void CMainFrame::testWriting() {
	DL_Dxf* dxf = new DL_Dxf();
	DL_Codes::version exportVersion = DL_Codes::AC1015;
	DL_WriterA* dw = dxf->out("myfile.dxf", exportVersion);
	if (dw == NULL) {
		printf("Cannot open file 'myfile.dxf' \
               for writing.");
		// abort function e.g. with return
	}

	dxf->writeHeader(*dw);
	dw->sectionEnd();
	dw->sectionTables();
	dxf->writeVPort(*dw);

	dw->tableLinetypes(3);
	dxf->writeLinetype(*dw, DL_LinetypeData("BYBLOCK", "BYBLOCK", 0, 0, 0.0));
	dxf->writeLinetype(*dw, DL_LinetypeData("BYLAYER", "BYLAYER", 0, 0, 0.0));
	dxf->writeLinetype(*dw, DL_LinetypeData("CONTINUOUS", "Continuous", 0, 0, 0.0));
	dw->tableEnd();

	int numberOfLayers = 3;
	dw->tableLayers(numberOfLayers);

	dxf->writeLayer(*dw,
		DL_LayerData("0", 0),
		DL_Attributes(
			std::string(""),      // leave empty
			DL_Codes::black,        // default color
			100,                  // default width
			"CONTINUOUS", 1.0));       // default line style

	dxf->writeLayer(*dw,
		DL_LayerData("mainlayer", 0),
		DL_Attributes(
			std::string(""),
			DL_Codes::red,
			100,
			"CONTINUOUS", 1.0));

	dxf->writeLayer(*dw,
		DL_LayerData("anotherlayer", 0),
		DL_Attributes(
			std::string(""),
			DL_Codes::black,
			100,
			"CONTINUOUS", 1.0));

	dw->tableEnd();

	dw->tableStyle(1);
	dxf->writeStyle(*dw, DL_StyleData("standard", 0, 2.5, 1.0, 0.0, 0, 2.5, "txt", ""));
	dw->tableEnd();

	dxf->writeView(*dw);
	dxf->writeUcs(*dw);

	dw->tableAppid(1);
	dxf->writeAppid(*dw, "ACAD");
	dw->tableEnd();

	dxf->writeDimStyle(*dw, 1, 1, 1, 1, 1);

	dxf->writeBlockRecord(*dw);
	dxf->writeBlockRecord(*dw, "myblock1");
	dxf->writeBlockRecord(*dw, "myblock2");
	dw->tableEnd();

	dw->sectionEnd();

	dw->sectionBlocks();
	dxf->writeBlock(*dw, DL_BlockData("*Model_Space", 0, 0.0, 0.0, 0.0));
	dxf->writeEndBlock(*dw, "*Model_Space");
	dxf->writeBlock(*dw, DL_BlockData("*Paper_Space", 0, 0.0, 0.0, 0.0));
	dxf->writeEndBlock(*dw, "*Paper_Space");
	dxf->writeBlock(*dw, DL_BlockData("*Paper_Space0", 0, 0.0, 0.0, 0.0));
	dxf->writeEndBlock(*dw, "*Paper_Space0");

	dxf->writeBlock(*dw, DL_BlockData("myblock1", 0, 0.0, 0.0, 0.0));
	// ...
	// write block entities e.g. with dxf->writeLine(), ..
	// ...
	dxf->writeEndBlock(*dw, "myblock1");

	dxf->writeBlock(*dw, DL_BlockData("myblock2", 0, 0.0, 0.0, 0.0));
	// ...
	// write block entities e.g. with dxf->writeLine(), ..
	// ...
	dxf->writeEndBlock(*dw, "myblock2");

	dw->sectionEnd();
	dw->sectionEntities();

	// write all entities in model space:
	dxf->writePoint(
		*dw,
		DL_PointData(10.0,
			45.0,
			0.0),
		DL_Attributes("mainlayer", 256, -1, "BYLAYER", 1.0));

	dxf->writeLine(
		*dw,
		DL_LineData(25.0,   // start point
			30.0,
			0.0,
			100.0,   // end point
			120.0,
			0.0),
		DL_Attributes("mainlayer", 256, -1, "BYLAYER", 1.0));
	dxf->writeLine(
		*dw,
		DL_LineData(35.0,   // start point
			130.0,
			0.0,
			100.0,   // end point
			35.0,
			0.0),
		DL_Attributes("ZGlayer", 110, -3, "BYLAYER", 1.0));
	dxf->writeArc(
		*dw,
		DL_ArcData(135.0,   // start point
			0.0,
			0.0,
			200.0,   // 半径
			35.0,
			135.0),
		DL_Attributes("ZGARClayer", 110, -3, "BYLAYER", 1.0));

	dxf->writeCircle(
		*dw,
		DL_CircleData(155.0,   // start point
			55.0,
			0.0,
			135.0),
		DL_Attributes("ZGCirclelayer", 60, -3, "BYLAYER", 1.0));

	dxf->writePolyline(*dw,
		DL_PolylineData(4, 0, 0, DL_CLOSED_PLINE),
		DL_Attributes("ZGPolylinelayer", 130, -3, "BYLAYER", 1.0));
	dxf->writeVertex(*dw,
		DL_VertexData(
			20,
			30,
			0, 0));
	dxf->writeVertex(*dw,
		DL_VertexData(
			60,
			90,
			0, 0));
	dxf->writeVertex(*dw,
		DL_VertexData(
			80,
			60,
			0, 0));

	dxf->writePolylineEnd(*dw);

	dw->sectionEnd();

	dxf->writeObjects(*dw);
	dxf->writeObjectsEnd(*dw);

	dw->dxfEOF();
	dw->close();

//	std::cout << "writting finish..." << std::endl;


	delete dw;
	delete dxf;
}

void CMainFrame::OnImportpatterns()
{
	// TODO: 在此添加命令处理程序代码
		// TODO: 在此添加命令处理程序代码
	CString filter;
	CString fileFullStr, fileStr,strfileExt;
	filter = "All Support Design|*.DXF;*.PLT|DXF document(*.DXF)|*.DXF|PLT document(*.PLT)|*.PLT|ISO document(*.iso)|*.ISO";
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, filter);
	dlg.m_ofn.lpstrTitle = _T("ZG Open design file");
	if (dlg.DoModal() == IDOK)
	{
		fileFullStr = dlg.GetPathName();
		fileStr = dlg.GetFileName();
		strfileExt = dlg.GetFileExt();
		strfileExt.MakeUpper();
		if (strfileExt == "PLT"||strfileExt == "HPGL") {
			OnImportPltpatterns(fileFullStr);
		}
		else {
			if (strfileExt == "DXF") {
				OnImportDxfpatterns(fileFullStr);
			}
		}

#ifdef __DEBUG
		MessageBoxA(NULL, fileFullStr, "Open", MB_OK);
#endif
	}
	else
		return;

	if (fileFullStr.GetLength() < 0)
		return;
	fileFullStr.MakeUpper();
	char* tmp = new char[MAX_PATH];
	sprintf(tmp, "%S", fileFullStr);

	//std::string fileName = CStringToSTDStr(fileFullStr);
	testReading(tmp);
	testWriting();
	delete tmp;
}

void CMainFrame::OnImportPltpatterns(CString strFullfilename) {

	int nFileNum = 0;

	CStdioFile myFile;

	CStringArray strPDFFileName;

	if (myFile.Open(strFullfilename, CFile::modeRead))

	{
		//读取；

		CString strLine;
		CPoint CcurrPoint(0,0), CToPoint(0,0);
		while (myFile.ReadString(strLine))
		{
			strLine.MakeUpper();
			int nStart = strLine.Find(_T("pu"));
			int nEnd = strLine.Find(_T("pu"), nStart + 2);
			int yPos = 0;
			int nPos = strLine.Find(_T("PD"));
			if (nPos < 0)
			{
				MessageBoxA(NULL,"Plt文件为空！","ZG nest",MB_OK );
				return;
			}
			for (int i = nStart; ; i < nEnd) {
				int nxPos = strLine.Find(_T(","), nPos + 2);
				CString strXPos = strLine.Mid(nPos + 2, nxPos - nPos - 2);//x坐标

				int nyPos = strLine.Find(_T(","), nxPos + 1);
				CString strYPos = strLine.Mid(nxPos + 1, nyPos - nxPos - 1);//y坐标
				CcurrPoint.x = _ttoi(strXPos);
				CcurrPoint.y = _ttoi(strYPos);

				i = i+yPos;
			}
		}
	}
		myFile.Close();
}
void CMainFrame::OnImportDxfpatterns(CString fullfilename) {


}


void CMainFrame::OnMenuexportqrcode()
{
	// TODO: 在此添加命令处理程序代码
#define MAX_URI_LENGTH 255
#define QR_IMAGE_SIZE  300


	//	ui.lblQRCode->setText("");
	CString uri( "123456abcdefgHIGKLMNOPQRSTUVWXYZ");
	if (!uri.IsEmpty())
	{

		// limit URI length

		if (uri.GetLength() > MAX_URI_LENGTH)
		{
			MessageBoxA(NULL,"字符太长","ZG nest",MB_OK);
		}
		else {
			char* ptr;
			#ifdef _UNICODE    
			LONG len;
			len = WideCharToMultiByte(CP_ACP, 0, uri, -1, NULL, 0, NULL, NULL);
			ptr = new char[len + 1];    memset(ptr, 0, len + 1);
			WideCharToMultiByte(CP_ACP, 0, uri, -1, ptr, len + 1, NULL, NULL);
			#else    
			ptr = new char[str.GetAllocLength() + 1];

			sprintf(ptr, "%s", uri);
			#endif   
			Save_QEImage(ptr);
			if (ptr)
				delete[]ptr;
		}
		CMaterials m_Dlg_materials;
		m_Dlg_materials.DoModal();

	}

}

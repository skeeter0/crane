
// ZFNestView.cpp: CZFNestView 类的实现
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "ZFNest.h"
#endif

#include "ZFNestDoc.h"
#include "ZFNestView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CZFNestView

IMPLEMENT_DYNCREATE(CZFNestView, CView)

BEGIN_MESSAGE_MAP(CZFNestView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CZFNestView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CZFNestView 构造/析构

CZFNestView::CZFNestView() noexcept
{
	// TODO: 在此处添加构造代码

}

CZFNestView::~CZFNestView()
{
}

BOOL CZFNestView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CZFNestView 绘图

void CZFNestView::OnDraw(CDC* /*pDC*/)
{
	CZFNestDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
}


// CZFNestView 打印


void CZFNestView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CZFNestView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CZFNestView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CZFNestView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}

void CZFNestView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CZFNestView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CZFNestView 诊断

#ifdef _DEBUG
void CZFNestView::AssertValid() const
{
	CView::AssertValid();
}

void CZFNestView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CZFNestDoc* CZFNestView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CZFNestDoc)));
	return (CZFNestDoc*)m_pDocument;
}
#endif //_DEBUG


// CZFNestView 消息处理程序

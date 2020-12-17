// dllmain.cpp: DllMain 的实现。

#include "pch.h"
#include "framework.h"
#include "resource.h"
#include "ZFNestHandlers_i.h"
#include "dllmain.h"
#include "xdlldata.h"

CZFNestHandlersModule _AtlModule;

class CZFNestHandlersApp : public CWinApp
{
public:

// 重写
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CZFNestHandlersApp, CWinApp)
END_MESSAGE_MAP()

CZFNestHandlersApp theApp;

BOOL CZFNestHandlersApp::InitInstance()
{
	if (!PrxDllMain(m_hInstance, DLL_PROCESS_ATTACH, nullptr))
		return FALSE;
	return CWinApp::InitInstance();
}

int CZFNestHandlersApp::ExitInstance()
{
	return CWinApp::ExitInstance();
}

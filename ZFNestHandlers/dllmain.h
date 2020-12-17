// dllmain.h: 模块类的声明。

class CZFNestHandlersModule : public ATL::CAtlDllModuleT<CZFNestHandlersModule>
{
public :
	DECLARE_LIBID(LIBID_ZFNestHandlersLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_ZFNESTHANDLERS, "{081c36ff-7ebc-4423-b85d-bbfb44686de5}")
};

extern class CZFNestHandlersModule _AtlModule;

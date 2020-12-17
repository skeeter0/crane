

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 8.01.0622 */
/* at Tue Jan 19 11:14:07 2038
 */
/* Compiler settings for ZFNestHandlers.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 8.01.0622 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */



#ifdef __cplusplus
extern "C"{
#endif 


#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        EXTERN_C __declspec(selectany) const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif // !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, IID_IPreview,0x1c4eb2a4,0x3232,0x49bc,0xbb,0x19,0xf6,0xdc,0x8e,0xb4,0x66,0x62);


MIDL_DEFINE_GUID(IID, IID_IThumbnail,0xb030d83f,0xa546,0x4d99,0x94,0x10,0xcc,0x38,0x8c,0xd1,0x94,0x06);


MIDL_DEFINE_GUID(IID, LIBID_ZFNestHandlersLib,0x92fb1cd5,0xf18f,0x4e14,0xa8,0x84,0x4a,0xd7,0x23,0x8f,0x98,0x8c);


MIDL_DEFINE_GUID(CLSID, CLSID_Preview,0xbdeedf78,0xeb37,0x4551,0xb7,0x42,0xca,0x20,0xb2,0x14,0x66,0x9e);


MIDL_DEFINE_GUID(CLSID, CLSID_Thumbnail,0xb1240f4f,0x7af7,0x438a,0x85,0xaf,0x79,0x2a,0xc8,0xdc,0x6a,0x50);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif




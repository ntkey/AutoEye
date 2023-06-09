#ifndef __BUILD_VERSION_DEF_H_INCLUDED__
#define __BUILD_VERSION_DEF_H_INCLUDED__


#define VER_MAJOR_DEF               2
#define VER_MINOR_DEF               0
#define VER_PATCH_DEF               0
#define VER_BUILD_DEF               1000

#define _Stringizing(v) #v
#define _VerJoin(a, b, c, d)        _Stringizing(a.b.c.d)

#define VER_SPECIALBUILD            _Stringizing(VER_BUILD_DEF)
#define VER_FILEVERSION             _VerJoin(VER_MAJOR_DEF, VER_MINOR_DEF, VER_PATCH_DEF, VER_BUILD_DEF)
#define VER_VERSION_NUMBER          MAKELONG(MAKEWORD(VER_MAJOR_DEF, VER_MINOR_DEF), VER_PATCH_DEF)

#define VER_PRUDUCT_NAME            "AutoEye(乾坤镜)"
#define VER_AUTHOR                  "ntkey"
#define VER_COPYRIGHT               "Copyright(C) 2011-2023 " ##VER_AUTHOR
#define VER_TITLEINFO               L"" ##VER_PRUDUCT_NAME ##" " ##VER_FILEVERSION
#define VER_ABOUTINFO               L"" ##VER_PRUDUCT_NAME ##" " ##VER_FILEVERSION ##"  Author: " ##VER_AUTHOR

#endif // __BUILD_VERSION_DEF_H_INCLUDED__

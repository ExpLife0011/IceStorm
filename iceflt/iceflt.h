#ifndef __ICEFLT_H__
#define __ICEFLT_H__


#ifndef RTL_USE_AVL_TABLES
#define RTL_USE_AVL_TABLES
#endif // RTL_USE_AVL_TABLES

#define AV_VISTA    (NTDDI_VERSION >= NTDDI_VISTA)

#include <initguid.h>
#include <fltKernel.h>
#include <dontuse.h>
#include <suppress.h>
#include <intsafe.h>

#pragma prefast(disable:__WARNING_ENCODE_MEMBER_FUNCTION_POINTER, "Not valid for kernel mode drivers")


#endif // !__ICEFLT_H__

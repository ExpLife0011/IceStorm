#ifndef __UTILITY_H__
#define __UTILITY_H__

#include <fltKernel.h>


#define ICE_INVALID_FILE_REFERENCE( _fileid_ ) \
    (((_fileid_).FileId64.UpperZeroes == 0ll) && \
     ((_fileid_).FileId64.Value == (ULONGLONG)FILE_INVALID_FILE_ID))

#define ICE_SET_INVALID_FILE_REFERENCE( _fileid_ ) \
      (_fileid_).FileId64.UpperZeroes = 0ll;\
      (_fileid_).FileId64.Value = (ULONGLONG)FILE_INVALID_FILE_ID


typedef union _ICE_FILE_REFERENCE
{
    struct
    {
        ULONGLONG   Value;
        ULONGLONG   UpperZeroes;
    } FileId64;

    FILE_ID_128     FileId128;

} ICE_FILE_REFERENCE, *PICE_FILE_REFERENCE;


#endif // !__UTILITY_H__

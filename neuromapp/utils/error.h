#pragma once

#ifdef __cplusplus
namespace mapp{
#endif

enum mapp_error {
    MAPP_OK =0,
    MAPP_BAD_ARG,
    MAPP_USAGE,
    MAPP_BAD_DATA,
    MAPP_BAD_THREAD
};

#ifdef __cplusplus
     } // end namespace
#endif

#ifndef _Included_slc_server
#define _Included_slc_server
#ifdef __cplusplus
extern "C" {
#endif

#include "aida_types.h"

#define MAXPTS (MAX_DGRP_BPMS*MAX_BUFF_MEAS)

/* Override prototypes of externals to uppcase names, since compile.com
adds cc/names=UPPERCASE on compiles by default, but if the ATTRIBUTE=JNI
is in effect (as is for this module), then it's /names=AS_IS.
*/
void CVT_VMS_TO_IEEE_FLT(void* sorc_a, float dest_a[], unsigned short* nlong_p);
extern vmsstat_t DPSLCBUFF_INIT(void);

#ifdef __cplusplus
}
#endif
#endif

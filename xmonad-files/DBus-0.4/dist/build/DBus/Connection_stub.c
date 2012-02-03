#define IN_STG_CODE 0
#include "Rts.h"
#include "Stg.h"
#ifdef __cplusplus
extern "C" {
#endif
 
HsInt32 DBusziConnection_d30g(StgStablePtr the_stableptr, HsPtr a1, HsPtr a2, HsPtr a3)
{
Capability *cap;
HaskellObj ret;
HsInt32 cret;
cap = rts_lock();
cap=rts_evalIO(cap,rts_apply(cap,(HaskellObj)runIO_closure,rts_apply(cap,rts_apply(cap,rts_apply(cap,(StgClosure*)deRefStablePtr(the_stableptr),rts_mkPtr(cap,a1)),rts_mkPtr(cap,a2)),rts_mkPtr(cap,a3))) ,&ret);
rts_checkSchedStatus("DBusziConnection_d30g",cap);
cret=rts_getInt32(ret);
rts_unlock(cap);
return cret;
}
 
void DBusziConnection_d30p(StgStablePtr the_stableptr, HsStablePtr a1)
{
Capability *cap;
HaskellObj ret;
cap = rts_lock();
cap=rts_evalIO(cap,rts_apply(cap,(HaskellObj)runIO_closure,rts_apply(cap,(StgClosure*)deRefStablePtr(the_stableptr),rts_mkStablePtr(cap,a1))) ,&ret);
rts_checkSchedStatus("DBusziConnection_d30p",cap);
rts_unlock(cap);
}
#ifdef __cplusplus
}
#endif


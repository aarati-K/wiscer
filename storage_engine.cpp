#ifdef DEFAULT
#include "chained.cpp"
#endif

#ifdef VIP
#include "chained_adaptive.cpp"
#endif

#ifdef STORE
#include "store_workload.cpp"
#endif

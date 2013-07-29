/*
 * CPP guard
 */
#ifndef AERO_GUARD_HXX_INCLUDED
#define AERO_GUARD_HXX_INCLUDED

#ifdef __cplusplus
#define AERO_C_DECL_GUARD extern "C"
#define AERO_C_DECL_GUARD_BEGIN extern "C" {
#define AERO_C_DECL_GUARD_END }
#else
#define AERO_C_DECL_GUARD
#define AERO_C_DECL_GUARD_BEGIN
#define AERO_C_DECL_GUARD_END
#endif

#endif // AERO_GUARD_HXX_INCLUDED

#ifndef SAFE_BOOL_H
#define SAFE_BOOL_H

/** Macros for safe-bool idiom.
 *  This is to help stop wrong use of the operator bool ()
 */
#ifdef VCL_BORLAND
# define SPTR_SAFE_BOOL_TRUE true
# define SPTR_SAFE_BOOL_DEFINE typedef bool safe_bool
#else
# define SPTR_SAFE_BOOL_TRUE (&safe_bool_dummy::dummy)
# define SPTR_SAFE_BOOL_DEFINE \
   struct safe_bool_dummy { void dummy() {} }; \
   typedef void (safe_bool_dummy::* safe_bool)()
#endif

#endif

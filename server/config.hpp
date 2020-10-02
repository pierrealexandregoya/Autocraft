#pragma once

//// Tweak c++20
//#ifndef _HAS_CXX20
//# define _HAS_CXX20
//#endif

//// Test c++17
//#ifndef _HAS_CXX17
//# if defined(_MSVC_LANG) && !(defined(__EDG__) && defined(__clang__))  // TRANSITION, VSO#273681
//#  if _MSVC_LANG > 201402
//#   define _HAS_CXX17  1
//#  else /* _MSVC_LANG > 201402 */
//#   define _HAS_CXX17  0
//#  endif /* _MSVC_LANG > 201402 */
//# else /* _MSVC_LANG etc. */
//#  if __cplusplus > 201402
//#   define _HAS_CXX17  1
//#  else /* __cplusplus > 201402 */
//#   define _HAS_CXX17  0
//#  endif /* __cplusplus > 201402 */
//# endif /* _MSVC_LANG etc. */
//#endif /* !_HAS_CXX17 */

#if defined(_WIN32)
# define AUTOCRAFT_SYSTEM_WINDOWS
#elif defined(__APPLE__) && defined(__MACH__)
# include "TargetConditionals.h"
# if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
#  define AUTOCRAFT_SYSTEM_IOS
# elif TARGET_OS_MAC
#  define AUTOCRAFT_SYSTEM_MACOS
# else
#  error This Apple operating system is not supported by Autocraft library
# endif
#elif defined(__unix__)
# if defined(__ANDROID__)
#  define AUTOCRAFT_SYSTEM_ANDROID
# elif defined(__linux__)
#  define AUTOCRAFT_SYSTEM_LINUX
# elif defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
#  define AUTOCRAFT_SYSTEM_FREEBSD
# else
#  error This UNIX operating system is not supported by Autocraft library
# endif
#else
# error This operating system is not supported by Autocraft library
# endif

#if !defined(AUTOCRAFT_STATIC)
# if defined(AUTOCRAFT_SYSTEM_WINDOWS)
#  define AUTOCRAFT_SYMBOL_EXPORT __declspec(dllexport)
#  define AUTOCRAFT_SYMBOL_IMPORT __declspec(dllimport)
#  ifdef _MSC_VER
#   pragma warning(disable: 4251)
#  endif
# else // Linux, FreeBSD, Mac OS X
#  if __GNUC__ >= 4
#   define AUTOCRAFT_SYMBOL_EXPORT __attribute__ ((__visibility__ ("default")))
#   define AUTOCRAFT_SYMBOL_IMPORT __attribute__ ((__visibility__ ("default")))
#  else
#   define AUTOCRAFT_SYMBOL_EXPORT
#   define AUTOCRAFT_SYMBOL_IMPORT
#  endif
# endif
#else
# define AUTOCRAFT_SYMBOL_EXPORT
# define AUTOCRAFT_SYMBOL_IMPORT
#endif

#if defined(AUTOCRAFT_BUILDING_THE_LIB)
# define AUTOCRAFT_API AUTOCRAFT_SYMBOL_EXPORT
#else
# define AUTOCRAFT_API AUTOCRAFT_SYMBOL_IMPORT
#endif


#ifndef http_lib_EXPORT_H
#define http_lib_EXPORT_H

#ifdef http_lib_BUILT_AS_STATIC
#  define http_lib_EXPORT
#  define HTTP_LIB_NO_EXPORT
#else
#  ifndef http_lib_EXPORT
#    ifdef http_lib_EXPORTS
        /* We are building this library */
#      define http_lib_EXPORT __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define http_lib_EXPORT __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef HTTP_LIB_NO_EXPORT
#    define HTTP_LIB_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef HTTP_LIB_DEPRECATED
#  define HTTP_LIB_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef HTTP_LIB_DEPRECATED_EXPORT
#  define HTTP_LIB_DEPRECATED_EXPORT http_lib_EXPORT HTTP_LIB_DEPRECATED
#endif

#ifndef HTTP_LIB_DEPRECATED_NO_EXPORT
#  define HTTP_LIB_DEPRECATED_NO_EXPORT HTTP_LIB_NO_EXPORT HTTP_LIB_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef HTTP_LIB_NO_DEPRECATED
#    define HTTP_LIB_NO_DEPRECATED
#  endif
#endif

#endif /* http_lib_EXPORT_H */


#ifndef HTTP_LIB_EXPORT_H
#define HTTP_LIB_EXPORT_H

#ifdef HTTP_LIB_STATIC_DEFINE
#  define HTTP_LIB_EXPORT
#  define HTTP_LIB_NO_EXPORT
#else
#  ifndef HTTP_LIB_EXPORT
#    ifdef http_lib_EXPORTS
        /* We are building this library */
#      define HTTP_LIB_EXPORT __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define HTTP_LIB_EXPORT __attribute__((visibility("default")))
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
#  define HTTP_LIB_DEPRECATED_EXPORT HTTP_LIB_EXPORT HTTP_LIB_DEPRECATED
#endif

#ifndef HTTP_LIB_DEPRECATED_NO_EXPORT
#  define HTTP_LIB_DEPRECATED_NO_EXPORT HTTP_LIB_NO_EXPORT HTTP_LIB_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef HTTP_LIB_NO_DEPRECATED
#    define HTTP_LIB_NO_DEPRECATED
#  endif
#endif

#endif /* HTTP_LIB_EXPORT_H */

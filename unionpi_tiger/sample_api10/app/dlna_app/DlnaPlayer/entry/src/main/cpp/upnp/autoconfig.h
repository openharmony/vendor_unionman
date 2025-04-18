/* autoconfig.h.  Generated from autoconfig.h.in by configure.  */
/* autoconfig.h.in.  Generated from configure.ac by autoheader.  */

/* Define if building universal (internal helper macro) */
/* #undef AC_APPLE_UNIVERSAL_BUILD */

/* Define to 1 to compile debug code */
#define DEBUG 1

/* Define to 1 if you have the <arpa/inet.h> header file. */
#define HAVE_ARPA_INET_H 1

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define to 1 if you don't have `vprintf' but do have `_doprnt.' */
/* #undef HAVE_DOPRNT */

/* Define to 1 if you have the <fcntl.h> header file. */
#define HAVE_FCNTL_H 1

/* Define to 1 if fseeko (and presumably ftello) exists and is declared. */
#define HAVE_FSEEKO 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the <limits.h> header file. */
#define HAVE_LIMITS_H 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the <netdb.h> header file. */
#define HAVE_NETDB_H 1

/* Define to 1 if you have the <netinet/in.h> header file. */
#define HAVE_NETINET_IN_H 1

/* Define if you have POSIX threads libraries and header files. */
#define HAVE_PTHREAD 1

/* Have PTHREAD_PRIO_INHERIT. */
#define HAVE_PTHREAD_PRIO_INHERIT 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Defines if strndup is available on your system */
#define HAVE_STRNDUP 1

/* Defines if strnlen is available on your system */
#define HAVE_STRNLEN 1

/* Define to 1 if you have the <syslog.h> header file. */
#define HAVE_SYSLOG_H 1

/* Define to 1 if you have the <sys/ioctl.h> header file. */
#define HAVE_SYS_IOCTL_H 1

/* Define to 1 if you have the <sys/socket.h> header file. */
#define HAVE_SYS_SOCKET_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/time.h> header file. */
#define HAVE_SYS_TIME_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to 1 if you have the `vprintf' function. */
#define HAVE_VPRINTF 1

/* Define to 1 if you have the <ws2tcpip.h> header file. */
/* #undef HAVE_WS2TCPIP_H */

/* see upnpconfig.h */
#define IXML_HAVE_SCRIPTSUPPORT 1

/* whether the system defaults to 32bit off_t but can do 64bit when requested
   */
/* #undef LARGEFILE_SENSITIVE */

/* Define to the sub-directory where libtool stores uninstalled libraries. */
#define LT_OBJDIR ".libs/"

/* Define to 1 to prevent compilation of assert() */
/* #undef NDEBUG */

/* Name of package */
#define PACKAGE "libupnp"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "mroberto@users.sourceforge.net"

/* Define to the full name of this package. */
#define PACKAGE_NAME "libupnp"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "libupnp 1.14.19"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "libupnp"

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION "1.14.19"

/* Define to necessary symbol if this constant uses a non-standard name on
   your system. */
/* #undef PTHREAD_CREATE_JOINABLE */

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* see upnpconfig.h */
#define UPNP_ENABLE_BLOCKING_TCP_CONNECTIONS 1

/* see upnpconfig.h */
#define UPNP_ENABLE_IPV6 1

/* see upnpconfig.h */
/* #undef UPNP_ENABLE_OPEN_SSL */

/* see upnpconfig.h */
/* #undef UPNP_ENABLE_POST_WRITE */

/* see upnpconfig.h */
/* #undef UPNP_ENABLE_UNSPECIFIED_SERVER */

/* see upnpconfig.h */
#define UPNP_HAVE_CLIENT 1

/* see upnpconfig.h */
#define UPNP_HAVE_DEBUG 1

/* see upnpconfig.h */
#define UPNP_HAVE_DEVICE 1

/* see upnpconfig.h */
#define UPNP_HAVE_GENA 1

/* see upnpconfig.h */
#define UPNP_HAVE_OPTSSDP 1

/* see upnpconfig.h */
#define UPNP_HAVE_SOAP 1

/* see upnpconfig.h */
#define UPNP_HAVE_SSDP 1

/* see upnpconfig.h */
#define UPNP_HAVE_TOOLS 1

/* see upnpconfig.h */
#define UPNP_HAVE_WEBSERVER 1

/* whether the system defaults to 32bit off_t but can do 64bit when requested
   */
/* #undef UPNP_LARGEFILE_SENSITIVE */

/* see upnpconfig.h */
/* #undef UPNP_MINISERVER_REUSEADDR */

/* Do not use pthread_rwlock_t */
#define UPNP_USE_RWLOCK 1

/* see upnpconfig.h */
#define UPNP_VERSION_MAJOR 1

/* see upnpconfig.h */
#define UPNP_VERSION_MINOR 14

/* see upnpconfig.h */
#define UPNP_VERSION_PATCH 19

/* see upnpconfig.h */
#define UPNP_VERSION_STRING "1.14.19"

/* Version number of package */
#define VERSION "1.14.19"

/* Define WORDS_BIGENDIAN to 1 if your processor stores words with the most
   significant byte first (like Motorola and SPARC, unlike Intel). */
#if defined AC_APPLE_UNIVERSAL_BUILD
# if defined __BIG_ENDIAN__
#  define WORDS_BIGENDIAN 1
# endif
#else
# ifndef WORDS_BIGENDIAN
/* #  undef WORDS_BIGENDIAN */
# endif
#endif

/* Enable large inode numbers on Mac OS X 10.5.  */
#ifndef _DARWIN_USE_64_BIT_INODE
# define _DARWIN_USE_64_BIT_INODE 1
#endif

/* Number of bits in a file offset, on hosts where this is settable. */
/* #undef _FILE_OFFSET_BITS */

/* Define to 1 to make fseeko visible on some hosts (e.g. glibc 2.2). */
/* #undef _LARGEFILE_SOURCE */

/* Define for large files, on AIX-style hosts. */
/* #undef _LARGE_FILES */

/* Define to empty if `const' does not conform to ANSI C. */
/* #undef const */

/* Define to `unsigned int' if <sys/types.h> does not define. */
/* #undef size_t */

/* Type for storing the length of struct sockaddr */
/* #undef socklen_t */

#ifndef MELP_DEFINES_H
#define MELP_DEFINES_H

#if !defined(MELP_EXPORT)
# if defined(__GNUC__)
#  define MELP_EXPORT __attribute__((visibility("default")))
# else
#  define MELP_EXPORT
# endif
#endif

#endif

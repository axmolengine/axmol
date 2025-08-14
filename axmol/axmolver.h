#pragma once

/* Define the axmol version */
// 0x00 HI ME LO
// 00   03 08 00
#define AX_VERSION_MAJOR 3
#define AX_VERSION_MINOR 0
#define AX_VERSION_PATCH 0

/* Define axmol version helper macros */
#define AX_VERSION_MAKE(a, b, c) ((a << 16) | (b << 8) | (c & 0xff))
#define AX_VERSION_NUM           AX_VERSION_MAKE(AX_VERSION_MAJOR, AX_VERSION_MINOR, AX_VERSION_PATCH)
#define AX_VERSION               AX_VERSION_NUM

/* Define whether axmol/.git present */
#define AX_GIT_PRESENT 1

/* Define to the library build number from git commit count */
#define AX_BUILD_NUM "4475"

/* Define the branch being built */
#define AX_GIT_BRANCH "source-folder-adjustment"

/* Define the hash of the head commit */
#define AX_GIT_COMMIT_HASH "9cb8cc8"

/* Define the axmol version string */
#define _AXSTR(R)      #R
#define _AXMACROSTR(R) _AXSTR(R)
#define AX_VERSION_STR _AXMACROSTR(AX_VERSION_MAJOR) "." _AXMACROSTR(AX_VERSION_MINOR) "." _AXMACROSTR(AX_VERSION_PATCH)

#if AX_GIT_PRESENT
#    define AX_VERSION_STR_FULL AX_VERSION_STR "-" AX_GIT_COMMIT_HASH
#else
#    define AX_VERSION_STR_FULL AX_VERSION_STR
#endif

/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

/**
 * A set of macros to configure the Cubism Framework on build.
 */

/**
 * A set of macros to use for debugging Cubism Framework.
 */
/** Runs Cubism Framework as debug mode. */
// #define CSM_DEBUG

/**
 * Tracks memory leaking of Cubism Framework.
 *
 * @note Leakage is verified at the termination of Cubism Framework.
 */
// #define CSM_DEBUG_MEMORY_LEAKING


/**
 * A set of macros to configure the logging level forcefully.
 *
 * @note The set logging level is applied by changing the definition of a group of macros that call logging in the CubismDebug class.
 */
/** Indicates that the logging of all is enabled. */
#define CSM_LOG_LEVEL_VERBOSE  0

/** Indicates that up to the logging level of Debug. */
#define CSM_LOG_LEVEL_DEBUG    1

/** Indicates that up to the logging level of Info. */
#define CSM_LOG_LEVEL_INFO     2

/** Indicates that up to the logging level of Warning. */
#define CSM_LOG_LEVEL_WARNING  3

/** Indicates that up to the logging level of Error. */
#define CSM_LOG_LEVEL_ERROR    4

/**  Indicates that the logging of all is disabled. */
#define CSM_LOG_LEVEL_OFF      5

/**
 * Logging level to set
 *
 * @note Set one of the logging levels from CSM_LOG_LEVEL_VERBOSE to CSM_LOG_LEVEL_OFF.
 */
#define CSM_LOG_LEVEL CSM_LOG_LEVEL_VERBOSE

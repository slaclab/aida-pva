/** @file
 *  @brief The Header File for uri and channel name manipulation functions.
 *       **CMS**=C_INC
 */
#ifndef aida_pva_uri_h
#define aida_pva_uri_h
#ifdef __cplusplus
extern "C" {
#endif

#include "aida_pva.h"

/**
 * The maximum length of the PMU part of a URI, includes primary, micro and unit strings with spaces between them
 */
#define MAX_PMU_LEN 18
/**
 * The maximum length of a URI
 */
#define MAX_URI_LEN 30
/**
 * The length of the primary part of a PMU string
 */
#define PRIM_LEN 4
/**
 * The length of the micro part of a PMU string
 */
#define MICRO_LEN 4

/**
 * Get a PMU (Primary-Micro-Unit) string from the supplied URI.
 *
 * @param _uri the uri to pull the pmu string from
 * @param _var the name of the variable to store the resulting pmu string
 */
#define PMU_STRING_FROM_URI(_var, _uri)  \
    char _var[MAX_PMU_LEN]; \
    pmuStringFromUri(_var, _uri);

/**
 * Get a slcName from the provided uri and store it in the given variable name.
 * A slcName is the URI with the last ':' converted to a dot
 *
 * @param _uri the uri to pull the slcName from
 * @param _var the name of the variable to store the resulting slcName in
 */
#define TO_SLC_NAME(_uri, _var)\
    char _var[MAX_URI_LEN]; \
    uriToSlcName(_var, _uri);

/**
 * Get a legacy AIDA name from the provided uri and store it in the given variable name.
 *
 * @param _uri the uri to pull the slcName from
 * @param _var the name of the variable to store the resulting slcName in
 */
#define TO_LEGACY_NAME(_uri, _var)\
    char _var[MAX_URI_LEN]; \
    uriLegacyName(_var, _uri);

/**
 * Get a display group name from the provided uri and store it in the given variable name.
 *
 * A display group name  is the the part of the URI before the last ':'
 *
 * @param _uri the uri to pull the dgroup from
 * @param _var the name of the variable to store the resulting dgroup in
 */
#define TO_DGROUP(_var, _uri) \
    char _var[MAX_URI_LEN]; \
    groupNameFromUri(_var, _uri);

#ifdef __cplusplus
}
#endif
#endif


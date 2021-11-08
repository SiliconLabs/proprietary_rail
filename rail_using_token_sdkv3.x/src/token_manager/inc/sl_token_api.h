/***************************************************************************//**
* @file sl_token_api.h
* @brief header file for token api
*******************************************************************************
* # License
* <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
*******************************************************************************
*
* SPDX-License-Identifier: Zlib
*
* The licensor of this software is Silicon Laboratories Inc.
*
* This software is provided \'as-is\', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
*
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
*
* 1. The origin of this software must not be misrepresented; you must not
*    claim that you wrote the original software. If you use this software
*    in a product, an acknowledgment in the product documentation would be
*    appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
*    misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*
*******************************************************************************
* # Experimental Quality
* This code has not been formally tested and is provided as-is. It is not
* suitable for production environments. In addition, this code will not be
* maintained and there may be no bug maintenance planned for these resources.
* Silicon Labs may update projects from time to time.
******************************************************************************/

#ifndef _TOKEN_API_H_
#define _TOKEN_API_H_

#ifndef PGM
#define PGM     const
#endif

typedef uint8_t EmberStatus;
#define EMBER_SUCCESS (0x00u)
#define EMBER_ERR_FATAL (0x01u)
#define EMBER_ERR_FLASH_PROG_FAIL (0x4Bu)

// NVM3KEY domain base keys
#define NVM3KEY_DOMAIN_USER    0x00000U
#define NVM3KEY_DOMAIN_CONNECT 0x30000U
#define NVM3KEY_DOMAIN_COMMON  0x80000U

#define DEFINE_BASIC_TOKEN(name, type, ...) \
  TOKEN_DEF(name, CREATOR_##name, 0, 0, type, 1, __VA_ARGS__)

#define DEFINE_COUNTER_TOKEN(name, type, ...) \
  TOKEN_DEF(name, CREATOR_##name, 1, 0, type, 1, __VA_ARGS__)

#define DEFINE_INDEXED_TOKEN(name, type, arraysize, ...) \
  TOKEN_DEF(name, CREATOR_##name, 0, 1, type, (arraysize), __VA_ARGS__)

#define DEFINE_FIXED_BASIC_TOKEN(name, type, address, ...) \
  TOKEN_NEXT_ADDRESS(name, (address))                      \
  TOKEN_DEF(name, CREATOR_##name, 0, 0, type, 1, __VA_ARGS__)

#define DEFINE_FIXED_COUNTER_TOKEN(name, type, address, ...) \
  TOKEN_NEXT_ADDRESS(name, (address))                        \
  TOKEN_DEF(name, CREATOR_##name, 1, 0, type, 1, __VA_ARGS__)

#define DEFINE_FIXED_INDEXED_TOKEN(name, type, arraysize, address, ...) \
  TOKEN_NEXT_ADDRESS(name, (address))                                   \
  TOKEN_DEF(name, CREATOR_##name, 0, 1, type, (arraysize), __VA_ARGS__)

#include "sl_token_manufacturing_api.h"

extern void token_manager_assert_handler(const char *filename, int linenumber);
#define assert(condition)  \
  do { \
    if (!(condition)) { \
      token_manager_assert_handler(__FILE__, __LINE__); \
    } \
  } while (0)

/**
 * @description Simple declarations of all of the token types so that they can
 * be referenced from anywhere in the code base.
 */
#define DEFINETYPES
// Multiple inclusion of unguarded token-related header files is by design; suppress violation.
//cstat !MISRAC2012-Dir-4.10
  #include SL_TOKEN_MANAGER_CUSTOM_TOKEN_HEADER
#undef DEFINETYPES

//-- Build parameter links
#define DEFINETOKENS

#undef TOKEN_DEF

/**
 * @description Enum for translating token defs into a number.  This number is
 * used as an index into the cache of token information the token system and
 * Simulated EEPROM hold. For NVM3 tokens this number is used as an index into
 * a table of NVM3 object keys used to identify the tokens.
 *
 * The special entry TOKEN_COUNT is always at the top of the enum, allowing
 * the token and sim-eeprom system to know how many tokens there are.
 *
 * @param name: The name of the token.
 */
#define TOKEN_DEF(name, creator, iscnt, isidx, type, arraysize, ...) \
  TOKEN_##name,
enum {
  // Multiple inclusion of unguarded token-related header files is by design; suppress violation.
  //cstat !MISRAC2012-Dir-4.10
    #include SL_TOKEN_MANAGER_CUSTOM_TOKEN_HEADER
  TOKEN_COUNT
};

#undef TOKEN_DEF

/**
 * @description Macro for translating token definitions into size variables.
 * This provides a convenience for abstracting the 'sizeof(type)' anywhere.
 *
 * @param name: The name of the token.
 *
 * @param type: The token type.  The types are found in token-stack.h.
 */
#define TOKEN_DEF(name, creator, iscnt, isidx, type, arraysize, ...) \
  TOKEN_##name##_SIZE = sizeof(type),
enum {
  // Multiple inclusion of unguarded token-related header files is by design; suppress violation.
  //cstat !MISRAC2012-Dir-4.10
    #include SL_TOKEN_MANAGER_CUSTOM_TOKEN_HEADER
};

#undef TOKEN_DEF

/**
 * @description External declaration of an array of NVM3 object keys.
 * Since the token systems identify tokens through an enum (see above
 * for the enum) and these two systems need to link NVM3 object keys to
 * their tokens, this array instantiates that link.
 *
 * @param key: The NVM3 object key type.  The keys are found in
 * token-stack.h.
 */
extern const uint32_t tokenNvm3Keys[];

/**
 * @description External declaration of an array of creator codes.  Since
 * the token and sim-eeprom systems identify tokens through an enum (see
 * above for the enum) and these two systems need to link creator codes to
 * their tokens, this array instantiates that link.
 *
 * @param creator: The creator code type.  The codes are found in
 * token-stack.h.
 */
extern const uint16_t tokenCreators[];

/**
 * @description External declaration of an array of IsCnt flags.  Since
 * the token and sim-eeprom systems identify tokens through an enum (see
 * below for the enum) and these two systems need to know which tokens
 * are counter tokens, this array provides that information.
 *
 * @param iscnt: The flag indicating if the token is a counter.  The iscnt's
 * are found in token-stack.h.
 */
extern const bool tokenIsCnt[];

/**
 * @description External declaration of an array of IsIdx flags.  Since
 * the token and sim-eeprom systems identify tokens through an enum (see
 * below for the enum) and these two systems need to know which tokens
 * are indexed tokens, this array provides that information.
 *
 * @param iscnt: The flag indicating if the token is an indexed token.
 * The isidx's are found in token-stack.h.
 */
extern const bool tokenIsIdx[];

/**
 * @description External declaration of an array of sizes.  Since
 * the token and sim-eeprom systems identify tokens through an enum (see
 * below for the enum) and these two systems need to know the size of each
 * token, this array provides that information.
 *
 * @param type: The token type.  The types are found in token-stack.h.
 */
extern const uint8_t tokenSize[];

/**
 * @description External declaration of an array of array sizes.  Since
 * the token and sim-eeprom systems identify tokens through an enum (see
 * below for the enum) and these two systems need to know the array size of
 * each token, this array provides that information.
 *
 * @param arraysize: The array size.
 */
extern const uint8_t tokenArraySize[];

/**
 * @description External declaration of an array of all token default values.
 * This array is filled with pointers to the set of constant declarations of
 * all of the token default values.  Therefore, the index into this array
 * chooses which token's defaults to access, and the address offset chooses the
 * byte in the defaults to use.
 *
 * For example, to get the n-th byte of the i-th token, use:
 * uint8_t byte = *(((uint8_t *)tokenDefaults[i])+(n)
 *
 * @param TOKEN_##name##_DEFAULTS: A constant declaration of the token default
 * values, generated for all tokens.
 */
extern const void * const tokenDefaults[];

/**
 * @description A define for the token and Simulated EEPROM system that
 * specifies, in bytes, the space allocated to a counter token for
 * +1 marks.  The number of +1 marks varies between chips based on the
 * minimum write granularity for a chip's flash.  EM35x chips can use 8bit
 * per +1 while EFM32/EZM32 chips use 16bit per +1.
 */
#define COUNTER_TOKEN_PAD        50

/**
 * @description Macro for typedef'ing the CamelCase token type found in
 * token-stack.h to a capitalized TOKEN style name that ends in _TYPE.
 * This macro allows other macros below to use 'token##_TYPE' to declare
 * a local copy of that token.
 *
 * @param name: The name of the token.
 *
 * @param type: The token type.  The types are found in token-stack.h.
 */
#define TOKEN_DEF(name, creator, iscnt, isidx, type, arraysize, ...) \
  typedef type TOKEN_##name##_TYPE;
// Multiple inclusion of unguarded token-related header files is by design; suppress violation.
//cstat !MISRAC2012-Dir-4.10
  #include SL_TOKEN_MANAGER_CUSTOM_TOKEN_HEADER
#undef TOKEN_DEF

#undef DEFINETOKENS


#define halCommonGetToken(data, token) \
  sl_token_get_data(token, 0x7F, data, token##_SIZE)

#define halCommonGetIndexedToken(data, token, index) \
  sl_token_get_data(token, index, data, token##_SIZE)

#define halCommonSetToken(token, data) \
  sl_token_set_data(token, 0x7F, data, token##_SIZE)

#define halCommonSetIndexedToken(token, index, data) \
  sl_token_set_data(token, index, data, token##_SIZE)

#define halCommonIncrementCounterToken(token) \
  sl_token_increment_counter(token);

#endif // _TOKEN_API_H_

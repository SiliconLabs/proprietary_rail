/***************************************************************************//**
* @file sl_token_manager.h
* @brief token manager api
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


/***************************************************************************//**
 * @addtogroup token_manager Token Manager
 * @{
 ******************************************************************************/

#ifndef TOKEN_MANAGER_H
#define TOKEN_MANAGER_H

#include <stdbool.h>
#include "ecode.h"
#if defined(USE_NVM3)
#include "nvm3.h"
#endif // defined(USE_NVM3)

#include "sl_token_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * Initialize the Token Manager.
 *
 * @note This function must be called before any other sl_token functions. If
 * it does not return success, the other sl_token functions will not work.
 *
 * @return 0 if successful. Error code otherwise.
 ******************************************************************************/
Ecode_t sl_token_init(void);

/***************************************************************************//**
 * Read the data stored in the specified data or manufacturing token.
 *
 * @param token The NVM3KEY or SimEE CREATOR define for the token.
 * @param index The index to access in the indexed token.
 * @param data A pointer to where the token data should be placed.
 * @param length The size of the token data in number of bytes.
 *
 * @return 0 if successful. Error code otherwise.
 ******************************************************************************/
Ecode_t sl_token_get_data(uint32_t token,
                          uint32_t index,
                          void *data,
                          uint32_t length);

/***************************************************************************//**
 * Read the data stored in or return a pointer to the specified data or
 * manufacturing token.
 *
 * @param token The NVM3KEY or SimEE CREATOR define for the token.
 * @param index The index to access in the indexed token.
 * @param pointer A pointer to where the token data should be placed or a
 *                pointer to where the data can be read.
 * @param length The size of the token data in number of bytes.
 *
 * @return 0 if successful. Error code otherwise.
 ******************************************************************************/
Ecode_t sl_token_get_pointer_or_data(uint32_t token,
                                     uint8_t index,
                                     void *pointer,
                                     uint32_t length);

/***************************************************************************//**
 * Writes the data to the specified token.
 *
 * @param token The NVM3KEY or SimEE CREATOR define for the token.
 * @param index The index to access in the indexed token.
 * @param data A pointer to the data being written.
 * @param length The size of the token data in number of bytes.
 *
 * @return 0 if successful. Error code otherwise.
 ******************************************************************************/
Ecode_t sl_token_set_data(uint32_t token,
                          uint32_t index,
                          void *data,
                          uint32_t length);

/***************************************************************************//**
 * Increments the value of a counter token.  This call does not support
 * manufacturing tokens.
 *
 * @param token The NVM3KEY or SimEE CREATOR define for the token.
 *
 * @return 0 if successful. Error code otherwise.
 ******************************************************************************/
Ecode_t sl_token_increment_counter(uint32_t token);

/***************************************************************************//**
 * Read the data associated with the specified manufacturing token.
 *
 * @param token The NVM3KEY or SimEE CREATOR define for the token.
 * @param index The index to access in the indexed token.
 * @param data A pointer to where the token data should be placed.
 * @param length The size of the token data in number of bytes.
 *
 * @return 0 if successful. Error code otherwise.
 ******************************************************************************/
Ecode_t sl_token_get_manufacturing_data(uint32_t token,
                                        uint32_t index,
                                        void *data,
                                        uint32_t length);

/***************************************************************************//**
 * Writes data to a manufacturing token.
 *
 * @note Only manufacturing token values that have not been written since
 * the last erase can be written.  For areas of flash that cannot be erased
 * by user code, those manufacturing tokens are effectively write-once.
 *
 * @param token The NVM3KEY or SimEE CREATOR define for the token.
 * @param data A pointer to the data being written.
 * @param length The size of the token data in number of bytes.
 *
 * @return 0 if successful. Error code otherwise.
 ******************************************************************************/
Ecode_t sl_token_set_manufacturing_data(uint32_t token,
                                        void *data,
                                        uint32_t length);

/** @} end token_manager */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/***************************************************************************//**
 * Internal assert failed function.
 *
 * @note Minimal assert functionality to support token manager asserts.  In
 * the future a fully formed assert component will become available and make
 * this assert code obsolete.
 *
 * @param filename
 * @param linenumber
 ******************************************************************************/
void halInternalAssertFailed(const char * filename, int linenumber);

/**
 * @description External declaration of an array of NVM3 object keys.
 * Since the token systems identify tokens through an enum (see above
 * for the enum) and these two systems need to link NVM3 object keys to
 * their tokens, this array instantiates that link.
 *
 * Provides the "key" parameter: The NVM3 object key type.  The keys are found
 * in token-stack.h.
 */
extern const uint32_t tokenNvm3Keys[];

/**
 * @description External declaration of an array of creator codes.  Since
 * the token and sim_eeprom systems identify tokens through an enum (see
 * above for the enum) and these two systems need to link creator codes to
 * their tokens, this array instantiates that link.
 *
 * Provides the "creator" parameter: The creator code type.  The codes are
 * found in token-stack.h.
 */
extern const uint16_t tokenCreators[];

/**
 * @description External declaration of an array of IsCnt flags.  Since
 * the token and sim_eeprom systems identify tokens through an enum (see
 * below for the enum) and these two systems need to know which tokens
 * are counter tokens, this array provides that information.
 *
 * Provides the "iscnt" parameter: The flag indicating if the token is a
 * counter.  The iscnt's are found in token-stack.h.
 */
extern const bool tokenIsCnt[];

/**
 * @description External declaration of an array of IsIdx flags.  Since
 * the token and sim_eeprom systems identify tokens through an enum (see
 * below for the enum) and these two systems need to know which tokens
 * are indexed tokens, this array provides that information.
 *
 * Provides the "isidx" parameter: The flag indicating if the token is an
 * indexed token. The isidx's are found in token-stack.h.
 */
extern const bool tokenIsIdx[];

/**
 * @description External declaration of an array of sizes.  Since
 * the token and sim_eeprom systems identify tokens through an enum (see
 * below for the enum) and these two systems need to know the size of each
 * token, this array provides that information.
 *
 * Provides the "type" parameter: The token type.  The types are found
 * in token-stack.h.
 */
extern const uint8_t tokenSize[];

/**
 * @description External declaration of an array of array sizes.  Since
 * the token and sim_eeprom systems identify tokens through an enum (see
 * below for the enum) and these two systems need to know the array size of
 * each token, this array provides that information.
 *
 * Provides the "arraysize" parameter: The array size.
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
 * Provides the "TOKEN_##name##_DEFAULTS" parameter: A constant declaration
 * of the token default values, generated for all tokens.
 */
extern const void * const tokenDefaults[];

#endif // DOXYGEN_SHOULD_SKIP_THIS

#ifdef __cplusplus
}
#endif

// ******** THE REST OF THE FILE IS DOCUMENTATION ONLY !***********************
/// @addtogroup token_manager Token Manager
/// @{
/// @brief Routines for working with tokens
///
///   @details
///
///   @li @ref token_manager_intro
///
/// @n @section token_manager_intro Introduction
///
/// The token system stores such non-volatile information as the manufacturing
/// ID, channel number, transmit power, and various pieces of information
/// that the application needs to be persistent between device power cycles.
/// The token system is design to abstract implementation details and simplify
/// interacting with differing non-volatile systems.  The majority of tokens
/// are stored in NVM3 or Simulated EEPROM (both use Flash) where they can be
/// rewritten. Manufacturing tokens are stored in dedicated regions of flash
/// and are not designed to be rewritten.
///
/// By default the Token Manager pulls in the NVM3 component for storage.  If
/// the SimEE1 or SimEE2 component is added into the project along side the
/// Token Manager, then SimEE1 or SimEE2 will be used for storage.  If the
/// Sim EEPROM 2 to NVM3 Upgrade component is added to the project along side
/// the Token Manager, then the upgrade will run at startup to transition
/// as much stored data as possible into NVM3 format.  When using the upgrade
/// component don't include NVM3 or SimEE2 components.  Once an upgrade
/// is run, it would be possible to use stock Token Manager since Token Manager
/// defaults to NVM3.
///
/// @} end token_manager ******************************************************/

#endif /* TOKEN_MANAGER_H */

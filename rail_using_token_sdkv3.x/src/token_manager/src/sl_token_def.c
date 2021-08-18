/***************************************************************************//**
* @file sl_token_def.c
*******************************************************************************
* # License
* <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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


#include <stdint.h>
#include <stdbool.h>
#include "sl_token_api.h"

//-- Build parameter links
#define DEFINETOKENS
#define TOKEN_MFG(name, creator, iscnt, isidx, type, arraysize, ...)

#define TOKEN_DEF(name, creator, iscnt, isidx, type, arraysize, ...) \
  NVM3KEY_##name,
const uint32_t tokenNvm3Keys[] = {
#if defined (USE_NVM3)
  // Multiple inclusion of unguarded token-related header files is by design; suppress violation.
  //cstat !MISRAC2012-Dir-4.10
  #include SL_TOKEN_MANAGER_CUSTOM_TOKEN_HEADER
#else
  0xFFFFFFFFUL // Dummy
#endif
};

#undef TOKEN_DEF

#define TOKEN_DEF(name, creator, iscnt, isidx, type, arraysize, ...) \
  creator,
const uint16_t tokenCreators[] = {
#if !defined (USE_NVM3) || defined (SIMEE2_TO_NVM3_UPGRADE)
  // Multiple inclusion of unguarded token-related header files is by design; suppress violation.
  //cstat !MISRAC2012-Dir-4.10
  #include SL_TOKEN_MANAGER_CUSTOM_TOKEN_HEADER
#else
  0xFFFFUL   // Dummy
#endif
};
#undef TOKEN_DEF

#define TOKEN_DEF(name, creator, iscnt, isidx, type, arraysize, ...) \
  iscnt,
const bool tokenIsCnt[] = {
  // Multiple inclusion of unguarded token-related header files is by design; suppress violation.
  //cstat !MISRAC2012-Dir-4.10
    #include SL_TOKEN_MANAGER_CUSTOM_TOKEN_HEADER
};
#undef TOKEN_DEF

#define TOKEN_DEF(name, creator, iscnt, isidx, type, arraysize, ...) \
  isidx,
const bool tokenIsIdx[] = {
#if defined (USE_NVM3)
  // Multiple inclusion of unguarded token-related header files is by design; suppress violation.
  //cstat !MISRAC2012-Dir-4.10
    #include SL_TOKEN_MANAGER_CUSTOM_TOKEN_HEADER
#else
  false // Dummy
#endif
};
#undef TOKEN_DEF

#define TOKEN_DEF(name, creator, iscnt, isidx, type, arraysize, ...) \
  sizeof(type),
const uint8_t tokenSize[] = {
  // Multiple inclusion of unguarded token-related header files is by design; suppress violation.
  //cstat !MISRAC2012-Dir-4.10
    #include SL_TOKEN_MANAGER_CUSTOM_TOKEN_HEADER
};
#undef TOKEN_DEF

#define TOKEN_DEF(name, creator, iscnt, isidx, type, arraysize, ...) \
  arraysize,
const uint8_t tokenArraySize[] = {
  // Multiple inclusion of unguarded token-related header files is by design; suppress violation.
  //cstat !MISRAC2012-Dir-4.10
    #include SL_TOKEN_MANAGER_CUSTOM_TOKEN_HEADER
};
#undef TOKEN_DEF

//These set of DEFAULT token values are only used in the tokenDefaults array
//below.

// -------------------------------------------------------------------------
// MISRAC2012-Rule-20.7 doesn't allow for this use of concatenation within
// a macro expansion. Moreover, since TOKEN_DEF is only being used here as a
// static declaration of token default values, there's no risk that it will
// interfere with any other logic.
// -------------------------------------------------------------------------
#define TOKEN_DEF(name, creator, iscnt, isidx, type, arraysize, ...) \
  const type TOKEN_##name##_DEFAULTS = __VA_ARGS__;
// Multiple inclusion of unguarded token-related header files is by design; suppress violation.
//cstat !MISRAC2012-Dir-4.10
  #include SL_TOKEN_MANAGER_CUSTOM_TOKEN_HEADER
#undef TOKEN_DEF
// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

#define TOKEN_DEF(name, creator, iscnt, isidx, type, arraysize, ...) \
  ((void *)&TOKEN_##name##_DEFAULTS),
const void * const tokenDefaults[] = {
  // Multiple inclusion of unguarded token-related header files is by design; suppress violation.
  //cstat !MISRAC2012-Dir-4.10
    #include SL_TOKEN_MANAGER_CUSTOM_TOKEN_HEADER
};
#undef TOKEN_DEF

#undef DEFINETOKENS

#undef TOKEN_MFG

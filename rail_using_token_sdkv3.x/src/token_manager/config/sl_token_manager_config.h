/***************************************************************************//**
* @file sl_token_manager_config.h
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

// <<< Use Configuration Wizard in Context Menu >>>

#ifndef SL_TOKEN_MANAGER_CONFIG_H
#define SL_TOKEN_MANAGER_CONFIG_H

// <h> TOKEN MANAGER Configurations

// <e SL_TOKEN_MANAGER_CUSTOM_TOKENS_PRESENT> Enable Custom Tokens
// <i> Default: 0
#define SL_TOKEN_MANAGER_CUSTOM_TOKENS_PRESENT  1

// <s SL_TOKEN_MANAGER_CUSTOM_TOKEN_HEADER> File containing custom tokens
// <i> Default: "sl_custom_token_header.h"
// <i> Header file containing custom tokens located at project_root/config
#define SL_TOKEN_MANAGER_CUSTOM_TOKEN_HEADER "sl_custom_token_header.h"

// </e>

// <e SL_TOKEN_MANAGER_CUSTOM_MANUFACTURING_TOKENS_PRESENT> Enable Custom Manufacturing Tokens
// <i> Default: 0
#define SL_TOKEN_MANAGER_CUSTOM_MANUFACTURING_TOKENS_PRESENT  0

// <s SL_TOKEN_MANAGER_CUSTOM_MANUFACTURING_TOKEN_HEADER> File containing custom manufacturing tokens
// <i> Default: "sl_custom_manufacturing_token_header.h"
// <i> Header file containing custom tokens located at project_root/config
#define SL_TOKEN_MANAGER_CUSTOM_MANUFACTURING_TOKEN_HEADER "sl_custom_manufacturing_token_header.h"

// </e>
// </h>

#if (SL_TOKEN_MANAGER_CUSTOM_MANUFACTURING_TOKENS_PRESENT)
#define APPLICATION_MFG_TOKEN_HEADER SL_TOKEN_MANAGER_CUSTOM_MANUFACTURING_TOKEN_HEADER
#endif // SL_TOKEN_MANAGER_CUSTOM_MANUFACTURING_TOKENS_PRESENT

#endif // SL_TOKEN_MANAGER_CONFIG_H

// <<< end of configuration section >>>

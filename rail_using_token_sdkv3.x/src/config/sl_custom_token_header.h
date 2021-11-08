/***************************************************************************//**
* @file sl_custom_token_header.h
* @brief define custom tokens
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
 * Custom Tokens Usage Examples
 *
 * #define BASICTOKEN1_DEFAULT                                            0xA5F0
 * #define BASICTOKEN2_DEFAULT                  { { 0xAA, 0xBB, 0xCC }, 0xDDDD }
 * #define BASICTOKEN3_DEFAULT                                         { { 0 } }
 * #define COUNTERTOKEN1_DEFAULT                                               0
 * #define COUNTERTOKEN2_DEFAULT                                      0xCCCCCCCC
 * #define INDEXEDTOKEN1_DEFAULT                                               0
 * #define INDEXEDTOKEN2_DEFAULT                                      { 0xDDDD }
 * #define INDEXEDTOKEN3_DEFAULT    { 0xAA, 0xBBBB, { 0x00, 0x11, 0x22 }, 0xCC }
 *
 * #ifdef DEFINETYPES
 * typedef uint16_t tokTypeBasicToken1;
 * typedef struct {
 *   uint8_t basicToken2Array[3];
 *   uint16_t basicToken2VarA;
 * } tokTypeBasicToken2;
 *
 * typedef struct {
 *   uint8_t basicToken3Array[254];
 * } tokTypeBasicToken3;
 *
 * typedef uint32_t tokTypeCounterToken1;
 * typedef uint32_t tokTypeCounterToken2;
 *
 * typedef uint8_t  tokTypeIndexedToken1Element;
 * typedef uint16_t tokTypeIndexedToken2Element;
 *
 * typedef struct {
 *   uint8_t indexedToken3VarA;
 *   uint16_t indexedToken3VarB;
 *   uint8_t indexedToken3Array[3];
 *   int8_t indexedToken3VarC;
 * } tokTypeIndexedToken3Element;
 *
 * #endif
 *
 * #ifdef DEFINETOKENS
 * DEFINE_BASIC_TOKEN(BASICTOKEN1,
 *                    tokTypeBasicToken1,
 *                    BASICTOKEN1_DEFAULT)
 * DEFINE_BASIC_TOKEN(BASICTOKEN2,
 *                    tokTypeBasicToken2,
 *                    BASICTOKEN2_DEFAULT)
 * DEFINE_BASIC_TOKEN(BASICTOKEN3,
 *                    tokTypeBasicToken3,
 *                    BASICTOKEN3_DEFAULT)
 * DEFINE_COUNTER_TOKEN(COUNTERTOKEN1,
 *                      tokTypeCounterToken1,
 *                      COUNTERTOKEN1_DEFAULT)
 * DEFINE_COUNTER_TOKEN(COUNTERTOKEN2,
 *                      tokTypeCounterToken2,
 *                      COUNTERTOKEN2_DEFAULT)
 * DEFINE_INDEXED_TOKEN(INDEXEDTOKEN1,
 *                      tokTypeIndexedToken1Element,
 *                      INDEXEDTOKEN1_ELEMENTS,
 *                      INDEXEDTOKEN1_DEFAULT)
 * DEFINE_INDEXED_TOKEN(INDEXEDTOKEN2,
 *                      tokTypeIndexedToken2Element,
 *                      INDEXEDTOKEN2_ELEMENTS,
 *                      INDEXEDTOKEN2_DEFAULT)
 * DEFINE_INDEXED_TOKEN(INDEXEDTOKEN3,
 *                      tokTypeIndexedToken3Element,
 *                      INDEXEDTOKEN3_ELEMENTS,
 *                      INDEXEDTOKEN3_DEFAULT)
 * #endif
 *
 ******************************************************************************/

#define NVM3KEY_BASIC_DEMO                                    (NVM3KEY_DOMAIN_USER | 0x0001)
#define NVM3KEY_COUNTER_DEMO                                  (NVM3KEY_DOMAIN_USER | 0x0002)

#define NVM3KEY_INDEX_DEMO                                    (NVM3KEY_DOMAIN_USER | 0x0100)
#define INDEX_DEMO_SIZE                                        32

#ifdef DEFINETYPES

/* define data type for the basic demo */
typedef uint16_t tokTypeBasicDemo;

/* define data type for the counter demo*/
typedef uint32_t tokTypeCounterDemo;

/* define data type for the index demo*/
typedef struct {
    uint8_t  field1;
    uint8_t  field2;
    uint8_t  field3;
    uint8_t  field4;
} tokTypeIndexDemo;
#endif


#ifdef DEFINETOKENS

/* define 'BASIC_DEMO' token */
DEFINE_BASIC_TOKEN(BASIC_DEMO,
                   tokTypeBasicDemo,
                   {0})

/* define 'COUNTER_DEMO' token */
DEFINE_COUNTER_TOKEN(COUNTER_DEMO,
                     tokTypeCounterDemo,
                     {0})

/* define 'INDEX_DEMO' token */   
DEFINE_INDEXED_TOKEN(INDEX_DEMO,
                     tokTypeIndexDemo,
                     INDEX_DEMO_SIZE,
                     {0,0,0,0})                                     
#endif
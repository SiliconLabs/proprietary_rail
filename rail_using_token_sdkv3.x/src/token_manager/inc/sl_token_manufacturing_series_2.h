/***************************************************************************//**
* @file sl_token_manufactring_series_2.h
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

// A header inclusion guard is not used since token definition files
// require being included multipled times in different contexts with
// different controlling defines.

/**
 * @name Convenience Macros
 * @brief The following convenience macros are used to simplify the definition
 * process for commonly specified parameters to the basic TOKEN_DEF macro.
 * Please see hal/micro/token.h for a more complete explanation.
 *@{
 */
#define DEFINE_MFG_TOKEN(name, type, address, ...) \
  TOKEN_NEXT_ADDRESS(name, (address))              \
  TOKEN_MFG(name, CREATOR_##name, 0, 0, type, 1, __VA_ARGS__)

/** @} END Convenience Macros */

#ifndef TOKEN_NEXT_ADDRESS
  #define TOKEN_NEXT_ADDRESS(region, address)
#endif

//////////////////////////////////////////////////////////////////////////////
// MANUFACTURING DATA
// *the addresses of these tokens must not change*

// MANUFACTURING CREATORS
// The creator codes are here in one list instead of next to their token
// definitions so comparision of the codes is easier.  The only requirement
// on these creator definitions is that they all must be unique.  A favorite
// method for picking creator codes is to use two ASCII characters in order
// to make the codes more memorable.  Also, the msb of Stack and Manufacturing
// token creator codes is always 1, while the msb of Application token creator
// codes is always 0.  This distinction allows Application tokens
// to freely use the lower 15 bits for creator codes without the risk of
// duplicating a Stack or Manufacturing token creator code.
//--- User Data ---
#define CREATOR_MFG_EMBER_EUI_64                          0xE545 // msb+'e'+'E'
#define CREATOR_MFG_CUSTOM_EUI_64                         0xE345
#define CREATOR_MFG_CUSTOM_VERSION                        0xC356
#define CREATOR_MFG_STRING                                0xED73
#define CREATOR_MFG_BOARD_NAME                            0xC24E // msb+'B'+'N' (Board Name)
#define CREATOR_MFG_MANUF_ID                              0xC944 // msb+'I'+'D' (Id)
#define CREATOR_MFG_PHY_CONFIG                            0xD043 // msb+'P'+'C' (Phy Config)
#define CREATOR_MFG_ASH_CONFIG                            0xC143 // msb+'A'+'C' (ASH Config)
#define CREATOR_MFG_SYNTH_FREQ_OFFSET                     0xD346 // msb+'S'+'F' (Synth Freq)
#define CREATOR_MFG_CCA_THRESHOLD                         0xC343 // msb+'C'+'C' (Clear Channel)
#define CREATOR_MFG_EZSP_STORAGE                          0xCD53
#define CREATOR_MFG_XO_TUNE                               0xD854 // msb+'X'+'T' (XO TUNE Value)
#define CREATOR_MFG_ZWAVE_COUNTRY_FREQ                    0xDA43 // msb+'Z'+'C' (ZWave Country)
#define CREATOR_MFG_ZWAVE_HW_VERSION                      0xDA48 // msb+'Z'+'H' (ZWave Hardware)
#define CREATOR_MFG_ZWAVE_PSEUDO_RANDOM_NUMBER            0xDA52 // msb+'Z'+'R' (ZWave Random number)
#define CREATOR_MFG_SERIAL_NUMBER                         0xD34E // msb+'S'+'E' (Serial Number)
#define CREATOR_MFG_LFXO_TUNE                             0xCC54 // msb+'L'+'T' (LFXO TUNE Value)
#define CREATOR_MFG_CTUNE                                 0xC354 // msb+'C'+'T' (CTUNE Value)
#define CREATOR_MFG_KIT_SIGNATURE                         0xCB53 // msb+'K'+'S' (Kit Signature)
#define CREATOR_MFG_EUI_64                                0xB634

//--- Lock Bits ---
#define CREATOR_MFG_LOCKBITS_PLW                          0xd057 // msb+'P'+'W' (Page Lock Word)
#define CREATOR_MFG_LOCKBITS_CLW0                         0xc357 // msb+'C'+'W' (Config Lock Word)
#define CREATOR_MFG_LOCKBITS_MLW                          0xcd57 // msb+'M'+'W' (Mass Erase Lock Word)
#define CREATOR_MFG_LOCKBITS_ULW                          0xd557 // msb+'U'+'W' (User Page Lock Word)
#define CREATOR_MFG_LOCKBITS_DLW                          0xc457 // msb+'P'+'W' (Debug Lock Word)

//--- Lock Bits Data ---
#define CREATOR_MFG_CBKE_DATA                             0xC342 // msb+'C'+'B' (CBke)
#define CREATOR_MFG_SECURITY_CONFIG                       0xD343 // msb+'S'+'C' (Security Config)
#define CREATOR_MFG_INSTALLATION_CODE                     0xC943 // msb+'I'+'C' (Installation Code)
#define CREATOR_MFG_SECURE_BOOTLOADER_KEY                 0xD342 // msb+'S'+'B' (Secure Bootloader)
#define CREATOR_MFG_CBKE_283K1_DATA                       0xC345 // msb+'C'+'B' (CBke) 283k1 ECC (a.k.a. SE 1.2)
#define CREATOR_MFG_BOOTLOAD_AES_KEY                      0xC24B // msb+'B'+'K' (Bootloader Key)
#define CREATOR_MFG_SIGNED_BOOTLOADER_KEY_X               0xD358 // msb+'S'+'X' (Signing key X)
#define CREATOR_MFG_SIGNED_BOOTLOADER_KEY_Y               0xD359 // msb+'S'+'Y' (Signing key Y)
#define CREATOR_MFG_THREAD_JOIN_KEY                       0xCA4B // msb+'J'+'K' (Join Key)
#define CREATOR_MFG_NVM3_CRYPTO_KEY                       0xCE4B // msb+'N'+'K' (NVM3 Key)
#define CREATOR_MFG_ZW_PRK                                0xDA50 // msb+'Z'+'P' (Zwave Private Key)
#define CREATOR_MFG_ZW_PUK                                0xDA55 // msb+'Z'+'U' (Zwave pUblic Key)
#define CREATOR_MFG_ZW_QR_CODE                            0xDA51 // msb+'Z'+'Q' (Zwave QR Code)
#define CREATOR_MFG_ZW_INITIALIZED                        0xDA49 // msb+'Z'+'I' (Zwave Initialized)
#define CREATOR_MFG_ZW_QR_CODE_EXT                        0xDA45 // msb+'Z'+'E' (Zwave qr code Extension)

// Defines indicating the verions number these definitions work with.
#define CURRENT_MFG_CUSTOM_VERSION 0x01FE //MSB is version, LSB is complement

#ifdef DEFINETYPES
//--- User Data ---
typedef uint8_t tokTypeMfgEmberEui64[8];
typedef uint8_t tokTypeMfgCustomEui64[8];
typedef uint16_t tokTypeMfgCustomVersion;
typedef uint8_t tokTypeMfgString[16];
typedef uint8_t tokTypeMfgBoardName[16];
typedef uint16_t tokTypeMfgManufId;
typedef uint16_t tokTypeMfgPhyConfig;
typedef uint16_t tokTypeMfgAshConfig;
typedef uint16_t tokTypeMfgSynthFreqOffset;
typedef uint16_t tokTypeMfgCcaThreshold;
typedef uint8_t tokTypeMfgEzspStorage[8];
typedef uint16_t tokTypeMfgXoTune;
typedef uint8_t tokTypeMfgZwaveCountryFreq;
typedef uint8_t tokTypeMfgZwaveHardwareVersion;
typedef uint8_t tokTypeMfgZwavePseudoRandomNumber[16];
typedef uint8_t tokTypeMfgSerialNumber[16];
typedef uint8_t tokTypeMfgLfxoTune;
typedef uint16_t tokTypeMfgCTune;
typedef uint8_t tokTypeMfgKitSignature[4];
typedef uint8_t tokTypeMfgEui64[8];

//--- Lock Bits ---
// Main block Page Lock Words (PLWs)
typedef uint32_t tokTypeLockBitsPlw[4];
// Configuration Word Zero (CLW0)
typedef uint32_t tokTypeLockBitsClw0;
// Mass erase Lock Word (MLW)
typedef uint32_t tokTypeLockBitsMlw;
// User data page Lock Word (ULWs)
typedef uint32_t tokTypeLockBitsUlw;
// Debug Lock Word (DLW)
typedef uint32_t tokTypeLockBitsDlw;

//--- Lock Bits Data ---
// Smart Energy 1.0 (ECC 163k1 based crypto - 80-bit symmetric equivalent)
typedef struct {
  uint8_t certificate[48];
  uint8_t caPublicKey[22];
  uint8_t privateKey[21];
  // The bottom flag bit is 1 for uninitialized, 0 for initialized.
  // The other flag bits should be set to 0 at initialization.
  uint8_t flags;
} tokTypeMfgCbkeData;
typedef uint16_t tokTypeMfgSecurityConfig;
typedef struct {
  // The bottom flag bit is 1 for uninitialized, 0 for initialized.
  // Bits 1 and 2 give the size of the value string:
  // 0 = 6 bytes, 1 = 8 bytes, 2 = 12 bytes, 3 = 16 bytes.
  // The other flag bits should be set to 0 at initialization.
  // Special flags support.  Due to a bug in the way some customers
  // had programmed the flags field, we will also examine the upper
  // bits 9 and 10 for the size field.  Those bits are also reserved.
  uint16_t flags;
  uint8_t value[16];
  uint16_t crc;
} tokTypeMfgInstallationCode;
typedef struct {
  uint8_t data[16];  // AES-128 key
} tokTypeMfgSecureBootloaderKey;
// Smart Energy 1.2 (ECC 283k1 based crypto - 128-bit symmetric equivalent)
typedef struct {
  uint8_t certificate[74];
  uint8_t caPublicKey[37];
  uint8_t privateKey[36];
  // The bottom flag bit is 1 for uninitialized, 0 for initialized.
  // The other flag bits should be set to 0 at initialization.
  uint8_t flags;
} tokTypeMfgCbke283k1Data;
typedef uint8_t tokTypeMfgBootloadAesKey[16];
typedef struct {
  uint8_t data[32];  // ECDSA key point on P256 curve
} tokTypeMfgSignedBootloaderKeyX;
typedef struct {
  uint8_t data[32];  // ECDSA key point on P256 curve
} tokTypeMfgSignedBootloaderKeyY;
// Network join key with max length of 32 bytes
typedef struct {
  uint8_t joinKey[32];
  uint16_t joinKeyLength;
} tokTypeMfgThreadJoinKey;
typedef uint8_t tokTypeMfgNvm3CryptoKey[16];
typedef uint8_t tokTypeMfgZwavePrivateKey[32];
typedef uint8_t tokTypeMfgZwavePublicKey[32];
typedef uint8_t tokTypeMfgQRCode[90];
typedef uint8_t tokTypeMfgZwaveInitialized[1];

#endif //DEFINETYPES

#ifdef DEFINETOKENS
//The Manufacturing tokens need to be stored at well-defined locations.
//None of these addresses should ever change without extremely great care.
//Any _LOCATION <256 will dispatch to SimEE by halInternalGetTokenData.

//NOTE: On the EFM32 platform the EMBER_EUI64_TOKEN is taken from the
//      DEVINFO info UNIQUEL and UNIQUEH register.

//NOTE: On series 2 devices addresses must be 4 byte aligned.
//      Any value with a size not 4 byte aligned must be followed by
//      enough buffer space to allow the next value to start at a 4 byte
//      aligned address.

//--- User Data ---
//User Data is mapped to 2kB at USERDATA_BASE (0x0FE00000-0x0FE007FF).
//Any _LOCATION <256 will dispatch to SimEE by halInternalGetTokenData.
//Overloading the location with 0x1000 will keep the token out of SimEE and
//indicate it is a token living in User Data space.
//0x1000 is stripped off by halInternalGetMfgTokenData() to get the correct
//UserData offset.
#define USERDATA_TOKENS                    0x1000
#define MFG_EMBER_EUI_64_LOCATION               (USERDATA_TOKENS | 0x1f0)  //   8 bytes
#define MFG_CUSTOM_EUI_64_LOCATION              (USERDATA_TOKENS | 0x002)  //   8 bytes
#define MFG_CUSTOM_VERSION_LOCATION             (USERDATA_TOKENS | 0x00C)  //   2 bytes
#define MFG_STRING_LOCATION                     (USERDATA_TOKENS | 0x010)  //  16 bytes
#define MFG_BOARD_NAME_LOCATION                 (USERDATA_TOKENS | 0x020)  //  16 bytes
#define MFG_MANUF_ID_LOCATION                   (USERDATA_TOKENS | 0x030)  //   2 bytes
#define MFG_PHY_CONFIG_LOCATION                 (USERDATA_TOKENS | 0x034)  //   2 bytes
#define MFG_ASH_CONFIG_LOCATION                 (USERDATA_TOKENS | 0x038)  //  40 bytes
#define MFG_SYNTH_FREQ_OFFSET_LOCATION          (USERDATA_TOKENS | 0x060)  //   2 bytes
#define MFG_CCA_THRESHOLD_LOCATION              (USERDATA_TOKENS | 0x064)  //   2 bytes
#define MFG_EZSP_STORAGE_LOCATION               (USERDATA_TOKENS | 0x068)  //   8 bytes
#define MFG_XO_TUNE_LOCATION                    (USERDATA_TOKENS | 0x070)  //   2 bytes
#define MFG_ZWAVE_COUNTRY_FREQ_LOCATION         (USERDATA_TOKENS | 0x074)  //   1 bytes
#define MFG_ZWAVE_HW_VERSION_LOCATION           (USERDATA_TOKENS | 0x078)  //   1 bytes
#define MFG_ZWAVE_PSEUDO_RANDOM_NUMBER_LOCATION (USERDATA_TOKENS | 0x07C)  //  16 bytes
#define MFG_SERIAL_NUMBER_LOCATION              (USERDATA_TOKENS | 0x08C)  //  16 bytes
#define MFG_LFXO_TUNE_LOCATION                  (USERDATA_TOKENS | 0x09C)  //   1 bytes
#define MFG_CTUNE_LOCATION                      (USERDATA_TOKENS | 0x100)  //   2 bytes
#define MFG_KIT_SIGNATURE_LOCATION              (USERDATA_TOKENS | 0x104)  //   4 bytes

//--- Lock Bits ---
//The LOCKBITS_BASE page is physically mapped to 0x0FE04000-0x0FE047FF.
//The lock words are fixed functionality in the first 512 bytes of the
//page so all Lock Bits Data must exist above 0x0FE04200.
//Overloading the location with 0x2000 will keep the token out of SimEE and
//indicate it is a token living in Lock Bits space.
#define LOCKBITS_TOKENS                    0x2000
#define MFG_LOCKBITS_PLW_LOCATION               (LOCKBITS_TOKENS | 0x000)  //  16 bytes
#define MFG_LOCKBITS_CLW0_LOCATION              (LOCKBITS_TOKENS | 0x1e8)  //   4 bytes
#define MFG_LOCKBITS_MLW_LOCATION               (LOCKBITS_TOKENS | 0x1f4)  //   4 bytes
#define MFG_LOCKBITS_ULW_LOCATION               (LOCKBITS_TOKENS | 0x1f8)  //   4 bytes
#define MFG_LOCKBITS_DLW_LOCATION               (LOCKBITS_TOKENS | 0x1fc)  //   4 bytes

//--- Lock Bits Data ---
//Lock Bits Data is mapped into LOCKBITS_BASE above the Lock Bits.
//Overloading the location with 0x3000 will keep the token out of SimEE and
//indicate it is a token living in Lock Bits Data space.
#define LOCKBITSDATA_TOKENS                0x3000
//Intentionally skipping the first four bytes.  The location might be
//best for a version or other type of information.
#define MFG_CBKE_DATA_LOCATION                  (LOCKBITSDATA_TOKENS | 0x204)  //  92 bytes
#define MFG_SECURITY_CONFIG_LOCATION            (LOCKBITSDATA_TOKENS | 0x260)  //   2 bytes
#define MFG_INSTALLATION_CODE_LOCATION          (LOCKBITSDATA_TOKENS | 0x270)  //  20 bytes
#define MFG_SECURE_BOOTLOADER_KEY_LOCATION      (LOCKBITSDATA_TOKENS | 0x286)  //  16 bytes
#define MFG_CBKE_283K1_DATA_LOCATION            (LOCKBITSDATA_TOKENS | 0x298)  // 148 bytes
#define MFG_BOOTLOAD_AES_KEY_LOCATION           (LOCKBITSDATA_TOKENS | 0x32C)  //  16 bytes
#define MFG_SIGNED_BOOTLOADER_KEY_X_LOCATION    (LOCKBITSDATA_TOKENS | 0x34C)  //  32 bytes
#define MFG_SIGNED_BOOTLOADER_KEY_Y_LOCATION    (LOCKBITSDATA_TOKENS | 0x36C)  //  32 bytes
#define MFG_THREAD_JOIN_KEY_LOCATION            (LOCKBITSDATA_TOKENS | 0x38C)  //  34 bytes
#define MFG_NVM3_CRYPTO_KEY_LOCATION            (LOCKBITSDATA_TOKENS | 0x3B0)  //  16 bytes
#define MFG_ZW_PRK_LOCATION                     (LOCKBITSDATA_TOKENS | 0x3C0)  //  32 bytes
#define MFG_ZW_PUK_LOCATION                     (LOCKBITSDATA_TOKENS | 0x3E0)  //  32 bytes
#define MFG_ZW_QR_CODE_LOCATION                 (LOCKBITSDATA_TOKENS | 0x400)  //  90 bytes
#define MFG_ZW_INITIALIZED_LOCATION             (LOCKBITSDATA_TOKENS | 0x45C)  //   1 bytes
#define MFG_ZW_QR_CODE_EXT_LOCATION             (LOCKBITSDATA_TOKENS | 0x460)  //  16 bytes

//--- Virtual MFG Tokens ---
#define MFG_EUI_64_LOCATION                       0xb634  //   8 bytes

// Define the size of indexed token array
#define MFG_ASH_CONFIG_ARRAY_SIZE       20

//--- User Data ---
TOKEN_NEXT_ADDRESS(MFG_EMBER_EUI_64_ADDR, MFG_EMBER_EUI_64_LOCATION)
TOKEN_MFG(MFG_EMBER_EUI_64, CREATOR_MFG_EMBER_EUI_64,
          0, 0, tokTypeMfgEmberEui64, 1,
          { 3, 0, 0, 0, 0, 0, 0, 3 })

TOKEN_NEXT_ADDRESS(MFG_CUSTOM_EUI_64_ADDR, MFG_CUSTOM_EUI_64_LOCATION)
TOKEN_MFG(MFG_CUSTOM_EUI_64, CREATOR_MFG_CUSTOM_EUI_64,
          0, 0, tokTypeMfgCustomEui64, 1,
          { 0, 3, 3, 3, 3, 3, 3, 0 })

TOKEN_NEXT_ADDRESS(MFG_CUSTOM_VERSION_ADDR, MFG_CUSTOM_VERSION_LOCATION)
TOKEN_MFG(MFG_CUSTOM_VERSION, CREATOR_MFG_CUSTOM_VERSION,
          0, 0, tokTypeMfgCustomVersion, 1,
          CURRENT_MFG_CUSTOM_VERSION)

TOKEN_NEXT_ADDRESS(MFG_STRING_ADDR, MFG_STRING_LOCATION)
TOKEN_MFG(MFG_STRING, CREATOR_MFG_STRING,
          0, 0, tokTypeMfgString, 1,
          { 0, })

TOKEN_NEXT_ADDRESS(MFG_BOARD_NAME_ADDR, MFG_BOARD_NAME_LOCATION)
TOKEN_MFG(MFG_BOARD_NAME, CREATOR_MFG_BOARD_NAME,
          0, 0, tokTypeMfgBoardName, 1,
          { 0, })

TOKEN_NEXT_ADDRESS(MFG_MANUF_ID_ADDR, MFG_MANUF_ID_LOCATION)
TOKEN_MFG(MFG_MANUF_ID, CREATOR_MFG_MANUF_ID,
          0, 0, tokTypeMfgManufId, 1,
          { 0x00, 0x00, })

TOKEN_NEXT_ADDRESS(MFG_PHY_CONFIG_ADDR, MFG_PHY_CONFIG_LOCATION)
TOKEN_MFG(MFG_PHY_CONFIG, CREATOR_MFG_PHY_CONFIG,
          0, 0, tokTypeMfgPhyConfig, 1,
          { 0x00, 0x00, })

TOKEN_NEXT_ADDRESS(MFG_ASH_CONFIG_ADDR, MFG_ASH_CONFIG_LOCATION)
TOKEN_MFG(MFG_ASH_CONFIG, CREATOR_MFG_ASH_CONFIG,
          0, 1, tokTypeMfgAshConfig, MFG_ASH_CONFIG_ARRAY_SIZE,
          { 0xFFFF, })

TOKEN_NEXT_ADDRESS(MFG_SYNTH_FREQ_OFFSET_ADDR, MFG_SYNTH_FREQ_OFFSET_LOCATION)
TOKEN_MFG(MFG_SYNTH_FREQ_OFFSET, CREATOR_MFG_SYNTH_FREQ_OFFSET,
          0, 0, tokTypeMfgSynthFreqOffset, 1,
          { 0xFF, 0xFF, })

TOKEN_NEXT_ADDRESS(MFG_CCA_THRESHOLD_ADDR, MFG_CCA_THRESHOLD_LOCATION)
TOKEN_MFG(MFG_CCA_THRESHOLD, CREATOR_MFG_CCA_THRESHOLD,
          0, 0, tokTypeMfgCcaThreshold, 1,
          { 0xFF, 0xFF, })

TOKEN_NEXT_ADDRESS(MFG_EZSP_STORAGE_ADDR, MFG_EZSP_STORAGE_LOCATION)
TOKEN_MFG(MFG_EZSP_STORAGE, CREATOR_MFG_EZSP_STORAGE,
          0, 0, tokTypeMfgEzspStorage, 1,
          { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF })

TOKEN_NEXT_ADDRESS(MFG_XO_TUNE_ADDR, MFG_XO_TUNE_LOCATION)
TOKEN_MFG(MFG_XO_TUNE, CREATOR_MFG_XO_TUNE,
          0, 0, tokTypeMfgXoTune, 1,
          { 0xFFFF })

TOKEN_NEXT_ADDRESS(MFG_ZWAVE_COUNTRY_FREQ_ADDR, MFG_ZWAVE_COUNTRY_FREQ_LOCATION)
TOKEN_MFG(MFG_ZWAVE_COUNTRY_FREQ, CREATOR_MFG_ZWAVE_COUNTRY_FREQ,
          0, 0, tokTypeMfgZwaveCountryFreq, 1,
          { 0xFF, })

TOKEN_NEXT_ADDRESS(MFG_ZWAVE_HW_VERSION_ADDR, MFG_ZWAVE_HW_VERSION_LOCATION)
TOKEN_MFG(MFG_ZWAVE_HW_VERSION, CREATOR_MFG_ZWAVE_HW_VERSION,
          0, 0, tokTypeMfgZwaveHardwareVersion, 1,
          { 0xFF, })

TOKEN_NEXT_ADDRESS(MFG_ZWAVE_PSEUDO_RANDOM_NUMBER_ADDR, MFG_ZWAVE_PSEUDO_RANDOM_NUMBER_LOCATION)
TOKEN_MFG(MFG_ZWAVE_PSEUDO_RANDOM_NUMBER, CREATOR_MFG_ZWAVE_PSEUDO_RANDOM_NUMBER,
          0, 0, tokTypeMfgZwavePseudoRandomNumber, 1,
          { 0xFF, })

TOKEN_NEXT_ADDRESS(MFG_SERIAL_NUMBER_ADDR, MFG_SERIAL_NUMBER_LOCATION)
TOKEN_MFG(MFG_SERIAL_NUMBER, CREATOR_MFG_SERIAL_NUMBER,
          0, 0, tokTypeMfgSerialNumber, 1,
          { 0xFF, })

TOKEN_NEXT_ADDRESS(MFG_LFXO_TUNE_ADDR, MFG_LFXO_TUNE_LOCATION)
TOKEN_MFG(MFG_LFXO_TUNE, CREATOR_MFG_LFXO_TUNE,
          0, 0, tokTypeMfgLfxoTune, 1,
          { 0xFF })

TOKEN_NEXT_ADDRESS(MFG_CTUNE_ADDR, MFG_CTUNE_LOCATION)
TOKEN_MFG(MFG_CTUNE, CREATOR_MFG_CTUNE,
          0, 0, tokTypeMfgCTune, 1,
          { 0xFFFF })

TOKEN_NEXT_ADDRESS(MFG_KIT_SIGNATURE_ADDR, MFG_KIT_SIGNATURE_LOCATION)
TOKEN_MFG(MFG_KIT_SIGNATURE, CREATOR_MFG_KIT_SIGNATURE,
          0, 0, tokTypeMfgKitSignature, 1,
          { 0xFF, })

//--- Lock Bits ---
TOKEN_NEXT_ADDRESS(MFG_LOCKBITS_PLW_ADDR, MFG_LOCKBITS_PLW_LOCATION)
TOKEN_MFG(MFG_LOCKBITS_PLW, CREATOR_MFG_LOCKBITS_PLW,
          0, 0, tokTypeLockBitsPlw, 1,
          { 0xFFFFFFFF, })

TOKEN_NEXT_ADDRESS(MFG_LOCKBITS_CLW0_ADDR, MFG_LOCKBITS_CLW0_LOCATION)
TOKEN_MFG(MFG_LOCKBITS_CLW0, CREATOR_MFG_LOCKBITS_CLW0,
          0, 0, tokTypeLockBitsClw0, 1,
          0xFFFFFFFF)

TOKEN_NEXT_ADDRESS(MFG_LOCKBITS_MLW_ADDR, MFG_LOCKBITS_MLW_LOCATION)
TOKEN_MFG(MFG_LOCKBITS_MLW, CREATOR_MFG_LOCKBITS_MLW,
          0, 0, tokTypeLockBitsMlw, 1,
          0xFFFFFFFF)

TOKEN_NEXT_ADDRESS(MFG_LOCKBITS_ULW_ADDR, MFG_LOCKBITS_ULW_LOCATION)
TOKEN_MFG(MFG_LOCKBITS_ULW, CREATOR_MFG_LOCKBITS_ULW,
          0, 0, tokTypeLockBitsUlw, 1,
          0xFFFFFFFF)

TOKEN_NEXT_ADDRESS(MFG_LOCKBITS_DLW_ADDR, MFG_LOCKBITS_DLW_LOCATION)
TOKEN_MFG(MFG_LOCKBITS_DLW, CREATOR_MFG_LOCKBITS_DLW,
          0, 0, tokTypeLockBitsDlw, 1,
          0xFFFFFFFF)

//--- Lock Bits Data ---
TOKEN_NEXT_ADDRESS(MFG_CBKE_DATA_ADDR, MFG_CBKE_DATA_LOCATION)
TOKEN_MFG(MFG_CBKE_DATA, CREATOR_MFG_CBKE_DATA,
          0, 0, tokTypeMfgCbkeData, 1,
          { 0xFF, })

TOKEN_NEXT_ADDRESS(MFG_SECURITY_CONFIG_ADDR, MFG_SECURITY_CONFIG_LOCATION)
TOKEN_MFG(MFG_SECURITY_CONFIG, CREATOR_MFG_SECURITY_CONFIG,
          0, 0, tokTypeMfgSecurityConfig, 1,
          { 0xFF, 0xFF })

TOKEN_NEXT_ADDRESS(MFG_INSTALLATION_CODE_ADDR, MFG_INSTALLATION_CODE_LOCATION)
TOKEN_MFG(MFG_INSTALLATION_CODE, CREATOR_MFG_INSTALLATION_CODE,
          0, 0, tokTypeMfgInstallationCode, 1,
          { 0xFF, })

TOKEN_NEXT_ADDRESS(MFG_SECURE_BOOTLOADER_KEY_ADDR, MFG_SECURE_BOOTLOADER_KEY_LOCATION)
TOKEN_MFG(MFG_SECURE_BOOTLOADER_KEY, CREATOR_MFG_SECURE_BOOTLOADER_KEY,
          0, 0, tokTypeMfgSecureBootloaderKey, 1,
          { 0xFF, }) // default key is all f's

TOKEN_NEXT_ADDRESS(MFG_CBKE_283K1_DATA_ADDR, MFG_CBKE_283K1_DATA_LOCATION)
TOKEN_MFG(MFG_CBKE_283K1_DATA, CREATOR_MFG_CBKE_283K1_DATA,
          0, 0, tokTypeMfgCbke283k1Data, 1,
          { 0xFF, })

TOKEN_NEXT_ADDRESS(MFG_BOOTLOAD_AES_KEY_ADDR, MFG_BOOTLOAD_AES_KEY_LOCATION)
TOKEN_MFG(MFG_BOOTLOAD_AES_KEY, CREATOR_MFG_BOOTLOAD_AES_KEY,
          0, 0, tokTypeMfgBootloadAesKey, 1,
          { 0xFF, }) // default key is all f's

TOKEN_NEXT_ADDRESS(MFG_SIGNED_BOOTLOADER_KEY_X_ADDR, MFG_SIGNED_BOOTLOADER_KEY_X_LOCATION)
TOKEN_MFG(MFG_SIGNED_BOOTLOADER_KEY_X, CREATOR_MFG_SIGNED_BOOTLOADER_KEY_X,
          0, 0, tokTypeMfgSignedBootloaderKeyX, 1,
          { 0xFF, }) // default key is all f's

TOKEN_NEXT_ADDRESS(MFG_SIGNED_BOOTLOADER_KEY_Y_ADDR, MFG_SIGNED_BOOTLOADER_KEY_Y_LOCATION)
TOKEN_MFG(MFG_SIGNED_BOOTLOADER_KEY_Y, CREATOR_MFG_SIGNED_BOOTLOADER_KEY_Y,
          0, 0, tokTypeMfgSignedBootloaderKeyY, 1,
          { 0xFF, }) // default key is all f's

TOKEN_NEXT_ADDRESS(MFG_THREAD_JOIN_KEY_ADDR, MFG_THREAD_JOIN_KEY_LOCATION)
TOKEN_MFG(MFG_THREAD_JOIN_KEY, CREATOR_MFG_THREAD_JOIN_KEY,
          0, 0, tokTypeMfgThreadJoinKey, 1,
          { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFFFF })

TOKEN_NEXT_ADDRESS(MFG_NVM3_CRYPTO_KEY_ADDR, MFG_NVM3_CRYPTO_KEY_LOCATION)
TOKEN_MFG(MFG_NVM3_CRYPTO_KEY, CREATOR_MFG_NVM3_CRYPTO_KEY,
          0, 0, tokTypeMfgNvm3CryptoKey, 1,
          { 0xFF, }) // default key is all f's

TOKEN_NEXT_ADDRESS(MFG_ZW_PRK_ADDR, MFG_ZW_PRK_LOCATION)
TOKEN_MFG(MFG_ZW_PRK, CREATOR_MFG_ZW_PRK,
          0, 0, tokTypeMfgZwavePrivateKey, 1,
          { 0xFF, }) // 0xFF = Z-Wave key is not initialized

TOKEN_NEXT_ADDRESS(MFG_ZW_PUK_ADDR, MFG_ZW_PUK_LOCATION)
TOKEN_MFG(MFG_ZW_PUK, CREATOR_MFG_ZW_PUK,
          0, 0, tokTypeMfgZwavePublicKey, 1,
          { 0xFF, }) // 0xFF = Z-Wave key is not initialized

TOKEN_NEXT_ADDRESS(MFG_ZW_QR_CODE_ADDR, MFG_ZW_QR_CODE_LOCATION)
TOKEN_MFG(MFG_ZW_QR_CODE, CREATOR_MFG_ZW_QR_CODE,
          0, 0, tokTypeMfgQRCode, 1,
          { 0xFF, }) // 0xFF = Z-Wave QR Code is not initialized

TOKEN_NEXT_ADDRESS(MFG_ZW_INITIALIZED_ADDR, MFG_ZW_INITIALIZED_LOCATION)
TOKEN_MFG(MFG_ZW_INITIALIZED, CREATOR_MFG_ZW_INITIALIZED,
          0, 0, tokTypeMfgZwaveInitialized, 1,
          { 0xFF, }) // 0xFF = Z-Wave fields are not initialized

TOKEN_NEXT_ADDRESS(MFG_ZW_QR_CODE_EXT_ADDR, MFG_ZW_QR_CODE_EXT_LOCATION)
TOKEN_MFG(MFG_ZW_QR_CODE_EXT, CREATOR_MFG_ZW_QR_CODE_EXT,
          0, 0, tokTypeMfgQRCode, 1,
          { 0xFF, }) // 0xFF = Z-Wave QR Code is not initialized

TOKEN_NEXT_ADDRESS(MFG_EUI_64_ADDR, MFG_EUI_64_LOCATION)
TOKEN_MFG(MFG_EUI_64, CREATOR_MFG_EUI_64,
          0, 0, tokTypeMfgEui64, 1,
          { 3, 3, 3, 3, 0, 0, 0, 0 })

#endif //DEFINETOKENS

#ifdef APPLICATION_MFG_TOKEN_HEADER
  #include APPLICATION_MFG_TOKEN_HEADER
#endif

#undef TOKEN_NEXT_ADDRESS

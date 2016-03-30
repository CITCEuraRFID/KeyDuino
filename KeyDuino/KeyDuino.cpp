/**************************************************************************/
/*!
    @file     KeyDuino.cpp
    @author   Adafruit Industries & Seeed Studio & CITC EuraRFID
    @license  BSD
*/
/**************************************************************************/

#include "KeyDuino.h"
#include "Arduino.h"
#include <string.h>
#include <stdint.h>


#define HAL(func)   (this->func)
#ifdef DEBUG
#define DMSG(args...)       Serial.print(args)
#define DMSG_STR(str)       Serial.println(str)
#define DMSG_HEX(num)       Serial.print(' '); Serial.print(num, HEX)
#define DMSG_INT(num)       Serial.print(' '); Serial.print(num)
#else
#define DMSG(args...)
#define DMSG_STR(str)
#define DMSG_HEX(num)
#define DMSG_INT(num)
#endif

#define PN532_PREAMBLE                (0x00)
#define PN532_STARTCODE1              (0x00)
#define PN532_STARTCODE2              (0xFF)
#define PN532_POSTAMBLE               (0x00)

#define PN532_HOSTTOPN532             (0xD4)
#define PN532_PN532TOHOST             (0xD5)

#define PN532_ACK_WAIT_TIME           (10)  // ms, timeout of waiting for ACK

#define PN532_INVALID_ACK             (-1)
#define PN532_TIMEOUT                 (-2)
#define PN532_INVALID_FRAME           (-3)
#define PN532_NO_SPACE                (-4)

#define REVERSE_BITS_ORDER(b)         b = (b & 0xF0) >> 4 | (b & 0x0F) << 4; \
                                      b = (b & 0xCC) >> 2 | (b & 0x33) << 2; \
                                      b = (b & 0xAA) >> 1 | (b & 0x55) << 1

#define BUZZER_PIN 		      15
									  
/**************************************************************************/
/*!
    @brief  Prints a hexadecimal value in plain characters

    @param  data      Pointer to the uint8_t data
    @param  numBytes  Data length in bytes
*/
/**************************************************************************/
void KeyDuino::PrintHex(const uint8_t *data, const uint32_t numBytes)
{
#ifdef ARDUINO
    for (uint8_t i = 0; i < numBytes; i++) {
        if (data[i] < 0x10) {
            Serial.print(" 0");
        } else {
            Serial.print(' ');
        }
        Serial.print(data[i], HEX);
    }
    Serial.println("");
#else
    for (uint8_t i = 0; i < numBytes; i++) {
        printf(" %2X", data[i]);
    }
    printf("\n");
#endif
}

/**************************************************************************/
/*!
    @brief  Prints a hexadecimal value in plain characters, along with
            the char equivalents in the following format

            00 00 00 00 00 00  ......

    @param  data      Pointer to the data
    @param  numBytes  Data length in bytes
*/
/**************************************************************************/
void KeyDuino::PrintHexChar(const uint8_t *data, const uint32_t numBytes)
{
#ifdef ARDUINO
    for (uint8_t i = 0; i < numBytes; i++) {
        if (data[i] < 0x10) {
            Serial.print(" 0");
        } else {
            Serial.print(' ');
        }
        Serial.print(data[i], HEX);
    }
    Serial.print("    ");
    for (uint8_t i = 0; i < numBytes; i++) {
        char c = data[i];
        if (c <= 0x1f || c > 0x7f) {
            Serial.print('.');
        } else {
            Serial.print(c);
        }
    }
    Serial.println("");
#else
    for (uint8_t i = 0; i < numBytes; i++) {
        printf(" %2X", data[i]);
    }
    printf("    ");
    for (uint8_t i = 0; i < numBytes; i++) {
        char c = data[i];
        if (c <= 0x1f || c > 0x7f) {
            printf(".");
        } else {
            printf("%c", c);
        }
        printf("\n");
    }
#endif
}


/**************************************************************************/
/*!
    @brief  Returns a hexadecimal value in plain characters

    @param  data      Pointer to the uint8_t data
    @param  numBytes  Data length in bytes

    @returns  The data converted to a string
*/
/**************************************************************************/
String KeyDuino::convertUintToString(const uint8_t *data, const uint8_t numBytes)
{
	String result;
	for (uint8_t i=0; i < numBytes; i++) {
	    if (data[i] < 0x10)
            	result += "0";
      	    result += String(data[i], HEX);
	}
    	return(result);
}

/**************************************************************************/
/*!
    @brief  Checks the firmware version of the PN5xx chip

    @returns  The chip's firmware version and ID
*/
/**************************************************************************/
uint32_t KeyDuino::getFirmwareVersion(void)
{
    uint32_t response;

    pn532_packetbuffer[0] = PN532_COMMAND_GETFIRMWAREVERSION;

    if (HAL(writeCommand)(pn532_packetbuffer, 1)) {
        return 0;
    }

    // read data packet
    int16_t status = HAL(readResponse)(pn532_packetbuffer, sizeof(pn532_packetbuffer));
    if (0 > status) {
        return 0;
    }

    response = pn532_packetbuffer[0];
    response <<= 8;
    response |= pn532_packetbuffer[1];
    response <<= 8;
    response |= pn532_packetbuffer[2];
    response <<= 8;
    response |= pn532_packetbuffer[3];

    return response;
}


/**************************************************************************/
/*!
    Writes an 8-bit value that sets the state of the PN532's GPIO pins

    @warning This function is provided exclusively for board testing and
             is dangerous since it will throw an error if any pin other
             than the ones marked "Can be used as GPIO" are modified!  All
             pins that can not be used as GPIO should ALWAYS be left high
             (value = 1) or the system will become unstable and a HW reset
             will be required to recover the PN532.

             pinState[0]  = P30     Can be used as GPIO
             pinState[1]  = P31     Can be used as GPIO
             pinState[2]  = P32     *** RESERVED (Must be 1!) ***
             pinState[3]  = P33     Can be used as GPIO
             pinState[4]  = P34     *** RESERVED (Must be 1!) ***
             pinState[5]  = P35     Can be used as GPIO

    @returns 1 if everything executed properly, 0 for an error
*/
/**************************************************************************/
bool KeyDuino::writeGPIO(uint8_t pinstate)
{
    // Make sure pinstate does not try to toggle P32 or P34
    pinstate |= (1 << PN532_GPIO_P32) | (1 << PN532_GPIO_P34);

    // Fill command buffer
    pn532_packetbuffer[0] = PN532_COMMAND_WRITEGPIO;
    pn532_packetbuffer[1] = PN532_GPIO_VALIDATIONBIT | pinstate;  // P3 Pins
    pn532_packetbuffer[2] = 0x00;    // P7 GPIO Pins (not used ... taken by I2C)

    DMSG_STR("Writing P3 GPIO: ");
    DMSG_HEX(pn532_packetbuffer[1]);
    DMSG("\n");

    // Send the WRITEGPIO command (0x0E)
    if (HAL(writeCommand)(pn532_packetbuffer, 3))
        return 0;

    return (0 < HAL(readResponse)(pn532_packetbuffer, sizeof(pn532_packetbuffer)));
}

/**************************************************************************/
/*!
    Reads the state of the PN532's GPIO pins

    @returns An 8-bit value containing the pin state where:

             pinState[0]  = P30
             pinState[1]  = P31
             pinState[2]  = P32
             pinState[3]  = P33
             pinState[4]  = P34
             pinState[5]  = P35
*/
/**************************************************************************/
uint8_t KeyDuino::readGPIO(void)
{
    pn532_packetbuffer[0] = PN532_COMMAND_READGPIO;

    // Send the READGPIO command (0x0C)
    if (HAL(writeCommand)(pn532_packetbuffer, 1))
        return 0x0;

    HAL(readResponse)(pn532_packetbuffer, sizeof(pn532_packetbuffer));

    /* READGPIO response without prefix and suffix should be in the following format:

      byte            Description
      -------------   ------------------------------------------
      b0              P3 GPIO Pins
      b1              P7 GPIO Pins (not used ... taken by I2C)
      b2              Interface Mode Pins (not used ... bus select pins)
    */


    DMSG("P3 GPIO: "); DMSG_HEX(pn532_packetbuffer[7]);
    DMSG("P7 GPIO: "); DMSG_HEX(pn532_packetbuffer[8]);
    DMSG("I0I1 GPIO: "); DMSG_HEX(pn532_packetbuffer[9]);
    DMSG("\n");

    return pn532_packetbuffer[0];
}

/**************************************************************************/
/*!
    @brief  Configures the SAM (Secure Access Module)
*/
/**************************************************************************/
bool KeyDuino::SAMConfig(void)
{
    pn532_packetbuffer[0] = PN532_COMMAND_SAMCONFIGURATION;
    pn532_packetbuffer[1] = 0x01; // normal mode;
    pn532_packetbuffer[2] = 0x14; // timeout 50ms * 20 = 1 second
    pn532_packetbuffer[3] = 0x01; // use IRQ pin!

    DMSG_STR("SAMConfig");

    if (HAL(writeCommand)(pn532_packetbuffer, 4))
        return false;

    return (0 < HAL(readResponse)(pn532_packetbuffer, sizeof(pn532_packetbuffer)));
}

/**************************************************************************/
/*!
    Sets the MxRtyPassiveActivation uint8_t of the RFConfiguration register

    @param  maxRetries    0xFF to wait forever, 0x00..0xFE to timeout
                          after mxRetries

    @returns 1 if everything executed properly, 0 for an error
*/
/**************************************************************************/
bool KeyDuino::setPassiveActivationRetries(uint8_t maxRetries)
{
    pn532_packetbuffer[0] = PN532_COMMAND_RFCONFIGURATION;
    pn532_packetbuffer[1] = 5;    // Config item 5 (MaxRetries)
    pn532_packetbuffer[2] = 0xFF; // MxRtyATR (default = 0xFF)
    pn532_packetbuffer[3] = 0x01; // MxRtyPSL (default = 0x01)
    pn532_packetbuffer[4] = maxRetries;

    if (HAL(writeCommand)(pn532_packetbuffer, 5))
        return 0x0;  // no ACK

    return (0 < HAL(readResponse)(pn532_packetbuffer, sizeof(pn532_packetbuffer)));
}


/***** ISO14443 Commands ******/

/**
    @brief    read the tag UID

    @param    uid     the var to write the uid
    @param    uidLen  the var to write the uid

    @returns  1 if the UID was correctly read, 0 if reading failed
*/
uint8_t KeyDuino::readTargetID(uint8_t cardbaudrate, uint8_t *uid, uint8_t *uidLength){
    if(cardbaudrate == PN532_ISO14443B)
        return readPassiveTargetID_B(uid, uidLength);
    return readPassiveTargetID(PN532_ISO14443A, uid, uidLength);
}


/**************************************************************************/
/*!
    Waits for an ISO14443A target to enter the field

    @param  cardBaudRate  Baud rate of the card
    @param  uid           Pointer to the array that will be populated
                          with the card's UID (up to 7 bytes)
    @param  uidLength     Pointer to the variable that will hold the
                          length of the card's UID.

    @returns 1 if everything executed properly, 0 for an error
*/
/**************************************************************************/
bool KeyDuino::readPassiveTargetID(uint8_t cardbaudrate, uint8_t *uid, uint8_t *uidLength, uint16_t timeout)
{
    pn532_packetbuffer[0] = PN532_COMMAND_INLISTPASSIVETARGET;
    pn532_packetbuffer[1] = 1;  // max 1 cards at once (we can set this to 2 later)
    pn532_packetbuffer[2] = cardbaudrate;

    if (HAL(writeCommand)(pn532_packetbuffer, 3)) {
        DMSG_STR("\nFailed writing");
        return 0x0;  // command failed
    }

    // read data packet
    if (HAL(readResponse)(pn532_packetbuffer, sizeof(pn532_packetbuffer), timeout) < 0) {
        DMSG_STR("\nFailed reading response");
        return 0x0;
    }

    // check some basic stuff
    /* ISO14443A card response should be in the following format:

      byte            Description
      -------------   ------------------------------------------
      b0              Tags Found
      b1              Tag Number (only one used in this example)
      b2..3           SENS_RES
      b4              SEL_RES
      b5              NFCID Length
      b6..NFCIDLen    NFCID
    */

    if (pn532_packetbuffer[0] != 1)
        return 0;

    inListedTag = pn532_packetbuffer[1];

    uint16_t sens_res = pn532_packetbuffer[2];
    sens_res <<= 8;
    sens_res |= pn532_packetbuffer[3];

    DMSG("\nATQA: 0x");  DMSG_HEX(sens_res);
    DMSG("\nSAK: 0x");  DMSG_HEX(pn532_packetbuffer[4]);
    DMSG("\n");

    /* Card appears to be Mifare Classic */
    *uidLength = pn532_packetbuffer[5];
    this->_uidLen = pn532_packetbuffer[5];

    for (uint8_t i = 0; i < pn532_packetbuffer[5]; i++) {
        uid[i] = pn532_packetbuffer[6 + i];
	    this->_uid[i] = pn532_packetbuffer[6 + i];
    }
    return 1;
}


/**************************************************************************/
/*!
    Waits for an ISO14443B target to enter the field

    @param  cardBaudRate  Baud rate of the card
    @param  uid           Pointer to the array that will be populated
                          with the card's PUPI (normally 4 bytes)
    @param  uidLength     Pointer to the variable that will hold the
                          length of the card's UID.

    @returns 1 if everything executed properly, 0 for an error
*/
/**************************************************************************/
bool KeyDuino::readPassiveTargetID_B(uint8_t *uid, uint8_t *uidLength, uint16_t timeout)
{
    pn532_packetbuffer[0] = PN532_COMMAND_INLISTPASSIVETARGET;
    pn532_packetbuffer[1] = 1;  // max 1 cards at once (we can set this to 2 later)
    pn532_packetbuffer[2] = PN532_ISO14443B;
    pn532_packetbuffer[3] = 0x00; //AFI

    if (HAL(writeCommand)(pn532_packetbuffer, 4)) {
        DMSG_STR("\nFailed writing");
        return 0x0;  // command failed
    }

    // read data packet
    if (HAL(readResponse)(pn532_packetbuffer, sizeof(pn532_packetbuffer), timeout) < 0) {
        DMSG_STR("\nFailed reading response");
        return 0x0;
    }


    /* ISO14443B card response should be in the following format:

      byte                  Description
      ------------------    ------------------------------------------
      b0                    Tags Found
      b1                    Tag Number (only one used in this example)
      b2..13                ATQB
      b14                   ATTRIB_RES Length
      b15..ATTRIB_RESLen    ATTRIB_RES
    */

    if (pn532_packetbuffer[0] != 1)
        return 0;

    inListedTag = pn532_packetbuffer[1];

    /* ISO14443B ATQ_B format:

      byte          Description
      -----         ------------------------------------------
      b0            0x50
      b1..4         PUPI
      b5..8         Application Data
      b9            Bit Rate Capacity
      b10           Max Frame Size/-4 Info
      b11           FWI/Coding Options
    */

    uint8_t atqb[12];
    DMSG("\nATQ_B: ");
    if (pn532_packetbuffer[2] == 0x50) {        //Looking for the 0x50 to check if ATQ_B is well shaped
        for (uint8_t i = 0; i < 12; i++) {
            atqb[i] = pn532_packetbuffer[i+2];
            DMSG_HEX(atqb[i]);
        }
        DMSG("\n");
    } else                                      //Sometimes there are remnant bytes, in that case just let it go ...
        return 0;

    DMSG_STR("\nATTRIB_RES: ");
    for (uint8_t i = 0; i < pn532_packetbuffer[14]; i++) {
        DMSG_HEX(pn532_packetbuffer[15 + i]);
    }
    DMSG("\n");

    *uidLength = 4;
    this->_uidLen = 4;

    for (uint8_t i = 0; i < 4; i++) {
        uid[i] = atqb[1 + i];
	    this->_uid[i] = atqb[1 + i];
    }
    
    return 1;
}


/***** Mifare Classic Functions ******/

/**************************************************************************/
/*!
      Indicates whether the specified block number is the first block
      in the sector (block 0 relative to the current sector)
*/
/**************************************************************************/
bool KeyDuino::mifareclassic_IsFirstBlock (uint32_t uiBlock)
{
    // Test if we are in the small or big sectors
    if (uiBlock < 128)
        return ((uiBlock) % 4 == 0);
    else
        return ((uiBlock) % 16 == 0);
}

/**************************************************************************/
/*!
      Indicates whether the specified block number is the sector trailer
*/
/**************************************************************************/
bool KeyDuino::mifareclassic_IsTrailerBlock (uint32_t uiBlock)
{
    // Test if we are in the small or big sectors
    if (uiBlock < 128)
        return ((uiBlock + 1) % 4 == 0);
    else
        return ((uiBlock + 1) % 16 == 0);
}


/**************************************************************************/
/*!
    Tries to authenticate a block of memory on a MIFARE card using the
    INDATAEXCHANGE command.  See section 7.3.8 of the PN532 User Manual
    for more information on sending MIFARE and other commands.

    @param  uid           Pointer to a byte array containing the card UID
    @param  uidLen        The length (in bytes) of the card's UID (Should
                          be 4 for MIFARE Classic)
    @param  blockNumber   The block number to authenticate.  (0..63 for
                          1KB cards, and 0..255 for 4KB cards).
    @param  keyNumber     Which key type to use during authentication
                          (0 = MIFARE_CMD_AUTH_A, 1 = MIFARE_CMD_AUTH_B)
    @param  keyData       Pointer to a byte array containing the 6 bytes
                          key value

    @returns 1 if everything executed properly, 0 for an error
*/
/**************************************************************************/
uint8_t KeyDuino::mifareclassic_AuthenticateBlock (uint8_t *uid, uint8_t uidLen, uint32_t blockNumber, uint8_t keyNumber, uint8_t *keyData)
{
    uint8_t i;

    // Hang on to the key and uid data
    memcpy (_key, keyData, 6);
    memcpy (_uid, uid, uidLen);
    _uidLen = uidLen;

    // Prepare the authentication command //
    pn532_packetbuffer[0] = PN532_COMMAND_INDATAEXCHANGE;   /* Data Exchange Header */
    pn532_packetbuffer[1] = 1;                              /* Max card numbers */
    pn532_packetbuffer[2] = (keyNumber) ? MIFARE_CMD_AUTH_B : MIFARE_CMD_AUTH_A;
    pn532_packetbuffer[3] = blockNumber;                    /* Block Number (1K = 0..63, 4K = 0..255 */
    memcpy (pn532_packetbuffer + 4, _key, 6);
    for (i = 0; i < _uidLen; i++) {
        pn532_packetbuffer[10 + i] = _uid[i];              /* 4 bytes card ID */
    }

    if (HAL(writeCommand)(pn532_packetbuffer, 10 + _uidLen))
        return 0;

    // Read the response packet
    HAL(readResponse)(pn532_packetbuffer, sizeof(pn532_packetbuffer));

    // Check if the response is valid and we are authenticated???
    // for an auth success it should be bytes 5-7: 0xD5 0x41 0x00
    // Mifare auth error is technically byte 7: 0x14 but anything other and 0x00 is not good
    if (pn532_packetbuffer[0] != 0x00) {
        DMSG("Authentification failed\n");
        return 0;
    }

    return 1;
}


/**************************************************************************/
/*!
    Tries to read an entire 16-bytes data block at the specified block
    address.

    @param  blockNumber   The block number to authenticate.  (0..63 for
                          1KB cards, and 0..255 for 4KB cards).
    @param  data          Pointer to the byte array that will hold the
                          retrieved data (if any)

    @returns 1 if everything executed properly, 0 for an error
*/
/**************************************************************************/
uint8_t KeyDuino::mifareclassic_ReadDataBlock (uint8_t blockNumber, uint8_t *data)
{
    DMSG("Trying to read 16 bytes from block ");
    DMSG_INT(blockNumber);

    /* Prepare the command */
    pn532_packetbuffer[0] = PN532_COMMAND_INDATAEXCHANGE;
    pn532_packetbuffer[1] = 1;                      /* Card number */
    pn532_packetbuffer[2] = MIFARE_CMD_READ;        /* Mifare Read command = 0x30 */
    pn532_packetbuffer[3] = blockNumber;            /* Block Number (0..63 for 1K, 0..255 for 4K) */

    /* Send the command */
    if (HAL(writeCommand)(pn532_packetbuffer, 4)) {
        return 0;
    }

    /* Read the response packet */
    HAL(readResponse)(pn532_packetbuffer, sizeof(pn532_packetbuffer));

    /* If byte 8 isn't 0x00 we probably have an error */
    if (pn532_packetbuffer[0] != 0x00) {
        return 0;
    }

    /* Copy the 16 data bytes to the output buffer        */
    /* Block content starts at byte 9 of a valid response */
    memcpy (data, pn532_packetbuffer + 1, 16);

    return 1;
}


/**************************************************************************/
/*!
    Tries to write an entire 16-bytes data block at the specified block
    address.

    @param  blockNumber   The block number to authenticate.  (0..63 for
                          1KB cards, and 0..255 for 4KB cards).
    @param  data          The byte array that contains the data to write.

    @returns 1 if everything executed properly, 0 for an error
*/
/**************************************************************************/
uint8_t KeyDuino::mifareclassic_WriteDataBlock (uint8_t blockNumber, uint8_t *data)
{
    /* Prepare the first command */
    pn532_packetbuffer[0] = PN532_COMMAND_INDATAEXCHANGE;
    pn532_packetbuffer[1] = 1;                      /* Card number */
    pn532_packetbuffer[2] = MIFARE_CMD_WRITE;       /* Mifare Write command = 0xA0 */
    pn532_packetbuffer[3] = blockNumber;            /* Block Number (0..63 for 1K, 0..255 for 4K) */
    memcpy (pn532_packetbuffer + 4, data, 16);        /* Data Payload */

    /* Send the command */
    if (HAL(writeCommand)(pn532_packetbuffer, 20)) {
        return 0;
    }

    /* Read the response packet */
    return (0 < HAL(readResponse)(pn532_packetbuffer, sizeof(pn532_packetbuffer)));
}


/**************************************************************************/
/*!
    Formats a Mifare Classic card to store NDEF Records

    @returns 1 if everything executed properly, 0 for an error
*/
/**************************************************************************/
uint8_t KeyDuino::mifareclassic_FormatNDEF (void)
{
    uint8_t sectorbuffer1[16] = {0x14, 0x01, 0x03, 0xE1, 0x03, 0xE1, 0x03, 0xE1, 0x03, 0xE1, 0x03, 0xE1, 0x03, 0xE1, 0x03, 0xE1};
    uint8_t sectorbuffer2[16] = {0x03, 0xE1, 0x03, 0xE1, 0x03, 0xE1, 0x03, 0xE1, 0x03, 0xE1, 0x03, 0xE1, 0x03, 0xE1, 0x03, 0xE1};
    uint8_t sectorbuffer3[16] = {0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0x78, 0x77, 0x88, 0xC1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    // Note 0xA0 0xA1 0xA2 0xA3 0xA4 0xA5 must be used for key A
    // for the MAD sector in NDEF records (sector 0)

    // Write block 1 and 2 to the card
    if (!(mifareclassic_WriteDataBlock (1, sectorbuffer1)))
        return 0;
    if (!(mifareclassic_WriteDataBlock (2, sectorbuffer2)))
        return 0;
    // Write key A and access rights card
    if (!(mifareclassic_WriteDataBlock (3, sectorbuffer3)))
        return 0;

    // Seems that everything was OK (?!)
    return 1;
}


/**************************************************************************/
/*!
    Writes an NDEF URI Record to the specified sector (1..15)

    Note that this function assumes that the Mifare Classic card is
    already formatted to work as an "NFC Forum Tag" and uses a MAD1
    file system.  You can use the NXP TagWriter app on Android to
    properly format cards for this.

    @param  sectorNumber  The sector that the URI record should be written
                          to (can be 1..15 for a 1K card)
    @param  uriIdentifier The uri identifier code (0 = none, 0x01 =
                          "http://www.", etc.)
    @param  url           The uri text to write (max 38 characters).

    @returns 1 if everything executed properly, 0 for an error
*/
/**************************************************************************/
uint8_t KeyDuino::mifareclassic_WriteNDEFURI (uint8_t sectorNumber, uint8_t uriIdentifier, const char *url)
{
    // Figure out how long the string is
    uint8_t len = strlen(url);

    // Make sure we're within a 1K limit for the sector number
    if ((sectorNumber < 1) || (sectorNumber > 15))
        return 0;

    // Make sure the URI payload is between 1 and 38 chars
    if ((len < 1) || (len > 38))
        return 0;

    // Note 0xD3 0xF7 0xD3 0xF7 0xD3 0xF7 must be used for key A
    // in NDEF records

    // Setup the sector buffer (w/pre-formatted TLV wrapper and NDEF message)
    uint8_t sectorbuffer1[16] = {0x00, 0x00, 0x03, len + 5, 0xD1, 0x01, len + 1, 0x55, uriIdentifier, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t sectorbuffer2[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t sectorbuffer3[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t sectorbuffer4[16] = {0xD3, 0xF7, 0xD3, 0xF7, 0xD3, 0xF7, 0x7F, 0x07, 0x88, 0x40, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    if (len <= 6) {
        // Unlikely we'll get a url this short, but why not ...
        memcpy (sectorbuffer1 + 9, url, len);
        sectorbuffer1[len + 9] = 0xFE;
    } else if (len == 7) {
        // 0xFE needs to be wrapped around to next block
        memcpy (sectorbuffer1 + 9, url, len);
        sectorbuffer2[0] = 0xFE;
    } else if ((len > 7) || (len <= 22)) {
        // Url fits in two blocks
        memcpy (sectorbuffer1 + 9, url, 7);
        memcpy (sectorbuffer2, url + 7, len - 7);
        sectorbuffer2[len - 7] = 0xFE;
    } else if (len == 23) {
        // 0xFE needs to be wrapped around to final block
        memcpy (sectorbuffer1 + 9, url, 7);
        memcpy (sectorbuffer2, url + 7, len - 7);
        sectorbuffer3[0] = 0xFE;
    } else {
        // Url fits in three blocks
        memcpy (sectorbuffer1 + 9, url, 7);
        memcpy (sectorbuffer2, url + 7, 16);
        memcpy (sectorbuffer3, url + 23, len - 24);
        sectorbuffer3[len - 22] = 0xFE;
    }

    // Now write all three blocks back to the card
    if (!(mifareclassic_WriteDataBlock (sectorNumber * 4, sectorbuffer1)))
        return 0;
    if (!(mifareclassic_WriteDataBlock ((sectorNumber * 4) + 1, sectorbuffer2)))
        return 0;
    if (!(mifareclassic_WriteDataBlock ((sectorNumber * 4) + 2, sectorbuffer3)))
        return 0;
    if (!(mifareclassic_WriteDataBlock ((sectorNumber * 4) + 3, sectorbuffer4)))
        return 0;

    // Seems that everything was OK (?!)
    return 1;
}


/***** Mifare Ultralight Functions ******/

/**************************************************************************/
/*!
    Tries to read an entire 4-bytes page at the specified address.

    @param  page        The page number (0..63 in most cases)
    @param  buffer      Pointer to the byte array that will hold the
                        retrieved data (if any)
*/
/**************************************************************************/
uint8_t KeyDuino::mifareultralight_ReadPage (uint8_t page, uint8_t *buffer)
{
    if (page >= 64) {
        DMSG("Page value out of range\n");
        return 0;
    }

    /* Prepare the command */
    pn532_packetbuffer[0] = PN532_COMMAND_INDATAEXCHANGE;
    pn532_packetbuffer[1] = 1;                   /* Card number */
    pn532_packetbuffer[2] = MIFARE_CMD_READ;     /* Mifare Read command = 0x30 */
    pn532_packetbuffer[3] = page;                /* Page Number (0..63 in most cases) */

    /* Send the command */
    if (HAL(writeCommand)(pn532_packetbuffer, 4)) {
        return 0;
    }

    /* Read the response packet */
    HAL(readResponse)(pn532_packetbuffer, sizeof(pn532_packetbuffer));

    /* If byte 8 isn't 0x00 we probably have an error */
    if (pn532_packetbuffer[0] == 0x00) {
        /* Copy the 4 data bytes to the output buffer         */
        /* Block content starts at byte 9 of a valid response */
        /* Note that the command actually reads 16 bytes or 4  */
        /* pages at a time ... we simply discard the last 12  */
        /* bytes                                              */
        memcpy (buffer, pn532_packetbuffer + 1, 4);
    } else {
        return 0;
    }

    // Return OK signal
    return 1;
}


/**************************************************************************/
/*!
    Tries to write an entire 4-bytes data buffer at the specified page
    address.

    @param  page     The page number to write into.  (0..63).
    @param  buffer   The byte array that contains the data to write.

    @returns 1 if everything executed properly, 0 for an error
*/
/**************************************************************************/
uint8_t KeyDuino::mifareultralight_WritePage (uint8_t page, uint8_t *buffer)
{
    /* Prepare the first command */
    pn532_packetbuffer[0] = PN532_COMMAND_INDATAEXCHANGE;
    pn532_packetbuffer[1] = 1;                           /* Card number */
    pn532_packetbuffer[2] = MIFARE_CMD_WRITE_ULTRALIGHT; /* Mifare UL Write cmd = 0xA2 */
    pn532_packetbuffer[3] = page;                        /* page Number (0..63) */
    memcpy (pn532_packetbuffer + 4, buffer, 4);          /* Data Payload */

    /* Send the command */
    if (HAL(writeCommand)(pn532_packetbuffer, 8)) {
        return 0;
    }

    /* Read the response packet */
    return (0 < HAL(readResponse)(pn532_packetbuffer, sizeof(pn532_packetbuffer)));
}


/**************************************************************************/
/*!
    @brief  Exchanges an APDU with the currently inlisted peer

    @param  send            Pointer to data to send
    @param  sendLength      Length of the data to send
    @param  response        Pointer to response data
    @param  responseLength  Pointer to the response data length
*/
/**************************************************************************/
bool KeyDuino::inDataExchange(uint8_t *send, uint8_t sendLength, uint8_t *response, uint8_t *responseLength)
{
    uint8_t i;

    DMSG_STR("\ninDataExchange");

    pn532_packetbuffer[0] = PN532_COMMAND_INDATAEXCHANGE;
    pn532_packetbuffer[1] = inListedTag;

    if (HAL(writeCommand)(pn532_packetbuffer, 2, send, sendLength)) {
        DMSG_STR("\nWrite error");
        return false;
    }

    int16_t status = HAL(readResponse)(response, *responseLength, 1000);
    if (status < 0) {
        DMSG("\nStatus error: ");
        DMSG_STR(status);
        return false;
    }

    if ((response[0] & 0x3f) != 0) {
        DMSG_STR("\nStatus code indicates an error");
        return false;
    }

    uint8_t length = status;
    length -= 1;

    if (length > *responseLength) {
        length = *responseLength; // silent truncation...
    }

    for (uint8_t i = 0; i < length; i++) {
        response[i] = response[i + 1];
    }
    *responseLength = length;

    return true;
}


/**************************************************************************/
/*!
    @brief  'InLists' a passive target. PN532 acting as reader/initiator,
            peer acting as card/responder.
*/
/**************************************************************************/
bool KeyDuino::inListPassiveTarget(uint8_t cardbaudrate, uint16_t timeout)
{
    pn532_packetbuffer[0] = PN532_COMMAND_INLISTPASSIVETARGET;
    pn532_packetbuffer[1] = 1;
    pn532_packetbuffer[2] = cardbaudrate;

    DMSG_STR("\ninList passive target");

    if(cardbaudrate == PN532_ISO14443B){
        pn532_packetbuffer[3] = 0x00;
        if (HAL(writeCommand)(pn532_packetbuffer, 4))
            return false;
    } else {
        if (HAL(writeCommand)(pn532_packetbuffer, 3))
            return false;
    }

    int16_t status = HAL(readResponse)(pn532_packetbuffer, sizeof(pn532_packetbuffer), timeout);
    if (status < 0) {
        return false;
    }

    if (pn532_packetbuffer[0] != 1) {
        return false;
    }

    inListedTag = pn532_packetbuffer[1];

    return true;
}


int8_t KeyDuino::tgInitAsTarget(const uint8_t* command, const uint8_t len, const uint16_t timeout){
  
  int8_t status = HAL(writeCommand)(command, len);
    if (status < 0) {
        return -1;
    }

    status = HAL(readResponse)(pn532_packetbuffer, sizeof(pn532_packetbuffer), timeout);
    if (status > 0) {
        return 1;
    } else if (PN532_TIMEOUT == status) {
        return 0;
    } else {
        return -2;
    }
}


/**
 * Peer to Peer
 */
int8_t KeyDuino::tgInitAsTarget(uint16_t timeout)
{
    const uint8_t command[] = {
        PN532_COMMAND_TGINITASTARGET,
        0,
        0x00, 0x00,         //SENS_RES
        0x00, 0x00, 0x00,   //NFCID1
        0x40,               //SEL_RES

        0x01, 0xFE, 0x0F, 0xBB, 0xBA, 0xA6, 0xC9, 0x89, // POL_RES
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xFF, 0xFF,

        0x01, 0xFE, 0x0F, 0xBB, 0xBA, 0xA6, 0xC9, 0x89, 0x00, 0x00, //NFCID3t: Change this to desired value

        0x06, 0x46,  0x66, 0x6D, 0x01, 0x01, 0x10, 0x00// LLCP magic number and version parameter
    };
    return tgInitAsTarget(command, sizeof(command), timeout);
}


int16_t KeyDuino::tgGetData(uint8_t *buf, uint8_t len)
{
    buf[0] = PN532_COMMAND_TGGETDATA;

    if (HAL(writeCommand)(buf, 1)) {
        return -1;
    }

    int16_t status = HAL(readResponse)(buf, len, 3000);
    if (0 >= status) {
        return status;
    }

    uint16_t length = status - 1;


    if (buf[0] != 0) {
        DMSG("status is not ok\n");
        return -5;
    }

    for (uint8_t i = 0; i < length; i++) {
        buf[i] = buf[i + 1];
    }

    return length;
}


bool KeyDuino::tgSetData(const uint8_t *header, uint8_t hlen, const uint8_t *body, uint8_t blen)
{
    if (hlen > (sizeof(pn532_packetbuffer) - 1)) {
        if ((body != 0) || (header == pn532_packetbuffer)) {
            DMSG("tgSetData:buffer too small\n");
            return false;
        }

        pn532_packetbuffer[0] = PN532_COMMAND_TGSETDATA;
        if (HAL(writeCommand)(pn532_packetbuffer, 1, header, hlen)) {
            return false;
        }
    } else {
        for (int8_t i = hlen - 1; i >= 0; i--){
            pn532_packetbuffer[i + 1] = header[i];
        }
        pn532_packetbuffer[0] = PN532_COMMAND_TGSETDATA;

        if (HAL(writeCommand)(pn532_packetbuffer, hlen + 1, body, blen)) {
            return false;
        }
    }

    if (0 > HAL(readResponse)(pn532_packetbuffer, sizeof(pn532_packetbuffer), 3000)) {
        return false;
    }

    if (0 != pn532_packetbuffer[0]) {
        return false;
    }

    return true;
}


int16_t KeyDuino::inRelease(const uint8_t relevantTarget){

    pn532_packetbuffer[0] = PN532_COMMAND_INRELEASE;
    pn532_packetbuffer[1] = relevantTarget;

    if (HAL(writeCommand)(pn532_packetbuffer, 2)) {
        return 0;
    }

    // read data packet
    return HAL(readResponse)(pn532_packetbuffer, sizeof(pn532_packetbuffer));

}


KeyDuino::KeyDuino()
{
    _serial = &Serial1;
    command = 0;
    //_interface = this;
}


void KeyDuino::begin()
{
    _serial->begin(115200);
    wakeup();
    SAMConfig();
	pinMode(BUZZER_PIN, OUTPUT);
}


void KeyDuino::wakeup()
{
    _serial->write(0x55);
    _serial->write(0x55);
    _serial->write(0);
    _serial->write(0);
    _serial->write(0);

    /** dump serial buffer */
    if(_serial->available()){
        DMSG("\nDump serial buffer: ");
    }
    while(_serial->available()){
        uint8_t ret = _serial->read();
        DMSG_HEX(ret);
    }

}


int8_t KeyDuino::writeCommand(const uint8_t *header, uint8_t hlen, const uint8_t *body, uint8_t blen)
{

    /** dump serial buffer */
    if(_serial->available()){
        DMSG("\nDump serial buffer: ");
    }
    while(_serial->available()){
        uint8_t ret = _serial->read();
        DMSG_HEX(ret);
    }

    command = header[0];
    
    _serial->write(PN532_PREAMBLE);
    _serial->write(PN532_STARTCODE1);
    _serial->write(PN532_STARTCODE2);
    
    uint8_t length = hlen + blen + 1;   // length of data field: TFI + DATA
    _serial->write(length);
    _serial->write(~length + 1);         // checksum of length
    
    _serial->write(PN532_HOSTTOPN532);
    uint8_t sum = PN532_HOSTTOPN532;    // sum of TFI + DATA

    DMSG("\nWrite: ");
    
    _serial->write(header, hlen);
    for (uint8_t i = 0; i < hlen; i++) {
        sum += header[i];

        DMSG_HEX(header[i]);
    }

    _serial->write(body, blen);
    for (uint8_t i = 0; i < blen; i++) {
        sum += body[i];

        DMSG_HEX(body[i]);
    }
    
    uint8_t checksum = ~sum + 1;            // checksum of TFI + DATA
    _serial->write(checksum);
    _serial->write(PN532_POSTAMBLE);

    return readAckFrame();
}


int16_t KeyDuino::readResponse(uint8_t buf[], uint8_t len, uint16_t timeout)
{
    uint8_t tmp[3];

    DMSG("\nRead:  ");
    
    /** Frame Preamble and Start Code */
    if(receive(tmp, 3, timeout)<=0){
        return PN532_TIMEOUT;
    }
    if(0 != tmp[0] || 0!= tmp[1] || 0xFF != tmp[2]){
        DMSG("Preamble error");
        return PN532_INVALID_FRAME;
    }
    
    /** receive length and check */
    uint8_t length[2];
    if(receive(length, 2, timeout) <= 0){
        return PN532_TIMEOUT;
    }
    if( 0 != (uint8_t)(length[0] + length[1]) ){
        DMSG("Length error");
        return PN532_INVALID_FRAME;
    }
    length[0] -= 2;
    if( length[0] > len){
        return PN532_NO_SPACE;
    }
    
    /** receive command byte */
    uint8_t cmd = command + 1;               // response command
    if(receive(tmp, 2, timeout) <= 0){
        return PN532_TIMEOUT;
    }
    if( PN532_PN532TOHOST != tmp[0] || cmd != tmp[1]){
        DMSG("Command error");
        return PN532_INVALID_FRAME;
    }
    
    if(receive(buf, length[0], timeout) != length[0]){
        return PN532_TIMEOUT;
    }
    uint8_t sum = PN532_PN532TOHOST + cmd;
    for(uint8_t i=0; i<length[0]; i++){
        sum += buf[i];
    }
    
    /** checksum and postamble */
    if(receive(tmp, 2, timeout) <= 0){
        return PN532_TIMEOUT;
    }
    if( 0 != (uint8_t)(sum + tmp[0]) || 0 != tmp[1] ){
        DMSG("Checksum error");
        return PN532_INVALID_FRAME;
    }
    
    return length[0];
}


int8_t KeyDuino::readAckFrame()
{
    const uint8_t PN532_ACK[] = {0, 0, 0xFF, 0, 0xFF, 0};
    uint8_t ackBuf[sizeof(PN532_ACK)];
    
    DMSG("\nAck: ");
    
    if( receive(ackBuf, sizeof(PN532_ACK), PN532_ACK_WAIT_TIME) <= 0 ){
        DMSG("Timeout\n");
        return PN532_TIMEOUT;
    }
    
    if( memcmp(ackBuf, PN532_ACK, sizeof(PN532_ACK)) ){
        DMSG("Invalid\n");
        return PN532_INVALID_ACK;
    }
    return 0;
}


/**
    @brief receive data .
    @param buf --> return value buffer.
           len --> length expect to receive.
           timeout --> time of reveiving
    @retval number of received bytes, 0 means no data received.
*/
int8_t KeyDuino::receive(uint8_t *buf, int len, uint16_t timeout)
{
  int read_bytes = 0;
  int ret;
  unsigned long start_millis;
  
  while (read_bytes < len) {
    start_millis = millis();
    do {
      ret = _serial->read();
      if (ret >= 0) {
        break;
     }
    } while((timeout == 0) || ((millis()- start_millis ) < timeout));
    
    if (ret < 0) {
        if(read_bytes){
            return read_bytes;
        }else{
            return PN532_TIMEOUT;
        }
    }
    buf[read_bytes] = (uint8_t)ret;
    DMSG_HEX(ret);
    read_bytes++;
  }
  return read_bytes;
}


/**
    @brief    try authentication with mifare classic default keys

    @param    sector  the sector number to authenticate to

    @returns  1 if authentication succeeded, 0 if it failed 
*/
uint8_t KeyDuino::mifareclassic_AuthenticateSectorDefaultKeys(uint8_t sector){
    uint8_t authentication;
    uint8_t key[6];
    int keyType;

    //Try each of default keys till you find one working
    for (int i = 0 ; i < DEFAULT_KEY_NUMBER ; i++) {
        for (int j = 0 ; j < 6 ; j++) {
            key[j] = this->mifareClassicDefaultKeys[i][j];
        }
	keyType = 0;
        authentication = this->mifareclassic_AuthenticateBlock(this->_uid, this->_uidLen, 4 * sector, keyType, key); //First try with A key
        if (authentication)
            break;
	//Re-read ID to allow to retry authentication
	int reAuth = this->readPassiveTargetID(PN532_ISO14443A, this->_uid, &this->_uidLen);
	keyType = 1;
        authentication = this->mifareclassic_AuthenticateBlock(this->_uid, this->_uidLen, 4 * sector, keyType, key); //Then try with B key
        if (authentication)
            break;
	reAuth = this->readPassiveTargetID(PN532_ISO14443A, this->_uid, &this->_uidLen);
    }

    if (authentication) {
        Serial.print("Authentication succeeded on sector ");
        Serial.print(sector);
        Serial.print(" with default key ");
        if(!keyType)
            Serial.print("A :");
    	else
      	    Serial.print("B :");
    	this->PrintHex(key, 6);
    	return true; 	
    }

    Serial.print("Failed authentication on sector ");
    Serial.print(sector);
    Serial.println(" with default keys.");
    return false;
}


/**
    @brief    buzz for specified duration

    @param    duration     duration of the buzz
*/
void KeyDuino::buzz(int duration){
	digitalWrite(BUZZER_PIN, HIGH);
	delay(duration);
	digitalWrite(BUZZER_PIN, LOW);
}



/**
    @brief    read and print a whole sector you authenticated to

    @param    sector  the sector number to read
*/
void MifareClassicKeyDuino::readSector(uint8_t sector){
    uint8_t reading;
    uint8_t data[16];
    for (int block = 4 * sector ; block < 4 * sector + 4 ; block++) {
        reading = this->mifareclassic_ReadDataBlock(block, data);
        if (reading) 
            this->PrintHex(data,16);
        else {
      	    Serial.print("   Failed reading block ");
      	    Serial.println(block);
    }
  }
  Serial.println();
}


/**
    @brief    authenticate with a defined key, printing the result

    @param    key      the key you are trying to authenticate with
    @param    keyType  type of key, A or B: use MIFARE_KEY_A or MIFARE_KEY_B
    @param    sector   the sector you wish to authenticate to

    @returns  1 if authentication succeeded, 0 if it failed
*/
uint8_t MifareClassicKeyDuino::authenticateDefinedKey(uint8_t key[6], int keyType, int sector){
    
    uint8_t auth = this->mifareclassic_AuthenticateBlock(this->_uid, this->_uidLen, sector * 4, keyType, key);
    
    if(auth) {
        Serial.print("Authentication succeeded on sector ");
        Serial.print(sector);
        Serial.print(" with key ");
        if(!keyType)
            Serial.print("A :");
        else
            Serial.print("B :");
        this->PrintHex(key, 6);
        return true;
    }
    else {
        Serial.print("Failed authentication on sector ");
        Serial.print(sector);
        Serial.print(" with key ");
        if(!keyType)
            Serial.print("A :");
        else
          Serial.print("B :");
        this->PrintHex(key, 6);
        Serial.println();
	//Re-read ID to allow to retry authentication
	int reAuth = this->readPassiveTargetID(PN532_ISO14443A, this->_uid, &this->_uidLen);
        return false;
  }
}


/**
    @brief    write data on a block, and prints the result

    @param    block  the block you are trying to write on
    @param    data   the data you wish to write

    @returns  1 if writing succeeded, 0 if it failed
*/
uint8_t MifareClassicKeyDuino::writeBlock(uint8_t block, uint8_t *data){
    if (this->mifareclassic_WriteDataBlock (block, data)) {
    	Serial.print("Succeeded writing on block ");
    	Serial.print(block);
    	Serial.print(" with data ");
    	this->PrintHex(data, 16);
    	Serial.println();
    } else {
    	Serial.print("Failed writing on block ");
    	Serial.println(block);
  }
}

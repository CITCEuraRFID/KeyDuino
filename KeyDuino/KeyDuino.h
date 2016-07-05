
#ifndef __KeyDuino_H__
#define __KeyDuino_H__

#include "Arduino.h"
#include <stdint.h>
#include <PN532Interface.h>

//#define DEBUG
#define KeyDuino_READ_TIMEOUT				(1000)

#define PN532_COMMAND_DIAGNOSE              (0x00)
#define PN532_COMMAND_GETFIRMWAREVERSION    (0x02)
#define PN532_COMMAND_GETGENERALSTATUS      (0x04)
#define PN532_COMMAND_READREGISTER          (0x06)
#define PN532_COMMAND_WRITEREGISTER         (0x08)
#define PN532_COMMAND_READGPIO              (0x0C)
#define PN532_COMMAND_WRITEGPIO             (0x0E)
#define PN532_COMMAND_SETSERIALBAUDRATE     (0x10)
#define PN532_COMMAND_SETPARAMETERS         (0x12)
#define PN532_COMMAND_SAMCONFIGURATION      (0x14)
#define PN532_COMMAND_POWERDOWN             (0x16)
#define PN532_COMMAND_RFCONFIGURATION       (0x32)
#define PN532_COMMAND_RFREGULATIONTEST      (0x58)
#define PN532_COMMAND_INJUMPFORDEP          (0x56)
#define PN532_COMMAND_INJUMPFORPSL          (0x46)
#define PN532_COMMAND_INLISTPASSIVETARGET   (0x4A)
#define PN532_COMMAND_INATR                 (0x50)
#define PN532_COMMAND_INPSL                 (0x4E)
#define PN532_COMMAND_INDATAEXCHANGE        (0x40)
#define PN532_COMMAND_INCOMMUNICATETHRU     (0x42)
#define PN532_COMMAND_INDESELECT            (0x44)
#define PN532_COMMAND_INRELEASE             (0x52)
#define PN532_COMMAND_INSELECT              (0x54)
#define PN532_COMMAND_INAUTOPOLL            (0x60)
#define PN532_COMMAND_TGINITASTARGET        (0x8C)
#define PN532_COMMAND_TGSETGENERALBYTES     (0x92)
#define PN532_COMMAND_TGGETDATA             (0x86)
#define PN532_COMMAND_TGSETDATA             (0x8E)
#define PN532_COMMAND_TGSETMETADATA         (0x94)
#define PN532_COMMAND_TGGETINITIATORCOMMAND (0x88)
#define PN532_COMMAND_TGRESPONSETOINITIATOR (0x90)
#define PN532_COMMAND_TGGETTARGETSTATUS     (0x8A)

#define PN532_RESPONSE_INDATAEXCHANGE       (0x41)
#define PN532_RESPONSE_INLISTPASSIVETARGET  (0x4B)


#define PN532_MIFARE_ISO14443A              (0x00)
#define PN532_ISO14443B              	    (0x03)

// Mifare Commands
#define MIFARE_CMD_AUTH_A                   (0x60)
#define MIFARE_CMD_AUTH_B                   (0x61)
#define MIFARE_CMD_READ                     (0x30)
#define MIFARE_CMD_WRITE                    (0xA0)
#define MIFARE_CMD_WRITE_ULTRALIGHT         (0xA2)
#define MIFARE_CMD_TRANSFER                 (0xB0)
#define MIFARE_CMD_DECREMENT                (0xC0)
#define MIFARE_CMD_INCREMENT                (0xC1)
#define MIFARE_CMD_STORE                    (0xC2)

// Prefixes for NDEF Records (to identify record type)
#define NDEF_URIPREFIX_NONE                 (0x00)
#define NDEF_URIPREFIX_HTTP_WWWDOT          (0x01)
#define NDEF_URIPREFIX_HTTPS_WWWDOT         (0x02)
#define NDEF_URIPREFIX_HTTP                 (0x03)
#define NDEF_URIPREFIX_HTTPS                (0x04)
#define NDEF_URIPREFIX_TEL                  (0x05)
#define NDEF_URIPREFIX_MAILTO               (0x06)
#define NDEF_URIPREFIX_FTP_ANONAT           (0x07)
#define NDEF_URIPREFIX_FTP_FTPDOT           (0x08)
#define NDEF_URIPREFIX_FTPS                 (0x09)
#define NDEF_URIPREFIX_SFTP                 (0x0A)
#define NDEF_URIPREFIX_SMB                  (0x0B)
#define NDEF_URIPREFIX_NFS                  (0x0C)
#define NDEF_URIPREFIX_FTP                  (0x0D)
#define NDEF_URIPREFIX_DAV                  (0x0E)
#define NDEF_URIPREFIX_NEWS                 (0x0F)
#define NDEF_URIPREFIX_TELNET               (0x10)
#define NDEF_URIPREFIX_IMAP                 (0x11)
#define NDEF_URIPREFIX_RTSP                 (0x12)
#define NDEF_URIPREFIX_URN                  (0x13)
#define NDEF_URIPREFIX_POP                  (0x14)
#define NDEF_URIPREFIX_SIP                  (0x15)
#define NDEF_URIPREFIX_SIPS                 (0x16)
#define NDEF_URIPREFIX_TFTP                 (0x17)
#define NDEF_URIPREFIX_BTSPP                (0x18)
#define NDEF_URIPREFIX_BTL2CAP              (0x19)
#define NDEF_URIPREFIX_BTGOEP               (0x1A)
#define NDEF_URIPREFIX_TCPOBEX              (0x1B)
#define NDEF_URIPREFIX_IRDAOBEX             (0x1C)
#define NDEF_URIPREFIX_FILE                 (0x1D)
#define NDEF_URIPREFIX_URN_EPC_ID           (0x1E)
#define NDEF_URIPREFIX_URN_EPC_TAG          (0x1F)
#define NDEF_URIPREFIX_URN_EPC_PAT          (0x20)
#define NDEF_URIPREFIX_URN_EPC_RAW          (0x21)
#define NDEF_URIPREFIX_URN_EPC              (0x22)
#define NDEF_URIPREFIX_URN_NFC              (0x23)

#define PN532_GPIO_VALIDATIONBIT            (0x80)
#define PN532_GPIO_P30                      (0)
#define PN532_GPIO_P31                      (1)
#define PN532_GPIO_P32                      (2)
#define PN532_GPIO_P33                      (3)
#define PN532_GPIO_P34                      (4)
#define PN532_GPIO_P35                      (5)

//Mifare Keys
#define DEFAULT_KEY_NUMBER 		    (8)
#define MIFARE_KEY_A			    (0)
#define MIFARE_KEY_B			    (1)


class KeyDuino: public PN532Interface { 
public:
    KeyDuino();
    
    uint8_t mifareClassicDefaultKeys[DEFAULT_KEY_NUMBER][6] = {
  	{ 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5 },
  	{ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
	{ 0xD3, 0xF7, 0xD3, 0xF7, 0xD3, 0xF7 },
  	{ 0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5 },
  	{ 0x4D, 0x3A, 0x99, 0xC3, 0x51, 0xDD },
  	{ 0x1A, 0x98, 0x2C, 0x7E, 0x45, 0x9A },
  	{ 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF },
  	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
    };

    virtual void begin();
    virtual void wakeup();
    virtual int8_t writeCommand(const uint8_t *header, uint8_t hlen, const uint8_t *body = 0, uint8_t blen = 0);
    virtual int16_t readResponse(uint8_t buf[], uint8_t len, uint16_t timeout = 1000);

    
    //virtual int8_t writeCommand(const uint8_t *header, uint8_t hlen, const uint8_t *body = 0, uint8_t blen = 0);
    //int16_t readResponse(uint8_t buf[], uint8_t len, uint16_t timeout);
    
    bool SAMConfig(void);
    uint32_t getFirmwareVersion(void);
    bool writeGPIO(uint8_t pinstate);
    uint8_t readGPIO(void);
    bool setPassiveActivationRetries(uint8_t maxRetries);

    int8_t tgInitAsTarget(uint16_t timeout = 0);
    int8_t tgInitAsTarget(const uint8_t* command, const uint8_t len, const uint16_t timeout = 0);

    int16_t tgGetData(uint8_t *buf, uint8_t len);
    bool tgSetData(const uint8_t *header, uint8_t hlen, const uint8_t *body = 0, uint8_t blen = 0);

    int16_t inRelease(const uint8_t relevantTarget = 0);

    void buzz(int duration);
	
    // ISO14443 functions
    bool inListPassiveTarget(uint8_t cardbaudrate, uint16_t timeout = 30000);
    uint8_t readTargetID(uint8_t cardbaudrate, uint8_t *uid, uint8_t *uidLength);
    bool readPassiveTargetID(uint8_t cardbaudrate, uint8_t *uid, uint8_t *uidLength, uint16_t timeout = 1000);
    bool readPassiveTargetID_B(uint8_t *uid, uint8_t *uidLength, uint16_t timeout = 1000);
    bool inDataExchange(uint8_t *send, uint8_t sendLength, uint8_t *response, uint8_t *responseLength);

    // Mifare Classic functions
    bool mifareclassic_IsFirstBlock (uint32_t uiBlock);
    bool mifareclassic_IsTrailerBlock (uint32_t uiBlock);
    uint8_t mifareclassic_AuthenticateBlock (uint8_t *uid, uint8_t uidLen, uint32_t blockNumber, uint8_t keyNumber, uint8_t *keyData);
    uint8_t mifareclassic_AuthenticateSectorDefaultKeys (uint8_t sector);
    uint8_t mifareclassic_ReadDataBlock (uint8_t blockNumber, uint8_t *data);
    uint8_t mifareclassic_WriteDataBlock (uint8_t blockNumber, uint8_t *data);
    uint8_t mifareclassic_FormatNDEF (void);
    uint8_t mifareclassic_WriteNDEFURI (uint8_t sectorNumber, uint8_t uriIdentifier, const char *url);

    // Mifare Ultralight functions
    uint8_t mifareultralight_ReadPage (uint8_t page, uint8_t *buffer);
    uint8_t mifareultralight_WritePage (uint8_t page, uint8_t *buffer);

    // Help functions to display formatted text
    static void PrintHex(const uint8_t *data, const uint32_t numBytes);
    static void PrintHexChar(const uint8_t *pbtData, const uint32_t numBytes);
    static String convertUintToString(const uint8_t *data, const uint8_t numBytes);

    uint8_t *getBuffer(uint8_t *len) {
        *len = sizeof(pn532_packetbuffer) - 4;
        return pn532_packetbuffer;
    };
    
protected:
    HardwareSerial* _serial;
    uint8_t command;
    int8_t readAckFrame();
    uint8_t _uid[7];  // ISO14443 uid
    uint8_t _uidLen;  // uid len
    uint8_t _key[6];  // Mifare Classic key
    uint8_t inListedTag; // Tg number of inlisted tag.
    uint8_t pn532_packetbuffer[64];
    int8_t receive(uint8_t *buf, int len, uint16_t timeout=KeyDuino_READ_TIMEOUT);
};

class MifareClassicKeyDuino: private KeyDuino {
public:
    using KeyDuino::begin;
    using KeyDuino::SAMConfig;
    using KeyDuino::getFirmwareVersion;
    using KeyDuino::readTargetID;
    using KeyDuino::mifareclassic_IsFirstBlock;
    using KeyDuino::mifareclassic_IsTrailerBlock;
    using KeyDuino::mifareclassic_AuthenticateBlock;
    using KeyDuino::mifareclassic_AuthenticateSectorDefaultKeys;
    using KeyDuino::mifareclassic_ReadDataBlock;
    using KeyDuino::mifareclassic_WriteDataBlock;
    using KeyDuino::mifareclassic_FormatNDEF;
    using KeyDuino::mifareclassic_WriteNDEFURI;
    using KeyDuino::PrintHex;
    using KeyDuino::PrintHexChar;

    void readSector(uint8_t sector);
    uint8_t authenticateDefinedKey(uint8_t key[6], int keyType, int sector);
    uint8_t writeBlock(uint8_t block, uint8_t *data);
};

#endif

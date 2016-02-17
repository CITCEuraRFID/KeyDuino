//This class is only useful for using the NDEF library, that require a PN532 class.
//We've chosen to do this in order to use Don's NDEF library without having to modify it or integrate it in KeyDuino library.
//Basically, this class creates a normal KeyDuin object. That's all.

#ifndef __PN532_H__
#define __PN532_H__
#include <KeyDuino.h>


class PN532: public KeyDuino {
public:
    PN532(PN532Interface &interface) : KeyDuino() {}
    using KeyDuino::KeyDuino;
};

#endif

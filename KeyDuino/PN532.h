#ifndef __PN532_H__
#define __PN532_H__
#include <KeyDuino.h>


class PN532: public KeyDuino {
public:
    PN532(PN532Interface &interface) : KeyDuino() {}
    using KeyDuino::KeyDuino;
};

#endif

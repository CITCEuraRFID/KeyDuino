## KeyDuino Library

Welcome to KeyDuino official GitHub repository!  
Here you will find all the files you need to develop programs for your KeyDuino.  
You can also look at the [wiki](https://github.com/CITCEuraRFID/KeyDuino/wiki) for informations about KeyDuino and its capabilities.  

The library is still under development, and everything is not perfect yet!  
That is why we would be very glad to hear from you about your experience with KeyDuino library ; if you encounter any problem, please feel free to open issues or contact us directly: that way we will be able to correct errors and improve the library.

### Tutorials & Projects
If you want to discover what you can do with your KeyDuino, join http://keyduino.forumsactifs.com/ !  
There you will find projects ideas, discussions, and tutorials, to help you get the best from your KeyDuino.  
If you have ideas to share, or questions to ask, don't hesitate! We would be very glad to hear from you.  

### Content
In this repository, you will find pretty much everything we develop for KeyDuino.

```
/---KeyDuino                  //The root folder of the repository  
|   +---KeyDuino              //The lib subfolder you need to put in your Arduino/libraries folder  
|   |   +---examples          //All the examples supplied within the lib  
|   |   |   +---projects      //Examples associated with KeyDuino projects  
|   |   |   +---NDEF          //Examples using the NDEF lib you need to download separately  
|   |   |   +---...  
|   |   |  
|   |   +---...  
|   |   |  
|   +---android_app           //The folder containing everything about Android application  
|   |   +---nfc_talker.apk    //The apk of the Android application we developped  
|   |   |  
|   +---hardware              //All hardware files of KeyDuino  
|   |   +---KeyDuino_board    //Hardware files for KeyDuino board  
|   |   +---antenna           //Hardware files for KeyDuino remote antenna  
```

### Compatibility
KeyDuino library is made to be used with [KeyDuino board](https://www.kickstarter.com/projects/1535978353/keyduino), but it also works by default with a classical Arduino Leonardo using a PN532 Shield, as long as communication is done through HSU.

### Contribution
KeyDuino library is based on Adafruit_NFCShield_I2C, with improvements by Seeed Studio, and HSU (High Speed Uart) driver added by Elechouse. 

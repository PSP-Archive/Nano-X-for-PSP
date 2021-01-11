Release note from http://yaleh.blogspot.com/2005/07/nano-x-for-psp.html :


Monday, July 11, 2005
Nano-X For PSP

I ported Nano-X to PSP.

Standalone mode nanox demos and microwindows demos runs on PSP now. There are still some bugs. Microwindows game "Mine" works fine. Currently, the porting can run with graphic modes of 16bit(555) and 32bit(8888). And I can debug with pspDebugScreenPrintf in 32bit mode. Pad inputs are mapped into mouse and keyboard.

It's still an unstable version. Only hackers may have interests in it. It will take weeks to make the porting stable. Then porting of Mozzilla is possible.

http://www.microwindows.org/

"The Nano-X Window System is an Open Source project aimed at bringing the features of modern graphical windowing environments to smaller devices and platforms. Nano-X allows applications to be built and tested on the Linux desktop, as well as cross-compiled for the target device."

BTW: I'll post my crt0.S and assert.c later, which necessary to build the package. They are very similar to the standard ones.
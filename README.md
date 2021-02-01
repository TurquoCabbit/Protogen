# Protogen By Turquo Cabbit and Wolfgasm

* ## Environment setting guide at /firmware/00. environment_setting

* ## Run /firmware/Face2header/APP/New_Gens.exe to add new PROTOGEN or edit exist one
    * ### Enter {Protogen name}, color code, and REMOTE theme number
    * ### Enter face color for REMOTE : R, G, and B within 0 to 255
    * ### Enter REMOTE theme number, theme can be set at /firmware/image/color_theme, follow the folder arrange and naming rule of /00. cyber_neo

* ## Then Run /firmware/Face2header/APP/Face2header.exe to generate header files which content UUID for BLE and Face bitmap
    * ### Face was been set in /firmware/Face2header/Configure/{Protogen name}/LED Matrix.xlsx
    * ### Copy "empty" worksheet to the right of "empty" to add new face
    * ### Remember to fill in face name in the worksheet
    * ### The name of worksheets don't matter however the order of them will be the order of faces in REMOTE.
    * ### !!!DO NOT ADD OR REMOVE ANY WORKSHEET AT LEFT SIDE OF "empty" INCLUDE "empty" ITSELF!!!
    * ### Delete protogen use New_Gens.exe will delete its LED Matrix.xlsx as well, be careful

* ## Program /firmware/Protogen_matrixs/Protogen_matrixs.ino in to ESP32 at PROTOGEN_HEAD
    * ### Have to configur the app space as large as posibale in ESP32 (4MByte) first

* ## Program /firmware/ProtoGun/ProtoGun.ino in to TTGO T-Display in REMOTE
    * ### Have to configur the app space as large as posibale in ESP32 (4MByte) first


* ## TO DO
    * ### PROTOGEN_HEAD
        1. #### Update mainboard PCB made by Wolfgasm
        2. #### Faceture dittering
            * #### even during blinking
        3. #### Vocal sample with amp
            * #### use LM386 for mic amp and to speaker
            * #### ADC
        4. #### Optimize cpp code structure

    * ### REMOTE
        1. #### Translate REMOTE user guide
        2. #### Optimize cpp code structure

#####  Turquo   2021/02/01

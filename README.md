# Protogen By Turquo Cabbit and Wolfgasm

* ## Run /firmware/Face2header/APP/New_Gens.exe to add new PROTOGENs
    * ### Enter name, color code, and remote theme number

* ## Then Run /firmware/Face2header/APP/Face2header.exe to generate header files whitch content UUID for BLE and Faceture bitmap
    * ### Faceture was been set in /firmware/Face2header/LED Matrix.xlsx

* ## Program /firmware/Protogen_matrixs/Protogen_matrixs.ino in to ESP32 at PROTOGEN_HEAD
    * ### Have to configur the app space as large as posibale in ESP32 (4MByte)

* ## Program /firmware/ProtoGun/ProtoGun.ino in to TTGO T-Display ESP32 at REMOTE
    * ### Have to configur the app space as large as posibale in ESP32 (4MByte)


* ## TO DO
    * ### PROTOGEN_HEAD
        1. ### Geather scrpt into fuction in Face2header.exe
        2. ### Faceture dittering
            * #### even during blinking
        3. ### Vocal sampling
            * #### use LM386 for mic amp
            * #### and to speaker

    * ### REMOTE
        1. ### Edit/Delete Protogen fuction to New_Gens.exe
        2. ### Offline operation
            1. #### saving
                * ##### only save remote/blaster related feature
            2. #### reseting
                * ##### only save remote/blaster related feature

#####  2020/12/16
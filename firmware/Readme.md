# Building the firmware

To directly compile and deploy this project you'll need the [PlatformIO IDE](https://platformio.org/platformio-ide). We only used this with the *Visual Studio Code* version, but it should also work with the *Atom* version. 

Open this folder as a workspace and then use the IDE's *build* and *upload* tools to deploy (you should be able to find them in the lower command bar). 

If you are using another IDE (such as the Arduino one), the code should still work in that case. In case of the Arduino IDE, moving the code from *main.cpp* to the corresponding *PROJECT_NAME.ino* file likely works.

We do not guarantee anything with regards to this code. We have only used it with the Adafruit Feather M0 Bluetooth, but it should also work on other boards (with the appropriate changes). 
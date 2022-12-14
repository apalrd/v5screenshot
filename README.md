# v5screenshot

Take screenshots of your VEX V5 PROS displays!

This library allows you to take a screenshot of the current display state and save it to the SD card as a TARGA file. You can then use these screenshots e.g. in your design notebook.

The library is NOT intended to be running on the robot during matches. It will take some time to save the screenshot, and the function will block during this time.

To save a screenshot, simply include `pal/screenshot.h` and call `screenshot()` with the desired filename, without extension. Files will be saved in the root of the SD card with the `.tga` extension, and you can convert them to a better format on your computer.

![Example](https://github.com/apalrd/v5screenshot/raw/main/example.png?raw=true)

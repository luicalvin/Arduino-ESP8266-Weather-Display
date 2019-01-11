# Arduino ESP8266 Weather Display 
With the Wemos D1R2 board, the Arduino st7789 library needs to be used. ST7789 is the IC for the display, this is shown on the back of the display. Only 4 pins had to be used because the BLK light is for back light which means it just turns the display on or off. By default the display is on. This might need to be used if there is a button to turn the display on / off. The setup function just connects to wifi and sets up the display. In the void loop, where it is supposed to continouly repeat the code inside, for some reason, it doesn’t get the updated data. The old data is kept despite the fact that the getWeatherData() function is inside the loop. Is this due to the board? Or the library? Or the fact that you can’t request data so often from the weatherapi and it blocks you off? The displayGettingData() function only shows up in the initla part where the device connects to the wifi. Something wrong with the if loop that checks whether or not the counter reaches 30? It was already changed to greater equal. 

Notes:
    • The D1 is the D1 that is shown on the board. 
    • There are certain functions of the displaytest test example file that causes the system to freeze and this is an issue that others have talked about on the Arduino d1r2 board. 
    • ESP8266 is the CPU that also allows the board to have wifi. 
    • The settings on the flashing of the board required is 160Mhz at 115200 baud rate with 4M. 
    
Possible changes:
    • More custom text for the display (different colors, proper spacing)
    • Need to fix the loop issue where the new data isn’t updated by itself, but needs the device to be reset first. 


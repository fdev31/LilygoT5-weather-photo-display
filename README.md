![screenshot](https://github.com/fdev31/LilygoT5-weather-photo-display/raw/main/adhoc_data/looklike.jpg)

# Features

- pretty background images (easy to replace with your own!)
- "zero configuration" Weather display
    - temperature
    - wind speed
    - cloud coverage percentage
    - rain / precipitations
- moon phase
- battery level
- easy extensions (eg: garbage pickup display, included as an example)

- (optional) 3D printed case provided

# Configuration

No configuration is really needed, the screen will create a hotspot and allow you to enter your home wifi credentials on first start or if the network can't be reached.
If you need more control, look at src/configuration.h.

# Installing

You will need *platformio* installed

    pio run -t upload


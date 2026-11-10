# DIY_Dasai_Mochi_at_home-Tu_Code
Dự án tái tạo thiết bị Dasai Mochi thông minh sử dụng vi điều khiển ESP32- v/This DIY Dasai Mochi project uses an ESP32-C3 microcontroller and an OLED display to render adorable, expressive animated character emotes.
Project highlights:
Displaying animated Mochi emotes on oled screen
Using gif2cpp (https://huykhong.com/IOT/gif2cpp) to convert GIFs into lightweight C++ arrays
Built on ESP32
Runs smooth 15 FPS animations on ILI9341 / ST7789 displays
Ideal for robotic expressions, smart assistants, or pure cuteness
What is gif2cpp?
https://huykhong.com/IOT/gif2cpp
gif2cpp is an open-source tool that converts animated GIFs into C++ header files – perfect for embedding animations in microcontroller projects where you can't use real-time video formats.
How to use:
Tutorial in my tiktok

Extract emote sections using ffmpeg: (Example commands included) → ffmpeg -ss ... -to ... -i full.mp4 ... -y emote.gif
Run gif2cpp to generate .h file
Include in your ESP32 project.
Original Video Credit:
🎥 Dasai! Mochi Gen 2 All Emotes (almost) → All emotes are inspired and extracted from this video. Full image rights belong to the original creator.

[VIETNAMESE]

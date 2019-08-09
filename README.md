# RadioLocal
 ESP32-based "internet radio" server.

It's an all you can eat buffet of spaghetti in there! This was a side project of mine and I wanted to make some kind of audio broadcasting solution for expansive LAN networks, such as those found on school campuses. Combined with the low costs of the ESP32 to run/maintain/buy, it could be an effective way to get an amature internet radio type thing going on.

While that's a nice idea, in reality, it doesn't work so well, maybe because of the implementations I used. [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer/) is a necessity as file transfer speeds without it are completely abysmal - meaning stuttery audio or low quality audio using the stock Arduino WebServer library. However, maybe either due to my implementation or due to issues with other parts in the system, the server crashes too often to be usable over a long period of time or with larger numbers of users. On the other hand, it does work for enough time to serve as a local, low-bandwith file host.

The code has been made public because I'm too lazy to want to fix it. Maybe someone else can?
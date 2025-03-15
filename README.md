# Arkanoid: Space Ball

> Note: This is a very old and unsupported project. I’ve only made minor refactoring to ensure compatibility with recent Linux, macOS, and Emscripten.

Arkanoid: Space Ball brings back the timeless fun of Arkanoid, whether you’re playing on an arcade machine or your Mac.

In this version, you pilot a small spaceship, bouncing a ball to break as many colored tiles as possible. The game offers various difficulty levels and a range of power-ups to keep the action engaging.

While the graphics and sound are fairly basic and don’t stand out from other games in the genre, Arkanoid: Space Ball does include a level generator and editor—perfect for creating custom challenges, though casual players may overlook this feature.

***

## Screenshot

![Arkanoid: Space Ball](https://camo.githubusercontent.com/a4f02b2dc646d804faa53ea7db141df8234a4b18c46e7a5bad790f4cf916ba9a/68747470733a2f2f6269746275636b65742e6f72672f616e64726579752f61726b616e6f69642d73706163652d62616c6c2f646f776e6c6f6164732f61726b616e6f69642d73706163652d62616c6c2d312e6a7067) ![Arkanoid: Space Ball](https://camo.githubusercontent.com/e6e8ca7e765ac28fb743591e157f47c6acb3c248fa1ff9a2281c1095af186228/68747470733a2f2f6269746275636b65742e6f72672f616e64726579752f61726b616e6f69642d73706163652d62616c6c2f646f776e6c6f6164732f61726b616e6f69642d73706163652d62616c6c2d322e6a7067) ![Arkanoid: Space Ball](https://camo.githubusercontent.com/f818832c9320e31b429032f31b66fe8ac2d97c281e87a19f0b675a00b555f251/68747470733a2f2f6269746275636b65742e6f72672f616e64726579752f61726b616e6f69642d73706163652d62616c6c2f646f776e6c6f6164732f61726b616e6f69642d73706163652d62616c6c2d332e6a7067)

***

## Download and build from sources

You can get a copy using git with the following command:

```sh
git clone https://github.com/reybits/arkanoid-space-ball.git
cd arkanoid-space-ball
make release # Linux or macOS
make emscripten # Web (Emscripten)
```

***

## Dependencies

`CMake`, `SDL`, `SDL-mixer`, `SDL-image`

***

```
Copyright © 2006 Andrey A. Ugolnik. All Rights Reserved.
http://www.ugolnik.info
andrey@ugolnik.info
```

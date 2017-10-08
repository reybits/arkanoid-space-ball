###Arkanoid: Space Ball###

> note: This is very old unsupported project. I just slightly refactor code to support recent Linux, macOS and Emscripten.

Arkanoid is still as fun now as it ever was. Whether on a big old arcade box or on your Mac, the game always manages to keep its fun and excitement.
Arkanoid: Space Ball puts you in control of a little spaceship, with your goal being to destroy as many colored tiles as you can by hitting a ball back and forth. Arkanoid: Space Ball comes with plenty of bonuses and varying difficulty levels.
The quality of the graphics and sound isn't amazing, and I have to admit I expected something a little better, at least to separate Arkanoid: Space Ball from other games of the same genre.
Arkanoid: Space Ball comes with a level generator and editor, good for coming up with your own challenges, but most players will probably not matter with it.

***
##Screenshot##

![Arkanoid: Space Ball](https://bitbucket.org/andreyu/arkanoid-space-ball/downloads/arkanoid-space-ball-1.jpg)
![Arkanoid: Space Ball](https://bitbucket.org/andreyu/arkanoid-space-ball/downloads/arkanoid-space-ball-2.jpg)
![Arkanoid: Space Ball](https://bitbucket.org/andreyu/arkanoid-space-ball/downloads/arkanoid-space-ball-3.jpg)

***
##Download and build from sources##

You can get a copy using git with the following command:

```bash
$ git clone https://bitbucket.org/andreyu/arkanoid-space-ball.git
$ cd arkanoid-space-ball
$ make release # Linux or macOS
$ make emscripten # Web (Emscripten)
```

***
##Dependencies##

`CMake`, `SDL`, `SDL-mixer`, `SDL-image`

***
```
Copyright © 2006 Andrey A. Ugolnik. All Rights Reserved.
http://www.ugolnik.info
andrey@ugolnik.info
```

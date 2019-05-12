# Attack of the Obnoxious, Hangry, Puss-filled Multi-Dimensional Mutant Lasagne Monsters (starring Walter Chambers!)
**Lasagne Monsters** is a 2D platform game.

![Alt text](dev/screenshots/v0.3-03.png?raw=true "v0.3 screenshot")

This game is still in development and as such it may exit suddenly or crash. More screenshots can be found in the dev/screenshots folder. If the game crashes on startup, try deleting the save game data (see below).

## Compiling
This game requires SDL 2.0 (along with SDL Mixer, SDL Image, and SDL TTF). Compile by running make and then ./lasagneMonsters to run the game.

## Controls
* [A] - Move left
* [D] - Move right
* [I] - Jump
* [J] - Fire
* [L] - Use
* [Escape] - Menu

## Save data
Save game data is stored in ~/.local/share/lasagneMonsters. If the game crashes on start up, try deleting the game.json and config.json files and trying again. This may happen from time to time until the game reaches v1.0.

## Extended config
Some options won't be found in the main game UI, but can be accessed and tweaked the game's ~/.local/share/config.json file.

* deadzone: Specifies deadzone of the joystick. This can be used to control the sensitivity of the joystick. Increase the value if you experience drift or jitter. Takes a value between 0 and 127.

## Licenses
### Source code
Source code (src and tools) is license under GPL 3.0

### Graphics and data
Graphics (gfx) and data (data) are licensed under Creative Commons BY-NC-SA 3.0, with the following attribution: Copyright 2019, Parallel Realities | www.parallelrealities.co.uk.

<img src="gfx/by-nc-sa.png?raw=true" alt="CC BY-NC-SA logo" width="125">

### Music
#### (to be completed)

### Sound effects
#### (to be completed)

### Font
* EnterCommand.ttf, by jeti - https://www.dafont.com/enter-command.font (CC-BY 4.0)

### About
This game was made by Parallel Realities. You can find out more at https://www.parallelrealities.co.uk

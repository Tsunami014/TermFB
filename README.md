# A Terminal File Browser
This is my first completed project in C, please don't judge too harshly.

If you find any bugs it's probably your fault (e.g. accessing forbidden folders, mashing the keyboard unecessarily). And if it's running slowly then it's because you're on Windows because on my Linux partition it worked magnificently.

## Features
- Neat terminal interface
    - Adjusts for terminal size (each time an input is pressed, that is)
    - Can have any amount of columns (but currently only 2 are used)
- Display all files and folders in the current directory
- Can move between directories nicely
- List of directories set in a config file to the side for quick access
- Help screen!
- Filter files and folders by typing!

## AI Usage
As I have barely used C before this, I used chatGPT for learning how to do stuff; basically just a second Google. I did take entire chunks of code for platform specific things, those being getting a single character input, reading folders, setting the terminal to the right modes and finding a configuration directory; all this because I was crazy enough to not have any external libraries. But I wrote over quite a lot of that, and everything else was all of my own creation.


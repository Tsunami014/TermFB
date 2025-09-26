# A Terminal File Browser
This is my first completed project in C, please don't judge too harshly.

<img width="1352" height="349" alt="Screenshot 2025-09-19 113710" src="https://github.com/user-attachments/assets/13c8cfd8-4dfe-4c24-b853-65f905f320cd" />

And turns out if it's slow to start I'm pretty sure that's microsoft defender.

## Features
- **WORKS ON WINDOWS AND LINUX**
- No external dependencies to install (a main reason for how long it took)
- Neat terminal interface
    - Adjusts for terminal size (each time an input is pressed, that is)
    - Can have any amount of columns (but currently only 2 are used)
- Display all files and folders in the current directory
- Can move between directories nicely
- List of directories set in a config file to the side for quick access
- Help screen!
- Filter files and folders by typing!
- Run commands on files! (See help for more info)

## How to use
You could just use the executable normally, but that means if you exit the program you will return back to the directory you were before in the terminal. Let's fix that.

Doing what is stated below means running the command `tfb` in the terminal uses the terminal file browser and switches directories properly!
**This is not necessary**, it just makes using the script in production a little easier.
### Linux
In wherever you'd like (e.g. `~.bashrc`) have this alias:
```
alias tfb='cd "$(./termfb 3>&1 1>&2)"'
```
(change the `./termfb` to wherever your output file is located)

### Windows
Because Windows is... Windows, I could not figure out an easy way to accomplish this for both powershell and command prompt. ~~Sucks to be you.~~ You *can* make some powershell function, but that's your problem.

But to anyone who wants to make something for this, every time you exit this program it saves the path to a file `%temp%/termfb_dir.txt`.

## AI Usage
As I have barely used C before this, I used chatGPT for learning how to do stuff; basically just a second Google. I did take entire chunks of code for platform specific things, those being getting a single character input, reading folders, setting the terminal to the right modes and finding a configuration directory; all this because I was crazy enough to not have any external libraries. But I wrote over quite a lot of that, and everything else was all of my own creation.


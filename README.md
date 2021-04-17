# Seagull

## Interactive Conway's GoL

Written in C and linked with the ncurses terminal ui library, manually click on a Cartesian coordinate in 1 of 6 possible windows to initiate a live cell. Then press a key (that key has not been determined) and watch the CA simulation.

### **Current State**
Currently, you can install and run the simulation on a small terminal window without command-line arguments. This is the basic functionality as this project is in its infancy.

### **TODO**
*   Add interactivity
*   stop infinite simulation (no command-line arg) when CA becomes stagnant
*   Embed python using python/c API for easier pattern creation
*   concurrent resizing windows
*   command-line options
*   extend ncurses and include prompts, menus, etc
*   debug
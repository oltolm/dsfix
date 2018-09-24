DSfix Developer Readme
______________________
This file is intended for developers who want to improve DSfix or use it as a base for other modifications.
All the source code is released under the conditions of the GPLv3, except for:
- SuperFastHash
- SMAA
which have their own licensing terms included.
Note that the code base is currently in quite a terrible state. 
"TODO.txt" lists some of the things that should be done to improve it in the "Cleanup" section.
Also note that I would prefer for no one except myself to make binary releases of DSfix for now, 
simply to keep things easier to track for people and reduce confusion.
Additionally, if you start working on some feature, I'd appreciate if you contacted me, so that we don't duplicate effort.
- Peter
Requirements
____________
- Visual C++
- The DirectX SDK
- Microsoft Detours (Express) 3
- DSfix must be compiled in the Release/Win32 config to work with the game
File Overview
_____________
- The "DATA" folder contains the files for distribution, including .inis, effects & textures
- "TODO.txt" contains a list of open & closed TODOs, look here if you want to find something to work on
- "main.*" includes the main function & a few utilities
- The "d3d9*" files implement d3d wrapping
- The "dinput*" files are very straightforward dinput wrapping
- "Detouring.*" files implement function overriding using the Detours library
- "KeyActions.*" files implement keybindings, together with the Xmacro files "Keys.def" & "Actions.def"
- "SaveManager.*" files implement save backup management
- "WindowManager.*" files implement window management (cursor hiding & capturing, borderless fullscreen)
- "RenderstateManager.*" is where most of the magic happens, implements detection & rerouting of the games' rendering pipeline state
- "SMAA.*", "VSSAO.*", "GAUSS.*" & "Hud.*" are effects optionally used during rendering (derive from the base Effect)
- "Textures.def" is a database of known texture hashes
Standardisation
_______________
Introduction
____________
With a view towards improving file size/general code legibility as well as ensuring that the code all flows together as one; it becomes clear that guidelines need to be created in order to promote these values. please feel free to discuss these choices & make changes as & when needed to ensure that the largest number possible find the code legible & easy to use whilst also performing at its best.
__________
Sectioning
__________
When doing this please use underscores as they join together, creating a smooth, neat, connected line. ensure they only underline the length of the word & only use them above & below the section header to avoid confusion. done this way a section header need not have an upper gap. each section header should only take up one line, with an additional line above and below for the underscores.
___________________
Commenting:one line
___________________
Please comment using "//". using "/*" & "*/" for just one line adds unnecessary extra characters & is easier to break accidentally. do not leave a space before or after the mark used to denote a comment.
____________________
Commenting:two lines
____________________
Please comment "/*" & "*/". using "//" for multiple lines adds unnecessary extra characters. if required, "*" may be used to align text on lines below the topmost line along with a preceding space however only do this if necessary, otherwise do not use any spaces or other such characters for alignment. do not leave spaces around the comment marks.
___________
Empty lines
___________
Put simply, don't. it is rare that this would even be required.
_______
Quoting
_______
Please follow the author's example by using ""
_____
Lists
_____
Please follow the author's example by creating a section header (where applicable) & subsequently preceding each list item with "- ". when unable to use a section header please write a statement ending in ":".
______________________
Writing the word "and"
______________________
Please use "&". if it is required during a quote you may use "and"
______
Colons
______
Do not add a space following them.
__________
Signatures
__________
Please follow the the authors example by preceding a signature with "- "

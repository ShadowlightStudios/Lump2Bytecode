# Lump2Bytecode

Compiles a JSON file into a LMP file for the Shadowlight Engine (https://github.com/ShadowlightStudios/shadowlight-engine). 
It's still a WIP, it only reads from a fixed file, and only supports a few types of lumps.

# Usage

For now, just run the program after compiling it with Visual Studio, and it'll compile a LMP file from the 
JSON file that's directory is currently hard-coded into the source.

# To-do:

Add multiple files.
Add drag-and-drop, so that the JSON file is no longer hard-coded.
Add the rest of the lump-types.
Add double-checking to the sub-lumps, so that it can check that the lump's files actually exist.

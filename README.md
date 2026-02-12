<h1 align="center">RTX Remix Compatibility Codebase</h1>

<br>

<div align="center" markdown="1"> 

A codebase that can be used for compatibility mods for NVIDIA's [RTX Remix](https://github.com/NVIDIAGameWorks/rtx-remix).  

If you want to support my work,   
consider buying me a [Coffee](https://ko-fi.com/xoxor4d) or by becoming a [Patreon](https://patreon.com/xoxor4d)


Feel free to join the discord server: https://discord.gg/FMnfhpfZy9

<br>

</div>


# Overview
This repository contains a codebase that can be used as a starting point for a potential RTX Remix Compatibility Mod.  
It is not a generic fix, nor does it make a game compatible on its own. Any such functionality must be implemented by users themselves.

#### It features:

- A hooked D3D9 interface, with every function detoured for easy access and interception
- Logic to aid with drawcall modifications
- A basic ImGui menu for debugging purposes

The codebase includes [Ultimate ASI Loader](https://github.com/ThirteenAG/Ultimate-ASI-Loader/releases/tag/v9.7.0), which is used to load the Compatibility Mod itself.

<br>

## Documentation / Guides

Please see: https://github.com/xoxor4d/remix-comp-base/tree/master/documentation

<br>

## Compiling
- Clone the repository `git clone --recurse-submodules https://github.com/xoxor4d/remix-comp-base.git`
- Optional: Setup a global path variable named `REMIX_COMP_ROOT` that points to your game folder
  & `REMIX_COMP_ROOT_EXE` which includes the exe name of your game.
- Run `generate-buildfiles_vs22.bat` to generate VS project files
- Compile the mod

- Copy everything inside the `assets` folder into the game directory.  
  You may need to rename the Ultimate ASI Loader file if your game does not import `dinput8.dll`.

  > [!TIP]  
  > Determining which DLLs your game imports on startup is fairly straightforward, but I won’t go into detail here.  
  > I recommend using [Explorer Suite by NTCore](https://ntcore.com/explorer-suite/).

- If you did not setup the global path variable:  
  Move the `asi` file into a folder called `plugins` inside your game directory.

<br>

##  Credits
- [NVIDIA - RTX Remix](https://github.com/NVIDIAGameWorks/rtx-remix)
- [People of the showcase discord](https://discord.gg/j6sh7JD3v9) - especially the nvidia engineers ✌️
- [Dear ImGui](https://github.com/ocornut/imgui)
- [minhook](https://github.com/TsudaKageyu/minhook)
- [Ultimate-ASI-Loader](https://github.com/ThirteenAG/Ultimate-ASI-Loader)
- All 🍓 Testers

<div align="center" markdown="1"> 

And of course, all my fellow Ko-Fi and Patreon supporters  
and all the people that helped along the way!

</div>

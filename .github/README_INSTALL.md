
<br>

##### > Usage instructions are further down ...

# Installation
0. If you've used hopelessness' mod (ModDB) before:  
   - delete the `FX` folder
   - delete the `plugins` / `scripts` folder
   - remove `NFSC Remix.exe`
 
1. Download <LINK_TO_MOD_ZIP>
2. Open the zip and extract all files contained inside the `NFSC-Remix-CompatibilityMod` folder into your NFSC directory (next to the `NFSC.exe`). Overwrite all when prompted.

<br>

> [!NOTE] 
> This release also comes with a proper remix-mod (`nfsc_comp`) containing PBR Materials, Lights and other overrides.
> It is heavy wip and only covers a small section of the map currently.

<br>

> [!WARNING] 
> **IF** you use remix mod: https://www.moddb.com/mods/nfsc-remix-01-beta/downloads/nfsc-remix-012  
> - **ONLY** extract `nfsc_remix` from `to the main\rtx-remix\mods\` to `your_game\rtx-remix\mods\`  
> - Do **NOT** extract anything else

<br>


```
.  
├─ ...
├─ 📁 Need for Speed Carbon
│  ├── 📜 NFSC.exe
│  ├── 📜 d3d9.dll
│  ├── 📜 dinput8.dll
│  ├── 📜 nfsc-rtx.asi
│  ├── 📜 ...
│  │
│  ├── 📁 plugins
│  ├── 📁 rtx_comp
│  └── 📁 rtx-remix
│      └─📁 mods
│        └─📁 nfsc_comp
│          ├── 📜 comp_cars.usda
│          ├── 📜 comp_effects.usda
│          ├── 📜 mod.usda
│          └── ...
└── ...  
```

<br>

# Usage and general Info
- Run the game using NFSC.exe

  > Press Alt + X to open the Remix menu
  > Press F4 to open the Compatibility Mod menu
  
### Resolution
1. Open `plugins/NFSCarbon.WidescreenFix.ini`
2. Change ResX / ResY to desired resolution
3. Change `WindowedMode = 2` to 1 for borderless fullscreen (or keep it windowed)

### Baked lighting / vertex colors
- If you want baked, stylized lighting on world meshes:  
  F4 -> comp settings -> rendering settings -> `Enable Vertex Colors on World Meshes`

<br>

-----

> [!NOTE]  
> The release includes a custom [Remix Runtime](https://github.com/xoxor4d/dxvk-remix/tree/game/nfsc_3) build that contains a few necessary changes. 

#### Category Changes:
- Beam Category: Enables Vertex Colors
- Anti Culling Category: Disables custom car shader
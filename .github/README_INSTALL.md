# Installation
1. Download <LINK_TO_MOD_ZIP>
2. Open the zip and extract all files contained inside the `NFSC-Remix-CompatibilityMod` folder into your NFSC directory (next to the `NFSC.exe`). Overwrite all when prompted.

> [!WARNING] 
> Recommended remix mod: https://www.moddb.com/mods/nfsc-remix-01-beta/downloads/nfsc-remix-012  
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

<br>

> [!NOTE]  
> The release includes a custom [Remix Runtime](https://github.com/xoxor4d/dxvk-remix/tree/game/nfsc_1) build that contains a few necessary changes

#### Category Changes:
- Beam Category: Enables Vertex Colors
- Anti Culling Category: Disables custom car shader

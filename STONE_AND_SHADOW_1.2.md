# Horror Castle v1.2 — Stone & Shadow

## Visual strategy

Stone & Shadow uses a hybrid renderer rather than a single flat screenshot:

1. raster architectural plates derived from the approved Horror Castle concept;
2. procedural JUCE stone/rune/glow drawing;
3. live JUCE controls and text above both layers.

The goal is to make controls feel mounted into the Castle rather than simply themed with dark colours.

### Runtime plates

`Assets/StoneShadow/`

- `header.png`
- `crypt_frame.png`
- `tower_frame.png`
- `center_spine.png`
- `ritual_grave_frame.png`
- `hex_frame.png`
- `inspector_frame.png`
- `backdrop.png`
- `undercroft.png`

These plates contain architecture and atmosphere, not the live parameter labels/values.

## Room structure

### CASTLE
CRYPT + TOWER -> RITUAL -> GRAVE, with HEX and Curse Inspector below.

### UNDERCROFT
Secondary/advanced machinery:
- Spectral Corpse altar
- CRYPT/TOWER filter drive
- Cross FM / Cross Ring
- Possession Matrix
- Rituals

### GRIMOIRE
Preset / spell browser with 40 factory spells.

## Spectral Corpse

The attached Spectral Corpse Alpha is integrated as CRYPT Chamber II and retains its existing generator index for preset compatibility. Stone & Shadow exposes all four parameters in the UNDERCROFT and adds dedicated regression renders.

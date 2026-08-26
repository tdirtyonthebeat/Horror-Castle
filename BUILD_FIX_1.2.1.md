# Horror Castle v1.2 — Stone & Shadow AppleClang Build Fix

## Symptom

AppleClang stopped in `HorrorCastleEditor.cpp` on two calls to `juce::Graphics::fillRect` in the UNDERCROFT accent-line rendering.

The arguments mixed `float` coordinates from `Rectangle<float>` with integer width/height literals, leaving both JUCE overloads viable:

- `fillRect(float, float, float, float)`
- `fillRect(int, int, int, int)`

## Fix

The two calls now use explicit floating-point literals for all four arguments:

```cpp
g.fillRect(area.getX() + 20.0f,  area.getY() + 42.0f, 350.0f, 1.0f);
g.fillRect(area.getX() + 690.0f, area.getY() + 42.0f, 350.0f, 1.0f);
```

This is a UI-only compile fix. No DSP, state IDs, factory spell values, or Spectral Corpse algorithms were changed.

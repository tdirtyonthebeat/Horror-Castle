# Horror Castle v0.12.4 — Ritual Compile Fix

The v0.12.3 Mac compile proved the new Ritual algorithms compile successfully.
The build then failed in `CastleEngine.cpp` because the v0.12.2 FM Depth patch typed a helper lambda as `Generator`, while Horror Castle's active architecture type is `GeneratorSlot`.

Fixed:

```cpp
auto modShape = [&](const GeneratorSlot& gen) { ... };
```

No Ritual DSP was removed or changed in this patch.

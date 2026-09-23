# Setting 6 WebAssembly build folder

This folder contains the setting-6-only v2 build target, separate from the old
`web/slot.wasm`. Settings 1-5 and 7 remain non-playable placeholders.

From the repository root:

```bash
bash tools/check-setting6-v2.sh
bash tools/build-wasm-v2.sh
node tools/verify-setting6-wasm.mjs
```

The build script writes `src/slot-v2/wasm/slot-v2.wasm`; it includes
`reel/stop/reel_candidate.cpp` and the new `special/chain_zone.cpp`.

After the runtime and WASM smoke check both succeed, a hosting build
may copy that file to `web/slot-v2.wasm`. **Do not** overwrite
`web/slot.wasm` or infer that the legacy browser app automatically uses v2.

There is no claim of 114% measured payout here. Native and WASM smoke tests
check execution of the setting-6 code, not full-game machine payout.

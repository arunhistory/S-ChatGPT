# Setting policy

## Setting layout

| ID | Role |
|---:|---|
| 1 | Japanese-compliance profile (lowest / L-style tuning may be used) |
| 2 | Japanese-compliance profile |
| 3 | Japanese-compliance profile |
| 4 | Japanese-compliance profile |
| 5 | Japanese-compliance profile |
| 6 | Highest-performance profile that is tuned to Japanese pachislot technical/regulatory targets |
| 7 / EX | Exhibition / personal-use full-spec profile; current game design baseline |

## Design rule

- EX is the source design for the intended game feel and full performance.
- Settings 1-6 reuse the same game rules and state machine, but their probabilities / payout parameters may be reduced or adjusted to meet the Japanese target profile.
- Setting 6 should preserve EX behavior as much as practical while staying inside the Japanese target envelope.
- EX is not used as the compliance reference profile.
- Current runtime defaults to EX until settings 1-6 receive their final tuned parameter sets.

## Current EX payout targets

- Lower AT: approximately +6 medals/game net.
- Middle AT: approximately +6 medals/game net.
- Upper AT: approximately +9 medals/game net.

## Implementation note

Do not fork the game engine per setting. Keep one state machine and select a setting parameter profile. This allows the same normal-mode, table, omen, CZ, bonus, AT, stock, favorable-section, and reel-control logic to be shared across settings.


## Setting 6 first-pass profile

Setting 6 is derived from EX while keeping the same game rules.

| Parameter | EX | Setting 6 first pass |
|---|---:|---:|
| CZ aggregate target | ~1/350 | ~1/370 |
| Hit aggregate target | ~1/400 | ~1/430 |
| Initial AT target | ~1/500 | ~1/550 |
| Lower AT net | +6 | +6 |
| Middle AT net | +6 | +6 |
| Upper AT net | +9 | +9 |
| Stock lottery | 10% | 8% |
| Upper comeback | 20% | 15% |
| Initial AT games mean | 50G | 49.25G |
| Ordinary add mean | 31G | 24.4G |
| Upper-special expected add | ~690G | ~167G |

The Setting 6 raw normal-route values are provisional calibration knobs until the full normal-state lottery model is integrated.
Settings 1-5 remain intentionally untuned.

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

## Target payout ratios

These are calibration targets, not measured results.

| Setting | Target payout ratio |
|---:|---:|
| 1 / L | 85% |
| 2 | 96% |
| 3 | 99% |
| 4 | 103% |
| 5 | 108% |
| 6 | 114% |
| 7 / EX | 150% |

Setting 6 is the highest Japanese-target profile. EX is the unconstrained personal/exhibition profile and is not part of the Japanese-compliance target set.

## Current EX payout targets

- Lower AT: approximately +6 medals/game net.
- Middle AT: approximately +6 medals/game net.
- Upper AT: approximately +9 medals/game net.

## Implementation note

Do not fork the game engine per setting. Keep one state machine and select a setting parameter profile. This allows the same normal-mode, table, omen, CZ, bonus, AT, stock, favorable-section, and reel-control logic to be shared across settings.


## Settings 1-5 calibrated profiles

Current completed-logic calibration check from 3,000,000 games x 5 deterministic seeds:

| Setting | Target | Measured mean |
|---:|---:|---:|
| 1 / L | 85% | 85.01% |
| 2 | 96% | 95.83% |
| 3 | 99% | 98.73% |
| 4 | 103% | 103.03% |
| 5 | 108% | 108.12% |
| 6 | 114% | 113.63% |
| 7 / EX | 150% | 150.80% |

Setting 1/L keeps the global five-regular-hit rescue rule. Its 85% target is achieved by suppressing optional AT conversion, continuation and growth rather than disabling that rescue. Settings 2-6 progressively increase AT conversion, stock, comeback and AT-event strength without forking the state machine.

## Current calibration candidate

Setting 6 and EX share the same core game rules, initial AT-game distribution, ordinary add distribution, and net rates (+6 / +6 / +9). Their probability profiles differ.

| Parameter | Setting 6 | EX |
|---|---:|---:|
| Long-run target | 114% | 150% |
| Raw CZ route | 1/440 | 1/414 |
| Raw bonus route | 1/600 | 1/548 |
| Raw AT route | 1/1200 | 1/1065 |
| Bonus stock lottery | 12% | 14.5% |
| Upper comeback | 22% | 25.5% |
| Lower AT hit base | 1/180 | 1/164 |
| Lower AT add base | 1/240 | 1/203 |
| Lower AT special base | 1/600 | 1/502 |
| Upper-special base | 1/4000 | 1/3075 |
| Middle/upper event scale | 2.10x | 2.37x |

Upper-special continuation is no longer hard-capped. The whole continuation chain is pre-decided at entry (lever-style predetermined result) using an unbounded geometric continuation process. Settings 1-6 use p=0.7543 (about 4.07 chains / about 167G expected add) and EX uses p=0.94055 (about 16.82 chains / about 690G expected add).

Implemented core logic now includes the six rare roles, strong-cherry guaranteed reward split, five consecutive 9-medal-bell AT, actual/display game separation, high-probability entry/exit and shortening, segment cold treatment, AT table transition matrix, +2400 favorable-section cut, stock-level section reward handling, CZ/bonus/AT/stock logic, and unbounded upper special.

The section-cut reward rates for stock levels 0/1/3/5 are currently configurable provisional values 0% / 25% / 60% / 100%. The user had fixed the four stock thresholds and reward behavior (tier-up in lower/middle, upper-special in upper) but had not fixed the exact percentage table.

Strong-chance has its fixed 1/180 role frequency and high-probability behavior. A separate 1% direct-reward destination was discussed but never finalized, so no destination is fabricated in the engine.

### Reproducible payout check

Run:

`tools/check-payout.sh 5000000`

This compiles the native simulator without GitHub Actions and runs all settings across five deterministic seeds. The simulator reports both the target payout ratio and measured payout ratio.

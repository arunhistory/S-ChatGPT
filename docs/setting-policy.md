# S-ChatGPT v2: setting profile policy (2026-09-23)

## Playable profiles

Only **setting 6** is implemented. Settings 1–5 and 7 retain their numeric
IDs and shared engine shape but have **no odds, payout calibration or playable
runtime state**. `resetWithSetting` rejects their selection as
`NotImplemented` without mutating the running game. Older
`src/slot_engine.cpp` and `web/slot.wasm` are legacy prototypes, **not** a
measurement or build of this setting-6 design.

## Approved setting 6: normal play

Draws are on separate 2^27 boxes in AT > bonus > CZ priority order. Later
thresholds compensate for earlier wins. The observed, ceiling-EXCLUDED basic
outcomes target:

| Outcome | Basic observed rate | Threshold / 134217728 |
|---|---:|---:|
| CZ | 1/1150 | 116824 |
| Ordinary bonus | 1/1700 | 78981 |
| Direct lower AT | 1/2650 | 50648 |

All normal modes, ceiling tables, rare-role direct hits, CZ rescue and
five-consecutive-nine-medal-bell rescue remain part of the shared state machine.
Preliminary ceiling-INCLUDED odds from a separate approximate model were
CZ ~1/551, normal bonus ~1/506, AT ~1/670, bonus+AT ~1/288;
**those are estimates, not runtime measurements**.

## Approved setting 6: AT

- Lower tier: approximate AT event total **1/80**.
- Middle and upper tiers: approximate AT event total **1/50**.
- Fall: separate fixed **1/400** for all tiers.
- New chain special zone: lower **1/500**, middle **1/450**, upper **1/400**,
  counted **inside** the AT event totals.
- Scale other AT event frequencies proportionally, keeping all four
  table-specific rate ratios and relative event weights. Preserve the
  existing 60% cold entry / 70% growth rate on cold starts. Global rates
  are statistical targets dependent on table occupancy, not a claim of
  identical odds in each AT table or during every cold/warm interval.
- Lower/middle AT net +6 per game, upper +9.
- Keep lower Fall's 50% predetermined PUSH revival, generic 5G revival,
  specials, upper comeback and existing bonus/stock/section mechanisms.

Withdrawn: independent 1/100 AT +5–50G reward and independent 1/100
normal-play 15-medal small hit. Neither may appear in setting 6.

## Validation / open items

114% is the **target**, not an achieved or legally certified payout. A complete
payout run must be measured from real total bets and payouts after the
remaining subsystems are wired. Unspecified rules must not be silently invented:
normal Penguin ZONE / Kingdom ZONE entry odds, any still-unapproved 30–100
medal reward weighting, and remaining mismatches between v2 and design notes.
Do not borrow earlier setting-specific legacy performance calibration.

No GitHub Actions loop or simulator deployment is required for this branch.

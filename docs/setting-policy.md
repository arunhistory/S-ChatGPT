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
  counted **inside** the AT event totals and unchanged during cold.
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

## Implemented v2 AT event normalization

One 2^27 mutually-exclusive event draw is shared by the existing six AT
events and the new chain special zone. The existing non-Fall event rates
share a single scalar per tier, preserving their inter-event ratios in
each table and relative table strengths. Fall remains separate 1/400.
For the stationary table weights (7/16,4/16,1/16,4/16), including a
60%-cold population with 70% cold growth on ordinary events,
but FIXED new-zone rates even while cold, approximate scalars are
lower 1.107812, middle 0.911673 and upper 0.897428. All three are
estimates for the corresponding 1/80 and 1/50 *statistical* averages,
not guarantees of actual aggregate odds.

## Chain special zone implemented in v2

The chain zone uses repeated full 5G sets; an early signal is latched
until the end of the current set. Each ordinary bell/replay role is
accepted with probability 8/9, giving approximately 1/10 combined
continuation per special-zone game at existing base role frequencies.
Each pair of successful set continuations earns one bonus. When the
zone ends, earned bonuses release through the original BONUS entry
mechanism immediately and sequentially in batches of up to four.
For a batch of 2, 3, or 4, a single bonus becomes an Episode with
5%, 7.5%, or 10% chance. Ordinary independent 1% completion upgrade
is suppressed for multi-batch bonuses to preserve those odds; single
bonuses keep the existing generic rule. Core AT ST is frozen during
the zone and entry/reward sequence. The UI-specific presentation is
still a separate, uncompleted deliverable.

Penguin ZONE / Kingdom ZONE entry probabilities and 30–100 medal
non-normal bonus reward weights are not finalized and remain disabled,
rather than inventing numbers.

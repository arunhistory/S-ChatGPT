# Setting 6 only — approved design checkpoint (2026-09-23)

This page overrides older design/calibration tables in README.md and
docs/setting-policy.md **for current development of setting 6**.
Settings 1–5 and EX/7 are not being retuned in this phase.

## Confirmed setting 6 base normal lotteries

All are baseline reward odds per eligible normal-game draw, **excluding**
ceiling, CZ reward, rare-role direct entry, special direct entry and
other rescues. The live v2 normal lottery prioritizes AT > Bonus > CZ;
the thresholds below compensate for that masking, so the *observed raw
outcomes* (when this lottery is actually called) match the intended odds.

| Route | Approved odds | 2^27 raw threshold |
|:--|--:|--:|
| CZ | 1/1,150 | 116824 |
| Normal bonus | 1/1,700 | 78981 |
| Lower AT direct | 1/2,650 | 50648 |

The independent, approximate normal-only simulation in the design
conversation suggested CZ ~1/551, bonus ~1/506, AT ~1/670, and
bonus+AT ~1/288 after ceilings/other routes. These are **not measured
from the live v2 runtime** and are not fixed lottery thresholds.

## Shared AT plan — applies to every future setting

The user has locked the **same AT gameplay rules for all settings**.
During this setting-6-only implementation phase, the approved rates are:

| Item | Lower | Middle | Upper |
|:--|--:|--:|--:|
| AT positive-event aggregate (excluding Fall; including new chain zone) | 1/80 | 1/50 | 1/50 |
| Fall (separate lottery region) | 1/400 | 1/400 | 1/400 |
| New five-game bonus chain zone (inside positive aggregate) | 1/500 | 1/450 | 1/400 |
| Pure net medals per AT game | +6 | +6 | +9 |

The `at/at_setting6_plan.hpp` constexpr design scales each existing
positive event proportionally, separately for each tier × table,
and reserves the chain-zone probability *within* the aggregate.
No episode events are added to tables that formerly had none. Fall
stays independent of the positive-event aggregate. Cold state may
reduce the actual positive frequency during runtime; nominal aggregate
is defined for a non-cold AT game.

**Implementation status:** the setting-6 raw normal thresholds and
upper pure net rate are wired in the v2 source. The shared AT event plan
is calculated and unit-tested but deliberately **not connected to the
live v2 AT lottery** yet. The separate five-game chain-zone state
machine has also been implemented and independently tested, including
up to four earned hits. The external 1/10 role-signal source and the
immediate multi-bonus release into the live bonus pipeline still need
to be connected. Do not call the current live AT odds 1/80 and 1/50 yet.

### Unchanged mechanics

- Initial AT = 100G; pre-existing table transitions and their ratios.
- Lower Fall success: fixed internal 50%, two wait games and PUSH,
  successful revival preserves the pre-Fall G count.
- Full-end five-game role-based revival; upper 64G comeback.
- Existing ordinary and upper special zones, section transition rules,
  stock and bonus features, normal ceiling/mode/shortening systems.
- New chain-zone set: 5G; combined continuation signal 1/10 per G;
  after each two successful set continuations, add one regular
  bonus-hit stock; release earned hits immediately after that
  event, up to four together; conditional episode upgrade odds for
  two/three/four simultaneous hits: 5%, 7.5%, 10%.
- The 30–100 medal bonus weights and Penguin / Kingdom zone entry
  probabilities are not established by the latest approved checkpoint;
  do not invent production probabilities.

### Specifically cancelled

- An extra independent AT 1/100 mini-G add.
- A normal-time 1/100 15-medal small hit.
- Prior proposals to lighten setting-6 normal odds to
  1/220 / 1/250 / 1/300 and similar variants.

## Measurement and release gate

114% is the intended setting-6 whole-machine return. It has **not**
been verified on the currently edited v2 runtime. Previously
published figures from the old `slot_engine` calibration or
approximate standalone reimplementations are not valid measurements
of this new specification.

Complete the five-game chain zone, verify normal+AT account ledgers
including replay and reel payouts, and simulate long, identical-seed
setting-6 samples; then measure against 114%. This checkpoint makes
no claim of legal/regulatory certification.

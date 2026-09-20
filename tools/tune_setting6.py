#!/usr/bin/env python3
# Setting 6 calibration harness for S-ChatGPT.
# This is OFFLINE tuning only; it does not drive the browser runtime.
# Provisional assumptions are marked below and must not be treated as locked game rules.

import random
from dataclasses import dataclass

SPINS = 10_000_000
SEED = 20260920

ROLE_P = {
    "one": 4/5,
    "bell9": 1/15,
    "bell15": 1/80,
    "replay": 1/30,
    "weak_cherry": 1/150,
    "strong_cherry": 1/1000,
    "watermelon": 1/100,
    "weak_chance": 1/90,
    "strong_chance": 1/180,
    "penguin": 1/500,
}
ROLE_P["miss"] = 1.0 - sum(ROLE_P.values())

# Locked high-probability entry rates (normal state, actual games > 50).
HIGH_ENTRY = {
    "bell15": .20,
    "watermelon": .15,
    "weak_cherry": .80,
    "weak_chance": .30,
    "strong_chance": .90,
    "miss": .005,
}

# Locked in-high hit rates.
HIGH_HIT = {
    "bell15": .30,
    "watermelon": .20,
    "weak_cherry": .80,
    "weak_chance": .50,
    "strong_chance": .99,
    "miss": .001,
}

# Provisional table-dependent fake-omen rates selected by calibration.
# 100G development itself is locked at 80%.
FAKE_RATE = {"A": .93, "B": .84, "H": .70, "SH": .58}

# First-pass calibration result. Aggregate target:
# CZ ~=1/350, hit ~=1/400, initial AT ~=1/500.
RAW_CZ = 1/455
RAW_HIT = 1/1350
RAW_AT = 1/9000

TABLES = ("A", "B", "H", "SH")
TABLE_W = (.45, .35, .15, .05)
CEILINGS = {
    "A": (500,700,750,900,1000,1100,1250,1350,1450,1500),
    "B": (250,300,400,500,600,700,750,800,1000,1250),
    "H": (100,200,250,300,400,500,600,700,750,750),
    "SH": (50,50,100,100,200,200,250,250,300,300),
}

ROLES = tuple(ROLE_P)
CUM = []
_acc = 0.0
for r in ROLES:
    _acc += ROLE_P[r]
    CUM.append(_acc)

@dataclass
class M:
    normal_spins: int = 0
    cz: int = 0
    hit: int = 0
    at: int = 0
    direct_at: int = 0
    high_entries: int = 0
    high_hits: int = 0
    short_hits: int = 0
    cycle_devs: int = 0
    cycle_true: int = 0
    ceilings: int = 0

def role(rng):
    x = rng.random()
    for r, c in zip(ROLES, CUM):
        if x < c:
            return r
    return "miss"

def table(rng):
    x = rng.random()
    a = 0.0
    for name, w in zip(TABLES, TABLE_W):
        a += w
        if x < a:
            return name, rng.choice(CEILINGS[name])
    return "SH", rng.choice(CEILINGS["SH"])

def run(spins=SPINS, seed=SEED):
    rng = random.Random(seed)
    m = M()
    current_table, ceiling = table(rng)
    actual = display = 0
    high = False
    high_age = 0
    continuous_window = 0
    bell9_streak = 0
    cz_misses = 0
    hit_at_misses = 0

    def reset_normal():
        nonlocal current_table, ceiling, actual, display, high, high_age
        nonlocal continuous_window, bell9_streak
        current_table, ceiling = table(rng)
        actual = display = 0
        high = False
        high_age = 0
        continuous_window = 0
        bell9_streak = 0

    def resolve_hit():
        nonlocal hit_at_misses
        m.hit += 1
        if rng.random() < .34 or hit_at_misses >= 4:
            m.at += 1
            hit_at_misses = 0
        else:
            hit_at_misses += 1

    def resolve_cz():
        nonlocal cz_misses
        m.cz += 1
        resolved = False
        for _ in range(10):
            if rng.random() < 1/1000:
                m.at += 1
                cz_misses = 0
                resolved = True
                break
            if rng.random() < 1/100:
                resolve_hit()
                cz_misses = 0
                resolved = True
                break
        if not resolved:
            cz_misses += 1
            if cz_misses >= 3:
                cz_misses = 0
                resolve_hit()

    while m.normal_spins < spins:
        m.normal_spins += 1
        actual += 1
        display += 1
        r = role(rng)

        bell9_streak = bell9_streak + 1 if r == "bell9" else 0
        if bell9_streak >= 5:
            m.at += 1
            m.direct_at += 1
            reset_normal()
            continue

        if r == "strong_cherry" and rng.random() < .5:
            m.at += 1
            m.direct_at += 1
            reset_normal()
            continue

        # Fixed rare direct routes: freeze, upper AT, middle AT+stock.
        x = rng.random()
        if x < 1/134_217_728 + 1/32_768 + 1/8_192:
            m.at += 1
            m.direct_at += 1
            reset_normal()
            continue

        if rng.random() < RAW_AT:
            m.at += 1
            reset_normal()
            continue
        if rng.random() < RAW_HIT:
            resolve_hit()
            reset_normal()
            continue
        if rng.random() < RAW_CZ:
            resolve_cz()
            reset_normal()
            continue

        extra = 0

        # Provisional interpretation of continuous shortening:
        # both bell types qualify; a bell within 10G adds uniform 5..100 and
        # resets the 10G continuation window.
        if continuous_window > 0:
            if r in ("bell9", "bell15"):
                extra += rng.randint(5, 100)
                continuous_window = 10
            else:
                continuous_window -= 1

        terminal = None
        if high:
            high_age += 1
            if rng.random() < HIGH_HIT.get(r, 0.0):
                m.high_hits += 1
                y = rng.random()
                if y < .005:
                    terminal = "hit"
                elif y < .05:
                    terminal = "cz"
                else:
                    m.short_hits += 1
                    z = rng.random()
                    if z < .80:
                        # Provisional uniform distribution inside locked 5..50 range.
                        extra += rng.randint(5, 50)
                    elif z < .95:
                        # Provisional uniform distribution inside locked 20..200 range.
                        extra += rng.randint(20, 200)
                    else:
                        continuous_window = 10

            # Locked: minimum 5G, then 1/10 drop every game.
            if terminal is None and high_age >= 5 and rng.random() < .10:
                high = False
                high_age = 0
        else:
            if actual > 50 and rng.random() < HIGH_ENTRY.get(r, 0.0):
                high = True
                high_age = 0
                m.high_entries += 1

        display += extra

        # Locked: 80% development at every crossed displayed 100G boundary.
        # If it develops, whether it is fake/true is table-dependent.
        before = display - 1 - extra
        boundary = (before // 100 + 1) * 100
        cycle_terminal = None
        while boundary <= display:
            if rng.random() < .80:
                m.cycle_devs += 1
                if rng.random() > FAKE_RATE[current_table]:
                    m.cycle_true += 1
                    q = rng.random()
                    cycle_terminal = "cz" if q < .20 else ("at" if q < .40 else "hit")
                    break
            boundary += 100

        if terminal is None:
            terminal = cycle_terminal

        if terminal is not None:
            if terminal == "cz":
                resolve_cz()
            elif terminal == "hit":
                resolve_hit()
            else:
                m.at += 1
            reset_normal()
            continue

        # Existing provisional non-special ceiling result weights.
        if display >= ceiling:
            m.ceilings += 1
            q = rng.random()
            if q < .70:
                resolve_cz()
            elif q < .95:
                resolve_hit()
            else:
                m.at += 1
            reset_normal()

    return m

if __name__ == "__main__":
    m = run()
    def den(n):
        return float("inf") if n == 0 else m.normal_spins / n

    print(f"normal_spins={m.normal_spins}")
    print(f"CZ={m.cz} 1/{den(m.cz):.2f}")
    print(f"HIT={m.hit} 1/{den(m.hit):.2f}")
    print(f"AT={m.at} 1/{den(m.at):.2f}")
    print(f"high_entries={m.high_entries}")
    print(f"high_hits={m.high_hits}")
    print(f"short_hits={m.short_hits}")
    print(f"cycle_developments={m.cycle_devs}")
    print(f"cycle_true={m.cycle_true}")
    print(f"ceilings={m.ceilings}")

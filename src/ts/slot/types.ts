export type ReelId = 0 | 1 | 2;
export type ReelPosition = number;

export const enum SpecialHit {
  None = 0,
  MiddleATStock = 1,
  UpperAT = 2,
  Freeze = 3,
}

export const enum RoleFlag {
  None = 0,
  Miss,
  OneMedal,
  Bell9,
  Bell15,
  Replay,
  WeakCherry,
  StrongCherry,
  Watermelon,
  WeakChance,
  StrongChance,
  PenguinChance,
}

export interface LeverResult {
  special: SpecialHit;
  role: RoleFlag;
  mainLotteryRan: boolean;
}

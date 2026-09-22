export const enum NormalMode {
  NormalA = 0,
  NormalB = 1,
  Heaven = 2,
  SuperHeaven = 3,
  Special = 4,
}

export const enum ATEventBit {
  Hit = 1 << 0,
  Fall = 1 << 1,
  AddGames = 1 << 2,
  Special = 1 << 3,
  Episode = 1 << 4,
  UpperSpecial = 1 << 5,
}

export const enum ATResolutionStatus {
  None = 0,
  Single = 1,
  Multiple = 2,
}

export const enum ATResolvedEvent {
  None = 0,
  Hit = 1,
  Fall = 2,
  AddGames = 3,
  Special = 4,
  Episode = 5,
  UpperSpecial = 6,
}

export const enum PendingEvent {
  BellFiveAT = 1 << 0,
  CZThreeMissHit = 1 << 1,
  NextHitAT = 1 << 2,

  ATHit = 1 << 3,
  ATFall = 1 << 4,
  ATAddGames = 1 << 5,
  ATSpecial = 1 << 6,
  ATEpisode = 1 << 7,
  ATUpperSpecial = 1 << 8,
  ATMultiple = 1 << 9,

  SectionTierUp = 1 << 10,
  SectionSpecial = 1 << 11,
  SectionUpperSpecial = 1 << 12,

  CZHit = 1 << 13,
  ATWindowEmpty = 1 << 14,
  ATStockAvailable = 1 << 15,
}

export const enum SectionRewardKind {
  None = 0,
  TierUp = 1,
  Special = 2,
  UpperSpecial = 3,
}

export const enum CZFinalizeOutcome {
  None = 0,
  HitPending = 1,
  MissReturnNormal = 2,
  ThreeMissHitPending = 3,
}

export const enum ATWindowStatus {
  NotApplicable = 0,
  Running = 1,
  EmptyNoStock = 2,
  EmptyStockAvailable = 3,
}

export const enum ATTier {
  Lower = 0,
  Middle = 1,
  Upper = 2,
}

export const enum MachineArea {
  Normal = 0,
  CZ = 1,
  Bonus = 2,
  AT = 3,
}

export const enum CommandStatus {
  Ok = 0,
  RejectedPhase = 1,
}

export const enum SessionPhase {
  Idle = 0,
  Stopping = 1,
  SpecialPending = 2,
  Complete = 3,
}

export const enum EntryTarget {
  None = 0,
  MiddleAT = 1,
  UpperAT = 2,
}

export const enum StockProfile {
  None = 0,
  Middle = 1,
  Upper = 2,
}

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

export const enum StopStatus {
  Ok = 0,
  StripUndefined = 1,
  NoLegalCandidate = 2,
  InvalidReel = 3,
  SpecialControlPending = 4,
  RoleMissed = 5,
  SubstituteStop = 6,
  AssistGap = 7,
}

export interface LeverResult {
  commandStatus: CommandStatus;
  special: SpecialHit;
  role: RoleFlag;
  mainLotteryRan: boolean;
}

export interface SpecialResult {
  hit: SpecialHit;
  target: EntryTarget;
  stockProfile: StockProfile;
  freeze: boolean;
}

export interface StopResult {
  finalPosition: ReelPosition;
  slip: number;
  status: StopStatus;
}


export const enum AcquisitionStatus {
  NotReady = 0,
  Acquired = 1,
  Missed = 2,
  NoPayoutRole = 3,
  Substitute = 4,
  AssistGap = 5,
}

export interface AcquisitionResult {
  internalRole: RoleFlag;
  status: AcquisitionStatus;
  medals: number;
}


export const enum Symbol {
  Unknown = 0,
  Red7,
  Blue7,
  Bar,
  Bell,
  Replay,
  Cherry,
  Watermelon,
  Penguin,
  Snow,
}

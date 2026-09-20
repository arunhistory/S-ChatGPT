#include <stdint.h>
#include <stddef.h>

extern "C" void* memset(void* dst, int c, size_t n) {
    unsigned char* p = (unsigned char*)dst;
    for (size_t i=0;i<n;++i) p[i]=(unsigned char)c;
    return dst;
}
extern "C" void* memcpy(void* dst, const void* src, size_t n) {
    unsigned char* d=(unsigned char*)dst; const unsigned char* s=(const unsigned char*)src;
    for(size_t i=0;i<n;++i)d[i]=s[i]; return dst;
}
extern "C" void* memmove(void* dst, const void* src, size_t n) {
    unsigned char* d=(unsigned char*)dst; const unsigned char* s=(const unsigned char*)src;
    if(d<s){for(size_t i=0;i<n;++i)d[i]=s[i];} else if(d>s){for(size_t i=n;i>0;--i)d[i-1]=s[i-1];}
    return dst;
}

namespace slot {
static constexpr uint32_t RNG_SPACE = 134217728u;
static constexpr uint32_t ROLE_BELL9 = 8947849u;
static constexpr uint32_t ROLE_BELL15 = 1677722u;
static constexpr uint32_t ROLE_REPLAY = 4473924u;
static constexpr uint32_t ROLE_ONE = 107374182u;
static constexpr uint32_t ROLE_BASE = ROLE_BELL9+ROLE_BELL15+ROLE_REPLAY+ROLE_ONE;
static constexpr uint32_t ROLE_WEAK_CHERRY=894785u,ROLE_STRONG_CHERRY=134218u,ROLE_WATERMELON=1342177u;
static constexpr uint32_t ROLE_WEAK_CHANCE=1491308u,ROLE_STRONG_CHANCE=745654u,ROLE_PENGUIN=268435u;
static constexpr uint32_t ROLE_REMAINDER = RNG_SPACE-ROLE_BASE;

enum NormalTable : uint8_t { NormalA, NormalB, Heaven, SuperHeaven, Special };
enum Tier : uint8_t { Lower, Middle, Upper };
enum ATTable : uint8_t { ATNormal, ATHeaven, ATSuperHeaven, Specialized };
enum Role : uint8_t { Miss, OneMedal, Bell9, Bell15, Replay, WeakCherry, StrongCherry, Watermelon, WeakChance, StrongChance, PenguinChance };
enum EvType : uint8_t { EvNone, CZ, Bonus, EpisodeBonus, ATStart, ATAddGames, SpecialZone, UpperSpecialZone, StockGain, TierUp, TierDown, ATEnd, UpperComeback, Freeze, SectionCross, HighEnter, HighExit, Shorten, ColdEnter, SectionReward };
struct Event { EvType type; int value; const char* note; };
struct Events { Event e[256]; int n; void clear(){n=0;} void add(EvType t,int v,const char* s){if(n<256)e[n++]={t,v,s};} };
struct State {
    int setting;
    NormalTable normal_table;
    int normal_pattern;
    int normal_actual_games;
    int normal_display_games;
    int normal_ceiling;
    bool special_window_checked;
    bool high_active;
    int high_games;
    bool cold_at;
    bool cold_bonus;
    int cz_misses;
    int bonus_at_misses;
    int bell9_streak;
    bool in_at;
    Tier tier;
    ATTable at_table;
    int at_pattern;
    int at_games_left;
    int stocks;
    int64_t section_diff, section_min_diff, total_diff, section_count, total_games;
    Role last_role;
    int last_payout;
};
static State s;
static uint64_t rng=0x5343484154475054ULL;
static char jsonbuf[65536];

static uint64_t next64(){
    uint64_t x=rng; if(!x)x=0x9E3779B97F4A7C15ULL;
    x^=x>>12; x^=x<<25; x^=x>>27; rng=x; return x*2685821657736338717ULL;
}
static double u01(){ return (double)(next64()>>11) * (1.0/9007199254740992.0); }
static bool chance(double p){ return p>=1.0 || (p>0.0 && u01()<p); }
static int clampi(int v,int lo,int hi){return v<lo?lo:(v>hi?hi:v);}
struct Profile{double rawAT,rawBonus,rawCZ,stock,comeback,hit,fall,add,spec,upper,scale,bonusAT;int bonusMissCeiling;bool fullUpper;};
static Profile profile(){
    switch(s.setting){
        case 1:return{1.0/5000.0,1.0/136.94,1.0/700.0,.01102,.02255,1.0/873.79,1.0/172.51,1.0/1046.62,1.0/2494.0,1.0/16921.27,1.1204,.0851,999,false};
        case 2:return{1.0/1465.08,1.0/697.79,1.0/478.97,.08541,.16082,1.0/221.63,1.0/362.96,1.0/348.13,1.0/810.64,1.0/5860.31,1.72869,.28164,5,false};
        case 3:return{1.0/1393.28,1.0/675.52,1.0/471.33,.09285,.17570,1.0/211.59,1.0/373.40,1.0/327.10,1.0/764.66,1.0/5464.91,1.7856,.3014,5,false};
        case 4:return{1.0/1331.24,1.0/652.50,1.0/461.25,.09949,.18874,1.0/201.00,1.0/388.72,1.0/302.47,1.0/704.95,1.0/5037.12,1.84988,.31747,5,false};
        case 5:return{1.0/1259.57,1.0/620.62,1.0/447.82,.10974,.20460,1.0/190.24,1.0/404.15,1.0/270.57,1.0/641.17,1.0/4433.65,1.97736,.33217,5,false};
        case 6:return{1.0/1200.0,1.0/600.0,1.0/440.0,.12,.22,1.0/180.0,1.0/420.0,1.0/240.0,1.0/600.0,1.0/4000.0,2.10,.34,5,false};
        default:return{1.0/1065.0,1.0/548.0,1.0/414.0,.145,.255,1.0/164.0,1.0/452.0,1.0/203.0,1.0/502.0,1.0/3075.0,2.37,.34,5,true};
    }
}

static const char* normalName(NormalTable v){switch(v){case NormalA:return"normal_a";case NormalB:return"normal_b";case Heaven:return"heaven";case SuperHeaven:return"super_heaven";case Special:return"special";}return"unknown";}
static const char* tierName(Tier v){switch(v){case Lower:return"lower";case Middle:return"middle";case Upper:return"upper";}return"unknown";}
static const char* atTableName(ATTable v){switch(v){case ATNormal:return"normal";case ATHeaven:return"heaven";case ATSuperHeaven:return"super_heaven";case Specialized:return"specialized";}return"unknown";}
static const char* roleName(Role v){switch(v){case Miss:return"miss";case OneMedal:return"one_medal";case Bell9:return"bell9";case Bell15:return"bell15";case Replay:return"replay";case WeakCherry:return"weak_cherry";case StrongCherry:return"strong_cherry";case Watermelon:return"watermelon";case WeakChance:return"weak_chance";case StrongChance:return"strong_chance";case PenguinChance:return"penguin_chance";}return"miss";}
static const char* evName(EvType v){switch(v){case EvNone:return"none";case CZ:return"cz";case Bonus:return"bonus";case EpisodeBonus:return"episode_bonus";case ATStart:return"at_start";case ATAddGames:return"at_add_games";case SpecialZone:return"special_zone";case UpperSpecialZone:return"upper_special_zone";case StockGain:return"stock_gain";case TierUp:return"tier_up";case TierDown:return"tier_down";case ATEnd:return"at_end";case UpperComeback:return"upper_comeback";case Freeze:return"freeze";case SectionCross:return"section_cross";case HighEnter:return"high_enter";case HighExit:return"high_exit";case Shorten:return"shorten";case ColdEnter:return"cold_enter";case SectionReward:return"section_reward";}return"unknown";}

static int initialGames(){
    static const int g[9]={20,30,40,50,75,100,150,200,300};
    static const double w[9]={.20,.25,.20,.15,.08,.06,.03,.025,.005};
    double r=u01(),a=0;for(int i=0;i<9;++i){a+=w[i];if(r<a)return g[i];}return 300;
}
static int addGames(){
    static const int g[6]={10,20,30,50,100,200};
    static const double w[6]={.30,.30,.20,.12,.06,.02};
    double r=u01(),a=0;for(int i=0;i<6;++i){a+=w[i];if(r<a)return g[i];}return 200;
}
static int specialAdd(){ static const int g[6]={20,30,40,50,100,200}; static const double w[6]={.30,.25,.20,.15,.08,.02}; double r=u01(),a=0;for(int i=0;i<6;++i){a+=w[i];if(r<a)return g[i];}return 200; }
static int upperChains(){double p=profile().fullUpper?.94055:.7543;int n=1;while(chance(p))++n;return n;}
static int weakShorten(){static const int g[6]={5,10,15,20,25,50};static const double w[6]={.20,.25,.25,.15,.10,.05};double r=u01(),a=0;for(int i=0;i<6;++i){a+=w[i];if(r<a)return g[i];}return 50;}
static int strongShorten(){static const int g[6]={20,50,75,100,150,200};static const double w[6]={.15,.25,.25,.20,.10,.05};double r=u01(),a=0;for(int i=0;i<6;++i){a+=w[i];if(r<a)return g[i];}return 200;}
static int continuousShorten(){static const int g[6]={5,20,40,60,80,100};int total=0;bool again=true;while(again){total+=g[next64()%6];again=false;for(int i=0;i<10;++i){if(chance(1.0/15.0)){again=true;break;}}}return total;}
static void rerollAT(){double r=u01();if(s.at_table==ATHeaven||s.at_table==ATSuperHeaven){if(r<.30)s.at_table=ATNormal;else if(r<.55)s.at_table=ATHeaven;else if(r<.75)s.at_table=ATSuperHeaven;else s.at_table=Specialized;}else{if(r<.50)s.at_table=ATNormal;else if(r<.75)s.at_table=ATHeaven;else s.at_table=Specialized;}s.at_pattern=(int)(next64()%5);}
static int chooseCeiling(NormalTable m,int p){static const int A[10]={500,700,750,900,1000,1100,1250,1350,1450,1500};static const int B[10]={250,300,400,500,600,700,750,800,1000,1250};static const int H[10]={100,200,250,300,400,500,600,700,750,750};static const int SH[10]={50,50,100,100,200,200,250,250,300,300};p=clampi(p,0,9);switch(m){case NormalA:return A[p];case NormalB:return B[p];case Heaven:return H[p];case SuperHeaven:return SH[p];case Special:return chance(.95)?777:1500;}return 1500;}
static void rerollNormal(){double r=u01();if(r<.45)s.normal_table=NormalA;else if(r<.80)s.normal_table=NormalB;else if(r<.95)s.normal_table=Heaven;else s.normal_table=SuperHeaven;s.normal_pattern=(int)(next64()%10);s.normal_actual_games=0;s.normal_display_games=0;s.special_window_checked=false;s.high_active=false;s.high_games=0;s.cold_bonus=false;s.bell9_streak=0;s.normal_ceiling=chooseCeiling(s.normal_table,s.normal_pattern);}
static bool highUnlocked(){return s.normal_actual_games>50;}
static Role drawRole(){uint32_t d=(uint32_t)(next64()&(RNG_SPACE-1u));if(d<ROLE_ONE)return OneMedal;d-=ROLE_ONE;if(d<ROLE_BELL9)return Bell9;d-=ROLE_BELL9;if(d<ROLE_BELL15)return Bell15;d-=ROLE_BELL15;if(d<ROLE_REPLAY)return Replay;d-=ROLE_REPLAY;if(d<ROLE_WEAK_CHERRY)return WeakCherry;d-=ROLE_WEAK_CHERRY;if(d<ROLE_STRONG_CHERRY)return StrongCherry;d-=ROLE_STRONG_CHERRY;if(d<ROLE_WATERMELON)return Watermelon;d-=ROLE_WATERMELON;if(d<ROLE_WEAK_CHANCE)return WeakChance;d-=ROLE_WEAK_CHANCE;if(d<ROLE_STRONG_CHANCE)return StrongChance;d-=ROLE_STRONG_CHANCE;if(d<ROLE_PENGUIN)return PenguinChance;return Miss;}
static int prefLevel(){return s.stocks>=5?3:s.stocks>=3?2:s.stocks>=1?1:0;}
static void sectionDelta(int64_t v,Events& out){s.section_diff+=v;s.total_diff+=v;if(s.section_diff<s.section_min_diff)s.section_min_diff=s.section_diff;if(s.section_diff>=2400){int p=prefLevel();s.stocks=0;++s.section_count;s.section_diff=0;s.section_min_diff=0;out.add(SectionCross,p,"6.5 section cut; stocks collapsed to next-section preference level");rerollAT();}}
static void startAT(Tier t,bool stock,Events& out,const char* why){s.in_at=true;s.tier=t;s.at_games_left=initialGames();if(stock)++s.stocks;rerollAT();out.add(ATStart,s.at_games_left,why);}
static void playBonus(Events& out);
static void playCZ(Events& out){bool resolved=false;for(int g=0;g<10&&!resolved;++g){++s.total_games;sectionDelta(-3,out);if(chance(1.0/1000.0)){startAT(Lower,false,out,"CZ direct AT");s.cz_misses=0;resolved=true;}else if(chance(1.0/100.0)){out.add(Bonus,50,"CZ bonus hit");playBonus(out);s.cz_misses=0;resolved=true;}}if(!resolved){++s.cz_misses;if(s.cz_misses>=3){s.cz_misses=0;out.add(Bonus,50,"CZ 3-miss ceiling -> bonus");playBonus(out);}}}
static void playBonus(Events& out){Profile p=profile();bool was=s.in_at;sectionDelta(50,out);if(chance(p.stock)){++s.stocks;out.add(StockGain,s.stocks,"bonus stock lottery");}if(chance(.01)){sectionDelta(80,out);out.add(EpisodeBonus,80,"1% post-bonus episode promotion");if(chance(1.0/3.0)){if(was){if(s.tier==Lower){s.tier=Middle;out.add(TierUp,0,"episode 1/3 -> middle AT");}}else{startAT(Middle,false,out,"episode 1/3 middle AT");s.bonus_at_misses=0;rerollNormal();return;}}}if(was)return;if(chance(p.bonusAT)||s.bonus_at_misses>=p.bonusMissCeiling-1){s.bonus_at_misses=0;startAT(Lower,false,out,"bonus performance cleared -> AT");}else ++s.bonus_at_misses;rerollNormal();}
static void resolveCeiling(Events& out){if(s.normal_table==Special){if(s.normal_ceiling==777){int r=(int)(next64()%3);if(r==0)startAT(Lower,false,out,"special 777 ceiling: AT");else if(r==1)startAT(Middle,true,out,"special 777 ceiling: middle AT + stock");else startAT(Upper,false,out,"special 777 ceiling: upper AT");}else{out.add(Freeze,0,"special 1500 ceiling: freeze-favored");startAT(Upper,true,out,"special 1500 ceiling freeze reward");}rerollNormal();return;}double r=u01();if(r<.70){out.add(CZ,0,"normal ceiling -> CZ");playCZ(out);}else if(r<.95){out.add(Bonus,50,"normal ceiling -> bonus");playBonus(out);}else{startAT(Lower,false,out,"normal ceiling -> AT");rerollNormal();}}
static void specialZoneRun(bool upper,Events& out){if(!upper){for(int g=0;g<5;++g){if(!chance(.5))continue;if(chance(.95)){int x=specialAdd();s.at_games_left+=x;out.add(ATAddGames,x,"special zone chained add");}else{out.add(Bonus,50,"special zone bonus");playBonus(out);}}return;}int chains=upperChains();for(int c=0;c<chains;++c){if(chance(.95)){int x=specialAdd();s.at_games_left+=x;out.add(ATAddGames,x,"upper-special preset add");}else{out.add(Bonus,50,"upper-special preset bonus");playBonus(out);}}}
static void endAT(Events& out);
static void resolveATEvent(Events& out){Profile p=profile();double scale=s.tier==Lower?1.0:p.scale;double hit=p.hit*scale,fall=s.tier==Lower?p.fall:(s.tier==Middle?1.0/180.0:1.0/150.0),add=p.add*scale,spec=p.spec*scale,epi=(1.0/1000.0)*scale,up=p.upper*scale;if(s.at_table==ATHeaven){hit*=1.9;epi*=1.4;}else if(s.at_table==ATSuperHeaven){add*=2.4;epi*=1.25;}else if(s.at_table==Specialized){hit*=.55;add*=.55;spec*=2.8;up*=2.5;}static const double pfv[5]={.82,.92,1.0,1.10,1.20};double pf=pfv[clampi(s.at_pattern,0,4)];hit*=pf;add*=pf;spec*=pf;bool ea=s.at_table==ATHeaven||s.at_table==ATSuperHeaven;bool ua=s.at_table==Specialized;double total=hit+fall+add+spec+(ea?epi:0)+(ua?up:0);double x=u01();if(x>=total)return;if((x-=hit)<0){out.add(Bonus,50,"AT normal hit -> 50 medal bonus");playBonus(out);return;}if((x-=fall)<0){if(s.tier==Middle&&chance(.5)){s.tier=Lower;out.add(TierDown,0,"middle fall -> lower AT");}else if(s.tier==Upper&&chance(.5))endAT(out);return;}if((x-=add)<0){int g=addGames();s.at_games_left+=g;out.add(ATAddGames,g,"one-shot add");return;}if((x-=spec)<0){out.add(SpecialZone,0,"special zone");specialZoneRun(false,out);return;}if(ea&&(x-=epi)<0){sectionDelta(80,out);out.add(EpisodeBonus,80,"direct episode in heaven/super-heaven");if(s.tier==Lower&&chance(1.0/3.0)){s.tier=Middle;out.add(TierUp,0,"episode 1/3 -> middle AT");}return;}if(ua&&(x-=up)<0){out.add(UpperSpecialZone,0,"upper special zone");specialZoneRun(true,out);}}
static void endAT(Events& out){Profile p=profile();if(s.stocks>0){--s.stocks;s.at_games_left=initialGames();rerollAT();out.add(ATStart,s.at_games_left,"stock activated; initial-game lottery; table/pattern re-roll");return;}if(s.tier==Upper){s.in_at=false;if(chance(p.comeback)){s.in_at=true;s.tier=Upper;s.at_games_left=initialGames();rerollAT();out.add(UpperComeback,64,"64G comeback success -> upper AT restart");}else{out.add(ATEnd,64,"64G comeback failed");rerollNormal();}return;}s.in_at=false;out.add(ATEnd,0,"AT ended");rerollNormal();}

struct Writer{char* p;int n,cap;Writer(char* b,int c):p(b),n(0),cap(c){if(cap)p[0]=0;}void ch(char c){if(n+1<cap){p[n++]=c;p[n]=0;}}void str(const char* x){while(*x)ch(*x++);}void i64(int64_t v){if(v==0){ch('0');return;}if(v<0){ch('-');v=-v;}char t[32];int k=0;while(v&&k<31){t[k++]=(char)('0'+v%10);v/=10;}while(k)ch(t[--k]);}void q(const char*x){ch('"');while(*x){char c=*x++;if(c=='"'||c=='\\'){ch('\\');ch(c);}else if(c=='\n'){str("\\n");}else ch(c);}ch('"');}};
static const char* stateJSON(){Writer w(jsonbuf,sizeof(jsonbuf));w.str("{\"setting\":");w.i64(s.setting);w.str(",\"normalMode\":");w.q(normalName(s.normal_table));w.str(",\"normalPattern\":");w.i64(s.normal_pattern+1);w.str(",\"normalGames\":");w.i64(s.normal_display_games);w.str(",\"normalActualGames\":");w.i64(s.normal_actual_games);w.str(",\"normalDisplayGames\":");w.i64(s.normal_display_games);w.str(",\"highProbabilityUnlocked\":");w.str(highUnlocked()?"true":"false");w.str(",\"normalCeiling\":");w.i64(s.normal_ceiling);w.str(",\"inAT\":");w.str(s.in_at?"true":"false");w.str(",\"atTier\":");w.q(tierName(s.tier));w.str(",\"atTable\":");w.q(atTableName(s.at_table));w.str(",\"atPattern\":");w.i64(s.at_pattern+1);w.str(",\"atGamesLeft\":");w.i64(s.at_games_left);w.str(",\"stocks\":");w.i64(s.stocks);w.str(",\"sectionDiff\":");w.i64(s.section_diff);w.str(",\"sectionMinDiff\":");w.i64(s.section_min_diff);w.str(",\"sectionCount\":");w.i64(s.section_count);w.str(",\"totalDiff\":");w.i64(s.total_diff);w.str(",\"totalGames\":");w.i64(s.total_games);w.str(",\"lastReelRole\":");w.q(roleName(s.last_role));w.str(",\"lastReelPayout\":");w.i64(s.last_payout);w.str(",\"roleRemainderCount\":");w.i64(ROLE_REMAINDER);w.ch('}');return jsonbuf;}
static const char* eventsJSON(const Events& e){Writer w(jsonbuf,sizeof(jsonbuf));w.str("{\"events\":[");for(int i=0;i<e.n;++i){if(i)w.ch(',');w.str("{\"type\":");w.q(evName(e.e[i].type));w.str(",\"value\":");w.i64(e.e[i].value);w.str(",\"note\":");w.q(e.e[i].note);w.ch('}');}w.str("],\"inAT\":");w.str(s.in_at?"true":"false");w.str(",\"gamesLeft\":");w.i64(s.at_games_left);w.str(",\"stocks\":");w.i64(s.stocks);w.str(",\"reelRole\":");w.q(roleName(s.last_role));w.str(",\"reelPayout\":");w.i64(s.last_payout);w.ch('}');return jsonbuf;}
static void resetState(uint64_t seed){int keep=(s.setting>=1&&s.setting<=7)?s.setting:7;memset(&s,0,sizeof(s));s.setting=keep;s.tier=Lower;s.at_table=ATNormal;s.last_role=Miss;rng=seed?seed:0x5343484154475054ULL;rerollNormal();}
static Events spinNormal(){Events out;out.clear();if(s.in_at)return out;++s.total_games;++s.normal_actual_games;++s.normal_display_games;s.last_role=drawRole();s.last_payout=s.last_role==OneMedal?1:s.last_role==Bell9?9:s.last_role==Bell15?15:s.last_role==Replay?3:0;if(s.last_role==Bell9)++s.bell9_streak;else s.bell9_streak=0;sectionDelta(-3,out);if(s.bell9_streak>=5){s.bell9_streak=0;startAT(Lower,false,out,"five consecutive 9-medal bells -> lower AT");rerollNormal();return out;}if(s.normal_table==SuperHeaven&&!s.special_window_checked&&s.normal_actual_games>=20){s.special_window_checked=true;if(chance(.10)){s.normal_table=Special;s.normal_pattern=(int)(next64()%10);s.normal_ceiling=chooseCeiling(Special,s.normal_pattern);}}uint32_t d=(uint32_t)(next64()&(RNG_SPACE-1));if(d==0){out.add(Freeze,0,"1/134217728 freeze");startAT(Upper,true,out,"freeze -> upper AT + stock");rerollNormal();return out;}if(d<(RNG_SPACE/32768u)+1u){startAT(Upper,false,out,"1/32768 upper AT direct");rerollNormal();return out;}if(d<(RNG_SPACE/8192u)+(RNG_SPACE/32768u)+1u){startAT(Middle,true,out,"1/8192 middle AT + stock");rerollNormal();return out;}Profile p=profile();if(chance(p.rawAT)){startAT(Lower,false,out,"raw AT route");rerollNormal();return out;}if(chance(p.rawBonus)){out.add(Bonus,50,"raw bonus");playBonus(out);return out;}if(chance(p.rawCZ)){out.add(CZ,0,"raw CZ");playCZ(out);return out;}if(s.normal_display_games>=s.normal_ceiling)resolveCeiling(out);return out;}
static Events spinAT(){Events out;out.clear();if(!s.in_at)return out;s.last_role=Miss;s.last_payout=0;++s.total_games;if(s.at_games_left<=0){endAT(out);return out;}--s.at_games_left;int net=s.tier==Upper?9:6;sectionDelta(net,out);if(chance(1.0/99.0))rerollAT();resolveATEvent(out);if(s.in_at&&s.at_games_left<=0)endAT(out);return out;}
}

extern "C" {
__attribute__((visibility("default"))) void slot_reset(uint32_t lo,uint32_t hi){slot::resetState(((uint64_t)hi<<32)|lo);}
__attribute__((visibility("default"))) void slot_set_setting(int setting){if(setting<1||setting>7)return;slot::s.setting=setting;slot::resetState(slot::next64());}
__attribute__((visibility("default"))) const char* slot_state_json(){return slot::stateJSON();}
__attribute__((visibility("default"))) const char* slot_spin_normal_json(){static slot::Events e;e=slot::spinNormal();return slot::eventsJSON(e);}
__attribute__((visibility("default"))) const char* slot_spin_at_json(){static slot::Events e;e=slot::spinAT();return slot::eventsJSON(e);}
__attribute__((visibility("default"))) const char* slot_apply_reel_payout_json(int medals){static slot::Events e;e.clear();if(medals>0)slot::sectionDelta(medals,e);return slot::eventsJSON(e);}
}

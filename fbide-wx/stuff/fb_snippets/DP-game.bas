#ifdef DP_GAME

' ***** POKEMON items *******
Type ITEM_STRUCT Field=1
    TITLE As String
    DESCRIPTION As String
End Type

' ***** POKEMON abilitys ********
Type ABILITY_STRUCT Field=1
    TITLE As String
    DESCRIPTION As String
End Type

' ***** POKEMON in BOX structure *****
Type MONSTER_STRUCT Field=1
    NICK As Zstring * 16
    ESPECIE As Short
    ABILITY As Short
    GENDER As Short
    LEVEL As Byte
    NATURE As Byte
    ITEM As Short
    ATKS(1 To 4) As Short
    EV_HP As Short
    EV_ATK As Short
    EV_DEF As Short
    EV_SPD As Short
    EV_SATK As Short
    EV_SDEF As Short
    DV_HP As Short
    DV_ATK As Short
    DV_DEF As Short
    DV_SPD As Short
    DV_SATK As Short
    DV_SDEF As Short
End Type

' **** USER data structure ****
Type USER_STRUCT Field=1
    NICK As Zstring * 31
    PASS As Zstring * 31
    PKMNS(30) As MONSTER_STRUCT
    LASTLOG As Double
    TEAM(1 To 6) As Byte
    MAXTEAM As Byte
    LOGINTYPE As Byte
End Type

' **** POKEMON in battle structure ****
Type POKE_STRUCT Field=1
    NICK As Zstring * 16
    ESPECIE As Short
    ABILITY As Short
    GENDER As Short
    LEVEL As Short
    TYPEA As Short
    TYPEB As Short
    ITEM As Short
    NATURE As Byte
    ATKS(1 To 5) As Short
    PPMAX(1 To 5) As Byte
    PPATU(1 To 5) As Byte
    HPMAX As Short
    HPATU As Short
    ATK As Short
    DEF As Short
    SATK As Short
    SDEF As Short
    SPD As Short  
    STATUS As Byte
    SLEEPTIME As Byte
    HPTYPE As Byte
    HPPOWER As Short
    BADPSNMUL As Byte
    EVHP As Short
    EVATK As Short
    EVDEF As Short
    EVSPD As Short
    EVSATK As Short
    EVSDEF As Short
End Type

Type BATTLE_PLAYER_STRUCT Field=1
    ENEMYA As BATTLE_PLAYER_STRUCT Ptr
    ENEMYB As BATTLE_PLAYER_STRUCT Ptr
    NICK As Zstring * 31
    MAXTEAM As Byte
    SCORE As Byte
    BATNUM As Byte
    PKMNS(1 To 6) As POKE_STRUCT
    CHANGETO As Byte      'the number of pokemon to be changed
    OUTBATTLE As Byte     'the user need to switch out of battle
    NEEDCHANGE As Byte    'the user need to switch the pokemon?
    CHANGETYPE As Byte    'why the user need to switch (fainted/bp)
    PKMN As Byte          'number of atual pokemon in arena 1-6
    NORMALSPK As Byte     'normal spike count
    ROCKSPK As Byte       'rock spike count
    TOXICSPK As Byte     'poison spike count
    WISHTIME As Byte      'turns before wish work
    WISHPKMN As Byte      'pokemon that made the wish
    PKMNSPD As Short      'speed of the pokemon including stats modifiers
    HEALINGWISH As Byte   'the healing wish is waiting?
    LUNARDANCE As Byte    'the lunar dance is waiting?
    MOVED As Short        'the player already moved?
    TAILWINDTIME As Byte  'turns remaining of the tail wind effect
    HEALBLOCKTIME As Byte 'turns remaining of the heal block effect
    LUCKYTIME As Byte     'turns remaining of the lucky chant effect
    LIGHTTIME As Byte     'light screen remaining rounds
    REFLECTTIME As Byte   'reflect remaining rounds
    MISTTIME As Byte      'mist remaining rounds
    SAFETIME As Byte      'safeguard remaining rounds
    NOFLY As Byte         'the user has the roost effect this round
    NOTRAIT As Byte       'the user ability is blocked
    BPCONTINUE As Byte    'baton pass switch ok?
    BPFIRST As Byte       'pokemon attack first or second?
    BPPKASPD As Integer   'PKA final speed calculated
    BPPKBSPD As Integer   'PKB final speed calculated
    DOOMTIME As Byte      'doom desire wait turns
    DOOMATK As Short      'ATK of the pokemon that used DD
    DOOMCHGS As Byte      'changes in ATK of the pokemon that used DD
    FUTURETIME As Byte    'Future Sight wait turns
    FUTUREATK As Short    'SATK of the pokemon that used FS
    FUTURECHGS As Byte    'changes in SATK of the caller pkmn
    NULLSTART As Byte    '--------------------------------------------------
    PKDATA As POKE_STRUCT 'structure of current pokemon
    YAWNTIME As Byte      'time left before YAWN effect
    GASTROACID As Byte    'user has gastro acid effect
    MAGNETRISE As Byte    'user has the magnet rise effect
    MIRACLEEYE As Byte    'user has miracle eye effect
    SLOWSTART As Byte     'ability SLOW START round count
    MOTORDRIVE As Byte    'ability MOTORDRIVE is active?
    ABILITYBACKUP As Byte 'ability temp storage  
    AQUARING As Byte      'user has aquaring effect?
    RISETIME As Byte      'magnet rise turns left
    CHGATK As Byte        'attack status change count
    CHGDEF As Byte        'defense status change count
    CHGSPD As Byte        'speed status change count
    CHGSATK As Byte       'special attack status change count
    CHGSDEF As Byte       'special defense status change count
    CHGCH As Byte         'critical hit chance
    EVASION As Byte       'the pkmn evasion count
    ACCURACY As Byte      'the pkmn accuracy count  
    FOCUS As Byte         'pkmn is focused?
    MINIMIZED As Byte     'pkmn used minimize?
    SUBSTITUTEHP As Short 'substite heal points (if any then active)
    FORESIGHT As Byte     'pkmn is foresighted?
    PERISH As Byte        'perish song remaning rounds
    CONFUSETIME As Byte   'confuse remaining rounds (if any, pkmn is confused)
    CURSEMODE As Byte     'pkmn is cursed?
    ATTRACT As Any Ptr    'pkmn is attracted
    STOCKPILE As Byte     'stockpiled count
    INGRAIN As Byte       'the user is ingrained?
    FLASHFIRE As Byte     'the user has the flash fire status?
    BATONPASSEND As Byte ' --------------------------------------
    MULTITURN As Byte     'Need to use the next turn?
    CHOICEMOVE As Short   'the move that choice items allows
    MIMICPP As Byte       'PP mimic have when changed to a new attack
    MIMICSLOT As Byte     'The slot that have the mimic attack
    FLINCH As Byte        'pkmn got flinched?
    COUNTERDMG As Short   'fisical damage number
    MIRRORDMG As Short    'especial damage number
    LASTMOVE As Short     'the pkmn last move
    ATUSLOT As Byte       'the atk slot of the new move
    LASTSLOT As Byte      'the atk slot of last move
    DEFCURMODE As Byte    'defense curl combo mode
    WILLNOTFAIL As Byte   'the next move will have absoluty accuracy
    PROTECT As Byte       'pkmn is protecting itself?
    ENDURE As Byte        'pkmn is enduring?
    RESISTGRADE As Byte   'repetition count
    POWERMUL As Byte      'rollout power count
    FURYCUTMUL As Byte    'fury cutter power count
    DESTINY As Byte       'destiny bond effect active?  
    ENCORETIME As Byte    'encore remaning rounds
    NEXTTURN As Byte      'automatic use next turn if one...
    BADPSNMUL As Byte     'badly poison damage count
    SLEEPTIME As Byte     'sleeping remaning rounds
    THRASHCOUNT As Byte   'thrash like remanining rounds
    LEECH As Any Ptr      'leech seed is active?
    NIGHTMARE As Byte     'nigthmare is active?
    RAGEPOW As Byte       'rage build count
    BIDETIME As Byte      'bide remaining rounds
    BIDEDMG As Short      'bide damage endured
    DISABLETIME As Byte   'rounds that a move is disabled
    DISABLED As Byte      'slot move of a disabled move
    NOTCHANGE As Byte     'the user cannot switch your pokemon?
    TRAPTIME As Byte      'remaining rounds for trap moves (bind)
    TRAPMOVE As Short     'move that trapped the user
    UPROARTIME As Byte    'uproar remaining turns
    TORMENT As Byte       'the pokemon is tormented?
    HPPREROUND As Short   'user HP to focus punch =)
    TAUNTTIME As Byte     'the user has been taunted?
    GRUDGE As Byte        'the user has the grudge effect?
    IMPRISON As Byte      'the user has the imprision effect?
    MUDSPORT As Byte      'the user has the mud sport effect?
    WATERSPORT As Byte    'the user has the water sport effect?
    LOAFING As Byte       'the user cannot attack this turn  
    NULLEND As Byte      '-----------------------------------------------------
End Type

' ************ BATTLE structure ************
Type BATTLE_STRUCT Field=1  
    AA As BATTLE_PLAYER_STRUCT    'player A battle structure
    BB As BATTLE_PLAYER_STRUCT    'player B battle strucutre
    ' ***** Environment specific ******
    TRICKROOM As Byte             'trick room effect is active?
    GRAVITYTIME As Byte           'theres gravity effect in arena
    COPYCATMOVE As Short          'last sucessfull move in battle
    WEATHER As Byte               'the weather of the arena
    WEATHERTIME As Short          'the weather remaining rounds of the arena
    ' ********* BOT specific **********
    MOVETIME As Double = -1       'time the last round happened
    COLORS As Zstring*8           'the colors for this battle
End Type

' *************** Pokemon Data Struct *****************
Type PKDATA_STRUCT Field=1
    ESPECIE As Zstring*11
    GV As Short
    BHP As Short
    BATK As Short
    BDEF As Short
    BSPD As Short
    BSATK As Short
    BSDEF As Short
    TYPEA As Byte
    TYPEB As Byte
    ABILITYA As Byte
    ABILITYB As Byte
    WEIGHT As Single
End Type

' ************** Move Data Struct *************
Type MOVE_STRUCT Field=1
    TEXT As Zstring*13
    EFFECT As Short
    MODEA As Byte
    MODEB As Byte
    POWER As Short
    TYPEA As Byte
    ACC As Short
    PP As Short
    SID As Short
    PRIOR As Short
    FLAGS As Ubyte
End Type

Type CLIENT_STRUCT
    SOCKET As Uinteger
    NICK As Zstring*31
    CHALLNICK As Zstring*31
    CHALLCOLOR As String
    CHALLTYPE As Byte  
    CHALLSLOT As Byte
    CHALLPKA As Short
    CHALLPKB As Short
    LASTMSG As Double
    WARN As Byte
End Type

Type MSG_STRUCT
    TLINE As String
    NICK As String
End Type

'******** Declaring subs and functions **********
Declare Sub OnText(TEXT As String, FROM As String, NICK As String)
Declare Sub IrcSay(DESTY As String,TEXTY As String)
Declare Sub WaitConnect(NEWPORT As Integer Ptr)
Declare Sub TimeoutCount(ID As Integer)
Declare Sub EvaluateRound(BATNUMB As Integer)
Declare Sub WaitDCC(NEWNICK As Zstring Ptr)
Declare Function LAST2() As Integer
Declare Function LAST3() As Integer
Declare Function LAST4() As Integer
Declare Function LAST5() As Integer
Declare Function LAST6() As Integer
Declare Function LAST10() As Integer
Declare Function LAST11() As Integer
Declare Function ProceedAttack(AAPLAY As BATTLE_PLAYER_STRUCT Ptr, BBPLAY As BATTLE_PLAYER_STRUCT Ptr,BBCOR As String,FIRST As Byte,BABATTLE As Byte) As Integer
Declare Sub ChangePKMN(PKA As Integer,PKB As Integer, XXPLAY As BATTLE_PLAYER_STRUCT Ptr,BBCOR As String)

' ********* Macros *********
#define DEBUG_ERROR "Debug: "+__FILE__ +"("+str$(__LINE__)+") -> "
#define GetHP(PLEVEL,PBASE,PDV,PEV) ((((PLEVEL)*(((PBASE)*2)+(PDV)+((PEV)/4)))/100)+10+(PLEVEL))
#define GetST(PLEVEL,PBASE,PDV,PEV) (((((PLEVEL)*(((PBASE)*2)+(PDV)+((PEV)/4)))/100)+5)*1)
#define BOLD chr$(2)
#define UND chr$(31)
#define TINT chr$(3)

Enum BATTLE_TYPE
    BT_NORMAL = 0
    BT_RANDOM
    BT_SHARK 
    BT_QUICK
    BT_1V2
    BT_2V2
    BT_TRAINING
End Enum

Enum CHG_TYPE
    CT_FIRST = 0
    CT_NORMAL
    CT_FAINT
    CT_NORMALWAIT
    CT_FORCED
    CT_BATON  
End Enum

Enum SOME_MOVES  
    MOVE_GUST = 16
    MOVE_WHIRLWIND = 18
    MOVE_FLY = 19
    MOVE_SANDATTACK = 23
    MOVE_ROAR = 46
    MOVE_COUNTER = 68   
    MOVE_DIG = 91
    MOVE_BIDE = 117
    MOVE_METRONOME = 118
    MOVE_STRUGGLE = 165
    MOVE_CURSE = 174
    MOVE_SPIKES = 191
    MOVE_ENCORE = 227
    MOVE_HIDDENPOWER = 237
    MOVE_TWISTER = 239
    MOVE_MIRRORCOAT = 243
    MOVE_FUTURESIGHT = 248
    MOVE_FOCUSPUNCH = 264
    MOVE_DIVE = 291
    MOVE_AROMATHERAPY = 312
    MOVE_SKYUPPERCUT = 327
    MOVE_BOUNCE = 340  
    MOVE_DOOMDESIRE = 353
    MOVE_GYROBALL = 360
    MOVE_NATURALGIFT = 363  
    MOVE_HEALBLOCK = 377
    MOVE_COPYCAT = 383
    MOVE_TOXICSPIKES = 390
    MOVE_STEALTHROCK = 446
    MOVE_SHADOWFORCE = 467  
End Enum

Enum GENDERS
    GENDER_NONE = 32
    GENDER_MALE = 70
    GENDER_FEMALE = 77
End Enum

Enum WEATHER_TYPE
    WTH_NORMAL = 0
    WTH_SUNNY
    WTH_RAIN
    WTH_STORM
    WTH_HAIL
End Enum

Enum ATTACK_TYPE
    TYPE_NORMAL = 0
    TYPE_FIGHTING
    TYPE_FLYING
    TYPE_POISON
    TYPE_GROUND
    TYPE_ROCK
    TYPE_BUG
    TYPE_GHOST
    TYPE_STEEl
    TYPE_UNKNOWN
    TYPE_FIRE
    TYPE_WATER
    TYPE_GRASS
    TYPE_ELECTRIC
    TYPE_PSYCHIC
    TYPE_ICE
    TYPE_DRAGON
    TYPE_DARK
End Enum

Enum PKMN_STATUS
    PS_NONE = 0
    PS_PARALYZE
    PS_POISON
    PS_BURN
    PS_FROZEN
    PS_SLEEP
    PS_BADPSN
    PS_FAINTED
End Enum

Enum LEARNATKS
    L_BREED = 1
    L_TM = 2
End Enum

Enum NATURES
    N_TIMID = 0
    N_ADAMANT
    N_BASHFUL
    N_BOLD
    N_BRAVE
    N_CALM
    N_CAREFUL
    N_DOCILE
    N_GENTLE
    N_HARDY
    N_HASTY
    N_IMPISH
    N_JOLLY
    N_LAX
    N_LONELY
    N_MILD
    N_MODEST
    N_NAIVE
    N_NAUGHTY
    N_QUIET
    N_QUIRKY
    N_RASH
    N_RELAXED
    N_SASSY
    N_SERIOUS
End Enum

Enum NATURE_EFFECT
    NF_ATK = 0
    NF_DEF
    NF_SPD
    NF_SATK
    NF_SDEF
End Enum

Enum PKMNSTATS
    S_HP = 0
    S_ATK
    S_DEF
    S_SPD
    S_SATK
    S_SDEF
End Enum

Enum ATKMODE
    AM_PHYSICAL = 0
    AM_ESPECIAL = 1
    AM_OTHER = 2
End Enum

Const MAXABILITYS = 123
Const MAXNATURES = 24
Const MAXITEMS = 176 '500
Const MAXPOKES = 500
Const MAXMOVES = 467 '470
Const MAXEFFECTS = 276
Const MAXTYPES = 17
Const MAXSTATS = 7
Const MAXBATTLES = 8
Const MAXFLOOD = 1024
Const MAXCLIENTS = 127
Const CONNECTOUT = 30
Const CLIENTTIMEOUT = 10*60
Const BATTLETIMEOUT = 5*60

' ********** Variables **********
Dim Shared As Byte CHATWAIT
Dim Shared As Integer TOTBYTES,SETCNT
Dim Shared BATTLE(8) As BATTLE_STRUCT
Redim Shared As MONSTER_STRUCT PREDEFPKMN(64)
Dim Shared As Byte DEFPKMNUSED(MAXPOKES)
Dim Shared As PKDATA_STRUCT PKMN(MAXPOKES) 
Dim Shared As MOVE_STRUCT ATKS(MAXMOVES)
Dim Shared As ITEM_STRUCT ITEMS(MAXITEMS)
Dim Shared As String EFFECTS(MAXEFFECTS)
Dim Shared As ABILITY_STRUCT ABILITY(MAXABILITYS)
Dim Shared As Integer MSGCOUT,MSGWRITE,MSGREAD
Dim Shared As CLIENT_STRUCT CLIENTS(MAXCLIENTS)
Dim Shared As MSG_STRUCT LINES(1023)

' *************************** Type Matchups **********************************
' N   Ft  Fl  Po  Gd  R   B   Gh  S   -   Fi  W   Gr  E   Ps  I   D   Dk
Dim Shared As Single MATCHUP(MAXTYPES,MAXTYPES) = { _
{ 1 , 1 , 1 , 1 , 1 ,0.5, 1 , 0 ,0.5, 9 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 }, _
{ 2 , 1 ,0.5,0.5, 1 , 2 ,0.5, 0 , 2 , 9 , 1 , 1 , 1 , 1 ,0.5, 2 , 1 , 2 }, _
{ 1 , 2 , 1 , 1 , 1 ,0.5, 2 , 1 ,0.5, 9 , 1 , 1 , 2 ,0.5, 1 , 1 , 1 , 1 }, _
{ 1 , 1 , 1 ,0.5,0.5,0.5, 1 ,0.5, 0 , 9 , 1 , 1 , 2 , 1 , 1 , 1 , 1 , 1 }, _
{ 1 , 1 , 0 , 2 , 1 , 2 ,0.5, 1 , 2 , 9 , 2 , 1 ,0.5, 2 , 1 , 1 , 1 , 1 }, _
{ 1 ,0.5, 2 , 1 ,0.5, 1 , 2 , 1 ,0.5, 9 , 2 , 1 , 1 , 1 , 1 , 2 , 1 , 1 }, _
{ 1 ,0.5,0.5,0.5, 1 , 1 , 1 ,0.5,0.5, 9 ,0.5, 1 , 2 , 1 , 2 , 1 , 1 , 2 }, _
{ 0 , 1 , 1 , 1 , 1 , 1 , 1 , 2 ,0.5, 9 , 1 , 1 , 1 , 1 , 2 , 1 , 1 ,0.5}, _
{ 1 , 1 , 1 , 1 , 1 , 2 , 1 , 1 ,0.5, 9 ,0.5,0.5, 1 ,0.5, 1 , 2 , 1 , 1 }, _
{ 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 , 9 }, _
{ 1 , 1 , 1 , 1 , 1 ,0.5, 2 , 1 , 2 , 9 ,0.5,0.5, 2 , 1 , 1 , 2 ,0.5, 1 }, _
{ 1 , 1 , 1 , 1 , 2 , 2 , 1 , 1 , 1 , 9 , 2 ,0.5,0.5, 1 , 1 , 1 ,0.5, 1 }, _
{ 1 , 1 ,0.5,0.5, 2 , 2 ,0.5, 1 ,0.5, 9 ,0.5, 2 ,0.5, 1 , 1 , 1 ,0.5, 1 }, _
{ 1 , 1 , 2 , 1 , 0 , 1 , 1 , 1 , 1 , 9 , 1 , 2 ,0.5,0.5, 1 , 1 ,0.5, 1 }, _
{ 1 , 2 , 1 , 2 , 1 , 1 , 1 , 1 ,0.5, 9 , 1 , 1 , 1 , 1 ,0.5, 1 , 1 , 0 }, _
{ 1 , 1 , 2 , 1 , 2 , 1 , 1 , 1 ,0.5, 9 ,0.5,0.5, 2 , 1 , 1 ,0.5, 2 , 1 }, _
{ 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 ,0.5, 9 , 1 , 1 , 1 , 1 , 1 , 1 , 2 , 1 }, _
{ 1 ,0.5, 1 , 1 , 1 , 1 , 1 , 2 ,0.5, 9 , 1 , 1 , 1 , 1 , 2 , 1 , 1 ,0.5} }
Dim Shared As Integer HPCVTYPE(15) = { 1,2,3,4,5,6,7,8,10,11,12,13,14,15,16,17 }
Dim Shared As Byte DVVAL(15,5) = { {31,31,30,30,30,30}, _ 'fighting
{31,31,31,30,30,30} , {31,31,30,31,30,30} , _ ' flying / poison
{31,31,31,31,30,30} , {31,31,30,30,31,30} , _ ' ground / rock
{31,31,31,30,31,30} , {31,31,30,31,31,30} , _ ' Bug / Ghost
{31,31,31,31,31,30} , {31,30,31,30,30,31} , _ 'Steel / Fire
{31,31,31,30,30,31} , {31,30,31,31,30,31} , _ 'Water / Grass
{31,31,31,31,30,31} , {31,30,31,30,31,31} , _ ' Electric / Psychic
{31,30,30,31,31,31},{31,30,31,31,31,31},{31,31,31,31,31,31} } 'Ice/Dragon/Dark
Dim Shared As Zstring*10 PKTYPE(MAXTYPES) = { _ 
"Normal","Fighting","Flying","Poison","Ground","Rock","Bug","Ghost","Steel", _
"???","Fire","Water","Grass","Electric","Psychic","Ice","Dragon","Dark" }
Dim Shared As Zstring*10 LFT(MAXTYPES) = { _
TINT+"15",TINT+"5",TINT+"0",TINT+"6",TINT+"7",BOLD+TINT+"7", _
TINT+"3",TINT+"14",BOLD+TINT+"15",TINT+"10",TINT+"4",TINT+"12", _
TINT+"9",TINT+"8",TINT+"13",TINT+"11",BOLD+TINT+"6",BOLD+TINT+"14" }
Dim Shared As Zstring*10 RTT(MAXTYPES) = { TINT,TINT,TINT,TINT,TINT,BOLD+TINT, _
TINT,TINT,BOLD+TINT,TINT,TINT,TINT, TINT,TINT,TINT,TINT,BOLD+TINT,BOLD+TINT }
Dim Shared As Zstring*5 PKSTATS(MAXSTATS) = { _
""," PRZ "," PSN"," BRN"," FRZ"," SLP"," PSN"," FNT" }
Dim Shared As Ubyte LEARN(MAXPOKES,470)
Dim Shared As Short TMS(1 To 100) = { _
264,337,352,347,46,92,258,339,331,237,241,269,58,59,63,113,182,240,202,219,218,76, _
231,85,87,89,216,91,94,247,280,104,115,351,53,188,201,126,317,332,259,263,290, _
156,213,168,211,285,289,315,355,411,412,206,362,374,451,203,406,409,261,318,373, _
153,421,371,278,416,397,148,444,419,86,360,14,446,244,445,399,157,404,214,363,398, _
138,447,207,365,369,164,430,433,15,19,57,70,432,249,127,431}
Dim Shared As Zstring*10 ATKMOD(2) = {"Physical","special","Other"}
Dim Shared As Zstring*5 NATST(4) = {"atk","def","spd","satk","sdef"}
Dim Shared As Zstring*12 NATURE(24) = { "Timid" , _
"Adamant", "Bashful", "Bold", "Brave", "Calm", "Careful", _
"Docile", "Gentle", "Hardy", "Hasty", "Impish", "Jolly", _
"Lax", "Lonely", "Mild", "Modest", "Naive", "Naughty", _
"Quiet", "Quirky", "Rash", "Relaxed", "Sassy", "Serious" }
' ATK   DEF   SPD  SATK  SDEF       ATK   DEF   SPD  SATK  SDEF       [NAME1 NAME2]
Dim Shared As Single NATVAL(24,4)={{0.9 , 1.0 , 1.1 , 1.0 , 1.0 } , _ 'Timid 
{ 1.1 , 1.0 , 1.0 , 0.9 , 1.0 } , { 1.0 , 1.0 , 1.0 , 1.0 , 1.0 } , _ 'Adamant Bashful
{ 0.9 , 1.1 , 1.0 , 1.0 , 1.0 } , { 1.1 , 1.0 , 0.9 , 1.0 , 1.0 } , _ 'Bold Brave
{ 0.9 , 1.0 , 1.0 , 1.0 , 1.1 } , { 1.0 , 1.0 , 1.0 , 0.9 , 1.1 } , _ 'Calm Careful
{ 1.0 , 1.0 , 1.0 , 1.0 , 1.0 } , { 1.0 , 0.9 , 1.0 , 1.0 , 1.1 } , _ 'Docile Gentle
{ 1.0 , 1.0 , 1.0 , 1.0 , 1.0 } , { 1.0 , 0.9 , 1.1 , 1.0 , 1.0 } , _ 'Hardy Hasty
{ 1.0 , 1.1 , 1.0 , 0.9 , 1.0 } , { 1.0 , 1.0 , 1.1 , 0.9 , 1.0 } , _ 'Impish Jolly
{ 1.0 , 1.1 , 1.0 , 1.0 , 0.9 } , { 1.1 , 0.9 , 1.0 , 1.0 , 1.0 } , _ 'Lax Lonely
{ 1.0 , 0.9 , 1.0 , 1.1 , 1.0 } , { 0.9 , 1.0 , 1.0 , 1.1 , 1.0 } , _ 'Mild Modest
{ 1.0 , 1.0 , 1.1 , 1.0 , 0.9 } , { 1.1 , 1.0 , 1.0 , 1.0 , 0.9 } , _ 'Naive Naughty
{ 1.0 , 1.0 , 0.9 , 1.1 , 1.0 } , { 1.0 , 1.0 , 1.0 , 1.0 , 1.0 } , _ 'Quiet Quirky
{ 1.0 , 1.0 , 1.0 , 1.1 , 0.9 } , { 1.0 , 1.1 , 0.9 , 1.0 , 1.0 } , _ 'Rash Relaxed
{ 1.0 , 1.0 , 0.9 , 1.0 , 1.1 } , { 1.0 , 1.0 , 1.0 , 1.0 , 1.0 } }   'Sassy Serious

Dim Shared As Byte LEARNLVL(15,64),NIV,GRUP
Dim Shared As Byte LVG(15),RTM(12)
' ****** Eavsion & Accuracy Percent *******
Dim Shared As Single ACCEVA(-6 To 6) = _
{ 1.716,1.671,1.596,1.558,1.500,1.341,1, _
1-0.341,1-0.500,1-0.558,1-0.596,1-0.671,1-0.716 }
Dim As String TEMP
Dim As Integer FFILE,AUXI

Color 14
Print "Starting..."
Randomize Timer

' *************************************************************************
' ************************* Loading Pokemon DATA **************************
' *************************************************************************

Scope ' **** ITEMS NAMES ****
    TMR = Timer
    ITEMS(0).TITLE = "None"
    ITEMS(0).DESCRIPTION = "No Descrpition."
    Color 7:Print "Loading itens names... ";
    TEMP = "data\items.txt"
    If Dir$(TEMP) <> "" Then  
        FFILE = Freefile()
        Open TEMP For Input As #FFILE
        For COUNT = 1 To MAXITEMS
            Line Input #FFILE,TEMP
            ITEMS(COUNT).TITLE = Ucase$(Trim$(Mid$(TEMP,6,15)))
            ITEMS(COUNT).DESCRIPTION = Trim$(Mid$(TEMP,21))    
        Next COUNT
        Close #FFILE
        Color 10: Print "OK ";Csng(Timer-TMR)*1000;"ms"
    Else
        Color 12: Print "Failed."
        Sleep 10000: End
    End If
End Scope
Scope ' **** ABILITYS ****
    TMR = Timer
    ABILITY(0).TITLE = "None"
    ABILITY(0).DESCRIPTION = "No Descrpition."
    Color 7:Print "Loading abilities... ";
    TEMP = "data\ability.txt"
    If Dir$(TEMP) <> "" Then  
        FFILE = Freefile()
        Open TEMP For Input As #FFILE
        For COUNT = 1 To MAXABILITYS
            Line Input #FFILE,TEMP
            ABILITY(COUNT).TITLE = Ucase$(Trim$(Mid$(TEMP,5,16)))
            ABILITY(COUNT).DESCRIPTION = Trim$(Mid$(TEMP,22))    
        Next COUNT
        Close #FFILE
        Color 10: Print "OK ";Csng(Timer-TMR)*1000;"ms"
    Else
        Color 12: Print "Failed."
        Sleep 10000: End
    End If
End Scope
Scope ' **** Especies *****
    TMR = Timer
    Color 7:Print "Loading especies... ";
    TEMP = "data\especies.txt"
    If Dir$(TEMP) <> "" Then  
        FFILE = Freefile()
        Open TEMP For Input As #FFILE
        For COUNT = 1 To MAXPOKES
            Line Input #FFILE,TEMP
            PKMN(COUNT).ESPECIE = Ucase$(Trim$(TEMP))
        Next COUNT
        Close #FFILE
        Color 10: Print "OK ";Csng(Timer-TMR)*1000;"ms"
    Else
        Color 12: Print "Failed."
        Sleep 10000: End
    End If
End Scope
Scope ' *** Base/Type/Especie/Ability/TMs ***
    TMR = Timer
    Color 7:Print "Loading stats... ";
    TEMP = "data\dppokes.txt"
    If Dir$(TEMP) <> "" Then
        FFILE = Freefile()
        Open TEMP For Input As #FFILE
        ' *** Skip Header ***
        Line Input #FFILE,TEMP
        Line Input #FFILE,TEMP
        Line Input #FFILE,TEMP
        
        For COUNT = 1 To MAXPOKES
            Line Input #FFILE,TEMP
            If Val(Left$(TEMP,3)) <> COUNT Then Exit For    
            With PKMN(COUNT)
                .BHP = Val(Mid$(TEMP,10,3))
                .BATK = Val(Mid$(TEMP,14,3))
                .BDEF = Val(Mid$(TEMP,18,3))
                .BSPD = Val(Mid$(TEMP,22,3))
                .BSATK = Val(Mid$(TEMP,26,3))
                .BSDEF = Val(Mid$(TEMP,30,3))      
                .TYPEA = Val(Mid$(TEMP,35,2))
                .TYPEB = Val(Mid$(TEMP,39,2))
                .GV = Val(Mid$(TEMP,74,3))
                .ABILITYA = Val(Mid$(TEMP,98,3))
                .ABILITYB = Val(Mid$(TEMP,102,3))      
            End With
            'if COUNT = 3 then
            '  print PKMN(COUNT).BHP;PKMN(COUNT).BATK;PKMN(COUNT).DEF;PKMN(COUNT).BSPD; _
            '  PKMN(COUNT).BSATK;PKMN(COUNT).
        Next COUNT  
        If COUNT = 501 Then
            Color 10: Print "OK ";Csng(Timer-TMR)*1000;"ms"
            Close #FFILE
        Else
            Color 12: Print "Failed."
            Sleep 10000:End
        End If
    Else
        Color 12: Print "Failed."
        Sleep 10000:End
    End If
End Scope
Scope ' *** move names ***
    TMR = Timer
    Color 7:Print "Loading moves names... ";
    ATKS(0).TEXT = "none"
    TEMP = "data\moves.txt"
    If Dir$(TEMP) <> "" Then
        FFILE = Freefile()
        Open TEMP For Input As #FFILE    
        For COUNT = 1 To MAXMOVES
            Line Input #FFILE,TEMP    
            TEMP = Trim$(TEMP)
            Mid$(TEMP,1,1) = Ucase$(Left$(TEMP,1))
            AUXI = Instr(1,TEMP," ")
            If AUXI Then Mid$(TEMP,AUXI+1,1) = Ucase$(Mid$(TEMP,AUXI+1,1))
            ATKS(COUNT).TEXT = Ucase$(TEMP)
        Next COUNT
        Color 10: Print "OK";Csng(Timer-TMR)*1000;"ms"
        Close #FFILE 
    Else
        Color 12: Print "Failed."
        Sleep 10000:End
    End If
End Scope
Scope ' ** moves **
    TMR = Timer
    Color 7:Print "Loading moves... ";
    TEMP = "data\dpmoves2.txt"
    If Dir$(TEMP) <> "" Then
        FFILE = Freefile()
        Open TEMP For Input As #FFILE  
        ' *** Skip Header ***
        'line input #FFILE,TEMP
        'line input #FFILE,TEMP
        'line input #FFILE,TEMP  
        For COUNT = 1 To MAXMOVES
            Line Input #FFILE,TEMP
            'print TEMP
            'print Gettok("10 20 30",1,32);" <- O.o"
            'sleep 50
            If Val(Gettok(TEMP,1,32)) <> COUNT Then Exit For
            'if val(left$(TEMP,3)) <> COUNT then exit for
            With ATKS(COUNT)
                .EFFECT = Val(Gettok(TEMP,2,32)) 'val(mid$(TEMP,10,3))
                .MODEA = Val(Gettok(TEMP,3,32)) 'val(mid$(TEMP,16,1))
                .MODEB = Val(Gettok(TEMP,4,32)) 'val(mid$(TEMP,20,1))
                .POWER = Val(Gettok(TEMP,5,32)) 'val(mid$(TEMP,22,3))
                .TYPEA = Val(Gettok(TEMP,6,32)) 'val(mid$(TEMP,27,2))
                .ACC = Val(Gettok(TEMP,7,32)) 'val(mid$(TEMP,30,3))
                .PP = Val(Gettok(TEMP,8,32)) 'val(mid$(TEMP,35,2))
                .SID = Val(Gettok(TEMP,9,32)) 'val(mid$(TEMP,38,3))
                .PRIOR = Cbyte(Val(Gettok(TEMP,12,32))) 'cbyte(val(mid$(TEMP,50,3)))
                .FLAGS = Val(Gettok(TEMP,13,32)) 'val(mid$(TEMP,54,3))
            End With
        Next COUNT
        Color 10: Print "OK";Csng(Timer-TMR)*1000;"ms"
        Close #FFILE
    Else
        Color 12: Print "Failed."
        Sleep 10000:End
    End If
End Scope
Scope ' ** move effect names **
    TMR = Timer
    Color 7:Print "Loading move effects names... ";
    TEMP = "data\effects.txt"
    If Dir$(TEMP) <> "" Then
        FFILE = Freefile()
        Open TEMP For Input As #FFILE
        For COUNT = 0 To MAXEFFECTS    
            Line Input #FFILE,TEMP
            AUXI = Instr(1,TEMP,"=")    
            If AUXI Then
                EFFECTS(Val(Left$(TEMP,AUXI-1))) = Mid$(TEMP,AUXI+1)
            Else
                Color 14: Print "eof "+Str$(COUNT)+" but... ";
                Exit For
            End If
        Next COUNT
        Color 10: Print "OK";Csng(Timer-TMR)*1000;"ms"
        Close #FFILE
    Else
        Color 12: Print "Failed."
        Sleep 10000:End
    End If
End Scope
Scope ' ** learn sets **
    TMR = Timer
    Color 7:Print "Loading learn sets... ";
    TEMP = "data\moveset.bin"
    If Dir$(TEMP) <> "" Then
        FFILE = Freefile()  
        Open TEMP For Binary Access Read As #FFILE
        Get #FFILE,1,LVG()
        Get #FFILE,,LEARNLVL()
        Get #FFILE,,LEARN()  
        Color 10: Print "OK";Csng(Timer-TMR)*1000;"ms"
        Close #FFILE
    Else
        Color 12: Print "Failed."
        Sleep 10000:End
    End If
End Scope
Scope ' ** pokemon weight **
    TMR = Timer
    Color 7:Print "Loading species weight... ";
    TEMP = "data\weight.txt"
    If Dir$(TEMP) <> "" Then
        FFILE = Freefile()  
        Open TEMP For Input As #FFILE
        For COUNT = 1 To 493
            If Eof(FFILE) Then
                Color 12: Print "Failed."
                Sleep 10000:End
            End If
            Input #FFILE,PKMN(COUNT).WEIGHT
        Next COUNT
        Color 10: Print "OK";Csng(Timer-TMR)*1000;"ms"
        Close #FFILE
    Else
        Color 12: Print "Failed."
        Sleep 10000:End
    End If
End Scope
Scope ' *** Loading Quick battle movesets ***
    Dim As String TEMPDT,ATKNAME(1 To 4)
    Dim As Integer LINECNT,ITMP,EVSUM
    TMR = Timer
    Color 7:Print "Loading predefined teams... ";
    TEMP = "data\Teams.txt"
    If Dir$(TEMP) <> "" Then
        FFILE = Freefile()
        Open TEMP For Input As #FFILE
        While Not Eof(FFILE)
            Line Input #FFILE,TEMP
            LINECNT += 1
            TEMP = Trim$(TEMP)
            If TEMP[0] <> 45 And TEMP[0] <> 0 Then        
                If SETCNT > Ubound(PREDEFPKMN) Then
                    Redim Preserve PREDEFPKMN(SETCNT+64)
                End If
                With PREDEFPKMN(SETCNT)
                    ' *** checking for valid species name ***
                    TEMPDT = Ucase$(Trim$(Left$(TEMP,11)))
                    For COUNT = 1 To MAXPOKES 
                        If TEMPDT = PKMN(COUNT).ESPECIE Then Exit For
                    Next COUNT
                    If COUNT > MAXPOKES Then
                        Color 12:Print Chr$(13,10) & "Pokemon '"+TEMPDT+"' not found. on line: " & LINECNT
                        Sleep:End
                    End If
                    .ESPECIE = COUNT: .NICK = Ucase$(PKMN(.ESPECIE).ESPECIE)
                    DEFPKMNUSED(.ESPECIE) = 1
                    ' *** checking for valid ability name ***
                    TEMPDT = Lcase$(Trim$(Mid$(TEMP,12,14)))
                    If Lcase$(ABILITY(PKMN(.ESPECIE).ABILITYA).TITLE) = TEMPDT Then
                        .ABILITY = 0
                    Elseif Lcase$(ABILITY(PKMN(.ESPECIE).ABILITYB).TITLE) = TEMPDT Then
                        .ABILITY = 1
                    Else
                        Color 12:Print Chr$(13,10) & "Ability '"+TEMPDT+"' isnt valid for "+ _
                        "'"+PKMN(.ESPECIE).ESPECIE+"'. on line: " & LINECNT
                        Sleep:End
                    End If
                    ' *** checking for valid nature name ***
                    TEMPDT = Lcase$(Trim$(Mid$(TEMP,26,9)))
                    For COUNT = 0 To MAXNATURES
                        If Lcase$(NATURE(COUNT)) = TEMPDT Then Exit For
                    Next COUNT
                    If COUNT > MAXNATURES Then
                        Color 12:Print Chr$(13,10) & "nature '"+TEMPDT+"' not found. on line: " & LINECNT
                        Sleep:End
                    End If
                    .NATURE = COUNT
                    ' *** checking for valid item name ***
                    TEMPDT = Ucase$(Trim$(Mid$(TEMP,35,13)))
                    For COUNT = 0 To MAXITEMS
                        If ITEMS(COUNT).TITLE = TEMPDT Then Exit For
                    Next COUNT
                    If COUNT > MAXITEMS Then
                        Color 12:Print Chr$(13,10) & "item '"+TEMPDT+"' not found. on line: " & LINECNT
                        Sleep:End
                    End If
                    .ITEM = COUNT
                    ATKNAME(1) = Ucase$(Trim$(Mid$(TEMP,48,13)))
                    ATKNAME(2) = Ucase$(Trim$(Mid$(TEMP,61,13)))
                    ATKNAME(3) = Ucase$(Trim$(Mid$(TEMP,74,13)))
                    ATKNAME(4) = Ucase$(Trim$(Mid$(TEMP,87,13)))
                    For CNT As Integer = 1 To 4
                        For COUNT = 0 To MAXMOVES
                            If ATKNAME(CNT) = ATKS(COUNT).TEXT Then Exit For
                        Next COUNT            
                        If COUNT > MAXMOVES Then
                            Color 12:Print Chr$(13,10) & "move '"+ATKNAME(CNT)+"' not found. on line: " & LINECNT
                            Sleep:End            
                        End If
                        .ATKS(CNT)=COUNT
                        If LEARN(.ESPECIE,COUNT) = 0 Then
                            Color 12:Print Chr$(13,10)+"'"+PKMN(.ESPECIE).ESPECIE+"' doesnt learn " + _
                            "the move '"+ATKNAME(CNT)+"'. on line: " & LINECNT
                            Sleep:End
                        End If
                    Next CNT
                    'DVVAL(15,5)
                    TEMPDT = Lcase$(Trim$(Mid$(TEMP,120)))
                    For COUNT = 0 To 15
                        If TEMPDT = Lcase$(PKTYPE(HPCVTYPE(COUNT))) Then
                            .DV_HP = DVVAL(COUNT,0)
                            .DV_ATK = DVVAL(COUNT,1)
                            .DV_DEF = DVVAL(COUNT,2)
                            .DV_SPD = DVVAL(COUNT,3)
                            .DV_SATK = DVVAL(COUNT,4)
                            .DV_SDEF = DVVAL(COUNT,5)
                            Exit For
                        End If
                    Next COUNT
                    If COUNT > 15 Then 
                        Color 12:Print Chr$(13,10) & "hidden power type '"+TEMPDT+"' not found. on line: " & LINECNT
                        Sleep:End
                    End If
                    TEMP = Trim$(Mid$(TEMP,100,20))+","
                    EVSUM = 0
                    Do
                        #macro DvEvCheck( DVEV )
                        ITMP = Instr(1,TEMP,",")-1    ' HP
                        If ITMP < 0 Then Exit Do
                        TEMPDT = Left$(TEMP,ITMP): TEMP = Mid$(TEMP,ITMP+2)
                        ITMP = Val(TEMPDT): If ITMP < 0 Then .DV_##DVEV -= ITMP:ITMP=0
                        If .DV_##DVEV < 0 Then ITMP=-1:Exit Do
                        EVSUM += ITMP: .EV_##DVEV = ITMP
                        If ITMP > 255 Then ITMP=-1: Exit Do
                        #endmacro
                        DvEvCheck( HP )
                        DvEvCheck( ATK )
                        DvEvCheck( DEF )
                        DvEvCheck( SPD )
                        DvEvCheck( SATK )
                        DvEvCheck( SDEF )
                        ITMP = 0
                    Loop Until ITMP=0
                    If ITMP < 0 Then
                        Color 12:Print Chr$(13,10) & "ev string damaged! on line: " & LINECNT
                        Sleep:End
                    End If
                    If EVSUM > 510 Then
                        Color 12:Print Chr$(13,10) & "ev sum exceed limit! on line: " & LINECNT
                        Sleep:End
                    End If
                    SETCNT += 1
                End With
            End If
        Wend
        SETCNT -= 1: Redim Preserve PREDEFPKMN(SETCNT)    
        Color 10: Print "OK";Csng(Timer-TMR)*1000;"ms"
        Close #FFILE
    Else
        Color 12: Print "Failed."
        Sleep 10000:End
    End If
End Scope

'sleep
'end

#ifdef MYDEBUG
Sleep 10000
#endif

Threadcreate(@TimeoutCount,1)

' *************************************************************************
' ******************* TEXT EVENTS BOTH CHANNEL AND QUERY ******************
' *************************************************************************
#include "DP-items.bas"
#include "DP-Ability.bas"
#include "DP-Events.bas"

' ***********************************************************************
' *********************** EXCHANGE 2 POKEMONS ***************************
' ***********************************************************************
Sub ChangePKMN(PKA As Integer,PKB As Integer, XXPLAY As BATTLE_PLAYER_STRUCT Ptr,BBCOR As String)
    
    Dim As Integer TEMPI,SLOT
    Dim As Uinteger TSTPTR 
    Dim As Integer COUNT,TEMP
    Dim As Byte NOWEATHER,NOSELFPSN,NOSTATUS
    Dim As Double CHGSTS,TEMPP
    Dim As Short DEFP,SDEFP
    Dim ZZPLAY As BATTLE_PLAYER_STRUCT Ptr
    
    #ifdef MyDebug
    Ircsay(BOTCHAN, DEBUG_ERROR+": Change From: "+Str$(PKA)+" to "+Str$(PKB))
    #Endif
    
    ZZPLAY = XXPLAY->ENEMYA
    #define PKZDATA(WHAT) ZZPLAY->PKDATA.WHAT
    #define PKXDATA(WHAT) XXPLAY->PKDATA.WHAT
    #define PLAYXX(WHAT) XXPLAY->PKDATA.WHAT
    
    If PKB = 0 Then
        If XXPLAY->PKMN <> ZZPLAY->PKMN Then
            Ircsay(BOTCHAN, DEBUG_ERROR+" Tried to Switch to a null pokemon @_@")
        End If
        Exit Sub
    End If
    
    BATTLE(XXPLAY->BATNUM).MOVETIME = Timer
    XXPLAY->MOVED = 0
    XXPLAY->CHANGETO = 0
    If (XXPLAY->CHANGETYPE) <> CT_FIRST Then
        
        ' ************* ABILITES BEFORE CHANGE *********
        ABILITY_NATURALCURE()    
        XXPLAY->NEEDCHANGE = 0
        
        If XXPLAY->PKDATA.ITEM < 0 Then XXPLAY->PKDATA.ITEM = 0
        
        ' restoring mimic
        If XXPLAY->MIMICPP Then 
            XXPLAY->PKDATA.ATKS(XXPLAY->MIMICSLOT) = 102
            XXPLAY->PKDATA.PPATU(XXPLAY->MIMICSLOT) = XXPLAY->MIMICPP
        End If
        
        If PKA<>PKB Then
            With XXPLAY->PKMNS(XXPLAY->PKMN)          
                .SLEEPTIME = XXPLAY->SLEEPTIME
                .HPATU = XXPLAY->PKDATA.HPATU
                .STATUS = XXPLAY->PKDATA.STATUS
                .ITEM = XXPLAY->PKDATA.ITEM
                .PPATU(1) = XXPLAY->PKDATA.PPATU(1)          
                .PPATU(2) = XXPLAY->PKDATA.PPATU(2)          
                .PPATU(3) = XXPLAY->PKDATA.PPATU(3)          
                .PPATU(4) = XXPLAY->PKDATA.PPATU(4)
                .BADPSNMUL = XXPLAY->BADPSNMUL
            End With
        End If
        
        If XXPLAY->HEALINGWISH Or XXPLAY->LUNARDANCE Then
            XXPLAY->PKMNS(PKA).HPATU = 0
            XXPLAY->PKMNS(PKA).STATUS = PS_FAINTED
            XXPLAY->THRASHCOUNT = 0      
            XXPLAY->NEXTTURN = 0
            XXPLAY->NOTCHANGE = 0
            ZZPLAY->NOTCHANGE = 0
        End If
        
        TEMPI = XXPLAY->CHANGETYPE
        TEMP = Cuint(@XXPLAY->NULLSTART)
        If TEMPI = CT_BATON Then TEMP = @XXPLAY->BATONPASSEND
        #ifdef MyDebug
        Ircsay(BOTCHAN, DEBUG_ERROR+" Change Type: "+Str$(TEMPI))
        #Endif
        For TSTPTR = Cuint(TEMP) To Cuint(@XXPLAY->NULLEND)      
            Poke TSTPTR,0
        Next TSTPTR
        
        If TEMPI <> CT_BATON Then
            ZZPLAY->ATTRACT = 0
            ZZPLAY->IMPRISON = 0
            ZZPLAY->TORMENT = 0
        End If
        
        #ifdef MyDebug
        Ircsay(BOTCHAN, DEBUG_ERROR+" Test: "+Str$(PKB)+" "+Str$(XXPLAY))
        #Endif
        
        XXPLAY->PKMN = PKB
        XXPLAY->BADPSNMUL = XXPLAY->PKMNS(PKB).BADPSNMUL
        XXPLAY->SLEEPTIME = XXPLAY->PKMNS(PKB).SLEEPTIME
        XXPLAY->NOTRAIT = 0
        XXPLAY->TRAPTIME = 0
        ZZPLAY->TRAPTIME = 0    
        With XXPLAY->PKDATA            
            .STATUS = XXPLAY->PKMNS(PKB).STATUS
            .ESPECIE = XXPLAY->PKMNS(PKB).ESPECIE
            .NICK = XXPLAY->PKMNS(PKB).NICK
            .ABILITY = XXPLAY->PKMNS(PKB).ABILITY      
            .LEVEL = XXPLAY->PKMNS(PKB).LEVEL
            .TYPEA = XXPLAY->PKMNS(PKB).TYPEA
            .TYPEB = XXPLAY->PKMNS(PKB).TYPEB
            .ITEM = XXPLAY->PKMNS(PKB).ITEM
            .GENDER = XXPLAY->PKMNS(PKB).GENDER
            .HPMAX = XXPLAY->PKMNS(PKB).HPMAX
            .HPATU = XXPLAY->PKMNS(PKB).HPATU
            .ATK = XXPLAY->PKMNS(PKB).ATK
            .DEF = XXPLAY->PKMNS(PKB).DEF
            .SPD = XXPLAY->PKMNS(PKB).SPD
            .SATK = XXPLAY->PKMNS(PKB).SATK
            .SDEF = XXPLAY->PKMNS(PKB).SDEF
            .HPTYPE = XXPLAY->PKMNS(PKB).HPTYPE
            .HPPOWER = XXPLAY->PKMNS(PKB).HPPOWER
            .ATKS(1) = XXPLAY->PKMNS(PKB).ATKS(1)
            .ATKS(2) = XXPLAY->PKMNS(PKB).ATKS(2)
            .ATKS(3) = XXPLAY->PKMNS(PKB).ATKS(3)
            .ATKS(4) = XXPLAY->PKMNS(PKB).ATKS(4)
            .PPMAX(1) = XXPLAY->PKMNS(PKB).PPMAX(1)
            .PPATU(1) = XXPLAY->PKMNS(PKB).PPATU(1)
            .PPMAX(2) = XXPLAY->PKMNS(PKB).PPMAX(2)
            .PPATU(2) = XXPLAY->PKMNS(PKB).PPATU(2)
            .PPMAX(3) = XXPLAY->PKMNS(PKB).PPMAX(3)
            .PPATU(3) = XXPLAY->PKMNS(PKB).PPATU(3)
            .PPMAX(4) = XXPLAY->PKMNS(PKB).PPMAX(4)
            .PPATU(4) = XXPLAY->PKMNS(PKB).PPATU(4)
        End With
        
        If TEMPI = CT_FORCED Then
            TEMP = Int(((XXPLAY->PKDATA.HPATU/XXPLAY->PKDATA.HPMAX)*100))
            If TEMP < 1 Then TEMP = 1
            IrcSay(BOTCHAN,BBCOR + " "+bold+XXPLAY->NICK+bold + _
            "'s "+PKXDATA(NICK)+ " ( " + _
            PKMN(PKXDATA(ESPECIE)).ESPECIE  + " "+Trim$(Chr$(PKXDATA(GENDER))) + _
            " - LV." + Str$(PKXDATA(LEVEL))+" - HP "+Str$(TEMP)+"%"+ _
            Rtrim$(" "+PKSTATS(PKXDATA(STATUS)))+" ) is dragged out! ")
        Else
            If XXPLAY->CHANGETYPE <> CT_FAINT Then         
                ircsay(BOTCHAN,BBCOR+" "+XXPLAY->NICK+" withdrew "+ _
                PKMN(XXPLAY->PKMNS(PKA).ESPECIE).ESPECIE+" ")        
            End If
            TEMP = Int(((XXPLAY->PKDATA.HPATU/XXPLAY->PKDATA.HPMAX)*100))
            If TEMP < 1 Then TEMP = 1
            IrcSay(BOTCHAN,BBCOR + " " + XXPLAY->NICK+" sent out " + _
            bold + PKXDATA(NICK) + bold + " ( " + _
            PKMN(PKXDATA(ESPECIE)).ESPECIE + _
            Rtrim$(" "+Chr$(PKXDATA(GENDER))) + " - LV." + Str$(PKXDATA(LEVEL))+ _
            " - HP "+Str$(TEMP)+"%"+Rtrim$(" "+PKSTATS(PKXDATA(STATUS)))+" ) ")
        End If    
    End If
    
    ' ******** ABILITIES EFFECT *********
    ABILITY_CLOUDNINEX()
    ABILITIES_IN()
    ABILITY_FORECAST(XXPLAY,ZZPLAY)
    
    
    ' ******** Toxic Spikes *********
    If XXPLAY->TOXICSPK Then
        If NOSTATUS = 0 And NOSELFPSN = 0 And XXPLAY->SAFETIME=0 Then        
            If XXPLAY->SUBSTITUTEHP = 0 Then            
                TEMP = 0
                TEMP -= (XXPLAY->PKDATA.TYPEA = TYPE_POISON)*32 
                TEMP -= (XXPLAY->PKDATA.TYPEB = TYPE_POISON)*32
                TEMP -= XXPLAY->PKDATA.TYPEA = TYPE_STEEL
                TEMP -= XXPLAY->PKDATA.TYPEB = TYPE_STEEL
                TEMP -= XXPLAY->PKDATA.TYPEA = TYPE_FLYING
                TEMP -= XXPLAY->PKDATA.TYPEB = TYPE_FLYING        
                TEMP -= XXPLAY->PKDATA.ABILITY = ABT_LEVITATE
                If TEMP = 0 And XXPLAY->PKDATA.STATUS = PS_NONE Then
                    If XXPLAY->TOXICSPK = 1 Then
                        Ircsay(BOTCHAN, BBCOR+" "+bold+XXPLAY->NICK+bold+ _
                        "'s "+XXPLAY->PKDATA.NICK+" was poisoned!" )
                        XXPLAY->PKDATA.STATUS = PS_POISON
                    Else
                        Ircsay(BOTCHAN, BBCOR+" "+bold+XXPLAY->NICK+bold+ _
                        "'s "+XXPLAY->PKDATA.NICK+" was badly poisoned!" )
                        XXPLAY->PKDATA.STATUS = PS_POISON
                        XXPLAY->BADPSNMUL = 0
                    End If          
                End If
                If TEMP >= 32 Then
                    TEMP = 0
                    TEMP -= XXPLAY->PKDATA.TYPEA = TYPE_FLYING
                    TEMP -= XXPLAY->PKDATA.TYPEB = TYPE_FLYING
                    TEMP -= XXPLAY->PKDATA.ABILITY = ABT_LEVITATE
                    If TEMP = 0 Then
                        Ircsay(BOTCHAN,BBCOR+" "+XXPLAY->PKDATA.NICK+ _
                        " absorved the "+ATKS(MOVE_TOXICSPIKES).TEXT+"! ")
                        XXPLAY->TOXICSPK = 0
                    End If
                End If
            End If
        End If
    End If 
    
    ' ******** Spikes LIKE damage ********
    If (XXPLAY->NORMALSPK) And XXPLAY->PKDATA.ABILITY <> ABT_MAGICGUARD Then
        If XXPLAY->PKDATA.ABILITY <> ABT_LEVITATE Then
            If PKXDATA(TYPEA) <> TYPE_FLYING And PKXDATA(TYPEB) <> TYPE_FLYING Then
                Ircsay(BOTCHAN,BBCOR+" "+bold+XXPLAY->NICK+bold+"'s "+PKXDATA(NICK) + _
                " is hurt by the "+ATKS(191).TEXT+"! ")
                TEMP = PKXDATA(HPMAX)*(1/(10-((XXPLAY->NORMALSPK)*2)))
                If TEMP < 1 Then TEMP = 1
                PKXDATA(HPATU) -= TEMP
                If PKXDATA(HPATU) < 0 Then PKXDATA(HPATU) = 0
                If PKXDATA(HPATU) > 0 Then        
                    Ircsay(BOTCHAN,BBCOR+" "+bold+XXPLAY->NICK+bold+" "+ _
                    PKXDATA(NICK)+"'s health fell down to "+ _
                    Str$(-Int(-((PKXDATA(HPATU)/PKXDATA(HPMAX))*100)))+"%. ")
                End If      
            End If
        End If
    End If
    
    ' ******** Stealth Rock damage ********
    If (XXPLAY->ROCKSPK) And XXPLAY->PKDATA.ABILITY <> ABT_MAGICGUARD Then
        If PKXDATA(TYPEA) = PKXDATA(TYPEB) Then
            TEMPP = 1 * MATCHUP(TYPE_ROCK,PKXDATA(TYPEA))      
        Else
            TEMPP = 1 * MATCHUP(TYPE_ROCK,PKXDATA(TYPEA)) 
            TEMPP *= MATCHUP(TYPE_ROCK,PKXDATA(TYPEB))
        End If    
        Ircsay(BOTCHAN,BBCOR+" Pointed stones dug into "+_ 
        bold+XXPLAY->NICK+bold+"'s "+PKXDATA(NICK)+"! ")
        TEMP = (PKXDATA(HPMAX)/8)*TEMPP
        If TEMP < 1 Then TEMP = 1
        PKXDATA(HPATU) -= TEMP
        If PKXDATA(HPATU) < 0 Then PKXDATA(HPATU) = 0
        If PKXDATA(HPATU) > 0 Then
            Ircsay(BOTCHAN,BBCOR+" "+bold+XXPLAY->NICK+bold+" "+ _
            PKXDATA(NICK)+"'s health fell down to "+ _
            Str$(-Int(-((PKXDATA(HPATU)/PKXDATA(HPMAX))*100)))+"%. ")
        End If      
    End If
    
    ' ******** Healing wish ************
    If XXPLAY->HEALINGWISH And XXPLAY->PKDATA.HPATU > 0 And XXPLAY->PKDATA.STATUS <> PS_FAINTED Then
        XXPLAY->HEALINGWISH = 0
        If XXPLAY->PKDATA.HPATU = XXPLAY->PKDATA.HPMAX Then
            Ircsay(BOTCHAN,BBCOR+" The healing wish came true! "+_
            XXPLAY->PKDATA.NICK+"'s HP is full! ")
        Else
            Ircsay(BOTCHAN,BBCOR+" The healing wish came true! "+_
            XXPLAY->PKDATA.NICK+"'s regained health! ")
            XXPLAY->PKDATA.HPATU = XXPLAY->PKDATA.HPMAX
        End If
        If XXPLAY->PKDATA.STATUS <> PS_NONE Then    
            Ircsay(BOTCHAN,BBCOR+" "+XXPLAY->PKDATA.NICK+" HEALING WISH EFFECT STATUS HEAL MESSAGE ")
            XXPLAY->PKDATA.STATUS = PS_NONE
        End If
    End If
    
    ' ******** lunar dance ************
    If XXPLAY->LUNARDANCE And XXPLAY->PKDATA.HPATU > 0 Then
        XXPLAY->LUNARDANCE = 0
        If XXPLAY->PKDATA.HPATU = XXPLAY->PKDATA.HPMAX Then
            Ircsay(BOTCHAN,BBCOR+" LUNAR DANCE HP FULL FAIL MESSAGE ")
            'Ircsay(BOTCHAN,BBCOR+" The healing wish came true! "+_
            'XXPLAY->PKDATA.NICK+"'s HP is full! ")
        Else
            'Ircsay(BOTCHAN,BBCOR+" The healing wish came true! "+_
            'XXPLAY->PKDATA.NICK+"'s regained health! ")
            XXPLAY->PKDATA.HPATU = XXPLAY->PKDATA.HPMAX
            Ircsay(BOTCHAN,BBCOR+" LUNAR DANCE RECOVER MESSAGE ")
        End If
        Ircsay(BOTCHAN,BBCOR+" LUNAR DANCE PP RESTORE MESSAGE ")
        If XXPLAY->PKDATA.STATUS <> PS_NONE Then    
            Ircsay(BOTCHAN,BBCOR+" "+XXPLAY->PKDATA.NICK+" LUNAR DANCE EFFECT STATUS HEAL MESSAGE ")
            XXPLAY->PKDATA.STATUS = PS_NONE
        End If
        For COUNT = 1 To 4
            XXPLAY->PKDATA.PPATU(COUNT) = XXPLAY->PKDATA.PPMAX(COUNT)
        Next COUNT
    End If
    
    ' ******* is POKEMON XX fainted? ********
    If XXPLAY->PKDATA.HPATU <= 0 Or XXPLAY->PKDATA.STATUS = PS_FAINTED Then
        XXPLAY->PKDATA.HPATU = 0
        XXPLAY->PKDATA.STATUS = PS_FAINTED
        XXPLAY->PKMNS(XXPLAY->PKMN).STATUS = PS_FAINTED
        XXPLAY->PKMNS(XXPLAY->PKMN).HPATU = 0
        XXPLAY->SCORE -= 1
        Ircsay(BOTCHAN, BBCOR+" "+bold+XXPLAY->NICK+bold+"'s "+ _
        PKMN(XXPLAY->PKDATA.ESPECIE).ESPECIE+" fainted. ( " + _
        bold+XXPLAY->NICK+bold+": "+Str$(XXPLAY->SCORE) + _
        " - "+bold+ZZPLAY->NICK+bold+": "+Str$(ZZPLAY->SCORE)+" )")    
        
        XXPLAY->PKDATA.HPATU = 0
        XXPLAY->PKDATA.STATUS = PS_FAINTED
        XXPLAY->NOTCHANGE = 0
        ZZPLAY->NOTCHANGE = 0 
        XXPLAY->THRASHCOUNT = 0
        'if XXPLAY->CHANGETYPE <> CT_FAINT then
        XXPLAY->OUTBATTLE = 1    
        XXPLAY->NEXTTURN = 0
        XXPLAY->NEEDCHANGE = 1
        XXPLAY->CHANGETYPE = CT_FAINT
        
        'end if
        
        '' ***** Checking if the game have finished *****
        'for COUNT = 1 to ZZPLAY->MAXTEAM
        '  if ZZPLAY->PKMNS(COUNT).STATUS <> PS_FAINTED then
        '    exit for
        '  end if
        'next COUNT    
        'TEMP = COUNT
        ' 
        'for COUNT = 1 to XXPLAY->MAXTEAM
        '  if XXPLAY->PKMNS(COUNT).STATUS <> PS_FAINTED then      
        '    exit for
        '  end if
        'next COUNT
        
        ' ****** Have a winner? ********
        'if TEMP>ZZPLAY->MAXTEAM and COUNT>XXPLAY->MAXTEAM then
        If ZZPLAY->SCORE = 0 And XXPLAY->SCORE = 0 Then
            ircsay(BOTCHAN, BBCOR+" both has no more pokemon... ")
            Ircsay(BOTCHAN, BBCOR+" the game was "+und+"DRAW"+und+"! ")
            BATTLE(XXPLAY->BATNUM).MOVETIME = -1
            BATTLE(XXPLAY->BATNUM).COLORS = ""
            Exit Sub
            'elseif TEMP > ZZPLAY->MAXTEAM then
        Elseif ZZPLAY->SCORE = 0 Then
            BATTLE(XXPLAY->BATNUM).MOVETIME = -1
            BATTLE(XXPLAY->BATNUM).COLORS = ""
            ircsay(BOTCHAN, BBCOR+" "+bold+ZZPLAY->NICK+bold+" has no more pokemon... ")
            Ircsay(BOTCHAN, BBCOR+" "+bold+XXPLAY->NICK+bold+" team "+und+"WINS"+und+" the game!")
            Exit Sub
            'elseif COUNT > XXPLAY->MAXTEAM then
        Elseif XXPLAY->SCORE = 0 Then
            BATTLE(XXPLAY->BATNUM).MOVETIME = -1
            BATTLE(XXPLAY->BATNUM).COLORS = ""
            ircsay(BOTCHAN, BBCOR+" "+bold+XXPLAY->NICK+bold+" has no more pokemon... ")
            Ircsay(BOTCHAN, BBCOR+" "+bold+ZZPLAY->NICK+bold+" team "+und+"WINS"+und+" the game!")
            Exit Sub
        End If
        
        If XXPLAY->PKDATA.HPATU < 1 Or XXPLAY->PKDATA.STATUS = PS_FAINTED Then
            Ircsay(XXPLAY->NICK, "Please Switch your pokemon.")
        End If
        
    End If
    
End Sub

' ***********************************************************************
' ********************* Evaluate the Round of a battle ******************
' ***********************************************************************
#include "DP-Evaluate.bas"

' ***********************************************************************
' ********************* Proceed with one player attack ******************
' ***********************************************************************
#include "DP-GoAttack.bas"

' ************************ Write Message in the IRC **********************
Sub IrcSay(DESTY As String,TEXTY As String)  
    
    Dim As String NICKDEST
    Dim As Integer COUNT
    Dim As Zstring*16384 OUTBUFF
    
    If Instr(1,DESTY,"#") = 0 Then
        COUNT = Instr(1,DESTY,"?")
        If COUNT Then
            DESTY = Mid$(DESTY,COUNT+1)
        Else
            NICKDEST = Trim$(Lcase$(DESTY))  
            For COUNT = 0 To MAXCLIENTS    
                If Lcase$(CLIENTS(COUNT).NICK) = Trim$(Lcase$(NICKDEST)) Then
                    If CLIENTS(COUNT).SOCKET <> 0 Then
                        OUTBUFF = TEXTY+Chr$(13,10)
                        hsend(CLIENTS(COUNT).SOCKET,@OUTBUFF,Len(OUTBUFF))        
                        TOTBYTES += Len(OUTBUFF)
                        Sleep 1
                        Exit Sub
                    End If
                End If
            Next COUNT
        End If
    End If
    
    OUTBUFF = "privmsg "+DESTY+" :"+TEXTY+Chr$(13,10)
    #ifdef ANTISPLIT
    While DOSEND
        Sleep 1
    Wend
    DOSEND = 2
    BUFFBUFF += OUTBUFF
    If Len(BUFFBUFF) >= MAXFLOOD Then
        DOSEND = 1
    Else
        DOSEND = 0
        Sleep 15
    End If
    #Else  
    TOTBYTES += Len(OUTBUFF)
    hsend(SOCKOUT,@OUTBUFF,Len(OUTBUFF))
    Sleep 15
    #Endif
    
    'sleep 7
    'sleep len(SENDBUFF)
End Sub

' **************** Send the text buffer to the irc client *************
Sub SENDTHREAD(SOCKOUT As Integer)
    
    Dim As Double TMR  
    TMR = Timer
    Do    
        While (Timer-TMR) < 1 And TRBUSY <> -1
            Sleep 15
        Wend
        While DOSEND = 2
            Sleep 1
        Wend
        If TRBUSY = -1 Then 
            Exit Sub
        End If
        DOSEND = 1
        If BUFFBUFF <> "" Then
            Sleep Len(BUFFBUFF)*2
            If Len(BUFFBUFF) > MAXFLOOD Then
                hsend(SOCKOUT,@BUFFBUFF,MAXFLOOD)
                BUFFBUFF = Mid$(BUFFBUFF,MAXFLOOD+1)
            Else
                hsend(SOCKOUT,@BUFFBUFF,Len(BUFFBUFF))
                BUFFBUFF = ""
            End If
        End If
        DOSEND=0 
        TMR += 1    
    Loop Until TRBUSY = -1
    
End Sub

' ******************************************************************
' *********************** NEW CLIENT THREAD ************************
' ******************************************************************
Sub WaitDCC(NEWNICK As Zstring Ptr)
    
    Dim As sockaddr_in FROMINFO  
    Dim As Integer SOCKLST,SOCKIN,LSPORT,AA,BB
    Dim As Integer TRYCNT,TEMPSOCK,COUNT
    Dim As Integer RECL,SLOTIN
    Dim As Uinteger TMPIP,OUTIP
    Dim As Zstring*16384 RECBUFF,RCVBUFF,OUTBUFF
    Dim As String MSGNICK,TLINHA
    
    While CHATWAIT 
        Sleep 50
    Wend
    
    ' ********* Localizando Socket Livre **********
    CHATWAIT = 1
    MSGNICK = *NEWNICK  
    *NEWNICK = ""
    Randomize Timer
    SOCKLST = hOpen()
    TRYCNT = 0
    Do
        If TRYCNT > 16384 Then 
            TRYCNT = -1
            Exit Do
        End If
        LSPORT = 8192+Cint(Rnd*8192)  
        TRYCNT += 1
    Loop Until hBind(SOCKLST,LSPORT)
    'print "lsport: ";LSPORT
    
    ' ********** Enviando Convite de DCC chat ************
    If Instr(1,Lcase$(MSGNICK),"mysoft")>0 Or Trim$(Lcase$(MSGNICK)) = "[user]" Then
        TMPIP = hResolve("127.0.0.1")
        'elseif instr(1,lcase$(MSGNICK),"novato")>0 then
        '  TMPIP = 1862314176
    Else
        TMPIP = hResolve(BOTHOST)
    End If
    OUTIP =  (TMPIP And &h000000FF) Shl 24
    OUTIP += (TMPIP And &H0000FF00) Shl 8
    OUTIP += (TMPIP And &h00FF0000) Shr 8
    OUTIP += (TMPIP And &hFF000000) Shr 24
    OUTBUFF = "PRIVMSG "+MSGNICK+" :"+_
    Chr$(1)+"DCC CHAT chat "+Str$(OUTIP)+ _
    " "+Str$(LSPORT)+Chr$(1,13,10)  
    hsend(SOCKOUT,@OUTBUFF,Len(OUTBUFF))
    ' ******** Aguardando Conexão **********
    TEMPSOCK = SOCKLST
    Threadcreate(@WaitConnect,@TEMPSOCK)
    hlisten(SOCKLST,30)  
    CHATWAIT = 0
    SOCKIN = haccept(SOCKLST,@FROMINFO)
    TEMPSOCK = 0
    hClose(SOCKLST)
    If SOCKIN <> -1 And SOCKIN <> 0 Then
        For COUNT = 0 To MAXCLIENTS
            If CLIENTS(COUNT).NICK = "" Then
                With CLIENTS(COUNT)      
                    .NICK = Trim$(MSGNICK)
                    .SOCKET = SOCKIN
                    .LASTMSG = Timer
                End With
                RECBUFF = MSGNICK+" logged in the CHAT!"+Chr$(13,10)
                hsend(SOCKIN,@RECBUFF,Len(RECBUFF))
                Exit For
            End If
        Next COUNT
        'print "sockin: ";SOCKIN  
    Else
        ' *** conexão falhou ***
        'print "Connection will be discarded"
        'sleep
        'end
        Exit Sub
    End If
    
    ' **** no free client slots? O.O ****
    If COUNT > MAXCLIENTS Then
        'color 12
        'print "FATAL ERROR!"
        'sleep
        'end
    End If  
    RECBUFF = ""
    SLOTIN = COUNT
    
    ' *********** Receiving text from chat ***********
    Do
        Do
            COUNT = Instr(1,RECBUFF,Chr$(13,10))
            BB = 1
            If COUNT = 0 Then
                COUNT = Instr(1,RECBUFF,Chr$(10))
                BB = 2
            End If
            If COUNT Then
                TLINHA = Left$(RECBUFF,COUNT-1)
                RECBUFF = Mid$(RECBUFF,COUNT+BB)
                Exit Do
            End If
            RECL = hreceive(SOCKIN,@RCVBUFF,16000)
            If RECL <= 0 Then        
                With CLIENTS(SLOTIN)
                    .LASTMSG = 0
                    .WARN = 0
                    .SOCKET = 0
                    .NICK = ""
                    .CHALLNICK = ""
                    .CHALLCOLOR = ""
                    .CHALLPKA = 0
                    .CHALLPKB = 0
                    .CHALLTYPE = 0
                    .CHALLSLOT = 0        
                End With
                
                Exit Sub
            End If
            RECBUFF += Left$(RCVBUFF,RECL)
        Loop
        CLIENTS(SLOTIN).LASTMSG = Timer
        CLIENTS(SLOTIN).WARN = 0
        OnText(TLINHA, "="+MSGNICK, MSGNICK)
        Sleep 50,1
    Loop
End Sub

' **************************************************
' ************ DCC Connection Timeout **************
' **************************************************
Sub WaitConnect(NEWPORT As Integer Ptr)
    Dim As Double TMRCNT  
    TMRCNT = Timer
    While (Timer-TMRCNT)<CONNECTOUT And *NEWPORT<>0 And TRBUSY <> -1
        Sleep 100,1
    Wend
    If *NEWPORT Then 
        'print "Connection Timed out."
        hclose(*NEWPORT)
    End If
End Sub

' **************************************************
' ****** Battles and DCC connection timeouts *******
' **************************************************
Sub TimeoutCount(ID As Integer)
    Dim As Byte FLAG
    Dim As Integer COUNT,TEMP
    Dim As Double TMP
    Dim As Zstring*1024 OUTBUFF
    Dim As String BBCOR
    
    Do
        
        ' ********* Verifing Client DCC CHAT timeout **********
        For COUNT = 0 To MAXCLIENTS
            If CLIENTS(COUNT).NICK <> "" Then
                TMP = Timer-(CLIENTS(COUNT).LASTMSG)
                If TMP > CLIENTTIMEOUT-60 And CLIENTS(COUNT).WARN = 0 Then
                    OUTBUFF = "The Connection will be closed in 1 minute."+Chr$(13,10)       
                    hsend(CLIENTS(COUNT).SOCKET,@OUTBUFF,Len(OUTBUFF))
                    CLIENTS(COUNT).WARN = 1
                Elseif TMP > CLIENTTIMEOUT Then
                    OUTBUFF =  "Conection with " + _
                    CLIENTS(COUNT).NICK + " has timed out."+Chr$(13,10)
                    hsend(CLIENTS(COUNT).SOCKET,@OUTBUFF,Len(OUTBUFF))
                    CLIENTS(COUNT).WARN = 0
                    hclose(CLIENTS(COUNT).SOCKET)
                    CLIENTS(COUNT).NICK = ""
                End If
            End If
        Next COUNT
        
        ' ************** Verifing BATTLES TIMEOUTS **************
        For COUNT = 1 To MAXBATTLES
            If BATTLE(COUNT).MOVETIME <> -1 Then
                If (Timer-(BATTLE(COUNT).MOVETIME)) > BATTLETIMEOUT Then
                    For TEMP = 0 To MAXCLIENTS
                        If CLIENTS(TEMP).CHALLSLOT = COUNT Then            
                            Ircsay(CLIENTS(TEMP).NICK," the challenge was TIMED OUT!... ")
                            Ircsay(CLIENTS(TEMP).CHALLNICK," the challenge was TIMED OUT!... ")
                            CLIENTS(TEMP).CHALLNICK = ""
                            BATTLE(COUNT).MOVETIME = -1
                            BATTLE(COUNT).COLORS = ""
                            FLAG = 1            
                        End If
                    Next TEMP
                    If FLAG = 0 Then
                        BBCOR = BATTLE(COUNT).COLORS
                        ircsay(BOTCHAN, BBCOR+" the battle between " + _
                        bold+BATTLE(COUNT).AA.NICK+bold+" and "+ _
                        bold+BATTLE(COUNT).BB.NICK+bold+" was TIMED OUT!... ")
                        If BATTLE(COUNT).AA.MOVED <> 0 And BATTLE(COUNT).BB.MOVED = 0 Or BATTLE(COUNT).BB.NEEDCHANGE <> 0 Then
                            Ircsay(BOTCHAN, BBCOR+" "+bold+BATTLE(COUNT).AA.NICK+bold+ _
                            " team "+und+"WINS"+und+" the game!")
                        Elseif BATTLE(COUNT).BB.MOVED <> 0 And BATTLE(COUNT).AA.MOVED = 0 Or BATTLE(COUNT).AA.NEEDCHANGE <> 0 Then
                            Ircsay(BOTCHAN, BBCOR+" "+bold+BATTLE(COUNT).BB.NICK+bold+ _
                            " team "+und+"WINS"+und+" the game!")
                        Else  
                            Ircsay(BOTCHAN, BBCOR+" the game was "+und+"DRAW"+und+"! ")
                        End If
                    End If
                    BATTLE(COUNT).MOVETIME = -1
                    BATTLE(COUNT).COLORS = ""
                End If
            End If
        Next COUNT    
        ' ******** IRC server Timeout ********
        If TOUTTMR>0 Then
            If (Timer-TOUTTMR) > 180 Then
                SENDBUFF = "ping chanserv"+Chr$(13,10)
                hsend(SOCKOUT,@SENDBUFF,Len(SENDBUFF))
            End If
            If (Timer-TOUTTMR) > 300 Then
                For COUNT = 0 To MAXCLIENTS  
                    CLIENTS(COUNT).WARN = 0
                    hclose(CLIENTS(COUNT).SOCKET)
                    CLIENTS(COUNT).NICK = ""
                    SENDBUFF = "quit No Response in 300 secs"+Chr$(13,10)
                    hsend(SOCKOUT,@SENDBUFF,Len(SENDBUFF))
                Next COUNT        
                End
            End If
            
        End If
        
        ' ******** WAIT E RE-WAIT *******
        For COUNT = 1 To 100
            
            ' ****** Hide Show Window ******
            If GetAsyncKeyState(VK_LCONTROL)<>0 And GetAsyncKeyState(VK_RCONTROL)<>0 Then
                If WINHIDE <> 0 And GetAsyncKeyState(VK_UP)<>0 And GetAsyncKeyState(VK_DOWN)=0 Then
                    IRCWIN = FindWindow(null,"DP-BOT by: Mysoft")
                    If IRCWIN Then
                        ShowWindow(IRCWIN,SW_SHOW)
                        WINHIDE = 0
                    End If
                Elseif WINHIDE <> 1 And GetAsyncKeyState(VK_DOWN)<>0 And GetAsyncKeyState(VK_UP)=0 Then
                    IRCWIN = FindWindow(null,"DP-BOT by: Mysoft")
                    If IRCWIN Then
                        ShowWindow(IRCWIN,SW_HIDE)
                        WINHIDE = 1
                    End If
                End If
            End If    
            
            Sleep 50,1
            If TRBUSY = -1 Then Exit Do
        Next COUNT
    Loop Until TRBUSY = -1
    For COUNT = 0 To MAXCLIENTS  
        CLIENTS(COUNT).WARN = 0
        hclose(CLIENTS(COUNT).SOCKET)
        CLIENTS(COUNT).NICK = ""
    Next COUNT
End Sub

' ************************************ EOF **************************
Function LAST2() As Integer
    Return __line__
End Function
#endif

' ****************************************
#ifdef Errors!
ENCORE ... não está permitindo trocar
FLY ... está levando atks... qndo usado no metronome
#endif
' ***************************************
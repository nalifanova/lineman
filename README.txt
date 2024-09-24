----------------------------------------------------------------
Config Files
----------------------------------------------------------------
There will be two configuration files in this assignment. The Assets config
file, and the Level configuration file.

----------------------------------------------------------------
Assets File Specification
----------------------------------------------------------------
There will be three different line types in the Assets file, each of which
correspond to a different type of Asset. They are as follows:

Texture Asset Specification:
Texture N P
  Texture Name      N       std::string (it will have no spaces)
  Texture FilePath  P       std::string (it will have no spaces)

Animation Asset Specification:
Animation N T F S
  Animation Name        N   std::string (it will have no spaces)
  Texture Name          T   std::string (refers to an existing texture)
  Frame Count           F   int (number of frames in the Animation)
  Animation Speed       S   int (number of game frames between animation frames)

Font Asset Specification:
Font N P
  Font Name         N       std::string (it will have no spaces)
  Font File Path    P       std::string (it will have no spaces)

Sound Asset Specification:
Sound N P
  Sound Name        N       std::string (it will have no spaces)
  Sound FilePath    P       std::string (it will have no spaces)

----------------------------------------------------------------
Level Specification File
----------------------------------------------------------------

Note: in this assignment (0,0) is in top-left.

Player Specification:
Player X Y BX BY S H G J
  Spawn Position       X Y      int, int (in pixel coordinates)
  Bounding Box Size    BX BY    int, int
  Speed                S        float
  Max Health           H        int
  Gravity              G        int
  Jump                 J        int

Tile Specification:
Tile Name RX RY TX TY BM BV
  Animation Name       Name     std::string
  Room Coordinate      RX RY    int, int (in block coordinates)
  Tile Position        TX TY    int, int (in block coordinates)
  Block Movement       BM       int (1 = true, 0 = false)
  Block Vision         BV       int (1 = true, 0 = false)

Interactable Specification:
Intr Name RX RY TX TY BM BV KT OP L AT
  Animation Name       Name     std::string
  Room Coordinate      RX RY    int, int (in block coordinates)
  Tile Position        TX TY    int, int (in block coordinates)
  Block Movement       BM       int (1 = true, 0 = false)
  Block Vision         BV       int (1 = true, 0 = false)
  Key Type             KT       int (90, 93, 95, 97, 99)
  isOpen               OP       bool
  isLocked             L        bool
  ActionType           AT       int

Dec Specification:
Decoration Name RX RY TX TY
  Animation Name       Name     std::string
  Room Coordinate      RX RY    int, int (in block coordinates)
  Dec Position         TX TY    int, int (in block coordinates)

Cons Specification:
Consumable Name RX RY TX TY H G SPR
  Animation Name       Name     std::string
  Room Coordinate      RX RY    int, int (in block coordinates)
  Cons Position        TX TY    int, int (in block coordinates)
  Health               H        int
  Gravity              G        int
  Surprise             SPR      Surprise
  - 0
  - sRX, sRY, sTX, sTY, tagId

NPC Specification:
  NPC Name RX RY TX TY BM BV H D AI ...
  Animation Name       Name     std::string
  Room Coordinate      RX RY    int, int
  Tile Position        TX TY    int, int
  Block Movement       BM       int (1 = true, 0 = false)
  Block Vision         BV       int (1 = true, 0 = false)
  Max Health           H        int (> 0)
  Damage               D        int (> 0)
  AI Behavior Name     AI       std::string
  AI Parameters        ...      (see below)

AI = Follow
  ... = S
  Follow Speed         S        float (speed to follow player)

AI = Patrol
  ... = S N X1 Y1 X2 Y2 ... XN YN
  Patrol Speed         S        float
  Patrol Position      N        int (number of patrol positions)
  Position 1-N         Xi Yi    int, int (Tile Position of Patrol Position i)

For Example:
NPC Tektite 0 0 15 10 0 0 2 1 Patrol 2 4 15 10 15 7 17 7 17 10
  - Spawn an NPC with animation name Tektite in room (0,0) with tile pos (15,10)
  - This NPC has max health of 2 and damage of 1
  - This NPC does not block movement or vision
  - This NPC has a Patrol AI with speed 2 and 4 positions, each in room (0,0)
    Positions: (15,10) (15,7) (17,7) (17,10)

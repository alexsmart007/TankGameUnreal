# TankGameUnreal
Tank Game in Unreal C++ (Wii Play Tanks)

<!-- Drop a gameplay GIF or video link here, the debug view of the AI states makes a good clip -->

This is my tank game from my CSE 3541 class, remade in Unreal Engine 5.8 with C++. The original was a Unity project for lab 6 ([CSE-3541-Tank-Game](https://github.com/alexsmart007/CSE-3541-Tank-Game)), a procedurally generated tank game where every level throws more tanks at you. I wanted to bring it over to Unreal and keep the same controls, rules and feel, and then build on it. The biggest addition is the enemy AI. In the Unity version the enemy tanks just sat in place and spun until they saw you, now they roam the arena, chase you down, shoot when they have a clear shot, and go looking for you when you hide behind a wall.

The game is straightforward, you drive your tank around the arena and destroy every enemy tank to move on to the next level. Each level has one more tank than the last. Beat level 11 to win. You have 3 lives, and if you lose them all it's game over.

## How to Play

You can use keyboard or gamepad, whichever you want.

| Action | Keyboard | Gamepad |
|---|---|---|
| Drive forward / back | W / S or Up / Down arrows | Left stick up / down |
| Turn | A / D or Left / Right arrows | Right stick left / right |
| Shoot | Space | Right trigger or A |
| Reset your position | R | X |
| Quit | Q | Select |

A few things to know:
- Your shells bounce off a wall once and blow up on the second wall.
- You can't spam shots. You have to wait for your last shell to bounce (or hit something) before you can fire again, so make your shots count.
- Bounced shells can hit you too, so watch your angles.
- Shells that hit each other both blow up, so you can shoot down incoming shells.
- If you get hit, everything resets, you go back to the start and the enemy tanks still alive go back to where they spawned. The ones you already destroyed stay destroyed.

## Running It

If you just want to play, grab the latest build from the Releases page and run `TankGame.exe`.

If you want to open the project:
1. Install Unreal Engine 5.8 and Visual Studio 2022 with the "Game development with C++" workload.
2. Open `TankGame 5.3 5.8/TankGame.uproject` and say yes when it asks to rebuild the modules.
3. Hit Play. The main level is `Content/Levels/TankGame`.

The `TankGame` and `TankGame 5.3` folders are older versions of the project from before I moved it to 5.8, the one to use is `TankGame 5.3 5.8`.

## How the Code Works

All the gameplay is in C++ under `TankGame 5.3 5.8/Source/TankGame`. The Blueprints are mostly just there to set meshes, sounds and tuning values, plus the HUD.

I tried to keep each class focused on one job and not have them all reaching into each other. Tanks announce when they get destroyed through an event, and the game mode listens for it, so the shell doesn't need to know anything about the game rules and the AI doesn't need to know anything about the game mode.

### The tanks

**`ATankPawn`** is the base class for every tank. It has the body (collision box, chassis, turret, tracks and a fire point at the end of the barrel), the driving code and the cannon. Driving uses a collision sweep so the tank slides along walls instead of getting stuck on them. The cannon handles all the firing rules like the cooldown, how many shells you can have out at once and the wait for bounce rule, and the tank keeps track of the shells it fired. When a tank gets hit it broadcasts `OnTankDestroyed` and destroys itself.

**`APlayerTank`** is the tank you control. It binds the inputs using Enhanced Input, has the overhead camera that follows you without spinning with you, and plays the engine sound while you drive. This one class covers what used to be five scripts in Unity (InputManager, MovementControl, RotateControl, Respawner and QuitHandler).

**`AEnemyTank`** is the enemy's body. It doesn't make any decisions itself, it holds the AI settings in a `FTankAIConfig` struct (how far it can see, how wide its view cone is, attack range, how far it roams and so on) and remembers where it spawned so it can go back there when the round resets. Since all the tuning is in that struct, making a new enemy type is just making a new Blueprint with different numbers.

### The shell

**`AShell`** is the projectile. It flies flat with no gravity and bounces off walls using Unreal's Projectile Movement. When it hits something:
- another shell, they both blow up
- a tank, it tells the tank it got hit and blows up
- a wall for the first time, it bounces
- a wall for the second time, it blows up

The tank that fired it can't be hit by it until after the first bounce, that way you can't blow yourself up the moment you shoot.

### The AI

**`ATankAIController`** is the enemy brain. It's a state machine with four states:
- **Roam**, drives around to random spots on the nav mesh and pauses between them
- **Chase**, it spotted you but you're too far, so it drives toward you and updates its path every half second
- **Attack**, you're in range, so it turns to face you and fires when it's lined up and has a clear shot
- **Search**, it lost sight of you, so it drives to where it last saw you, looks around for a few seconds and then goes back to roaming

It sees using Unreal's AI Perception, so walls block its view. It drives like a tank too, it turns toward where it wants to go first and then drives forward, so it never slides sideways. Before firing it does a line trace at shell height to make sure there's no wall or other enemy in the way. If two tanks get stuck on each other it notices it isn't moving, backs up while turning, and picks a new path. Turning on Draw Debug in the enemy Blueprint shows each tank's state, view cone and path while you play.

### Game flow

**`ATankGameMode`** runs the rules of a level, it covers what GenerateLevel, GameOver, LevelLoader and Win did in Unity. At the start of a level it spawns enemies at the spawn points tagged `EnemySpawn1` up to `EnemySpawnN` (level N gets N enemies) and listens to every tank's `OnTankDestroyed`. When you get destroyed it takes a life, waits a second, then resets the round and respawns you. When the last enemy goes down it shows Level Complete and loads the next level, or You Win after level 11. It also has Blueprint events for the HUD and music (`OnLevelStarted`, `OnLivesChanged`, `OnLevelComplete`, `OnGameOver` and `OnWin`).

**`UTankGameInstance`** holds your level and lives. The game instance sticks around when the map reloads, so this is how your progress carries over between levels. It's the same idea as the static variables I used in Unity.

**`ATankPlayerController`** handles the mouse cursor and input mode, so the Play Again and Quit buttons can be clicked on the Game Over and Win screens.

**`UMyInputConfigData`** is a Data Asset that holds the input actions so the player tank can bind them in C++.

### What happens when you take a shot

1. You hit Space, the player tank checks `CanFire()` and spawns a shell at the end of the barrel.
2. The shell bounces off a wall, so now you're allowed to fire again.
3. The shell hits an enemy and tells it it got hit. The enemy broadcasts `OnTankDestroyed` and is destroyed.
4. The game mode hears about it and counts down the enemies. If that was the last one, it shuts off every cannon, shows Level Complete and loads the next level, with the game instance carrying over your lives and level.

## What Changed From the Unity Version

- The enemies actually move now. In Unity they stayed in one spot spinning and shooting down a single raycast. Now they roam, chase, attack and search.
- Every enemy has its own fire cooldown and shell limit, Unity only let one enemy shell exist in the whole game at a time.
- You have to wait for your shell to bounce before firing again, no more spamming.
- Shells blow each other up, and any shell can destroy any tank, including enemies hitting each other with a ricochet.
- Getting hit resets everyone's positions instead of just respawning you.
- Instead of 11 separate scenes, the same map reloads with one more enemy each level. The game mode can still use a different map per level if I add more.

## What's Next

Some stuff I want to add later:
- Different terrain and arenas, like mud that slows you down, ice, destructible walls and landmines
- Different enemy types, like a sniper with fast shells, one whose shells bounce twice, a fast rusher and a heavy tank that takes two hits
- Smarter enemies that can aim bank shots off the walls

## License

GPL-3.0, see [LICENSE](LICENSE).

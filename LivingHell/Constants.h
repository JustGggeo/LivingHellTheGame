#pragma once

namespace Constants {

// --- World ---
inline constexpr int kFieldSize = 30;
inline constexpr int kMaxSpawnAttempts = 100;
inline constexpr int kSpawnMargin = 2;

// --- Window ---
inline constexpr int kWindowWidth = 800;
inline constexpr int kWindowHeight = 600;
inline constexpr int kFramerateLimit = 60;

// --- Rendering ---
inline constexpr float kTileSize = 20.f;
inline constexpr float kUiX = 620.f;

// --- Enemy AI ---
inline constexpr int kEnemyVisionRange = 5;

// --- Timer ---
inline constexpr int kTimerLimit = 100;

// --- Spawning ---
inline constexpr int kSoulAshurnCount = 3;
inline constexpr int kSoulSpawnVariation = 3;

// --- Player initial state ---
inline constexpr int kPlayerInitialHealth = 5;
inline constexpr int kPlayerInitialMaxCoreHeat = 3;
inline constexpr int kPlayerInitialInventorySize = 12;
inline constexpr int kPlayerInitialAttackRange = 1;

// --- Ability: Phasing ---
inline constexpr int kPhasingHeatGain = 2;
inline constexpr int kPhasingSelfDamage = 1;
inline constexpr int kPhasingAoeDamage = 5;

// --- Ability: Deformation ---
inline constexpr int kDeformationHeatPerHp = 3;

// --- Special tiles ---
inline constexpr int kMagmaHeat = 1;
inline constexpr int kLavaHeat = 2;
inline constexpr int kAshCool = 1;
inline constexpr int kIceCool = 2;
inline constexpr int kSpecialTileCount = 6;  // сколько спецтайлов спавнить

// --- Ability: Disintegration ---
inline constexpr int kDisintegrationHeatDivisor = 2;
inline constexpr int kDisintegrationDamageMultiplier = 4;
inline constexpr int kDisintegrationRayLength = 5;
inline constexpr int kDisintegrationHeatGain = 5;

}  // namespace Constants

#ifndef BOSSCONFIG_H
#define BOSSCONFIG_H

namespace BossConfig{
    // スプライトの設定
    static inline constexpr int    FRAME_W       = 128; // スプライトの幅
    static inline constexpr int    FRAME_H       = 128; // スプライトの高さ
    static inline constexpr int    NUM_FRAMES    = 10;  // スプライトアニメーション枚数
    static inline constexpr double ANIM_INTERVAL = 0.1; // スプライトアニメーション切り替え秒数
    // 各種行動の設定
    static inline constexpr double MOVE_SPEED             = 100.0;    // 最高水平方向速度[px/s]
    static inline constexpr double JUMP_VELOCITY          = 400.0;    // ジャンプ初速[px/s]
    static inline constexpr double JUMP_INTERVAL          = 2.5;      // ジャンプ間隔[s]
    static inline constexpr double CHASE_RANGE            = 500.0;    // 追跡範囲[px]
    static inline constexpr double FIRE_INTERVAL          = 1.5;      // ファイアボール発射間隔[s]
    static inline constexpr double DAMAGE_INVINCIBLE_TIME = 1.5;      // ダメージ後の無敵時間[s]
    static inline constexpr double DYING_DURATION         = 1.5;      // 死亡演出間隔[s]

    static inline constexpr int CONTACT_DAMAGE  = 1;    // 接触時のダメージ
    static inline constexpr int FIREBALL_DAMAGE = 1;    // ファイアボールのダメージ
    static inline constexpr int SCORE_AT_DEATH  = 1000; // 撃破時のスコア
    static inline constexpr int STOMP_DAMAGE    = 1;    // 踏みつけで受けるダメージ
    // ボスの視界
    static inline constexpr double SIGHT_RANGE = 600.0;             // X軸
    // 攻撃発射を実施する高さ
    static inline constexpr double ATTACK_HEIGHT_TOLERANCE = 96.0;  // Y軸
}   // BossConfig

#endif  // BOSSCONFIG_H

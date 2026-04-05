#ifndef PLAYER_H
#define PLAYER_H

#include <vector>
#include <string>

#include <SDL2/SDL.h>

#include "Character.hpp"
#include "AnimationController.hpp"
#include "Sprite.hpp"
#include "GameEvent.hpp"

// 定数
#include "PlayerConfig.hpp"
#include "Direction.hpp"
#include "DrawBounds.hpp"

class Texture;
class Renderer;
struct Block;
struct Camera;

/**
 * @brief プレイヤー専用の見た目テクスチャ管理用
 * Playerの処理内部でしか使用しないためGameContextには保持しない
 * 
 */
struct PlayerTextureSet{
    Texture& small;
    Texture& super;
    Texture& fire;
};

class Player : public Character{
private:
    // テクスチャ管理
    PlayerTextureSet textures;
    // 接地状態管理
    bool onGround = false;
    // ダッシュ状態管理
    bool isDashing = false;
    // ジャンプ状態管理
    bool isJumping = false;     // ジャンプ中か
    double jumpElapsed = 0.0;   // ジャンプボタン押下からの経過時間
    // コヨーテタイム
    // ※落下判定の初期段階でジャンプを許可するやつ
    double coyoteTimer = 0.0;
    // 接地直前でもジャンプ可能とする時間の猶予
    double jumpableBufferTimer = 0.0;
    // 無敵時間
    double invincibleTimer = 0.0;
    // プレイヤーの状態
    PlayerForm form = PlayerForm::Small;
    // 天井のブロックを叩いたか
    bool ceilingBlockHit = false;
    // 叩いたブロックのインデックス
    std::size_t hitBlockIndex = static_cast<std::size_t>(-1);

public:
    // 定数
    // アニメーション枚数
    static inline constexpr int NUM_FRAMES = PlayerConfig::NUM_FRAMES;
    // コンストラクタ
    Player(const PlayerTextureSet& texSet);
    // 更新関数
    void update(double delta, const InputState& input, DrawBounds bounds, const std::vector<Block>& blocks) override;
    // Characterの衝突判定用矩形取得関数のオーバーライド
    SDL_Rect getCollisionRect() const override;
    // Renderer::drawを呼び出す関数
    void draw(Renderer& renderer, const Camera& camera) override;
    // リセットは2つの方針で区別する
    // 完全なリセット(new game用)
    void resetForNewGame();
    // 次のステージへ移動する用リセット
    void resetForStageTransition();

    // プレイヤーの頭座標サンプリング関数
    void beginFrameCollisionSample();
    // ダメージを受けられるか
    bool tryTakeDamage();
    // デバッグ用テキスト表示用
    std::string debugMoveContext();
private:
    // update()関係
    void inputProcessing(
        double delta, 
        const InputState& input, 
        double& moveDir, 
        bool& moving, 
        bool& dropThrough
    );
    // update関数の責務分利用関数群
    // x/vv/yを更新する関数
    void moveElementsUpdate(double delta, const InputState& input, const double moveDir);
    // ジャンプボタン押下時間の確認用
    void detectJumpButtonState(double delta, const InputState& input);
    // 物理処理
    void physicsProcessing(const std::vector<Block>& blocks, const bool dropThrough);
    // x軸方向の位置修正関数
    void clampHorizontalPosition(const DrawBounds& bounds);
    // アニメーション更新関数
    void animationProcessing(double delta, const bool moving);
    // 下からブロックを突き上げる際の判定関係初期化用関数
    void clearCeilingBlockHit();
    // 描画するべきか(無敵時間表現用)
    bool shouldRender() const;

    // PlayerFormの遷移に関する関数群
    void applyFormAppearance(bool keepFeet);
    Texture& currentFormTexture() const;
    const PlayerConfig::FormMetrics& currentFormMetrics() const;

public: // getterなどのインターフェイス
    void startInvincible(double duration){
        invincibleTimer = duration;
    }
    // getter/setter
    PlayerForm getForm() const{
        return form;
    }
    void setForm(PlayerForm pf){
        form = pf;
    }
    bool hasCeilingBlockHit() const{
        return ceilingBlockHit;
    }
    std::size_t getHitBlockIndex() const{
        return hitBlockIndex;
    }
    bool isInvincible() const{
        return invincibleTimer > 0.0;
    }
    // ダメージを受けられるか
    bool canTakeDamage() const{
        return !isInvincible();
    }
    // ダッシュ中か
    bool isDashingNow() const{
        return isDashing;
    }
};

#endif  // PLAYER_H

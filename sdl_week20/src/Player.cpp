#include "Player.hpp"

#include <algorithm>
#include <vector>
#include <string>

#include <SDL2/SDL.h>

#include "Renderer.hpp"
#include "Texture.hpp"
#include "Input.hpp"
#include "Block.hpp"
#include "Physics.hpp"

#include "Direction.hpp"
#include "DrawBounds.hpp"
#include "GameConfig.hpp"
#include "PlayerConfig.hpp"
#include "GameEvent.hpp"

/**
 * Playerクラスは操作するオブジェクトを管理する
 * 描画に関する処理はしない
 * SDLへの依存は最小限にする
 */

/**
 * @brief Construct a new Player:: Player object
 * 
 * @param tex 
 */
Player::Player(const PlayerTextureSet& texSet)
    : Character(
        PlayerConfig::POS_X,                // 初期座標
        PlayerConfig::POS_Y,                
        PlayerConfig::RUN_MAX_SPEED,        // speed
        0.0,                                // 水平速度
        0.0,                                // 垂直速度
        Direction::Right,                   // dir
        texSet.small,                       // texture
        PlayerConfig::SMALL_METRICS.frame_W,// Small状態 
        PlayerConfig::SMALL_METRICS.frame_H,// Small状態
        PlayerConfig::NUM_FRAMES,           // maxFrames
        0.1                                 // animInterval
    )
    , textures(texSet)
{
    sprite.setFrame(0);
    applyFormAppearance(false); // 足場を考慮する必要がないのでfalse
}

/**
 * @brief キーの入力に対して，差分をふまえて更新する
 * ウィンドウの描画範囲を超えないように画面内に座標を抑える
 * ジャンプを実装するので上下移動はなくなる
 * 
 * @param delta: 差分 
 * @param input: Inputクラスの状態=キー入力 
 * @param bounds: 描画可能範囲 
 */
void Player::update(double delta, const InputState& input, DrawBounds bounds, const std::vector<Block>& blocks){
    // 1. 1つ前のフレームのサンプリング 
    // 頭頂(プレイヤーの最上部)の位置
    beginFrameTopPhysicsSample();
    // 足元(プレイヤーの最下部)の位置
    beginFrameFeetPhysicsSample();   // prevFeetPhisicsのサンプリング
    // ブロック頭突き変数初期化
    clearCeilingBlockHit();
    // 水平移動用変数
    double moveDir = 0.0;
    // 移動中以外はfalse
    bool moving = false;
    // DropThroughの判定
    bool dropThrough = false;
    // 無敵時間減衰
    if(invincibleTimer > 0.0){
        invincibleTimer = std::max(0.0, invincibleTimer - delta);
    }
    // 2. 入力処理
    inputProcessing(
        delta, 
        input, 
        moveDir, 
        moving, 
        dropThrough
    );
    // 3. x/y/vvの更新
    moveElementsUpdate(delta, input, moveDir);
    // 4. 物理処理
    physicsProcessing(blocks, dropThrough);
    // 5. x軸方向のクランプ処理
    clampHorizontalPosition(bounds);
    // 6. アニメーション処理
    animationProcessing(delta, moving);
}

/**
 * @brief 衝突判定を返す関数
 * 衝突処理(ゲームロジック)をspriteから分離させる
 * 
 * @return SDL_Rect 
 */
SDL_Rect Player::getCollisionRect() const{
    // テクスチャに応じた幅を取得
    const auto& metrics = currentFormMetrics();
    // 当たり判定用の矩形を返す
    return{
        static_cast<int>(x) + metrics.collisionMargin_X, 
        static_cast<int>(y) + metrics.collisionMargin_Y,
        sprite.getDrawWidth() - metrics.collisionMargin_X *2,
        sprite.getDrawHeight() - metrics.collisionMargin_Y * 2
    };
}

/**
 * @brief Characterのdrawをオーバライドしている
 * 点滅用にCharacter::draw()の呼び出しを切り替える
 * 
 * @param renderer 
 * @param camera 
 */
void Player::draw(Renderer& renderer, const Camera& camera){
    if(!shouldRender()){
        return;
    }
    Character::draw(renderer, camera);
}

/**
 * @brief プレイヤーの状態を初期状態に戻す
 * 
 */
void Player::resetForNewGame(){
    // Playerの座標
    setPosition(PlayerConfig::POS_X, PlayerConfig::POS_Y);
    // 水平速度
    hv = 0.0;
    // 垂直速度
    vv = 0.0;
    // 設置状態(trueにしてclampで正常にする)
    onGround = false;
    isJumping = false;
    // タイマーなど
    jumpElapsed = 0.0;
    coyoteTimer = 0.0;
    jumpableBufferTimer = 0.0;
    invincibleTimer = 0.0;
    isDashing = false;
    // PlayerFormのリセット
    form = PlayerForm::Small;
    applyFormAppearance(false);
    // 天井判定リセット
    clearCeilingBlockHit();
    // アニメーションリセット
    anim.reset();
    sprite.setFrame(0);
}

/**
 * @brief プレイヤーの状態を初期状態に戻す
 * ただしPlayerFormは維持する
 */
void Player::resetForStageTransition(){
    // 水平速度
    hv = 0.0;
    // 垂直速度
    vv = 0.0;
    // 設置状態(trueにしてclampで正常にする)
    onGround = false;
    isJumping = false;
    // タイマーなど
    jumpElapsed = 0.0;
    coyoteTimer = 0.0;
    jumpableBufferTimer = 0.0;
    invincibleTimer = 0.0;  // 無敵時間は状態維持の対象外
    isDashing = false;
    // formは維持
    applyFormAppearance(false);
    // 天井判定リセット
    clearCeilingBlockHit();
    // アニメーションリセット
    anim.reset();
    sprite.setFrame(0);
}

/**
 * @brief x軸との接地管理用のクランプ関数
 * 
 * @param bounds 
 */
void Player::clampHorizontalPosition(const DrawBounds& bounds){
    x = std::clamp(x, bounds.min_X, bounds.max_X - sprite.getDrawWidth());
}

/**
 * @brief ジャンプボタン押下時間に応じた垂直速度設定関数
 * 
 * @param delta 
 * @param input 
 */
void Player::detectJumpButtonState(double delta, const InputState& input){
    // 重力
    double gravity = PlayerConfig::PLAYER_GRAVITY;
    // ジャンプによる上昇中のみ可変重力を設定
    if(isJumping && vv < 0.0){
        // ジャンプボタン押下時間計測
        jumpElapsed += delta;
        // ジャンプボタンが押されているか
        bool jumpButtomReleased = !input.pressed[static_cast<int>(Action::Jump)];
        // ジャンプボタンが規定時間以上押されているか
        bool overMaxHold = (jumpElapsed >= PlayerConfig::JUMP_HOLD_MAX_TIME);
        // ジャンプボタンを離した or 押下時間の上限に達したら通常の重力を適用
        if(jumpButtomReleased || overMaxHold){
            // 通常の重力
            isJumping = false;
        } else {
            // ジャンプボタンを押し続けているときは重力を弱くする(高く飛べる)
            gravity *= 0.2;
        }
    } 
    // 重力の加算は最後に加算
    vv += gravity * delta;
}

/**
 * @brief 入力の読み取りとその処理
 * 各キーの押下に応じた処理(変数の設定)を実施する
 * 
 * @param delta 
 * @param input 
 * @param moveDir 
 * @param moving 
 * @param dropThrough 
 */
void Player::inputProcessing(
    double delta, 
    const InputState& input, 
    double& moveDir, 
    bool& moving, 
    bool& dropThrough)
{
    // ダッシュ中か
    isDashing = input.pressed[static_cast<int>(Action::Dash)];
    // 左右
    if(input.pressed[static_cast<int>(Action::MoveLeft)]){
        moveDir -= 1.0;
        dir = Direction::Left;
        moving = true;
    }
    if(input.pressed[static_cast<int>(Action::MoveRight)]){
        moveDir += 1.0;
        dir = Direction::Right;
        moving = true;
    }
    // Jump
    if(input.justPressed[static_cast<int>(Action::Jump)]){
        jumpableBufferTimer = PlayerConfig::JUMPABLE_BUFFER_TIME;
    } else {
        jumpableBufferTimer = std::max(0.0, jumpableBufferTimer - delta);
    }
    // コヨーテタイマー
    if(onGround){
        coyoteTimer = PlayerConfig::COYOTE_TIME;
    }else{
        coyoteTimer = std::max(0.0, coyoteTimer - delta);
    }
    // ジャンプ可能条件
    const bool canUseCoyote = (onGround || coyoteTimer > 0.0);
    const bool hasBufferJump = (jumpableBufferTimer > 0.0);
    // ジャンプ動作
    if(canUseCoyote && hasBufferJump){
        vv = -PlayerConfig::JUMP_VELOCITY;  // 上方向はy軸的には負
        onGround = false;
        isJumping = true;
        jumpElapsed = 0.0;
        coyoteTimer = 0.0;  // ジャンプの猶予は0にする
        // jumpableBufferTimerはjustPressedがfalseになったら減衰して0になるが，ここで明示的に0にしておく
        jumpableBufferTimer = 0.0;
    }
    // DropThrough：接地中 かつ 下を押下
    if(onGround && input.pressed[static_cast<int>(Action::MoveDown)]){
        onGround = false;   // onGroundを外す
        dropThrough = true;
        coyoteTimer = 0.0;  // すり抜け時のジャンプは許可しない

    }
}

/**
 * @brief x/vv/yを更新する関数
 * 
 * @param delta 
 * @param input 
 * @param moveDir 
 */
void Player::moveElementsUpdate(double delta, const InputState& input, const double moveDir){
    // ダッシュ/非ダッシュ時でパラメータを切り替える
    // 最高速度
    const double maxSpeed = isDashing ? PlayerConfig::DASH_MAX_SPEED : PlayerConfig::RUN_MAX_SPEED;
    // 加速度
    const double accel = isDashing ? PlayerConfig::DASH_ACCELERATION : PlayerConfig::ACCELERATION;
    // 摩擦係数
    const double friction = isDashing ? PlayerConfig::DASH_FRICTION : PlayerConfig::FRICTION;
    // 物理の更新
    // 水平方向移動
    if(moveDir != 0.0){
        hv += moveDir * accel *delta;
        hv = std::clamp(hv, -maxSpeed, maxSpeed);
    } else {
        // 入力がないなら摩擦で減速する
        if(hv > 0.0){
            hv = std::max(0.0, hv - friction * delta);
        } else if(hv < 0.0){
            hv = std::min(0.0, hv + friction * delta);
        }
    }
    x += hv * delta;

    // 垂直方向移動
    // 可変ジャンプ
    detectJumpButtonState(delta, input);
    // detectJumpButtonState()でvvを更新からyを更新
    y += vv * delta;
}

/**
 * @brief 物理処理の呼び出しと状態の更新
 * 物理処理の順番は縦方向の補正→横方向の補正
 * 
 * @param blocks 
 * @param dropThrough 
 */
void Player::physicsProcessing(const std::vector<Block>& blocks, const bool dropThrough){
    // 移動後の(このフレームの)頭頂の位置の計算
    double newTop = y;
    // 移動後の(このフレームの)足元の位置の計算
    const double sprite_H = static_cast<double>(sprite.getDrawHeight());
    double newFeet = y + sprite_H;

    // 縦方向の補正
    // DTO作成
    VerticalCollisionState vcs{
        .prevTop  = getPrevTopPhysics(), 
        .newTop   = newTop, 
        .prevFeet = getPrevFeetPhysics(),  // 処理の最初で保存したprevFeetを参照
        .newFeet  = newFeet,
        .x        = x,
        .width    = static_cast<double>(sprite.getDrawWidth()),
        .vv       = vv,
        .onGround = onGround, 
        .ignoreDropThrough = dropThrough
    };
    // 垂直速度で判定を分岐する
    if (vv > 0.0) {
        // 落下：床判定だけ
        Physics::resolveBlockCollisionFromTop(vcs, blocks);
        y        = vcs.newFeet - sprite_H;
        vv       = vcs.vv;
        onGround = vcs.onGround;
    } else if (vv < 0.0) {
        // 上昇：天井判定だけ
        Physics::resolveBlockCollisionFromBottom(vcs, blocks);
        y  = vcs.newTop;
        vv = vcs.vv;
        // 戻ってきたvcsから頭突き判定を確認
        if(vcs.hitCeilingBlock){
            ceilingBlockHit = true;
            hitBlockIndex = vcs.hitBlockIndex;
        }
    } else {
        // vv == 0.0 → 何もしない
    }

    // 横方向の補正
    SDL_Rect col = getCollisionRect();
    HorizontalCollisionState hcs{
        .x      = static_cast<double>(col.x), 
        .y      = static_cast<double>(col.y), 
        .width  = static_cast<double>(col.w),
        .height = static_cast<double>(col.h),
        .hv     = hv
    };
    Physics::resolveHorizontalBlockCollision(hcs, blocks);
    // 帰ってきた結果をPlayer内部へ反映
    const auto& metrics = currentFormMetrics();
    x = hcs.x - metrics.collisionMargin_X;   // getCollisionRectのオフセット(現状はハードコード)
    hv = hcs.hv;

    // 状態の整理
    if(onGround && vv >= 0.0){
        // 頭をぶつけたようなvv>=0のタイミングで状態がちぐはぐになるのを防止
        isJumping = false;
        jumpElapsed = 0.0;
    }
}

/**
 * @brief アニメーション更新関数
 * 
 * @param delta 
 * @param moving 
 */
void Player::animationProcessing(double delta, const bool moving){
    // アニメーション処理
    if(!moving){
        anim.reset();
        sprite.setFrame(anim.getFrame());
        return;
    }
    // フレームを動かして描画
    anim.update(delta);
    sprite.setFrame(anim.getFrame());
}

/**
 * @brief 当たり判定用に用いる前フレームのプレイヤーのTop(y)とFeet(y+h)
 * をサンプリングする関数を呼ぶ
 * 
 */
void Player::beginFrameCollisionSample(){
    beginFrameTopCollisionSample();
    beginFrameFeetCollisionSample();
}

/**
 * @brief 天井ブロック判定の初期化
 * 
 */
void Player::clearCeilingBlockHit(){
    ceilingBlockHit = false;
    hitBlockIndex = static_cast<std::size_t>(-1);
}

bool Player::tryTakeDamage(){
    // 無敵時間なら処理しない
    if(isInvincible()){
        return false;
    }
    // Fire状態ならSuperに遷移※GameOverにしない
    if(form == PlayerForm::Fire){
        setForm(PlayerForm::Super);
        startInvincible(PlayerConfig::DAMAGE_INVINCIBLE_TIME);
        return false;
    }
    // Super状態ならSmallに遷移※GameOverにしない
    if(form == PlayerForm::Super){
        setForm(PlayerForm::Small);
        startInvincible(PlayerConfig::DAMAGE_INVINCIBLE_TIME);
        return false;
    }
    return true;
}

/**
 * @brief 被ダメージ時の無敵時間中の点滅表現用関数
 * 
 * @return true 
 * @return false 
 */
bool Player::shouldRender() const{
    // 無敵じゃないときは処理しない=trueを返す
    if(!isInvincible()){
        return true;
    }
    // delta時間で減少するinvincibleTimerで点滅用にtrue/false切り替え
    const int phase = static_cast<int>(invincibleTimer * 20.0);
    return (phase % 2) == 0;
}

/**
 * @brief PlayerFormの変更に対して対応するスプライトに切り替える
 * 
 * @param keepFeet 足元の座標を固定化させるかの引数(falseなら) 
 */
void Player::applyFormAppearance(bool keepFeet){
    // フレーム更新前の足元の座標取得
    const double prevFeet = y + static_cast<double>(sprite.getDrawHeight());
    // 現在のPlayerFormに対応するスプライト情報取得
    const auto& metrics = currentFormMetrics();
    // スプライトシート切り替え
    sprite.setTexture(currentFormTexture());                // テクスチャ差し替え
    sprite.setFrameSize(metrics.frame_W, metrics.frame_H);  // フレームサイズ更新
    sprite.setFrame(anim.getFrame());                       // アニメーションのフレームを維持する
    // 足元は固定して，キャラクタを伸縮させる
    if(keepFeet){
        // 切り替わったスプライトシート描画の高さを足元の座標へ適用
        y = prevFeet + static_cast<double>(sprite.getDrawHeight());
    }
}

/**
 * @brief 現在のPlayerFormに対応したテクスチャを返す
 * 
 * @return const Texture& 
 */
Texture& Player::currentFormTexture() const{
    switch(form){
    case PlayerForm::Small:
        return textures.small;
    case PlayerForm::Super:
        return textures.super;
    case PlayerForm::Fire:
        return textures.fire;
    }
    // フォールバック
    return textures.small;
}

/**
 * @brief PlayerFormに対応した幅の情報を返す
 * 
 * @return const PlayerConfig::FormMetrics& 
 */
const PlayerConfig::FormMetrics& Player::currentFormMetrics() const{
    switch(form){
    case PlayerForm::Small:
        return PlayerConfig::SMALL_METRICS;
    case PlayerForm::Super:
        return PlayerConfig::SUPER_METRICS;
    case PlayerForm::Fire:
        return PlayerConfig::FIRE_METRICS;
    }
    // フォールバック
    return PlayerConfig::SMALL_METRICS;
}

/**
 * @brief PlayerFormの変更処理
 * 
 * @param pf 
 */
void Player::setForm(PlayerForm pf){
    // 要求されたフォームが現在と同じなら何もしない
    if(form == pf){
        return;
    }
    form = pf;
    // 座標情報を加味して外見を変更
    applyFormAppearance(true);
}

/**
 * @brief デバッグ用テキスト出力関数
 * PlayingSceneで画面に文字を表示するために文字列を構成する
 * 
 * @return std::string 
 */
std::string Player::debugMoveContext(){
    std::string mvCtx = "";
    mvCtx = "hv: "      + std::to_string(hv)
          + "vv: "      + std::to_string(vv)
        //  + "onG: "     + std::to_string(onGround)
          + "inv: "     + std::to_string(invincibleTimer)
          + "frm: "     + std::to_string(static_cast<int>(form));
        //   + "cyt: "     + std::to_string(coyoteTimer)
        //   + "jbt: "     + std::to_string(jumpableBufferTimer)
        //   + "isJ: "     + std::to_string(isJumping)
          + "Dsh: "     + std::to_string(isDashing);
    return mvCtx;
}

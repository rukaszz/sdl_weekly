# The Mysterious Forest プレイテスト

Date: 2026-06-02

Build: sdl_week29

Environment:

- OS：Ubuntu 24.04 LTS
- コンパイラ：g++
- SDL：SDL2/SDL_image/SDL_ttf/SDL_mixer

## 総括

- 通しプレイ：OK
- シーン遷移(Title〜Result)：OK
- ポーズ・再開：OK
- ボス戦(開始〜終了)：OK
- 発見したバグ，不具合：3件

## テストケース

| No | Area | Test | Expected | Result | Notes |
| ---: | --- | --- | --- | --- | --- |
| 1 | Title | Enter押下 | new game開始 | PASS |  |
| 2 | Title | Esc押下 | game終了 | PASS |  |
| 3 | Playing | ポーズ(Esc) | ゲーム更新停止 | PASS |  |
| 4 | Playing | 再開(Esc) | ゲーム再開 | PASS |  |
| 5 | Playing | BGM/SE再生 | 想定通り | PASS |  |
| 6 | Player | キノコ取得 | Small -> Superへ遷移 | PASS |  |
| 7 | Player | ファイアフラワー取得 | ファイア形態へ変化 | PASS |  |
| 8 | Player | スーパー状態でダメージ | Downgrade + SE + shakeを確認 | PASS |  |
| 9 | Player | チビ状態でダメージ | 死亡演出 -> GameOver | PASS |  |
| 10 | Player | スーパー状態で落下死 | 死亡演出 -> GameOver -> チビ状態で再開を確認 | PASS |  |
| 11 | Item | コイン取得 | Score + SE + particleを確認 | PASS |  |
| 12 | Enemy | 敵踏みつけ | 敵死亡演出 + score + particleを確認 | PASS |  |
| 13 | Boss | ボスエリアへ侵入 | Boss BGM + camera固定を確認 | PASS |  |
| 14 | Boss | ボスに勝利 | Score + Clearを確認 | PASS |  |
| 15 | Clear | Enter押下 | Next stage or Resultへの遷移を確認 | PASS |  |
| 16 | Result | Enter押下 | Titleへ遷移 | PASS |  |

## 不具合

### 1. SEの二重発火

Playerが被弾→無敵状態のとき，敵に接触し続けているとダメージ音が連続で鳴る．
プレイ時に違和感を抱きやすいため，優先して治す．

### 2. ChaserEnemyのめり込み

ChaserEnemyはPlayerを発見すると，追跡モードになりBlockは無視する．ただしブロックの内部にいる状態でPlayerが視界から外れると，ブロック内で動けなくなる．
軽微な不具合だが，仕様を再検討するべき．

### 3. 敵の落下

敵が落下時に左右を連続して向く．Dirの更新を落下時は控えるべきである．

### 4. ボスが画面外に出てしまう

ボスはダメージを受けるとノックバックをするが，その際に画面外に出てしまい進行に支障が生じる．カメラ固定時は画面外に出ないようにし，また画面外に万が一出てしまった際の処理も追加して，進行不可能状態にならないようにする必要がある．

## 不具合への対応

### SEの二重発火の原因

原因は単純明快で，敵との接触を管理するCollisionSystemにてダメージを受けた際の処理でreturnをしていないことが原因であった．
これによって，被弾→死亡でのSE二重発火を防止した．

```diff
// 敵との接触
if(result == PlayerEnemyCollisionResult::PlayerHit){
    // ダメージ結果取得
    DamageResult dr = player.tryTakeDamage();
    // プレイヤーがSuper状態でのダメージ
    if(dr == DamageResult::Downgraded){
        events.playSound(SoundId::Damage);
        events.startCameraShake(0.18, 10.0);
+        return;
    }
```

### ChaserEnemyのめり込みへの対処

当初の設計は追跡時，壁を無視するという方針であった．これはプレイヤーを焦らせることを目的としていたが，結果的には壁のめり込みによって敵を意図しない方法で無力化することになってしまった．Enemyも左右にを連続で向くという奇妙な挙動となってしまうため，方針を改めた．

プレイヤーを追跡する際も，ブロックを壁と認識するようにすることで，めり込みを防止するようにした．
これによって，プレイヤーを追いかけようとして壁に阻まれている感じが演出でき，またプレイヤーが視界から逃れてもすぐに追いかけてくる，というChaserの要素を残すことができた．

```diff
void ChaserEnemy::think(double /*delta*/, const EnemySensor& es){
+    bool cannotMove = (!es.groundAhead || es.wallAhead);
+    // プレイヤーがいるとき かつ 壁がなければ追いかける
+    if(cannotMove){
+        hv = 0.0;
+        return;
+    }
    // プレイヤーがいないときは歩き回る
    if(!es.playerInSight){
-        if(!es.groundAhead || es.wallAhead){
+        if(cannotMove){
            dir = (dir == Direction::Right ? Direction::Left : Direction::Right);
        }
        hv = (dir == Direction::Right ? speed : -speed);
        return;
    }

-    // 近づきすぎたら追いかけを止める(仮実装)
-    constexpr double MIN_CHASE_DISTANCE = 1.0;
-    if(es.distanceToPlayer < MIN_CHASE_DISTANCE){
-        hv = 0.0;
-        return;
-    }

    // プレイヤーのいる位置を見て追跡方向を決定する
    if(es.playerOnLeft){
        dir = Direction::Left;
        hv = -speed;
    } else {
        dir = Direction::Right;
        hv = speed;
    }
}
```

### 敵落下時の高速方向転換

この現象が発生しうるのは，WalkerEnemy/ChaserEnemyの2種類．原因ははっきりしており，各Enemyの行動決定を行うthink()メソッドに問題があった．具体的な原因は現在が空中であるかを考慮していないということである．

各Enemyの接地状態を確認してからDirectionを決定するように修正することで，空中で暴れるような挙動を抑えることができた．

ChaserEnemy:

```diff
 void ChaserEnemy::think(double /*delta*/, const EnemySensor& es){
-    // プレイヤーが見えたときは壁を通り抜ける
+    // 前に進めるか
+    bool cannotMove = (!es.groundAhead || es.wallAhead);
+    // プレイヤーがいるとき かつ 壁がなければ追いかける
+    if(es.playerInSight && cannotMove){
+        hv = 0.0;
+        return;
+    }
+    // 接地していない場合はここで返す※空中での高速方向転換を防止
+    if(!isOnGround()){
+        return;
+    }
+
+    // プレイヤーがいないときは歩き回る
     if(!es.playerInSight){
-        // hv = 0.0;    // プレイヤーが見えていないなら何もしない
-        // プレイヤーがいないときは歩き回る
-        if(!es.groundAhead || es.wallAhead){
+        if(cannotMove){
             dir = (dir == Direction::Right ? Direction::Left : Direction::Right);
         }
         hv = (dir == Direction::Right ? speed : -speed);
         return;
     }
-
-    // 近づきすぎたら追いかけを止める(仮実装)
-    constexpr double MIN_CHASE_DISTANCE = 1.0;
-    if(es.distanceToPlayer < MIN_CHASE_DISTANCE){
-        hv = 0.0;
-        return;
-    }
-
    // プレイヤーのいる位置を見て追跡方向を決定する
    if(es.playerOnLeft){
        dir = Direction::Left;
        hv = -speed;
    } else {
        dir = Direction::Right;
        hv = speed;
    }
```

WalkerEnemy:

```diff
void WalkerEnemy::think(double /*delta*/, const EnemySensor& es){
+    // 接地中のみ方向転換※空中で高速方向転換するのを防止
+    if(!isOnGround()){
+        return;
+    }
    if(!es.groundAhead || es.wallAhead){
        // 崖や壁があるなら方向転換
        if(dir == Direction::Right){
            dir = Direction::Left;
        } else {
            dir = Direction::Right;
        }
    }
    // dirに応じてhvを設定する
    hv = (dir == Direction::Right ? speed : -speed);
}
```

### ボスがノックバック時に画面外へ出てしまう

原因はEnemy.stepPhysics()内で左方向のclamp処理が無いことが原因であった：

```diff
Enemy::stepPhysics(double delta, DrawBounds bounds, const std::vector<Block>& blocks){
    ...
    // 3. ワールドの端でのclamp処理
    // ※反転の処理はAI側で解決する
-    double leftBound = 0.0;
+    double leftBound = bounds.min_X;
    double rightBound = bounds.max_X - sprite.getDrawWidth();
    if(x <= leftBound){
        x = leftBound;
    } else if(x >= rightBound){
        x = rightBound;
    }
}
```

カメラ固定状態はボス戦時のみの処理なので，通常のステージでの敵の挙動では検知できなかった．
また，ボス戦でファイアボールでハメ続けていると顕在化しやすいので，なかなか気づけなかった．
対策として，画面左でもrightBoundと同じように，bounds.min_Xを追加した．bounds.min_Xは，カメラ固定状態でなければ0.0なので，問題は生じない．

別の対応としては，Boss戦のみに限定して影響範囲を抑えるために，専用のclamp用関数clampToBossArea()を導入することも考えた．上記のleftBoundは，Enemyを継承する全体へ影響するため対応の一案としては有効と考えたが，今回の場合は単純にカメラ固定処理導入時の不具合とみなした．

```cpp
// BossEnemy.hpp
// ボス戦エリア内にX座標をクランプする
void clampToBossArea(double areaMin_X, double areaMax_X);
// BossEnemy.cpp
/**
 * @brief ボスのX座標をボス戦エリア内にクランプする
 * ノックバックによる画面外脱出を防止する
 *
 * @param areaMin_X: ボス戦エリアの左端(world座標)
 * @param areaMax_X: ボス戦エリアの右端(world座標)
 */
void BossEnemy::clampToBossArea(double areaMin_X, double areaMax_X){
    const double bossWidth = static_cast<double>(BossConfig::FRAME_W);
    x = std::clamp(x, areaMin_X, areaMax_X - bossWidth);
}
```

また，ゲーム制作時に何度かボスが落下してゲームが進行不可能状態になったことがあった．そのため今回の不具合とは少し外れるが，画面外へボスが消えた場合，ゲームの進行不可能状態になることを防止する措置を追加した．
具体的には，ボス戦の状態を監視する関数checkBattleResult()に，ボスが消えた場合を考慮する条件を加えた．：

```diff
void BossBattleSystem::checkBattleResult(IGameEvents& events){
    // ボス戦が有効でなければ何もしない
    if(!state.isActive()){
        return;
    }
    // ボスが死んでいない
    if(!boss.isDead()){
+        // ボスが画面外にでてしまった際に
+        // ゲームが進行不可能状態にならないようにする処置
+        const double bossCenter_X = boss.getEntityCenter_X();
+        constexpr double ESCAPE_MARGIN = 300.0;
+        // クランプを抜けてボスがエリア外に大きく出た場合はクリア扱い
+        if(bossCenter_X < state.cameraMin_X - ESCAPE_MARGIN
+        || bossCenter_X > state.cameraMax_X + ESCAPE_MARGIN){
+            // ボス戦状態を敗北にしてクリア
+            state.phase = BossBattlePhase::Defeated;
+            events.requestScene(GameScene::Clear);
+        }
        return;
    }
    // ボス戦を終了させる
    state.phase = BossBattlePhase::Defeated;
    // 演出
    events.addScore(BossConfig::SCORE_AT_DEATH);
...
```

### 追加のテスト

| No | Area | Test | Expected | Result | Notes |
| ---: | --- | --- | --- | --- | --- |
| 1 | Playing | 無敵中の敵への接触継続 | ダメージ音が連続再生されない | PASS |  |
| 2 | Playing | ChaserEnemyが追跡状態でブロックがある方向へ進む | ChaserEnemyがブロックにめり込まない | PASS |  |
| 3 | Playing | Enemyの落下を観測 | 敵が高速方向転換をしない | PASS |  |
| 4 | Playing | Boss戦でボスを画面端へ追いやる | ボスが画面外へ出ていかない | PASS |  |

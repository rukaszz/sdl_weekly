# sdl-weekly

My study journey of SDL2.

毎日のSDLの学習と成果物をこちらへ置いていきます．
目的はなるべく整理され拡張が容易なゲームをシステムから作ることです．

## なぜSDLを選んだか？

移植性が高く，プラットフォームごとにソースの書き換えが必要となる頻度を抑えられます．また，外部依存性が低いので，システム(エンジン)から自作で作りたいという動機にうまくフィットしました．ボイラープレート(定型的なコード)が増えてしまう懸念点もありますが，なるべく設計で吸収します．

低レイヤの機能も揃っていて，入出力やウィンドウの描画が簡単にできるのも，試しやすいですし，C/C++との相性が良いのも魅力的でした．
基礎から学ぶという目的に合致してます．ただ3Dなど高機能な実装は難しいですが，ひとまず2Dから始めていきます．

DXライブラリやSFMLなども候補にあがりましたが，DXライブラリはWindows向けで私のUbuntu環境に合わず，SFMLも選択肢として非常にありでしたが，SDLのほうが抽象化が低く，低レイヤを触っていきたかったのでSDLで進めていきます．

## week01

### 目的

Create basic classes and functions.

小規模でも拡張性がある，破綻しない2Dゲームの基盤を構築する．重要な点は次のとおり：

- 責務の分離
- RAIIを軸としてリソース管理を徹底する
- 次週(スプライトやシーン管理)を見据えた，SDLの依存範囲を抑えた設計にする

### week01の設計について

ゲームロジックとSDLへの依存は分離させなければならない．SDLに関わる処理はWindowとRendererへ集約させ，PlayerとGameは極力SDLに依存させない．
また，オブジェクトの寿命をRAIIを軸に明確化して，描画・更新・イベント処理を分けてメインループへ実装する．

上記の方針に沿って，次の4クラスを作成した：

- Window：SDLの初期化/終了とウィンドウの管理．いわゆるキャンバス．アプリの寿命管理を担う．
  - 役割
    - SDL_Init()/SDL_Quit()を管理する
    - SDLのライフサイクル(アプリの開始/終了とWindowの生成)を集約する
    - 責務はWindowの生成と破棄(アプリの開始と終了)
  - 理由
    - Rendererと疎結合にすることで描画処理を任せ，役割を肥大化させない
    - SDLの終了タイミングをここで管理して，他のクラスにライフサイクルを管理させない
    - mainは起動に集中させ依存を分散させない
- Renderer：描画処理を担う．いわゆる筆
  - 役割
    - Windowはあくまで描画する場所であり，描画処理をRendererが担う
    - GameやPlayerをSDLと描画に依存させない
  - 理由
    - Windowは描画する場所であり，Rendererは描画するための窓口
    - 描画処理を一元管理させて，後々のスプライト処理などの実装に備える
    - 役割の関係性から，オブジェクトの関係は次のようになる
      - Window生成→Rendererの生成，Rendererの破棄→Windowの破棄の順番
- Player：座標や動作，更新など．とりあえず矩形がプレイヤー
  - 役割
    - Playerの状態(座標・速度など)を管理する
    - 描画の責務はRendererへ委譲する
    - 描画可能な位置，サイズなどを返す
  - 理由
    - Playerは毎フレームRendererへ状態を渡すことで，状態の変化と描画が一致する
    - Rendererとの責務の境界を明確にした
- Game：ゲームの処理をまとめる部分．ループ処理と呼び出しがメイン
  - 役割
    - メインループ，イベント処理，更新処理，ゲーム全体の状態管理
    - main.cppは起動や引数の責務を負っているため，ゲームにまつわる状態遷移を分離させた
    - SDLのイベントやフレームごとの処理などを実施
  - 理由
    - ゲームの処理やループを一元管理させて，他のオブジェクトに破綻が伝搬しないようにする

### 成果

- RAIIを意識したWindowとRendererを作成
- SDLになるべく依存しないPlayer/Gameの実装
- ゲームループを作成

## week02

### 目的

Separate certain rendering responsibilities from the Renderer．

Sprite/Textureを導入し，スプライトシートを用いてゲームのオブジェクトを画像で表現する．SpriteはRendererの責務を受け持つクラスであり，SpriteがRendererを参照するような循環参照にならないようにする．

### week02の設計について

SpriteとTextureを追加．ただし，Rendererとの参照関係は明確にして，循環参照にならないように注意する．
現状の関係は次の通り：

- Texture  ---依存---> SDL_Texture*
- Sprite   ---関節依存---> Texture（参照）
- Player   ---依存---> Sprite/Renderer(静的)
- Renderer ---依存---> SDL_Renderer*
- Game     ---依存---> Texture / Player / Renderer

TextureはRendererに依存してはならない．また，PlayerはSDLへの依存を排除しているが，Rendererへ静的に依存する．SpriteはRendererが呼び出す論理的な描画対象だが，Spriteの内部処理を知っている動的な依存状態である．

Rendererの責務を分離し，描画処理を抽象化した．主に実装したものは次の通り：

- Texture：GPUへアップロードされる画像データ
  - 役割：
    - スプライトが取り扱う画像を保持する参照先
    - ファイルではなくグラフィックのリソースとして扱うために変換・配置されたデータを管理する
  - 理由：
    - レンダラーはテクスチャを指定する命令者，スプライトは指定されたテクスチャのどの領域を矩形として切り取るかをみる単位
    - テクスチャという単位でグラフィカルなリソースを取り扱う
- Sprite：テクスチャの一部で論理的な描画対象
  - 役割：
    - Textureへアップしたデータをオブジェクト単位であつかう
    - スプライトシートのようにコマ送りの画像があるとき，それをフレーム単位で動かす
  - 理由：
    - テクスチャは画像をGPUへ読み込み，スプライトはその読み込んだ画像を取り扱う矩形
    - アニメーションを実施するためにフレーム単位であつかう

### IMG_QuitとSDL_Quit(SDLライフサイクル)

week01ではWindowクラスで初期化処理とSDLの終了処理を任せていたが，Gameクラスで管理を集中させるために，初期化/終了処理を移管した．
しかしながら，Valgrindでメモリリークのテストをしたところ，終了時にTextureの二重破棄によるメモリリークが生じた(valgrind_exe2.log)．これはTextureの破棄より先にSDL_Quitが呼ばれていたために生じたと考えられる．

つまりSDL_Quitは内部Renderer/Textureを破棄する処理が呼ばれる，その後，unique_ptr\<Texture\>::~Texture()が更にSDL_DestroyTexture()を呼ぶ．これによって二重破棄となってしまう．
TextureはRendererが生存中は破棄されてはならないが，Window→Renderer→Textureの破棄順序がGameのデストラクタの呼び出しでは守られない様子．

week01の設計に戻し，Windowクラスの破棄によってWindow→Renderer→Textureの破棄の順序を安定させることで，IMG_QuitとSDL_Quitを安全に実行するようにした．これによって，Textureの破棄に関するメモリリークが解消された(valgrind_exe3.log)．

SDLのライフサイクルはGameクラスに最終的には管理させるが，現在はWindowクラスに管理させる．理由としては，SDLの生APIの依存がWindowとRendererに分散しているので，Gameクラスで初期化および終了を管理させ一元化する．week03のタイミングで，SDLのライフサイクルに関する責任を一箇所で管理するリファクタリングを実施する予定．

### DBusのリーク

DBusはどうやらdbus_shutdown()を呼び出す際に，他のライブラリがdbusを使用しているとクラッシュする可能性があるらしく，あえてdbus_shutdown()を呼んでいない．そのためにvalgrind上でDBusによるリークが生じている模様．これはSDL側の問題として無視する．

[https://github.com/libsdl-org/SDL/issues/8272](https://github.com/libsdl-org/SDL/issues/8272)

## week03

### 目的

設計の修正(リファクタリング)．

PlayerクラスとEnemyクラスは共通箇所が多いが，将来的には個別の実装が生じる．そのため，抽象基底クラスCharacterを用意して共通部分を抜き出す．
また，アニメーション処理は共通箇所が多いかつGameクラスが肥大化する原因になることから，AnimationControllerクラスに分離した．

Windowで管理していたSDLライフサイクルをGameクラスが管理するように修正する→week04へ持ち越し

ゲームらしさの向上．

現状は描画に終止しておりゲームらしい部分ができていない．最低限ゲームらしい状態に持っていく．
具体的には，ゲームに状態を追加する(Playing, GameOver)ことで，2つの状態を切り替える．
生存するたびに1得点獲得する時間ベースのスコアを導入し，キャラクタを操作して敵から逃げて得点を稼ぐゲーム性にした．

### week03の設計

- AnimationController：アニメーションを管理する
  - 役割：
    - フレームの制御によるFPSの安定化
    - 今後アニメーションに関する処理(フレーム制御など)はこのクラスに実装される
  - 理由：
    - これまでPlayerクラスで描画の処理を実装しており，Playerの責務が肥大化していた
    - Characterクラスに属するオブジェクトからアニメーションに関わる処理を分離することで，Characterクラスの役割外の処理を分ける
- Character：PlayerやEnemyなどのオブジェクトの抽象クラス
  - 役割：
    - PlayerやEnemyなどの動きの処理がある程度共通化しているクラスの共通部分を持つ抽象クラス
    - updateなどオブジェクトに依るものは純粋仮想関数，それ以外は仮想関数として定義する
  - 理由：
    - PlayerとEnemyで共通する処理が多い(画面外へはみ出さない補正処理，Rendererクラスへの橋渡しなど)ため，共通化することでコード量を減少させる
- Text：文字を描画するクラス
  - 役割：
    - 文字列を画面(Window)へ描画するためのクラス
    - フォントの読み込みがあるため，画像のようなTexture→Sprite→Rendererの流れに乗せず，Textureへ適用させたらRendererへ描画する別の流れをもたせる
  - 理由：
    - Sprite/Textureの流れへ押し込むと，現在の抽象化が破綻する
    - Textはフォントをロードし，Surface→Textureの短期生成で文字列を扱う
    - 描画処理はRendererの役割であり，Renderer側に文字列描画用のAPIを用意する

### 衝突処理

GameUtilというヘッダにintersectsという関数を実装した．これはAABB(Axis-Aligned Bounding Box)と呼ばれる，矩形の交差判定を行う衝突判定方式である．

回転していない矩形を用いるので，矩形同士の重なりが衝突となる．矩形A，Bがあると仮定したとき衝突しない条件とは：

- Aの右端がBの左端より左
- Aの左端がBの右端より右
- Aの下端がBの上端より上
- Aの上端がBの下端より下

これら条件が成り立つとき衝突していないということなので，これらが成り立たなければ衝突しているということになる．

### week04の予定

week03の設計不備の修正が必要．

1. Textクラス：都度Textureを生成する，ラスタライズ(TTF_RenderUTF8_Blended)は高負荷なので修正必須
2. scoreの計算：現状はフレームごとに加算しているが，これでは60FPSと30FPSでスコアがブレる
3. 衝突判定のSprite依存：Spriteは描画用の存在であり，衝突判定というゲームロジックが依存してはいけない

とりあえず上記の3つの修正はマスト．

## week04

### week04の設計

Gameクラスが多くの処理を担うようになり，責務過多になっていたためゲームの各状態に応じてSceneクラスを導入して責務を分離した．

- Scene：ゲームの各状態(シーン)を管理する抽象クラス
  - 役割：
    - 各シーンで共通する処理(update/render/event)を定義し，インターフェースを提供する
    - Gameをゲームループとリソースの管理に集中させる
    - Gameを参照することができ，Gameに関係するオブジェクトの更新などを行う
    - シーン遷移をonEnter/onExitで明確化する
  - 主要メソッド：
    - handleEvent()：キー入力やクリックなど，イベント処理
    - update()：シーン固有の更新処理
    - render()：シーン固有の描画処理
    - onEnter()：シーン遷移時の初期化処理
    - onExit()：シーン遷移時の終了処理
  - 理由：
    - ゲームの状態はweek04の時点でTitle/Playing/GameOverの3種
    - 各種の状態で共通する更新処理，描画処理を抽象化
    - 責務ごとにコードを分割でき，拡張しやすい設計にした
- TitleScene：タイトル画面の処理を担う
  - 役割：
    - GameConfig::Titleの状態
    - タイトルテキストの描画処理(フェードイン，点滅)
  - 理由：
    - ゲーム起動時に呼ばれるタイトルシーンで行う処理を管理する
    - ゲーム開始前の演出を担う入口である
- PlayingScene：ゲームプレイ時の処理を担う
  - 役割：
    - Game::update()の中心的な処理を担う(ゲームロジックの中心)
    - スコアの更新〜キャラクタの更新など
  - 理由：
    - ゲームをプレイするときの更新処理をここに集約した
    - 現状はGameの処理をそのまま移管しただけなので，さらに処理を分離する必要がある
      - update() が 5 種類の責務（入力処理/物理/敵AI/スコア/衝突判定）を持っている
      - これら処理を分割する
- GameOverScene：ゲームオーバー時の処理
  - 役割：
    - ゲームオーバー時の処理を担う
    - リトライを実施する
  - 理由：
    - 現状はTitleSceneと似た処理をしているが，TitleSceneはゲーム開始時の処理，GameOverSceneはゲーム終了時の処理を担う入口である
    - GameOverSceneはリトライというユーザの操作の入口である←Titleとは明確に役割が異なる

### なぜ分離が必要か

Gameクラスがいわゆる神クラスになっており可読性が低下していた．これによって，update/render/eventの分離ができず各処理の境界が曖昧になっていた．updateで描画に関する処理を多数になったり，eventで管理するべき情報をupdateでも確認しており，これによって保守が難しくなりつつあった．

各ゲームシーン(Title/Playing/GameOver)の責務も曖昧であり，またシーンごとの初期化・終了処理がGameクラスで実施されていた．密結合状態であり，初期化・終了処理の分離が難しく，これでは各シーンごとの単体テストを実施する際に，テストが煩雑になる．

これら問題を解消するためにゲームシーンを抽象化し，ゲームの状態別で管理する独立したクラスとして切り出す設計に移行した．
GameにchangeSceneを実装し，シーンの切り替えをonEnter/onExitで統一的に遷移するようにしたことで，初期化・終了処理が明確になった．

### SDLの終了処理について

SDL_Init()から各種サブシステムを初期化してゲームを動かしているが，ゲームのデストラクタ呼び出し時，すなわちゲーム終了時にコアダンプが生じるようになった．ゲームの終了処理順番としてはWindow→Renderer→Textureのように関連するシステムが順に終了していく．

SDL終了時にコアダンプが生じる場合，考えられるのは二重に破棄する処理が動いていることである．SDLのWikiで確認したところ，SDL_Quit()はすべてのサブシステムを終了するという旨の記述がある．
SDL_Quit は SDL 内部のサブシステムを終了するだけで，SDL_image や SDL_ttf の内部リソースは破棄しない．また，TTF_Init()はTTF_Quit()の破棄と呼び出し回数を一致させるなどの制約があるため，SDL_Quit()で残留するメモリは可能な限り減らす必要がある．

今回の場合は，TTF_Quit()関係でコアダンプが生じていた．

#### TTF_Quit()

曲者だったのはTTF関係の初期化と終了処理である．TTFはもちろんTTF_Init()とTTF_Quit()を呼び出さなければならないが，Textクラスはfontを読み込んでいる(TTF_OpenFont)．このとき，TTF_CloseFontは別途呼ぶ必要がある．

今回のコアダンプの原因はTTF_Quit()にある．原因としては次の流れである：

1. Game::~Game()が呼ばれる
2. Textオブジェクトが生存している
3. TTF_Quit()が呼ばれfont関係のサブシステムが終わる
4. Text::~Text()が呼ばれ，TTD_CloseFont()が呼ばれる
5. 終了済みのTTF関係のサブシステムへアクセス
6. コアダンプ

そのため，TTF_Quit()と~Text()の終了の順序を正しくするために，

```cpp
// Gameで管理しているオブジェクトを破棄させるスコープ
{
    Game game;
    game.run();
}
// Gameが破棄されたらSDLを終了
TTF_Quit();
IMG_Quit();
SDL_Quit();
```

Gameのスコープを明示し，終了の順序が~Text()→TTF_Quit()となるようにした．

Gameクラスが終了するとき，TTF_Quit()→IMG_Quit()→SDL_Quit()の順で呼ばれるが，Gameクラス終了までTextクラスのオブジェクトが残り続ける．TTF_Quit()が呼ばれたあとにTextのオブジェクトが破棄され，TTF_CloseFontが呼ばれてコアダンプが生じていると考えられる．

そのため，

1. Game インスタンスより早く SDL サブシステムを落とさない
2. Renderer → Texture → Font → SDL_ttf → SDL_image → SDL の順序が守られないとクラッシュする

以上の2点を守るように終了順序を定めた．

## week05

### GameとSceneの分離

week04の状態では Game と Scene が強く結合しており，Scene が Game クラスの詳細（メンバ変数や関数）を直接参照していた．その結果，

- Game の責務が肥大化する
- Scene 側から Game の修正が伝播しやすい

という問題があった．

week05 では，Game が管理しているオブジェクト群から Scene に本当に必要なものだけを抜き出し，GameContext 構造体としてまとめた．また，Scene から Game への呼び出しも SceneControl インターフェース経由に制限することで，Game と Scene の依存関係を整理した．

- GameContext：Scene から参照されるゲームのコンテキスト
  - 主なフィールド：
    - Renderer（描画用）
    - Player / Enemy 配列
    - TextTexture（スコア表示・FPS表示など）
    - 乱数生成器と分布（敵の初期位置・速度用）
    - Text（フォント）
    - Input（入力状態）
  - 役割：
    - Scene に必要な「ゲーム内オブジェクト」をひとまとめにし，Game 本体の詳細構造を Scene から隠蔽する
    - Scene は GameContext 経由でのみオブジェクトにアクセスする

Game は std::unique_ptr\<Scene\> scenes\[3\] と Scene* currentScene を持ち，changeScene() でシーンを切り替える．シーン切り替え時には onExit() / onEnter() を必ず呼ぶことで，各 Scene 自身が「入室時・退室時の初期化／後始末」を管理できるようにした．

#### SceneControlの導入

Scene 系のクラスが Game クラスの関数を直接呼ぶと，Scene が Game の実装に強く依存してしまう．これを避けるために，Game のうち Scene から利用したい操作だけを抽象インターフェース SceneControl として切り出した．

- SceneControl インターフェース：
  - 提供する機能（現時点）：
    - changeScene(GameScene id)：シーンの切り替え
    - resetGame()：プレイ開始時のゲーム状態リセット
    - getScore() / setScore()：スコアの取得・更新
  - 役割：
    - Scene が「Game そのもの」ではなく「SceneControl という抽象」に依存するようにする
    - Game 側は SceneControl を実装するだけでよく，Scene は Game の内部構造を一切知らない

Game は SceneControl を実装し，Scene はコンストラクタで SceneControl& を受け取る．これにより，Scene は Game の具体的な型に依存せず，必要最小限の操作だけを通じてゲーム全体を制御できる．

依存性逆転の原則（Dependency Inversion Principle）：

- 高水準モジュール（Scene）は低水準モジュール（Game）の具象実装に依存しない
- 両者とも抽象（SceneControl）に依存する
- Scene → SceneControl ← Game という形で依存を逆転させることで，Game と Scene の結合度を下げ，変更に強い構造にした

### 入力状態のテーブル化

これまでは，入力は Game や Player などの各クラス内で if / switch 文を直接書いて監視しており，

- 入力が増えると条件分岐が肥大化する
- 入力処理とゲームロジックが混在し，保守性が低い

といった問題があった．

week05 では，入力を抽象化した Action と，その状態をまとめる InputState を導入し，「入力状態のテーブル化（マッピング）」を行った．

```cpp
// 入力状態のマッピング
enum class Action{
    MoveLeft, 
    MoveRight, 
    MoveUp, 
    MoveDown,
    Jump, 
    Pause,
    Enter,
    None,   // 番兵（配列の範囲外判定用）
};

struct InputState{
    // 列挙値の最大（今回は Enter）+ 1 ぶん確保
    bool pressed[(int)Action::Enter + 1]     = {false};
    bool justPressed[(int)Action::Enter + 1] = {false};
};

// 使用例
const InputState& input = ctx.input.getState();
if(input.pressed[(int)Action::MoveLeft]){
    moveLeft();
}
```

- pressed：キーが押されている間，ずっと true

- justPressed：キーが押された「そのフレームのみ」true

Input::update() で毎フレーム false にリセットする

入力の流れは次のようになる：

1. Game::processEvents() で SDL のイベントをポーリング
2. Input::handleEvent() にイベントを渡し，pressed / justPressed を更新
3. Scene の update() から InputState を読み取る
4. Input::update() で justPressed をクリア

これにより，

- 各クラスは「SDL のキーコード」ではなく「Action列挙」に対してロジックを書く

- 入力処理はすべて Input クラスへ集約され，ゲームロジックから分離される

- 新しい入力を追加しても，Action とマッピングを追加するだけで済む

### week05の設計

入力を Input クラスに集約し，ゲーム全体は InputState の真偽値を読み取るだけにした．

- Input：入力を受け取るクラス
  - 役割：
    - キーボード（今後はパッドなども想定）からの入力を受け取り，アクションごとの押下状態をフラグで保持する
    - 押しっぱなし（pressed）と押した瞬間（justPressed）を区別して管理する
  - 理由：
  - 各クラスが SDL のキーコードを直接扱うのではなく，Input が窓口になって Action 状態を提供することで，入力処理とゲームロジックを分離できる
  - 「このフレームでジャンプボタンが押されたか」「押しっぱなしになっているか」を明確に区別できるようになる

Scene 側は SDL のイベントを気にせず，GameContext に渡された Input の状態だけを参照するようになった．例えば：

- TitleScene：Action::Enter の justPressed を見て PlayingScene へ遷移
- PlayingScene：
  - Action::Pause の justPressed で TitleScene へ戻る
  - Action::MoveLeft / MoveRight の pressed を見て Player の移動方向を決定
  - Action::Jump の justPressed でジャンプ開始

### 時間依存の更新への移行

week04 までは，SDL_GetTicks() を使った「フレーム時間ベース」の更新を行っていたが，より安定した時間制御のために SDL_GetPerformanceCounter() / SDL_GetPerformanceFrequency() を用いた高精度タイマに移行した．

- delta の計算：
  - 前フレームと現在の SDL_GetPerformanceCounter() の差分を SDL_GetPerformanceFrequency() で割って秒単位に変換
  - delta が極端に大きくなった場合（例：一時停止後など）は 0.1 秒（≒10 FPS）にクランプし，物理更新が暴れないようにした

- 更新ループ：
  - delta を各 Scene の update() に渡し，速度・重力・アニメーションなどは時間ベースで更新
  - フレームキャップは従来どおり SDL_Delay() で 60 FPS を上限にする

これにより，多少フレームレートが変動してもゲーム速度が極端に変わらないようになった．

### 重力とジャンプの導入

これまではキャラクタが上下左右へ自由に移動できるだけで，物理的な感覚はなかった．マリオライクなゲームを作成する方針に切り替えたため，プレイヤーキャラクタに重力とジャンプを導入した．

- Player クラス：
  - Character の派生として，水平速度に加えて垂直速度 vv を持つ
  - 毎フレーム vv に重力加速度を足し，y 座標に反映する
  - onGround フラグで接地状態を管理し，「地面にいるときだけジャンプ入力を受け付ける」
- 入力とジャンプ：
  - 左右移動：Action::MoveLeft / MoveRight の pressed をベースに水平移動
  - ジャンプ：onGround かつ Action::Jump の justPressed のとき vv に負の速度を与え，上方向へ跳躍する

- 接地判定：
  - clampToGround(const DrawBounds&)：
  - 床の Y 座標（画面下端 − スプライト高さ）を境界としてクランプ
    - プレイヤーがそれより下に行かないようにし，接地時には vv = 0 / onGround = true に戻す
  - clampHorizontalPosition(const DrawBounds&)：
    - X 座標を 0〜画面幅 − プレイヤー幅 にクランプし，画面外にはみ出さないようにする

なお，Character::update() のシグネチャを update(double delta, const InputState& input, DrawBounds bounds) のように拡張し，入力を受け取れる形にした．現状，プレイヤーは入力を利用し，敵キャラクタは入力を無視しているが，将来的には「プレイヤー入力に反応して挙動を変える敵」などにも拡張できるようにしてある．

### week06の予定

現状のゲームは，

「プレイヤーが左右移動とジャンプで敵を避ける」，「床はウィンドウ下端の一枚だけ」という最小限の遊び要素にとどまっている．week06 では，キャラクタ以外の要素でゲーム性を拡張していく．

- 床・ブロックの導入：
  - 実際の床スプライト（タイル）を描画し，当たり判定を実装する
  - プレイヤーがジャンプで「乗る」「乗り越える」ことができるブロックを配置する
- 当たり判定の拡張：
  - プレイヤーと床／ブロックとの衝突方向（上から着地・横から衝突など）を判定し，正しい位置補正と速度修正を行う
- シーン側の責務整理：
  - ブロックや床の管理をどこに持たせるか（専用マップクラスを作るか，簡易の配列で持つか）を決める

今後のマップ構造（タイルマップ／レベルデータ）を見据えた設計にしていく

## アセット

詳細はATTRIBUTIONを参照．

- Red Hat Boy

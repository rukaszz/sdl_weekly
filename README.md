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
- Sprite   ---間接依存---> Texture(参照)
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
      - update() が 5 種類の責務(入力処理/物理/敵AI/スコア/衝突判定)を持っている
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

week04の状態では Game と Scene が強く結合しており，Scene が Game クラスの詳細(メンバ変数や関数)を直接参照していた．その結果，

- Game の責務が肥大化する
- Scene 側から Game の修正が伝播しやすい

という問題があった．

week05 では，Game が管理しているオブジェクト群から Scene に本当に必要なものだけを抜き出し，GameContext 構造体としてまとめた．また，Scene から Game への呼び出しも SceneControl インターフェース経由に制限することで，Game と Scene の依存関係を整理した．

- GameContext：Scene から参照されるゲームのコンテキスト
  - 主なフィールド：
    - Renderer(描画用)
    - Player / Enemy 配列
    - TextTexture(スコア表示・FPS表示など)
    - 乱数生成器と分布(敵の初期位置・速度用)
    - Text(フォント)
    - Input(入力状態)
  - 役割：
    - Scene に必要な「ゲーム内オブジェクト」をひとまとめにし，Game 本体の詳細構造を Scene から隠蔽する
    - Scene は GameContext 経由でのみオブジェクトにアクセスする

Game は std::unique_ptr\<Scene\> scenes\[3\] と Scene/currentScene を持ち，changeScene() でシーンを切り替える．シーン切り替え時には onExit()/onEnter() を必ず呼ぶことで，各 Scene 自身が「入室時・退室時の初期化/後始末」を管理できるようにした．

#### SceneControlの導入

Scene 系のクラスが Game クラスの関数を直接呼ぶと，Scene が Game の実装に強く依存してしまう．これを避けるために，Game のうち Scene から利用したい操作だけを抽象インターフェース SceneControl として切り出した．

- SceneControl インターフェース：
  - 提供する機能(現時点)：
    - changeScene(GameScene id)：シーンの切り替え
    - resetGame()：プレイ開始時のゲーム状態リセット
    - getScore() / setScore()：スコアの取得・更新
  - 役割：
    - Scene が「Game そのもの」ではなく「SceneControl という抽象」に依存するようにする
    - Game 側は SceneControl を実装するだけでよく，Scene は Game の内部構造を一切知らない

Game は SceneControl を実装し，Scene はコンストラクタで SceneControl& を受け取る．これにより，Scene は Game の具体的な型に依存せず，必要最小限の操作だけを通じてゲーム全体を制御できる．

依存性逆転の原則(Dependency Inversion Principle)：

- 高水準モジュール(Scene)は低水準モジュール(Game)の具象実装に依存しない
- 両者とも抽象(SceneControl)に依存する
- Scene → SceneControl ← Game という形で依存を逆転させることで，Game と Scene の結合度を下げ，変更に強い構造にした

### 入力状態のテーブル化

これまでは，入力は Game や Player などの各クラス内で if / switch 文を直接書いて監視しており，

- 入力が増えると条件分岐が肥大化する
- 入力処理とゲームロジックが混在し，保守性が低い

といった問題があった．

week05 では，入力を抽象化した Action と，その状態をまとめる InputState を導入し，「入力状態のテーブル化(マッピング)」を行った．

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
    None,   // 番兵(配列の範囲外判定用)
};

struct InputState{
    // 列挙値の最大(今回は Enter)+ 1 ぶん確保
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
    - キーボード(今後はパッドなども想定)からの入力を受け取り，アクションごとの押下状態をフラグで保持する
    - 押しっぱなし(pressed)と押した瞬間(justPressed)を区別して管理する
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
  - delta が極端に大きくなった場合(例：一時停止後など)は 0.1 秒(≒10 FPS)にクランプし，物理更新が暴れないようにした

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
  - 床の Y 座標(画面下端 − スプライト高さ)を境界としてクランプ
    - プレイヤーがそれより下に行かないようにし，接地時には vv = 0 / onGround = true に戻す
  - clampHorizontalPosition(const DrawBounds&)：
    - X 座標を 0〜画面幅 − プレイヤー幅 にクランプし，画面外にはみ出さないようにする

なお，Character::update() のシグネチャを update(double delta, const InputState& input, DrawBounds bounds) のように拡張し，入力を受け取れる形にした．現状，プレイヤーは入力を利用し，敵キャラクタは入力を無視しているが，将来的には「プレイヤー入力に反応して挙動を変える敵」などにも拡張できるようにしてある．

### week06の予定

現状のゲームは，

「プレイヤーが左右移動とジャンプで敵を避ける」，「床はウィンドウ下端の一枚だけ」という最小限の遊び要素にとどまっている．week06 では，キャラクタ以外の要素でゲーム性を拡張していく．

- 床・ブロックの導入：
  - 実際の床スプライト(タイル)を描画し，当たり判定を実装する
  - プレイヤーがジャンプで「乗る」「乗り越える」ことができるブロックを配置する
- 当たり判定の拡張：
  - プレイヤーと床／ブロックとの衝突方向(上から着地・横から衝突など)を判定し，正しい位置補正と速度修正を行う
- シーン側の責務整理：
  - ブロックや床の管理をどこに持たせるか(専用マップクラスを作るか，簡易の配列で持つか)を決める

今後のマップ構造(タイルマップ／レベルデータ)を見据えた設計にしていく

## week06

### week06の設計

ブロックを導入し，床も含めてすべてブロックとして扱う方針に切り替えた．  
これに伴って，プレイヤーとブロックの接触判定を追加し，Standableブロック上への着地処理とDamageブロックとの衝突によるゲームオーバー処理を実装した．  
また，重力の実装によって画面外へ落ち続けていた状態だったため，「一定の高さまで落下したらゲームオーバーになる」落下死判定を追加した．

### Block構造体

ブロックの情報をまとめるためのデータ構造を用意した．

- 役割
  - ブロックの x, y 座標と幅・高さを管理する
  - BlockTypeによって床として乗れるのか，ダメージ床なのか，といった役割を区別する
- 種別(BlockType)
  - Standable：乗れる通常ブロック．色は白
  - DropThrough：乗ることができて，下ボタン押下ですり抜けられるブロック(挙動は未実装)．色は青
    - 現時点では種別として定義しているだけ(Standableと区別するため)
  - Damage：当たるとダメージを受けるブロック(現状は即死)．色は赤
- 理由
  - ブロックは画面上に複数個配置される前提であり，std::vector\<Block\> でまとめて管理できるよう，シンプルな構造体として定義した
  - SDL で描画するためのSDL_Rectへの変換はblockToRect()というヘルパを用意し，Block→SDL_Rectの変換を一箇所に集約している

### BlockLevelLoader クラス

ブロック配置を外部ファイルで定義するための読み込み用関数．

- 役割
  - レベルファイル(assets/level/level1.txt)を読み込み，各行をBlockの配列に変換する
  - レベルファイルのフォーマット:
    - 1 行につき 1 ブロック定義
    - 形式："種類文字" "x" "y" "w" "h" # コメント
    - 例：
      - S 0   750 800 50 # 床
      - T 200 380 100 20 # 空中の足場
      - D 300 780 100 20 # ダメージ床
- 実装上のポイント
  - 行の途中に # があれば，それ以降をコメントとして切り捨てるようにした
  - 行頭・行末の空白はトリムした上で，空行は読み飛ばす
  - パースに失敗した行は std::cerr にログを出してスキップし，ゲーム自体は継続できるようにした
  - ファイルオープンに失敗した場合は，Game側で例外をキャッチし，「画面下端に 1 本の Standable の床を置く」というフォールバックを行う
- BlockType マッピング
  - S→BlockType::Standable
  - T→BlockType::DropThrough
  - D→BlockType::Damage
  - それ以外の文字は無視してスキップする
- 理由
  - ブロックの形状・役割・配置はステージごとに変わる前提なので，コードにハードコードせず外部ファイルに切り出して柔軟性をもたせた
  - レベルファイルを差し替えるだけでステージ構成を変えられるようにしておくことで，あとからマップを増やすときの負荷を下げられる

### Player クラス(床判定)

Player は引き続き入力に応じて動くキャラクタだが，week06でブロックとの接地判定を Block ベースに統一した．

- update(double delta, const InputState& input, DrawBounds bounds, const std::vector\<Block\>& blocks)
  - 入力は左右移動とジャンプを受け取る
  - 重力を vv(垂直速度)に加算し，y へ反映する
  - 垂直移動の前後で「足の高さ(prevFeet / newFeet)」を計算し，clampToGround に渡す
- clampToGround(double prevFeet, double newFeet, const std::vector\<Block\>& blocks)
  - 垂直速度 vv > 0(落下中)のときだけ，Standable ブロックとの着地判定を行う
  - 各ブロックに対して次の内容を判定し，条件を満たしたら「そのブロックの上に着地」させる：
    - プレイヤーの左右の範囲とブロックの左右の範囲が重なっているか
    - 足が「前フレームではブロック上端より上，今フレームでブロック上端より下」という状態を跨いだか
  - 着地した場合はyをブロック上端にスナップし，vv=0，onGround=trueにする
  - どのブロックにも乗っていなければ onGround=falseにする
- 現時点の問題点
  - プレイヤーの物理挙動(重力・床との衝突・着地処理)が Player クラスにベタ書きされており，将来的に Enemy など他のオブジェクトにも同じような処理を書き始める破綻する
  - week07 では，この床判定を Physics モジュール側へ切り出し，「位置・速度などの値だけを Physics に渡して処理する」形にリファクタリングが必要

### PlayingScene クラス

ゲームプレイ中の描画と衝突判定を担当するシーンクラス．

- render
  - ctx.entityCtx.blocks を走査し，BlockTypeに応じて色を変えて矩形を描画する
    - Standable：白
    - DropThrough：青(ただし現段階では挙動は未実装)
    - Damage：赤
  - FPS，スコア，プレイヤー，敵キャラクタを描画する
- checkCollision
  - プレイヤーと敵との当たり判定を行い，衝突したら GameOverScene へ遷移する
  - プレイヤーと BlockType::Damageのブロックとの衝突判定を行い，当たった場合も GameOverScene へ遷移する(現状は即死扱い)
- hasFallenToGameOver
  - プレイヤーの足元が「画面下から一定距離を超えて落下した」ときにゲームオーバーにする
  - 現状は「ウィンドウ下端＋プレイヤースプライト高さ分」をしきい値としている

### トリム処理と Boost 依存

BlockLevelLoader::loadFromFileで，「#」 以降をコメントとして扱うために，行ごとにトリム処理を行っている．
現状は boost::algorithm::trim を使用しているが，これは設計としては微妙な判断である．

- 問題点
  - 行のトリムだけのために Boost へ依存しており，ビルド環境に余計な依存関係を増やしている
  - この規模のシンプルなゲームにおいて，「外部ライブラリのバージョンやインストール状況に引きずられる要素」を増やすメリットはほぼない
- 今後の方針
  - std::string と標準ライブラリだけで簡単なトリム関数を自前実装し，Boost への依存を外す
  - レベルファイルの読み込みという低レイヤな処理は，極力シンプルで移植性の高い実装にしておく

### week07の予定

現状の課題は次の 2 点に集約される．

1. 物理挙動のPlayerベタ書き問題
   - 重力・床との衝突・着地処理が Player に集中しており，Enemy など他のオブジェクトに同様の処理を追加するとコードが増殖する
   - Physicsクラスなどを用意し，「位置・速度・足の高さなどの値を受け取ってブロックとの衝突を解決する関数」を切り出す
   - Playerは自分のprivateメンバをさらさない方向性で，「必要な値だけを構造体や引数としてPhysicsに渡すDTOを構成し，結果を書き戻す」形にする

2. ステージ表現とカメラの問題
   - 現状はウィンドウ内に収まる前提でステージを作っており，横スクロール・カメラ移動が存在しない
   - 将来的にマリオライクなステージを実装するためにはを扱えるようにする必要がある
     - カメラ(ビューポート)の概念を導入
     - ワールド座標系と画面座標系の変換
   - week07では，まずPhysics 抽出を優先し，その後の週でスクロール／カメラ実装に手を付ける

## week07

### week07の設計

年末年始なので進捗はわずか．実装した内容はPhysicsの導入による，物理的な挙動をPlayerクラスからの切り分けである．Physicsという名前の通り，今後はEnemyクラスなどその他多くのオブジェクトがこのモジュールを介して物理的な処理を行う．

Playerクラスのupdate()は入力に対してx，yを増減させ，その後にブロックへ乗るという物理的な処理へ移っていた．今回移管したのはブロック(床)へ乗るという処理であり，Playerクラスのprivateなメンバ変数へアクセスする必要があった．そのため，DTO(データ転送オブジェクト)**であるVerticalCollisionStateを用意した．これらデータを介してprivateなメンバ変数へ必要最小限のアクセスを行う．

### Physics

- VerticalCollisionState構造体
  - 役割
    - ブロックへ乗る処理をするために，privateなメンバ変数とデータをやり取りするDTO
    - x, yの増減に対して，ブロックへ乗る処理を実施する．xは直接参照するが，yについてはprev/newFeetでフレーム前後の位置関係の把握を行う
  - 理由
    - x, yなどのメンバ変数はCharacterを継承したクラスではprivateであり，その他のオブジェクトにおいても原則はprivateとする

```cpp
struct VerticalCollisionState{
    // 入力
    double prevFeet;    // 1フレーム前のy座標
    double newFeet;     // 入力受け取り後のy座標
    double x;           // x座標
    double width;       // spriteのwidth
    double vv;          // 落下速度
    bool   onGround;    // 接地しているか
};
```

- Physics：resolveVerticalBlockCollision
  - 役割
    - VerticalCollisionState構造体を介してオブジェクトとブロックの接触判定を行う
    - ブロックを格納しているコンテナを分解してチェックする
  - 理由
    - Playerのみに実装していたブロック(床)へ乗る処理をEnemyなど他のオブジェクトへ適用する場合，ブロックへ乗るオブジェクトそれぞれに同様のコードを記述しなければならない
    - Physicsモジュールへ切り出すことで，VerticalCollisionStateという構造体を介してブロックへ乗る処理を実装できる

### week08の予定

ゲームとしてはクリアが無いなど未完成すぎる部分が目立っている．ゲームエンジンとしてはある程度設計できているが，まだゲームとしては成り立っていない．

考え方を変えてゲームとしての形を修正する．具体的には，

1. クリア条件の追加
   - フラグやゴールオブジェクトを配置して，クリアできるようにする
2. 1面をクリアできるゲームとして，不足している部分を整える
   - タイトル/プレイ/クリア/ゲームオーバの画面遷移の実装
   - 次のステージへ進む，というプレイヤー向けの導線の実装
3. ステージ1の作り込み
   - ブロック/敵を遊べるように配置する
   - ジャンプ/ダメージ床/落下など現状あるギミック利用する

まずはマリオライクなゲームとして，1-1面の完成を目指して2Dゲームを作成する方針で進めていく．

## week08

### Google Testの導入

week08まで，テストは全てゲームを起動して動作確認を兼ねたテストを実施していた．機能が増加してきているため，基礎的な処理の単体テストをGoogle Testを用いて実施した．

Google test(Google C++ Testing Framework)は名の通り，Googleが開発したC++向けの単体テストフレームワークである．候補としては導入が簡単なCatch2や軽量なDoctestなどがあった．本開発ではCMakeを使用しているため相性がよく，大規模な開発で使われている実績や情報量が多く，FetchContentやgtestによるCMake/CTestとの統合が簡単なのでGoogle testを用いた．

また，Google testはテストの独立性・再現性が重視されており環境への依存が少なく，CMakeと合わせて導入が容易である点も評価した．

Google Testの導入方法：

1. testsディレクトリの作成(もしなければ)
2. テスト用のCMakeLists.txtをtestsディレクトリへ配置
3. テスト用cppファイルを作成
4. ビルド/実行

   ```Bash
   ~$> cmake .. # build用ファイル作成
   # 現在のディレクトリをビルド対象に
   # -j$(nproc)：利用可能なコア数を使って並列でビルド
   ~$> cmake --build . -j$(nproc) 
   # ctestでテストランナーを実行して，enable_testing()でテスト情報を読み取る
   # --output-on-failure：失敗のみを詳細に表示させる
   ~$> ctest --output-on-failure
   ```

### google testの対象

次の3つの機能をテストした：

- Physics
  - 理由
    - ブロックへ乗ったときの処理を実施するため，境界値の処理が想定通りに動作しているかをテストする必要があった
  - テスト内容
    - ブロックが存在しないパターン
    - Standableに対する着地
    - x軸の重なり
    - DropThroughの条件による振る舞い
- BlockLevelLoader
  - 理由
    - level1.txtなどのテキストファイルを読み込む処理であり，ゲームの初期化に使われている
    - 外部のファイルとやり取りし，想定した形式のデータをパースする処理があるためテストが必要である
  - テスト内容
    - 正常な行のデータのパース処理
    - 壊れた行のデータへの処理
- GameUtil
  - 理由
    - ゲームで使用する処理をまとめており，汎用的な関数がそろっている
    - 今後も頻繁に呼ばれる処理であるためテストで処理を確認する必要がある
  - テストケース
    - trim()：前後スペースの除去，全スペースな行への処理
    - intersects()：重なり，辺/端の接触

### week08の設計

これまでゲームのオブジェクトはworld座標系(x, y)で管理され，動ける範囲はウィンドウの範囲であった．そのためステージもウィンドウの内部に固定されており，非常に狭いステージでゲームをプレイすることになっていた．

week08ではカメラが導入されたことで，いわゆるworld座標という，ゲームのオブジェクトが持つ絶対的な位置(x, y)に概念が追加された．画面上の座標がカメラの(x, y)分ずれることで，画面が動きスクロールしているように見えるカメラを考慮した描画処理が追加された．

Rendererなど描画処理はこれからカメラ座標を前提にした処理になった．これによってプレイヤーが画面上で動く=世界座標，カメラがプレイヤーを追う/描画にカメラが関与する=カメラ座標というようにオブジェクトと描画が分離された．

### カメラの導入

- Camera構造体
  - 役割
    - これまでの画面上の絶対的な位置(世界座標)とは異なる，新たな座標系の役割を持つ
  - 理由
    - 描画処理はカメラ座標を参照することで，世界座標と相対的な関係性になり，いわゆるスクロール的な処理が可能になる
    - Cameraの座標はRendererクラスで考慮するため，Characterなど画面上のオブジェクトはカメラを極力意識しないようになる
- Game::Game()
  - worldInfo
    - 世界(ステージ)の広さ(幅と高さ)を持っている
    - week08時点ではblockの配置に応じて拡張される(最小限ウィンドウのサイズには合う)
  - GameContextにcamera/worldInfoを導入し，シーンへ受け渡すように修正
- PlayingScene
  - update()
    - DrawBounds(Characterの移動範囲)にworldInfoを導入し，拡張された範囲に対応した
    - 既存の処理では，見えない壁に阻まれてしまいWINDOW_WIDTH以上先へ進めなくなる
    - cameraのupdate処理を呼び出すように修正
  - render()
    - 描画処理でRendererへ渡すdraw()メソッドに，cameraを渡すように修正した
  - updateCamera()
    - playerをカメラの中心に置いて，カメラが追従していく処理を実装している関数
    - playerの中心をカメラの中心へ置くが，画面の端(worldInfo)においては画面の端を優先し，ステージの端を超えないようにクランプする
    - クランプ処理によって，プレイヤーは世界の端っこで止まり，背景も止まる
- Character
  - draw()
    - cameraをシグネチャとして持ち，rendererのdraw関数でcamera座標系を考慮した版を呼び出すように修正
  - getEntityCenter_X()/getEntityCenter_Y()
    - Characterの中心位置を返す関数(カメラの追従処理で主に使用)
- Renderer
  - draw()
    - cameraを引数に持ち，カメラ座標系を考慮した描画処理がなされる用にしたオーバーロード版
  - drawRect()
    - cameraを引数に持ち，カメラ座標系を考慮した描画処理がなされる用にしたオーバーロード版

#### level1.txtについて

上述の通り，worldInfo(ステージの広さの情報)はブロックの配置場所を考慮して拡張される：

```cpp
// 世界の広さ
worldInfo = {static_cast<double>(GameConfig::WINDOW_WIDTH), 
              static_cast<double>(GameConfig::WINDOW_HEIGHT)
            };
// ブロックが置かれている範囲に応じて拡張
for(const auto& b : blocks){
    worldInfo.WorldWidth = std::max(worldInfo.WorldWidth, b.x + b.w);
    worldInfo.WorldHeight = std::max(worldInfo.WorldHeight, b.y + b.h);
}
```

そのため，level1.txtではWINDOW_WIDTHより大きい座標にブロックが配置されるように修正し，スクロールされるかを確認した．
結果としては，画面の拡張および落下死など各種処理も問題なく実行されていた：

```txt
S 0   750 1200 50 # 床
S 200 700 100 20 # 空中の足場
S 250 550 100 20 # 空中の足場
S 300 400 100 20 # 空中の足場
S 400 250 100 20 # 空中の足場
S 600 100 100 20 # 空中の足場
T 1400 700 100 20 # 空中の足場
T 200  80 100 20 # 空中の足場(すり抜け)
D 400 380 100 20 # 空中のダメージ床
C 650 80 20 20   # クリアオブジェクト
```

### week09での予定

敵が触れると即死のオブジェクトになっているので，対抗手段として踏みつけを用意する．

## week09

### 概要

week09では，次の2点を中心に実装・整理を行った．

- プレイヤーによる敵の踏みつけ処理の実装
- 踏みつけ後の敵の死亡演出(その場で点滅して消える)の実装
- 踏みつけ処理実装に伴った当たり判定まわりの整理とテストコードの追加

week08にて，床ブロックとの接地処理やカメラ・スクロール処理は整備されていたが，敵との接触は「当たったら即ゲームオーバー」という単純な仕様だった．そのためweek09ではここをゲームらしい処理に拡張している．

### 踏みつけ処理について

プレイヤーが敵を「踏みつけた」とみなす条件は次の通り：

1. 落下中であること

   - `player.vv > 0`(垂直速度が下向き)のときのみ踏みつけ判定を行う

2. プレイヤーの足が敵の頭頂をまたいでいること

   - 前フレームの足元が敵の頭頂より上
   - 今フレームの足元が敵の頭頂より下
   - という条件を，prevFeetとnewFeetとenemyTopを使ってチェックする

3. 敵の矩形と実際に衝突していること

   - SDL_RectベースのAABB当たり判定で，プレイヤー矩形と敵矩形が交差していること

判定結果はPlayerEnemyCollisionResultで表現される．

- None
  - そもそも矩形同士が接触していない，または踏みつけ条件を満たしていない(かつダメージにもならない)場合
- StompEnemy  
  - 上記条件をすべて満たし，「上から踏みつけた」と判断できる場合
- PlayerHit  
  - 矩形としては接触しているが踏みつけ条件を満たしていない場合  
    (側面や下からぶつかった場合など)

この結果に応じて，PlayingScene側で以下の処理を行う．

- None
  - 何もしない
- StompEnemy  
  - 敵に死亡演出を開始させる
  - プレイヤーの垂直速度を反転させ，少しだけバウンドさせる
  - スコアを加算する
- PlayerHit  
  - 現状は即ゲームオーバー

複数の敵が重なっている状況でも，条件を満たす限り連続で踏みつけられる設計になっている．

### 当たり判定まわりで発生した問題

#### 問題の背景

踏みつけ処理は当初は接触判定→さらに細かく踏みつけているかをワールド座標で判断，という想定で進んでいた．しかしながら，敵を踏みつけようとすると，ゲームオーバになってしまうという状態になってしまった．

これはいままで曖昧な状態にしていた，SDL_Rectによる矩形の当たり判定と，spriteによる描画範囲による当たり判定(若干小さい)の混在が招いた問題である．そのため，これら当たり判定を整理して当たり判定を構築しなければならない．

当初の実装では，次の2種類の「足元」の定義が混在していた．

- spriteの描画範囲(y + sprite.getDrawHeight())を使った足元
- getCollisionRect()で少し小さくした矩形(当たり判定用)のbottom

処理の流れはおおよそ次のようになっていた：

1. Rectでの接触判定が最初に実施される
2. 1の後に足元(前後のフレーム)の判定になる
3. feet系の接触判定はplayer.y + sprite.getDrawHeight()なので，Rectより若干小さい
4. 1のRect接触時点では，feet系の接触判定に引っかからない
5. 踏みつけ判定の条件式を満たさないので，その下のPlayerHitがreturnされる

このRect&AABBの当たり判定→踏みつけ判定の問題について，2つの解決策が考えられる：

- PlayerHitのreturnにx軸と下方向の判定を追加し，安易にPlayerHitを返さないようにする
- 踏みつけ判定に若干の許容範囲(tolerance)をもたせて，踏みつけ判定の条件式を緩める

この2つの案のうち，PlayerHitの接触を厳格に判定する方法は「上からの踏みつけ以外」を判定するより難しい．そのため，SDL_Rect&AABBによる接触判定後に，余裕(tolerance)を持って判定させる方法を考えた．

#### toleranceを導入することによる問題

toleranceを大きく取って判定を実施すると，今後厳格な判定を実施しようとした際に，仕様を大きく変更しなければならず将来の負債になる．

一時的な発想として，「踏みつけ判定にtolerance(許容範囲)を持たせて条件を緩めるという案を検討した．しかしこの方針は以下の理由から採用しないことにした．

- その場しのぎの調整値が増え，将来的に仕様を厳密にしたいときの負債になる
- 他の接触判定にも類似のtoleranceを入れ始めると，全体として挙動の理由が分かりにくくなる

つまり様々な接触判定にtoleranceが登場する可能性があるためこの方法はNGである．

### 解決方針：feet を用途ごとに分離する

最終的には，「座標系の混在をやめる」ことをゴールに次のように整理した．

1. Characterクラスにfeetを2系列持たせる

   - 物理処理(ブロックとの接地など)用  
     - prevFeetPhysics/getFeetPhysics()
     - y + sprite.getDrawHeight()ベースで「描画矩形のbottom」を扱う
   - 衝突処理(Player/Enemy)用  
     - prevFeetCollision/getFeetCollision()
     - getCollisionRect()ベースで「当たり判定用に縮めたRect bottom」を扱う

2. 物理処理と衝突処理で，使う feet を明確に分ける

   - Physics::resolveVerticalBlockCollision
     →prevFeetPhysics/getFeetPhysics()を使用
   - Player?Enemy の踏みつけ判定
     → prevFeetCollision/getFeetCollision()を使用

3. 踏みつけ判定ロジックをユーティリティに切り出す

   - PlayerEnemyCollisionUtilとしてresolvePlayerEnemyCollision(...)をPlayingSceneから分離
   - Sceneやrendererに依存しない「純粋な」関数にすることで，テストを容易にする

### week09の設計

#### Characterクラス

役割：物理計算・衝突判定で必要となる「前フレームの足元位置」をキャッシュする

主なメンバ

```cpp
double prevFeetPhysics;   // Physics 用：sprite 高さベース
double prevFeetCollision; // 衝突処理用：collisionRect ベース

// 主なメソッド

// Physics 用の feet のサンプリング
// update() の冒頭で呼び出し，物理計算に入る前の足元位置をキャッシュする
void beginFrameFeetPhysicsSample();
double getFeetPhysics() const;
double getPrevFeetPhysics() const;

// 衝突判定用の feet のサンプリング
// PlayingScene::update() の冒頭で呼び出し，シーン全体の更新前の足元位置をキャッシュする
void beginFrameCollisionSample();
double getFeetCollision() const;
double getPrevFeetCollision() const;
```

#### Playerクラス

Player::update()

Characterクラスで実装したprevFeet系を取得するサンプリングを実施する．流れとしては次の通り：

1. beginFrameFeetPhysicsSample()でprevFeetPhysicsをサンプリング
2. 垂直速度やy軸の更新
3. VerticalCollisionStateの作成
4. resolveVerticalBlockCollision()を呼び出す

#### Enemyクラス

EnemyState

新しく導入した，Enemyの状態を管理するenum class．次の3つの状態がある：

- Alive：生存状態．この状態は動ける
- Dying：死にかけの状態．踏まれたら(攻撃されたら)この状態になる
- Dead：死亡状態．描画などの処理の対象外になる

dyingTime

Dying状態は名の通り死にかけの状態なので，時間経過でDead状態へ移行する．そのためのDying状態管理用の時間．

- Enemy::update()
  - Alive
    - 従来のように左右への移動/worldBoundsで反射する
    - アニメーションを更新する
  - Dying
    - 位置はそのまま固定
    - dyingTimeを加算する
    - 指定時間(EnemyConfig::DYING_DURATION)経過するとDeadへ遷移する
  - Dead
    - 更新処理を行わない，スキップされる
    - PlayingScene::updateEntities()で削除される対象
- Enemy::draw()
  - Dead：描画しない
  - Dying：EnemyConfig::DYING_BLINK_INTERVALを用いて点滅する(点滅は描画のスキップで表現)
  - Alive/Dyingの可視フレーム：これまで通りCharacter::draw()を呼ぶ
- Enemy::startDying()
  - 外部から呼ばれ，Alive→Dyingへの状態遷移をする
  - Dying/Dead時は何もしないことで，多重踏みつけの防止

#### PlayingSceneクラス

- update(double delta)
   1. Pause 入力によるタイトルシーンへの遷移チェック
   2. player.beginFrameCollisionSample()で衝突用のfeet をサンプリング
   3. updateScore(delta)でスコア更新
   4. updateEntities(delta, worldBounds) で Player/Enemy を更新
   5. detectCollision()でブロックと敵の衝突処理
   6. hasFallenToGameOver()で落下死判定
   7. updateCamera()でカメラ追従
- updateEntities(...)
  - Player の更新(物理処理・入力処理)
  - 各Enemyの更新(状態に応じた動き／演出)
  - EnemyState::Deadの敵をremove_if + eraseで削除 → Dying 演出が終わった敵だけを自然に消す
- detectCollision()
  - 敵との衝突：resolveEnemyCollision()
  - ダメージ床・クリアオブジェクトとの衝突：resolveBlockCollision()
- resolveEnemyCollision()
  - PlayerのgetCollisionRect() を取得
  - prevFeetCollision/feetCollision/verticalVelocityを読み出し
  - EnemyState::Alive の敵のみ対象にループ
  - prevFeetCollisionの事前のサンプリングに強く依存しているので，サンプリングなしで呼び出すとうまく動作しないことに注意が必要
- PlayerEnemyCollision::resolvePlayerEnemyCollision(...)
  - resolveEnemyCollision()内で呼ばれ結果を判定
    - StompEnemy
      - 対象のEnemyへstartDying()呼ぶ
      - Playerをリバウンドさせる
      - スコアの加算
    - PlayerHit
      - ゲームオーバ

#### PlayerEnemyCollisionUtil

踏みつけ判定のロジックを純粋な関数として切り出した．

内部ロジック概要：

1. SDL_RectベースのAABBで矩形同士が交差しているかチェック → していなければNone
2. プレイヤーが落下中かどうかをplayerVv > 0.0で判定
3. playerPrevFeet, playerNewFeet, enemyTopを使って，足元が敵の頭頂をまたいだかどうかをチェック
   - playerPrevFeet <= enemyTop + eps
   - enemyTop <= playerNewFeet + eps
   - 境界の丸め誤差を抑えるためのepsは共通の定数として定義しており，1.0程度の許容範囲である
4. 以上を満たしていればStompEnemy，そうでなければPlayerHit

この関数はシーンやレンダラに依存しない純粋関数になっているため，Google Testでの単体テストが容易になっている．

#### PlayerEnemyCollisionUtilのテスト

踏みつけ処理は境界値のチェックが重要なので，次のテストケースを実施した：

- StompEnemy_FromAbove
  - プレイヤーが上から落下してきて，足元が敵の頭頂をまたいでいるケース → StompEnemy になることを確認
- PlayerHit_FromSide
  - 落下中だが，敵の側面から接触するケース → PlayerHit になることを確認
- None_WhenNotIntersecting
  - feet の条件を満たしていても，矩形同士がそもそも交差していないケース → None になることを確認
- StompEnemy_AtEdgeOfFailure
  - ギリギリ踏めていないケース(enemyTop をまたいでいない)
- StompEnemy_AtEdgeOfSuccess
  - ギリギリ踏めているケース(enemyTop をわずかにまたぐ)
- None_WhenNotFalling
  - playerVv <= 0 の場合は踏みつけにならないことを確認
- None_StompConditionWithoutIntersection
  - 「踏みつけ条件は満たしているが，Rect としては交差していない」ケース
  - resolvePlayerEnemyCollision 内の「まず矩形交差をチェックする」という前提が守られているかを検証
- PlayerHit_OnTheGround
  - 落下中ではない状態でintersectsした場合

### week10の予定

week09 でプレイヤーと敵の基本的なインタラクション(踏みつけ → 死亡演出)が実装できたので，今後は次のような方向性を検討している(優先順位は今後調整)

- ステージ遷移(ステージ 1 → ステージ 2 ...)とそれに伴うレベルデータ管理
- 敵のバリエーション追加(移動パターン，飛び跳ねる敵など)
- パーティクルエフェクト(踏みつけ時のエフェクト，ダメージ床の視覚効果など)
- デバッグ用途の軽量な可視化(ヒットボックス表示など)

## week10

### ステージの遷移

現状はステージ1しかない．また，クリア後はタイトル画面へ戻るしか無い．ステージクリアをすると，次のステージへ遷移していくような処理を実装することで，ゲームとしての骨格を整える．

具体的には，

- タイトル画面→スタート押下→ステージ1へ
- クリア条件を満たす→クリア画面→次のステージへ
  - 最終ステージをクリア→ゲームクリアの表示およびスコア・ステージ数を表示→タイトル画面へ
- ゲームオーバーの条件を満たす→ゲームオーバー画面
  - リトライを押下→直前のステージのはじめから
  - タイトルを押下→スコア・ステージ数の表示画面→タイトル画面へ

### ステージの定義とロードの整理

#### StageConfig

StageConfig.hppを作成し，ステージの情報を一箇所に集約した．これによって，ステージごとの違いはGame/Sceneから切り離され，StageDefinitionを参照する方針になった：

- StageDefinition
  - name：ステージ名
  - levelFile：ブロック配置のデータが記述されているレベルファイルパス
  - playerStart_X/playerStart_Y：プレイヤーの各ステージの初期座標
  - enemySpawns：ステージごとの敵の出現情報
    - x/y：敵の初期座標
    - speed：敵の移動速度

#### SceneControl::loadStage

ステージの遷移が必要になったので，今までのようにchangeSceneでシーンを切り替えるだけでは不足することになった．loadStage()によって，シーン(PlayingScene)へ入るとステージのインデックスにあった情報を読み込む処理が追加された．

要素としては次の通り：

- StageConfig::STAGES\[stageIndex\] からStageDefinitionを取得
- ctx.entityCtx.blocks/ctx.entityCtx.enemies を一度クリア
- BlockLevelLoader::loadFromFile(def.levelFile) でブロックをロード
  - 失敗時はフォールバックとして画面下端に単純な床ブロックを生成
- ctx.worldInfoの再計算
  - ブロック配置に応じて WorldWidth / WorldHeight を更新して，ステージ(世界)を拡張
- プレイヤー初期位置の設定
  - player.reset() → setPosition(def.playerStart_X, def.playerStart_Y)
- 敵の生成
  - StageDefinition.enemySpawnsを元にEnemyをstd::unique_ptrで生成
  - Enemy::applyEnemyParamForSpawn(x, y, speed)で各個体のパラメータを適用

ステージの読み込みはchangeScene()の仕様に合わせ，onEnter()で呼ぶ(つまり呼ばれたシーンの初期で読み込む)ようにした．

### シーン遷移のフロー

#### シーンごとの遷移のルール明確化

- TitleScene
  - ENTERキー押下：startNewGame()を実行してスコアなどを初期化しPlayingSceneへ
- PlayingScene
  - ESC(Pause)押下：TitleSceneへ
  - ダメージを受ける(Enemy/Damege床へ接触)：GameOverへ
  - クリアブロックに接触：ClearSceneへ
- ClearScene
  - ENTERキー押下：
    - goTonextStage()がtrue：次のステージへ
    - goTonextStage()がfalse：全ステージクリア
      - 全ステージクリア時は，ResultScene的なシーンへ遷移予定
  - ESC(Pause)押下：TitleSceneへ
- GameOverScene：
  - ENTERキー押下：同じステージを最初からやり直す→PlayingScene
  - ESC(Pause)押下：TitleScene
    - TitleSceneへの遷移=諦めるなので，ResultScen的なシーンへ遷移予定

ステージの遷移において，データのロードはすべてloadStage()が実行する．

#### SceneControlとGameの関係

SceneControl側はステージのインデックス，スコアの管理を実行する．シーンの遷移はGameSceneを介してGameへ伝搬し，Game::run()でcurrentSceneの差し替えが行われる．

現段階では，GameコンストラクタのbuildSceneでオブジェクト化され，遷移のルールはSceneが管理する．この出入り口となるonEnter/onExitをGameが呼び出すことで，シーンの遷移が行われる．

### Gameクラスの見直し

#### Gameクラスの初期化

SDL/SDL_image/SDL_ttfの初期化・終了処理をSdlSystemに管理させるようにした．これによって，RAIIに準拠するようになり，SdlSystemのコンストラクタ/デストラクタで初期化/終了処理が実施されるようになった．
これによって，Gameオブジェクトのライフタイムに追従したSDLサブシステムの解放が行われる．
これまではmain関数内部で波括弧によるスコープを明示することで，オブジェクトの解放→SDLの終了処理が順に行われていた．今後はGameオブジェクトのライフタイムに追従するので，main関数はシンプルなGame生成とrun()の実行のみになった．

#### Gameコンストラクタの分割

これまで，コンストラクタ内で一連の流れでオブジェクトの生成やリソースの確保を行っていたが，それぞれメソッド化して分割し，リソースの確保を明記して影響範囲を確定させた．

コンストラクタ内で次の順番で処理される：

1. sdl = std::make_unique<SdlSystem>()
   - SDLの初期化処理で，必ず最初に呼ぶ
2. bootstrapWindowAndRenderer()
   - Window / Renderer の生成
3. loadResources()
   - Texture/Text/TextTextureを生成
4. buildWorld()
   - プレイヤーインスタンスの生成と初期位置設定
   - WorldInfo の初期値設定
   - 敵生成用の乱数ディストリビューション (distX, distY, distSpeed) の設定
   - カメラの初期化
5. buildContexts()
   - GameContext の構築(Renderer / Input / Camera / WorldInfo / EntityContext / TextRenderContext / RandomContext)
6. buildScenes()
   - Title/Playing/GameOver/Clearの各Sceneインスタンス生成
7. startFromTitle()
   - 初期シーンを Title に設定し，onEnter()を呼ぶ

#### 固定デルタタイム+accumulator方式へ変更

ゲームループでのdeltaの計算を見直し，可変deltaとSDL_Delayのキャップ処理から，fpsごとの固定delta(1/60)を用いてupdateを回すことで，遅延時の安定性を向上させた．

accumulatorは以前のタイプスタンプからの累積値であり，deltaタイムが溜まっていく．その累積値から遅延処理時の「遅延を取り戻そうとするupdate」を複数回実行する．ただし，過剰なupdateでプレイヤーが関与できない更新が延々と実行される可能性を加味して，safetyを設け最大5回しか更新されないように制限をかけている．

主な変更点は次の通り：

- 時間管理にstd::chronoを用いる
- 固定deltaは1.0/60.0秒(≒16.666ms)
- ループ内では
  - processEvents()でSDLイベントを処理
  - now - prevをaccへ加算
  - acc >= fixedNsの間update．ただし最大5回まで
  - render()で画面描画更新処理
- capFrameRateで60fpsへキャップする

これによって，一時的な遅延でもdeltaが大きくならない，固定ステップで物理処理などが進むようになった．

なお，現在の仕様として，遅延が生じてupdate()が複数回走ったとしても，processEvents()は1回しか走らない．そのため，プレイヤーのキー入力の処理も1回のみ働く仕様となっていることに注意．

### week11の予定

- SceneControlの整理が終わっていないので整理する
- ResultSceneを導入してプレイヤーの体験がTitle〜Result〜Titleまで違和感なく回るようにする
- ゲームの要素の追加
  - levelファイルの再設計
  - ブロックの描画修正
  - 敵の挙動追加
  - プレイヤーの挙動(コヨーテジャンプなど)
- 演出の追加
  - 一部アニメーションの追加

## week11

### シーン遷移の修正

シーンがそれぞれchangeScene()を呼び出しており，今後ステージの遷移が複雑化すると，同一のフレーム内で複数のシーン遷移が実施される可能性があった．そこでSceneControlを経由してシーンを切り替える設計をより堅牢にした．

具体的には，各Sceneでは遷移要求(requestScene)を実施するのみとした．つまりシーンは呼ばれた場所で遷移しない．実際の遷移はGame::run()内のメインループ内で最後に要求されたシーンの要求を承認するフローに変更した．これによって，複数回のシーン遷移要求に対しても，最後の1回のみが有効となるようになったので注意が必要である．

また，簡単ながらリザルト画面を用意してゲームの全ステージをクリアしたら結果を表示するようにして，ゲームの体裁を最低限整えた．

### 固定フレームタイム・ゲームループの見直し

今までSDL_Delay()による過剰スリープの問題が残ったままであった．実際に私の使用するPCでは大きな差は感じなかったが，デルタタイムの制御を見直した．

60fps相当の固定デルタを算出し，1回のループでデルタタイムが固定化された．これによって，高性能なPCや一時的な遅延が生じてもゲームが壊れない最低限の整理ができた．また，update()も累積時間(accumlate)を導入し，最大5ステップ以上ゲームを更新しないことで，安全なゲームの実行及び安定した更新を実現した．

### プレイヤーのジャンプ処理の見直し

#### コヨーテタイム

プレイヤーの操作性に関係する挙動でコヨーテタイムというものがある．これは簡単に言うとプレイヤーが落下する瞬間にジャンプ可能な時間を設けることである．
これによって，プレイヤーはギリギリジャンプできた，というゲーム体験が得られる．判定を厳しくすると両足が端から飛び出た瞬間＝落下であるが，その瞬間にジャンプできることで，ギリギリジャンプできたという挙動になる．

実装としてはシンプルにコヨーテタイムをジャンプの条件に導入し，接地しているか，コヨーテタイムが残っているかでジャンプできるようにした．コヨーテタイムは接地状態ではなくなるとすぐに減算されていく．

#### 可変ジャンプ

これまでのジャンプは固定の高さで飛ぶようになっており，ボタン押下のタイミングにかかわらず，一瞬押しても長時間押してもジャンプの飛距離は変わらなかった．
ボタン押下の時間に応じてジャンプの高さを調節できるようにしたことで，自由度が上がった．

実装としては，重力を変化させる方向性にした．ジャンプボタンを押している間，重力が弱くなることでプレイヤーキャラクタは大きくジャンプできる．当然ながら，ジャンプボタンの押下時間には制限を設けている．制限に達するか，ジャンプボタンを話すと通常の重力がかかり落下する．

#### ジャンプのバッファ

着地直前の数フレームでもジャンプ入力を受け取り設置後即座にジャンプできるようにすることで，挙動がキビキビするようになった．

こちらの実装もシンプルで，ジャンプボタン押下時にバッファ用の時間を設け，その時間が残っている間にジャンプの条件(接地 or コヨーテタイムが0より大きい)を満たすと，ジャンプできる．

コヨーテタイムと同様に，プレイヤーの若干の先読みを踏まえた入力を受け入れるような挙動になるようにした．

### プレイヤーの水平方向の移動

#### 水平移動への加速度導入

ジャンプ同様にプレイヤーの水平方向の移動も常に一定の速度であった．常に最高速度で走るので，ピタリと止まる挙動になっていた．そこで加速度と摩擦を水平方向の移動に導入し，入力した瞬間は少しずつ加速して，やがて最高速度に達する．逆に入力を離すと減速して止まる滑るような挙動を実装した．

最高速度はPlayerConfig::RUN_MAX_SPEEDでクランプする．

#### ブロックとの衝突

水平方向の移動を整備する過程で，縦方向とは別に横方向からの衝突判定も実装した．縦方向と考え方は一緒で，現状はプレイヤーがブロックに横からぶつかったときにキャラクタの位置補正および水平方向の移動速度(hv)をゼロにする処理を実装した．

resolveVertical/HorizontalBlockCollision()はなるべく純粋な計算で成り立つようにしており，SDL_RectやGameUtilに依存しない物理処理にしている．

### week12について

- スプライトベースの当たり判定の調整
  - コリジョン矩形と描画矩形の整理
- DropThroughが実装されているが，「下から上」方向の処理が不定
  - ブロックに下から入ったとき(dropThrough)や下からぶつかるブロックを定めて，上昇時の挙動を定義する
- 複数ブロックが登場するステージで当たり判定を調整する
- プレイヤーの攻撃手段の追加(ファイアボール)
- 敵のバリエーションの追加(AI導入など)

## week12

### week12の概要

実装の方針としては2つに絞った．
1つはブロックの下から上への判定である．上から下へすり抜ける実装があるので，下方向からはすり抜けられないブロックを作成することでステージ構成に幅をもたせる．
もう1つはファイアボールで，プレイヤーに踏みつけ以外の攻撃手段を追加する．

### FireBallのスプライトシート作成

簡単なファイアボールアニメーションができるスプライトシートを作成した．大きさは縦横比で64*(64*4)という4枚構成で，大きかったので32pxという半分のサイズに縮小した．

使用したツールはGimpである．ベースとなる赤と白のコアと，外側にフレアのようなオレンジ色の光で炎を表現している．

詳しくは「assets/image/fireball.png」を参照．

### week12の実装内容

- 垂直方向の当たり判定の拡張
  - 既存の下方向への判定処理を拡張し，1フレーム前の頭頂(prevTop)/足元(prevFeet)と，移動後の頭頂 (newTop)/足元(newFeet)をサンプリングする仕組みを導入
    - VerticalCollisionStateを拡張し，床・天井との衝突判定に必要な情報をまとめて Physicsモジュールへ渡すように変更した
  - 1回の接触判定中に，床上床下両方の判定が必要になることは無いと仮定し，垂直速度vvの符号に応じて処理を分割：
    - vv > 0 のときは床側との接触(resolveBlockCollisionFromTop)だけを実行
    - vv < 0 のときは天井との接触処理(resolveBlockCollisionFromBottom)だけを実行
  - これにより，ジャンプ中の天井ヒットと落下中の床着地を安定して検出できるようになった

- Player物理処理の整理
  - Playerのupdate内で，「移動後の座標」からVerticalCollisionStateを組み立ててPhysicsに渡す形にリファクタリング
  - 衝突解決後のy/vv/onGroundをPlayer側に正しく反映するようにし，  頭をぶつけた直後の isJumping / jumpElapsed が破綻しないように状態リセット処理を実装した
  - 当たり判定矩形とスプライトの描画矩形を切り離すために，PlayerConfig::COLLISION_MARGIN_X/Yを導入してハードコードを抑えた
    - getCollisionRect()で余白を差し引いたヒットボックスを返すように明示して，当たり判定を微調整

- 飛び道具システム(Bullet/FireBall)の導入
  - 共通の弾クラスとしてBulletを追加し，ゲームに必要な要素を持つインターフェイスとした
    - 座標(x, y)，速度(hv, vv)，向き(Direction)，アニメーション (AnimationController)，スプライト (Sprite) を持つ
  - FireBall専用の設定をFireBallConfigに切り出し：
    - 初速 (SPEED_X)
    - 重力 (GRAVITY)
    - 反発係数 (BOUNCE)
    - スプライトシートの1コマサイズ/コマ数/アニメーション間隔などを定義．
  - Action::Fire(Bボタン)入力に応じて，Playerのキャラクタ中心からFireBallを生成する処理をPlayingSceneに実装した
  - FireBallに対して重力＋床バウンドの挙動を実装し，片付ける処理も実装している：
    - 一定以下の速度になった
    - 規定回数以上バウンドした
    - 画面(ワールド)外へ出た
  - 上記のいずれかの条件を満たしたタイミングで非活性化してcleanupFireBalls()で削除するライフサイクルで設計した
  - FireBallとEnemyの当たり判定(resolveFireBallEnemyCollision)を追加し，命中時に以下の処理を実施する：
    - Enemy を Dying 状態へ遷移
    - FireBall を非活性化
    - 敵撃破スコアを加算

### week13の予定

ファイアボールの骨子は完成したとみなして，次は敵のバリエーションを増やす．具体的には敵に簡単なAIを状態を持たせ，3種類を実装予定である：

- これまでのように歩き続けるWalker
- プレイヤーを追いかけるChaser
- 予測しにくい動作をするJumper

## week13

### 目標

week13の目標はこれまでの左右へ動く単純な動作をする敵を，特定の行動を取る敵バリエーションを追加した．
方針として，PlayingSceneやPhysicsに敵の動作を混在させず，Enemyクラスを継承させたクラスに行動判断用関数think()を用意した．これによって，プレイ中のupdate()で情報を収集→敵の行動へ反映という判断ロジックを実装するようにした．

具体的には次の通り：

- Characterクラス←Enemyクラスというこれまでの基底クラスからの継承の関係を利用する
- PlayingScece内のupdate()処理のパイプラインにて，行動判断用の情報収集(センサー)を実装
- Enemyを継承して2つの敵を実装
  - WalkerEnemy：基本形．崖や壁で方向転換し，床を左右に動く敵
  - ChaserEnemy：WalkerEnemyの発展形で，プレイヤーの方向へ追跡する行動を取る
- また，敵のスポーンがStageConfig.hppに直書きされているため，Jsonファイルを用いて外部へ切り出すための準備をした

### Enemy基底クラス

全ての敵はEnemyクラスを継承する．また，EnemyクラスはCharacterクラスから派生しているため，Character←Enemy←WalkerEnemyという継承関係になっている．

いわゆるどの敵でも共通して実装する処理を持っている．

Enemyクラスの主な役割：

- シンプルな状態マシンを保持：Alive，Dying，Dead
  - Alive: 通常動作とアニメーション
  - Dying: 一定時間点滅効果
  - Dead: PlayingScene::updateEntities()でゲームから削除
- 共通の物理演算・アニメーションを提供：
  - virtual void think(double delta, const EnemySensor& es)
    - オーバライドする
    - 行動決定用関数(水平速度・方向等の設定)
  - virtual void stepPhysics(double delta, DrawBounds, const std::vector\<Block\>&)
    - 共有物理演算: 重力，ブロックに対する垂直衝突判定，ワールド境界での折返しなど
- virtual void updateAnimation(double delta)
  - スプライトのフレーム進行
- スポーンと死亡処理：
  - 位置と速度を初期化する applyEnemyParamForSpawn(x, y, speed)
  - Dying→Deadシーケンスへ遷移するstartDying()

重要な設計上の決定事項はとして，think()がSDLやシーン状態を直接操作しないことがある．
EnemySensorを使用して内部の移動フィールド(hv，dirなど)を更新するのみである．

### EnemySensor

敵の行動決定のためにGameContextを渡すのは重たすぎると考え，さらに必要な情報に絞ったセンサー構造体を作成した．

```cpp
struct EnemySensor {
    double distanceToPlayer; // ユークリッド距離
    double dxToPlayer;       // Player.x - Enemy.x
    double dyToPlayer;       // Player.y - Enemy.y
    bool playerOnLeft;       // プレイヤーは左側か？
    bool playerInSight;      // 簡易矩形FOV内にいるか
    bool groundAhead;        // 1マス先に地面があるか(ないか)
    bool wallAhead;          // 1マス先に壁/障害物があるか
    bool playerBelow;        // プレイヤーが真下に位置するか
};
```

これら構造体の各要素を満たすように，PlayingScene::gatherEnemySensor()を実装し，3つの処理を行う補助的な関数を呼び出すように処理を分割した：

- fillPlayerRelation()
  - Playerとの位置関係などを導出し，敵キャラクターの視界やPlayer-Enemyの距離感を計算する
- fillGroundAhead()
  - Enemyの若干先に矩形を置き，AABBによるブロックとの接触を判定して落下するかどうかを調べる
  - ブロックが無い=落下するので引き返す，という判断ができる
- fillWallAhead()
  - Enemyの若干先に矩形をおいて，AABBによる壁(ブロック)に接触するかを調べてる

### PlayingScene::update()

これまでと同様にメインゲームループは固定化されたパイプラインで，決まった順番で処理を実施していく．

1. プレイヤーの位置のサンプリング
2. ワールドやスコアなどの更新
3. EnemySensorのデータ収集
4. EnemySensorの結果を基にEnemyの行動を決定
5. 物理演算の更新
   - Player-Enemyの座標更新
   - アニメーションなどの更新
6. 衝突の解決
   - Player，敵, ブロック, 火の玉
7. 処理結果に対する状態の解決
    - 落下死，Dead状態のEnemy，規定回数バウンドしたFireBallの片付けなど

### 実装できたEnemy

- WalkerEnemy
  - 基本的な敵
  - 崖やブロックにぶつかりそうになると進行方向を反転する
  - groundAhead/wallAheadに依存している→EnemySensorで行動を決定する
- ChaserEnemy
  - Playerを追尾する敵(ただし地面のみ)
  - 現状はジャンプせずPlayerを追尾する
  - 追尾状態ではないときは，WalkerEnemyのように徘徊するパトロール状態になる

### Enemyのスポーンの調整

現状はStageConfig.hppに敵のスポーン情報を直書きしており，拡張が大変である．静的配列な制御構造は残しつつステージのデザインがやりやすいように，Jsonファイルから敵の情報を読み取ってStageConfigへ渡すような処理になるように変更予定．

準備として次の構造体とインターフェイスを実装している：

- EnemyType
  - 既存のenum class
  - 敵のタイプ(Walker/Chaserなど)を定義している
- EnemySpawn
  - 敵のスポーンに必要な情報を持つ
  - x/y座標，移動速度，タイプ
- StageDefinition
  - 既存の構造体
  - name，定義ファイルパス，Playerのスタート位置，敵のスポーン情報を持つ
- JsonEnemySpawn(仮)
  - Jsonファイルから受け取ったデータを保持する構造体
- JsonStageDefinition(仮)
  - Jsonファイルの情報を基にするStageDefinition

### week14の予定

- 追加のEnemyTypeの実装
  - Jumper：Playerが近づいたらジャンプする
  - Turret：弾を発射し続ける砲台
- 敵のスポーンをJsonファイルへ移行する

## week14について

### week14の概要

Jumper/Turret型の敵を実装した．JumperはこれまでのEnemySensorに関するセンサーによる情報の収集→敵の行動判断，というプロセスでかんたんに実装ができた．しかし一方で，TurretEnemyは一筋縄では行かなかった．

また，使用しているPCがアップデートによって内蔵NICのドライバが読み込まれなくなり，ネットワークへ接続できない期間があった．

### JumperEmeyについて

- JumperEnemy
  - 概要
    - Playerが近づくとジャンプする
    - ジャンプは一定の高さ
    - ジャンプの周期はある程度ランダムである
  - JumperEnemy::think()
    - 条件1：いわゆる視界を設定し，プレイヤーが一定距離まで近づいているか
    - 条件2：地面に接地しており，かつ，ジャンプのクールダウンが0か
  - ジャンプのランダム性
    - 乱数生成器で1〜2秒のランダムな時間でジャンプする
    - 次のジャンプはJumperEnemy::nextInterval()でクールダウンを設定する

### TurretEnemyについて

- TurretEnemy
  - 概要
    - Playerが一定距離まで近づくと敵弾を発射する
    - TurretEnemyはその場から動かない
    - Walkerなどと同様に，踏みつけやファイアボールで死ぬ
  - 問題点
    - 当初はFireBallのロジックを踏襲し，等速直線運動をする弾を発射する想定だった
    - ただFireBallはEnemyとの衝突を念頭に設計しているため，衝突検知のロジックでPlayer/Enemyのどちらから発射されたか，Player/Enemyのどちらにぶつかったかを切り分ける必要があった
      - 当たり判定のロジックが複雑化する
  - 実装方針
    - FireBallクラスを使用するのではなく，EnemyBulletクラスを新しく用意する
    - EnemyBulletは基本的にFireBallを踏襲しているが，updateロジックなどは単純化する
    - TurretEnemyはEnemyBulletを発射する
  - TurretEnemy::think()
    - 発射条件
      - 条件1：プレイヤーが視界内にいるか
      - 条件2：プレイヤーの距離が射程圏内か
      - 条件3：高さが一定の間に収まっているか→当てるために発射しているように見せるため
    - TurretEnemyは方向(Direction)を持っている

#### week15の予定

今回の実装に関する問題点として次のものがあります：

1. 現状FireBallとほとんど処理が同じ
   - 処理は共通化できる部分が多く，Projectileクラスにまとめて似た処理は統一する
2. EnemySensorの収集頻度が高すぎる
   - update処理を見直して，収集する，データや更新するデータの処理を見直す必要あり
3. PlayingSceneの肥大化
   - GameクラスのGod Class化を抑えるためにSceneに分割したとはいえ，PlayingSceneの責務がかなり大きくなっている
   - update処理の見直しも含めてリファクタリングが必要

## week15

### week15の概要

week15ではweek14で実装したTurretEnemy/EnemyBullet周りを起点として，PlayingSceneへ混在していた処理をSystemというクラスへ分離し，更新処理の責務を整理した．
具体的には，

- 弾管理のProjectileSystem
- 敵の行動判断用のEnemyAISystem
- 衝突判定をまとめるCollisionSystem

これらを導入し，さらにrequestScene()によるSceneの遷移やスコア加算などの「ゲーム全体へ影響する処理」を各シーンで直接呼び出す方式から，GameEvent/GameEventBufferによるイベント駆動へ移行する準備を進めた．

### TurretEnemy / EnemyBulletの整理

week14の課題だった「敵弾の扱い」を明確化するため，TurretEnemyは発射判断のみを担当し，弾生成はSystem側へ寄せた．

- TurretEnemy
  - 概要
    - プレイヤーが視界内かつ射程圏内で，敵弾発射要求を出す
    - 自身は移動しない砲台
  - 実装内容
    - fireRequested(発射要求フラグ)を導入し，「撃つべきタイミング」と「弾生成」を分離
    - 連射抑制のため，fireRequested == trueの間は追加で要求を出さない(=要求の二重発行防止)
    - 発射位置調整のため MUZZLE_OFFSET(X/Y)を導入し，弾の出現座標を調整した
- EnemyBullet
  - 概要
    - プレイヤー向けの弾としてFireBallとは別クラスで管理
    - FireBallとは異なり，等速直線運動，当たり判定はPlayerである
  - 目的
    - 「発射元(Player/Enemy)」と「命中対象(Enemy/Player)」の切り分けをFireBallの流用で無理に行わず，責務を分離してロジックの複雑化を避けた

### ProjectileSystemの実装

弾に関する処理(更新・生成・衝突・片付け)をPlayingSceneから切り出し，弾の管理責務を集約した．

- ProjectileSystemの主な役割
  - 弾の生成
    - Player入力によるFireBall生成
    - TurretEnemyの発射要求を消費してEnemyBulletを生成
  - 弾の更新
    - FireBall/EnemyBulletのupdate()をまとめて呼び出す
  - 弾の衝突判定
    - FireBall vs Enemy
    - EnemyBullet vs Player
  - 弾の寿命管理(cleanup)
    - 非活性化・画面外などの条件で削除する
    - cleanup処理はテンプレート化し，FireBall/EnemyBulletで共通化した

### EnemyAISystemの実装

Enemyの行動判断に必要な情報収集(EnemySensorの構築)と，Enemyへのthink()呼び出しをSystemとして分離した．
これにより，PlayingSceneが「敵AIの詳細」を直接持たずに更新順序だけを管理する構造に近づいた．

- EnemyAISystemの主な役割
  - EnemySensorの収集
    - Playerとの位置関係(距離，dx/dy，視界判定)
    - 崖判定(groundAhead)
    - 壁判定(wallAhead)
  - 収集したSensorをEnemyへ渡してthink()を実行
- 改善点(week15時点)
  - 定数はEnemyAIConfig.hppへ切り出し，マジックナンバーを除去した
  - BlockのAABB判定で毎回Block→Rect変換するコストが問題になったため，今後キャッシュを導入する前提を明確化した

### CollisionSystemの実装

Player-Enemy / Player-Block(Damage/Clearなど)の衝突処理をCollisionSystemへ切り出し，PlayingSceneから判定ロジックを分離した．

- CollisionSystemの主な役割
  - Player vs Enemy
    - 踏みつけ判定(StompEnemy)
    - 被弾判定(PlayerHit → GameOver)
    - 踏みつけ時のバウンド処理(Playerの垂直速度更新)
    - スコア加算
  - Player vs Block(Damage/Clear)
    - Damage → GameOver
    - Clear → ClearScene
- 設計上のポイント
  - 「前フレームの足元座標(prevFeet)」を利用する判定のため，更新順序(サンプリング→更新→判定)が重要である点を明示した

### GameEvent / GameEventBufferの導入

PlayingSceneや各SystemからSceneControl::requestScene()やsetScore()を直接呼ぶ構造は結合が強く，処理順序の事故(同フレームで複数要求が出るなど)を誘発しやすい．
そのため，イベントをバッファへ積み，フレーム末尾にまとめて反映する方式へ移行する準備として，GameEventとGameEventBufferを実装した．

- GameEvent
  - std::variant により，継承や仮想関数なしで型安全にイベント種別を表現
  - 例：
    - RequestSceneEvent(シーン遷移要求)
    - AddScoreEvent(スコア加算要求)
- GameEventBuffer
  - イベントのキュー(vector)を保持し，発行側はpush，消費側はitemsで参照できる
- SceneControl::consumeEvents()
  - バッファ内のイベントを走査
    - スコア加算は合算して反映
    - シーン遷移要求は優先順位を比較して最終的な遷移先を1つに決定
  - 同フレームに「Clear」と「GameOver」など複数要求が出ても，決定規則に従って遷移を安定させる狙いがある

### week16の予定

week15の分離により構造は整理されたが，次の課題が残っている：

1. Stageロード後に必要なキャッシュ更新処理の整理
   - BlockのRectキャッシュなどをonStageLoaded()で統一して更新する仕組みを固める
2. GameEvent方式の導入範囲拡大
   - PlayingScene内の直接呼び出しを段階的にイベント発行へ置き換える
   - 他Scene(Title/Clear/GameOverなど)へ適用するかは，競合可能性とメリットを見て判断する
3. update頻度の見直し
   - EnemySensor収集の頻度(毎フレーム必須か)を再検討し，必要ならセンサー更新の間引きやキャッシュ導入を行う

## week16

### week16の概要

week16では，これまでPlayingSceneや各Systemに分散していたシーン遷移やスコア加算などの副作用を整理するために，GameEvent/GameEventBufferを導入し，イベント駆動の更新パイプラインへ移行した．

また，アイテムやブロックの機能拡張に備えて，ItemSystem/BlockSystem/PlayerStateSystem の土台を実装した．
さらにブロックを下から叩く処理についても，単純なAABB接触ではなく，Characterが持っている前フレーム位置情報を利用する方式へ改善した．

### GameEvent / GameEventBuffer

従来はCollisionSystemやProjectileSystemなどが SceneControl::requestScene()やsetScore()を直接呼んでいたため，副作用の発生箇所が分散していた．また，シーン遷移のリクエストは最新のものが優先されるため，可能性は低いがGameOverSceneへの遷移が正しいはずなのに，ClearSceneへの遷移に上書きされるようなエッジケースの可能性があった．
そのため，アイテム出現やプレイヤー状態変化などの新要素を追加した際にイベント関係の処理で責務が混線すると考え，イベントを蓄積してフレーム末尾にまとめて反映する方式へ変更した．

実装した主なイベントは次の通り：

- RequestSceneEvent
  - シーン遷移要求
- AddScoreEvent
  - スコア加算
- SpawnItemEvent
  - アイテム出現
- CollectItemEvent
  - アイテム取得
- BlockHitEvent
  - ブロックを下から叩いた
- SetPlayerFormEvent
  - プレイヤー形態変更(今後の拡張用)
  - チビ/デカ状態など

GameEventBufferはインターフェースIGameEvents を実装しており，各Systemは push()を直接触らずに，必要なイベントのみを発行するようにしている．
これにより，Systemはゲーム全体の状態変更を直接行わず，「何が起きたか」を通知するだけの構造にしている．

### SceneControl::consumeEvents()

イベントの最終的な反映は SceneControl::consumeEvents() に集約した．
この関数では，

1. フレーム中に蓄積されたイベントを走査して
2. スコア加算イベントを合算して反映
3. シーン遷移イベントを優先順位に基づいて1つに決定

をする，走査処理を実施する．

前述の問題であった，同じフレームでClearとGameOverが同時に要求された場合でも，優先順位ルールによって最終的な遷移先が安定して決定される．

### 各Sceneのシーン遷移の整理

PlayingSceneだけでなく，TitleSceneやGameOverSceneなども順次ctrl.requestScene()の直接呼び出しをやめ，
ctx.events.requestScene()を用いる形へ移行した．

これにより，シーン遷移の経路が1本化され，副作用の発生位置が整理された．

### BlockSystem / ItemSystem / PlayerStateSystem

アイテムやブロックの拡張に向けて，最低限のSystemを追加した．
これらはマリオライクにあるアイテムが出てくるブロック，壊せるブロック，アイテムなどの要素に必要である．

- BlockSystem
  - BlockHitEvent を消費し，?ブロックを使用済みブロックへ変更する
  - アイテム出現イベントを発行する
- ItemSystemSpawn
  - ItemEventを消費してアイテムを生成する
  - Playerとの接触を判定し，CollectItemEventを発行する
  - 非活性化アイテムのcleanupを行う
- PlayerStateSystem
  - CollectItemEvent を消費し，きのこ取得などのプレイヤー状態変化を扱う

この構成により，「ブロックを叩く → アイテムが出る → 取得する → プレイヤー状態が変わる」という流れを，
System同士がイベントを介して連携する形で実装できるようになった．

### ブロックを叩く処理

当初の resolveBlockHits()は，上昇中にブロック矩形と接触したかどうかだけを見ていたため，判定が粗かった．
week16では，Characterクラスが保持している前フレーム位置情報(prevTop など)を利用して，
前フレームではブロックの下にあり，今フレームでブロック下端を跨いだかを判定する方式へ変更した．

これにより，単純なAABBの重なりではなく，「下から頭突きをした」という状況をより厳密に捉えられるようになった．

### week17の予定

week16時点でイベント系の整理と，アイテム/ブロックの土台実装は進んだが，まだゲームとしては雛形に近い．
次は見栄えと遊びの説得力を増すために，次の実装を優先する予定である：

- Breakable ブロックの処理完成
  - プレイヤー形態と連動した破壊条件の実装
- ItemSystemの完成
  - コイン，きのこの取得と状態反映
- StageDefinitionLoader によるJSONベースのステージ定義への移行
- ブロック・アイテムの見た目の改善
- SDL依存の削減（SDL_Rect のラップや置換の検討）

## week17

### week17の概要

week17では，マリオライクゲームとして最低限必要な「成長」「アイテム」「被弾による状態遷移」「外部データによるステージ定義」を中心に実装を進めた．
これまでのweek16まで，敵や弾，各種System分割，イベント駆動の基盤はある程度できてきたが，プレイヤーの状態遷移やアイテムによるゲーム性はまだ弱かった．
そこでweek17では，PlayerFormを中心にゲームらしい状態変化を導入し，Questionブロックやコインなどのアイテムを実体としてステージ内へ配置できるようにした．

実装した主な内容は次の通り：

- PlayerFormによるSmall/Super/Fireの状態管理
- Mushroom取得によるSmall → Superへの遷移
- FireFlower取得によるFire状態への遷移
- 敵や敵弾による被弾時に，即GameOverではなく段階的な遷移を実装
  - Fire → Super
  - Super → Small
  - Small → GameOver
- 被弾後の一定時間の無敵処理と，プレイヤーの点滅表現
- ItemクラスとItemSystemによるコインやキノコの実体化
- Questionブロック(？ブロック)を下から叩いたときにアイテムを出現させる処理
- BreakableブロックをSuper状態以上で破壊できる処理
- StageConfigというヘッダファイルへの依存をやめ，stage.jsonを用いたStageDefinitionLoaderによるロード方式へ移行
  - ステージ定義のJSONにenemy/item/playerStartなどを記述する方式へ変更
- PlayerForm と被弾処理
  - プレイヤーの状態は PlayerForm で管理するようにした

```cpp
enum class PlayerForm : std::uint8_t{
    Small,
    Super,
    Fire,
};
```

これによって，単に「プレイヤーと敵との接触による即GameOver」から，アイテム取得や被弾によってプレイヤーの状態が段階的に変化するようになった．

### 被弾処理

被弾処理はPlayer::tryTakeDamage()に集約し，次の挙動を実装した：

- Fire状態で被弾 → Super状態へ戻る
- Super状態で被弾 → Small状態へ戻る
- Small状態で被弾 → GameOver

この実装により，従来の「敵に触れたら即 GameOver」という単純なゲームロジックから脱却した．
また，被弾直後は一定時間invincibleTimerによる無敵状態とし，連続ヒットによる即死を防いでいる．

### 無敵時間と点滅表現

被弾後の無敵状態はinvincibleTimerによって管理している．
毎フレーム減衰させ，正の値である間はダメージを無効化する．
これによって，連続で被弾してゲームオーバーになるという状況を避けた．

さらに視覚的にも無敵状態が分かるように，Player::draw() をオーバーライドし，点滅中は一定周期で描画しないフレームを作ることで簡易的な点滅表現を実装した．
現時点ではテクスチャ差し替えやシェーダ等は使わず，描画の有無だけで処理している．

### ItemとItemSystem

week17では，コインやキノコを単なるイベントではなく，ステージ上に存在する「取得可能な実体」として扱うために Item クラスと ItemSystem を導入した．

Item は少なくとも次の情報を持つ：

- 座標x, y
- アイテム種別ItemType
- 活性/非活性フラグ

現状の描画は暫定的に Renderer::drawRect() を用いており，コイン，キノコ，FireFlower で色分けしている．
将来的には PNG テクスチャ描画へ置き換える想定である．

ItemSystem の主な責務は以下である：

- SpawnItemEventを消費してアイテムを出現させる
- プレイヤーとの接触判定を行う
- 取得されたアイテムを非活性化し，CollectItemEventを発行する
- 非活性アイテムの cleanup

これにより，Questionブロックからのアイテム出現と，ステージに最初から配置されているコインの両方を同じ仕組みで扱えるようになった．

### ブロックの報酬分離

Questionブロックの見た目と中身を分離するため，BlockType とは別に BlockRewardType を導入した．
BlockTypeによる場合分けは見た目に関する情報であり，Super状態ならキノコではなくFireFlowerを出すなどの内部の処理にはBlockRewardTypeを導入した．

```cpp
enum class BlockRewardType : std::uint8_t{
    None,
    Coin,
    Mushroom,
    FireFlower,
};
```

これにより，ブロックの物理的な種類(Standable, Question, Breakable など)と，叩いたときに出る報酬(Coin, Mushroom, FireFlowerなど)を別々に定義できるようにしている．

Questionブロックを叩いた際には，BlockSystemがBlockHitEventを処理し，BlockType::Question → BlockType::UsedQuestion へタイプが変わるようになっている．また，QuestionブロックはBlockRewardType に応じたアイテムをスポーンするようになっている．

また，いわゆるスーパーキノコのようなパワーアップアイテムについてはMarioライクな仕様として

- Small状態でMushroom報酬のブロックを叩く → Mushroom
- Super/Fire状態で同じブロックを叩く → FireFlower

という分岐を入れている．

### Breakableブロック

Breakableブロックは壊すことができるブロックで，足場としての役割とは別の役割を持つようにした．
プレイヤーがSmall状態のときは叩いても壊れないが，Super以上のときは下から叩くことでBlockType::Emptyに変化し，破壊された状態になる．

現状は簡易的に「消える」だけで，破片エフェクトやサウンドは未実装である．
ただし，ゲームロジックとしては「成長状態によって壊せるブロックが増える」というマリオライクの要素を導入できた．

### StageConfigからJSONへの移行

これまでステージ定義は StageConfig.hpp の静的定義に依存していたが，week17でStageDefinitionLoaderによるJSON読み込み方式へ移行した．

これによって，ステージの状態を変更する際に毎回コンパイルする必要がなくなり，JSONやTXTファイルによる外部ファイルの読み込みで難易度の調整ができるようになっている．

stage.json には少なくとも次の情報を持たせている：

- ステージ名
- レベルファイル(ブロックの配置の定義ファイル)パス
- プレイヤー開始座標
- 敵のスポーン情報
- アイテムのスポーン情報

当初はステージ遷移のたびに都度JSONファイルを読み込んでいたが設計を見直し，Game起動時にJSONをロードしてSceneControlにinitStages()で渡し，その後のステージ読み込みはSceneControl::loadStage()が stageDefinitionsを参照して行うようにした．
これにより，ステージ数や配置データをコードから分離し，拡張しやすい構造になった．

### week17の実績

week17 終了時点で，ゲームとして次の流れが成立している：

- ステージ開始
- 敵やコインが配置されている
- コインを取る
- Questionブロックを叩いてMushroomやFireFlowerを出す
- Mushroom/FireFlowerによってプレイヤーの形態が変わる
- Super以上でBreakableブロックを壊せる
- 被弾しても即終了ではなく段階的にSmallへ戻る
- 被弾時の一時的な無敵時間(点滅で表現)
- ステージ定義はJSONからロードされる

week16までの「ゲームの骨組み」から，week17では「マリオライクとして最低限遊べる要素」をかなり増やすことができた．

### 残課題

現在実装できている機能に対して，追加できる機能やゲームの要素として次のものがある：

- アイテムやブロック破壊時の演出が弱い
- ブロックのバウンド
- 破片エフェクト
- 出現時の上昇アニメーション
- Fire状態専用の攻撃制御がまだ単純
- 同時発射数制限
  - ファイアボール発射クールダウン
- ボス戦が未実装
- 土管やサブエリア，水中などステージバリエーションが無い
- アイテムやブロックの見た目はまだ矩形描画に依存している

### week18の予定

- ボス敵の最小実装
- Fire状態の攻撃まわりの調整
- ブロック/アイテムの演出追加
- テクスチャ描画への移行
- ステージ演出の追加

## Week18

### week18の概要

week18ではゲーム全体の手触りを強化するために，主に次の3点を実装した．

- アイテム出現処理の順序整理
- プレイヤーのダッシュ実装
- ボス戦に向けた基礎構造の実装

week17までに，PlayerForm，Questionブロック，ItemSystem，StageDefinitionLoaderによるJson読み込みなど，ゲームの基本要素は一通り実装できてきている．
そこでweek18では，それらの上に「操作の強化」と「ステージ終盤の山場」を作るための実装を進めた．

### アイテム出現処理の整理

これまでQuestionブロックのヒットからアイテムの生成・取得までの処理は概ね実装できていたが，イベントの発行と消費の順序が複雑化していた．
week18では，PlayingScene::update()におけるイベント処理順序を見直し，少なくとも次の流れが明確になるよう整理した．

- CollisionSystemや各種Systemがイベントを発行する
- BlockSystemがBlockHitEventを消費してSpawnItemEventを発行する
- ItemSystemがSpawnItemEventを消費して実体のItemを生成する
- ItemSystemがPlayerとItemの接触を解決し，CollectItemEventを発行する
- PlayerStateSystemがCollectItemEventを消費してPlayerFormを更新する

これにより，Questionブロック→アイテム出現→取得→状態遷移，という一連の処理がイベント駆動で繋がるようになった．
処理の責務が各Systemへ分散されたことで，PlayingScene内部に分岐などのロジック極力を増やさずにゲーム要素を追加できるようになっている．

### プレイヤーのダッシュ

操作面の拡張として，Shiftキーによるダッシュを実装した．
ダッシュ中は通常移動と異なる速度・加速度・摩擦係数を用いるようにし，PlayerConfigで次のようなパラメータを分離した．

- DASH_MAX_SPEED
- DASH_ACCELERATION
- DASH_FRICTION

Player::inputProcessing()ではDash入力を検知してisDashingを更新し，
Player::moveElementsUpdate()では通常時とダッシュ時でパラメータを切り替えて水平移動を計算するようにしている．

この実装により，単に「速くなる」だけではなく，加速していることと減速していることの感触がわかりやすくなったと感じた．

### Fire状態の発射制御の整理

Fire状態ではファイアボールを発射できるが，無制限に撃てるとゲームバランスが崩れやすい．
そのため，ProjectileSystemに発射クールダウン制御を追加した．

#### 画面上に存在できるプレイヤー弾数の上限

ProjectileSystem側でplayerFireCooldownTimerを管理し，
canSpawnPlayerFireball()とtrySpawnPlayerFireball()によって発射可否を一元的に判定するようにした．
これにより，PlayingScene側は「撃てるかどうか」を細かく知らずに済み，責務がProjectileSystemにまとまった．

### ボス戦の基礎構造

week18では，ボス本体を完成させるのではなく，ボス戦をゲームに組み込むための土台作りを行った．主な内容は次の通り：

- BossBattleState の導入
  - ステージにボス戦が存在するか
  - 戦闘が開始したか
  - ボス撃破済みか
- 戦闘開始トリガー位置
- ボス戦中のカメラ制限範囲
- BossEnemy クラスの作成
  - Enemy を継承したボス専用クラス
  - HP，ダメージ無敵時間，発射要求などのボス固有状態を保持
- StageDefinition/stage.json の拡張
  - 各ステージにboss情報を持たせられるようにした
  - SceneControl::loadStage() でのボス読み込み準備
- PlayingScene におけるボス戦開始判定
  - trigger_X 到達で bossBattle.active を true にする
  - ボス戦中のカメラ・移動範囲制限
  - DrawBounds と camera clamp をボス戦用の範囲に切り替える

この段階では，ボス戦はまだ「演出と進行の基盤」であり，攻撃パターンやクリア演出は今後の実装していく．
ただし，ボス戦用の状態遷移とステージデータの流れ，ボスのスプライトシートは作成できている．

### 実装上のポイント

week18では特に，「新しい要素を直接 PlayingScene に足さない」ことを意識した．

- 移動制御はPlayerに集約
- 弾発射制御は ProjectileSystem に集約
- ボス戦の定義は StageDefinition / stage.json に集約
- ボス戦の進行状態は BossBattleState に集約

この方針によって，ゲーム要素を増やしても責務の境界を比較的維持しやすくなった．
なお，Bossは現状基盤構築であるため，BossSystemのようにサブシステム管理の方法は採っていない．
一方で，loadStage()やPlayingScene::update()は依然として肥大化しつつあり，今後はボス戦の進行や演出をさらに導入する前に補助関数への分割を実施して整理しなければならない．

### week19以降の課題

week18終了時点で残っている主な課題は次の通りである．

- BossEnemyへの攻撃パターン追加
- FireBall vs Boss の厳密な当たり判定とダメージ処理の整理
- Boss 撃破から Clear 遷移までの演出
- ボス戦開始時の演出強化
- カメラ固定
- プレイヤーの閉じ込め
- 戦闘開始の視覚的演出
- loadStage() の責務分割
- ボスと通常Enemyの状態管理の整理

## Week19

### Week19の概要

week19では，マリオライクゲームにおけるボス戦の基盤から撃破までの一連の流れを実装した．
これによって通常ステージとは異なる演出を持つゲーム進行をボスステージに導入し，プレイヤーの入力・弾・敵AI・シーン遷移をまたぐ形で，ボス戦を1つのステージとしてクリアまで確立させることができた．

### BossBattleStateの導入

ボス戦専用の状態管理としてBossBattleStateを追加した．
ボス戦が存在しないステージ，開始待機中，戦闘中，撃破後というフェーズを明示的に持たせることで，ボス戦の有無を基準にPlayingScene内の分岐を整理した．

- BossBattleStateの要素
  - None：ボス戦が無い
  - WaitingTrigger：プレイヤーがトリガをまたぐのを待っている
  - Active：ボス戦状態
  - Defeated：ボスが敗北=プレイヤーの勝利

これにより，ボス戦の開始条件・カメラ制御・撃破後の遷移を段階的に扱えるようにした．

### StageDefinition / stage.jsonの拡張

ステージ定義(StageDefinition)にボス戦用の情報を追加した．
具体的には以下をJSONファイルから読み込めるようにした．

- ボス戦の有無：enabled
- ボス戦開始トリガー位置：trigger_X
- カメラ固定範囲：cameraMax
- ボスのスポーン位置：spawn
- 初期HP：hp

ボス戦の有無やパラメータをコードから分離し，ステージデータとして管理できるようにしている．

### BossEnemy クラスの実装

BossはEnemyを継承することとして，BossEnemyを実装した．
BossEnemyは通常敵とは別枠で管理しており，Enemyの管理用vectorであるenemiesでは管理していない．
BossEnemyには以下の機能を実装した：

- HP/最大HPの保持
- 被弾処理
  - 被弾時の無敵時間
  - 無敵時間中の点滅描画
- ボス弾の発射要求
- 撃破時のDying状態遷移

通常敵と同じEnemyStateの流れ(Alive→Dying→Dead)を使いつつ，HP制の敵として成立するように拡張している．

### BossAISystemの導入

ボス専用のAI更新処理としてBossAISystemを追加した．
プレイヤーとボスの位置関係からEnemySensorを構築し，それを元にBossEnemyのthink()を呼び出す構成にした．

これにより，week18のようにPlayingSceneにセンサー収集ロジックを直書きせず，通常敵のAIシステムと同じ責務分割でボスを扱えるようになっている．

### ボス戦トリガーとカメラ固定

プレイヤーがステージ上の一定位置に到達すると，ボス戦フェーズをWaitingTriggerからActiveに遷移させる．
ボス戦中はDrawBoundsとカメラ更新を制限し，戦闘エリア内にプレイヤーを閉じ込めるようになっている．
これにより，「所定位置に入ると戦闘開始」，「戦闘中は逃げられない」というボス戦らしい状況が実現できた．

### ProjectileSystemの拡張

ProjectileSystemを拡張し，以下を追加した．

- ボス弾の生成
- FireBall vs Boss の当たり判定
- ボスへのダメージ付与
- ボス撃破時のスコア加算

これにより，プレイヤーのFire状態による攻撃がボス戦にも接続され，既存の弾システムを流用しつつボス戦ができるようになっている．

また，連続でダメージを与えられると，ボス戦が即終了する可能性があるため無敵時間を設けている．
しかしプレイヤーはボスが一時的に無敵になっていることを知ることができなかったため，Player被弾時の処理を流用し，被弾時の無敵時間分ボスを点滅させて無敵時間中であることがわかるようにしてた．

### ボス撃破演出から ClearScene への遷移

ボスのHPが0になった時点では即遷移させず，Alive状態→Dying状態へ遷移するようにして，一定時間の死亡演出を経てDead状態へ遷移するようにした．
PlayingScene側ではisDead()を監視し，その後ClearSceneへ遷移するようにしている．
これにより，撃破したことがプレイヤーは視覚的にわかり，倒す→クリアというゲーム進行の自然さが実現できていると考える．

### Boss HPバーの実装

ボス戦中のみ画面上部にHPバーを表示するようにした．
現状はSDL_RenderFillRectベースのシンプルな構成で，

- 枠
- 背景
- 現在HP

の描画順序で表示している．
まずはUIとして必要十分な最低限の情報提示を優先し，画像ベースではなくコードベースで実装した．
HPの残とバーの色は関連しており，ボスのHPの状況を色の情報としてわかるようにしている：

- HPが高い：緑
- HPが半分くらい：黄
- HPが低い：赤

### プレイヤー状態のステージ間維持

従来はステージ遷移時にPlayerFormを初期化してしまっていた．
そこでreset関数を分割し，resetForNewGame()とresetForStageTransition()の2つを実装した．
resetForNewGame()はNewGame時のみ完全リセット，resetForStageTransition()ステージ遷移時のもので，PlayerFormは維持される．

これにより，Stage1でFire状態になったプレイヤーがStage2以降でも状態を維持できるようになった．

### week19の設計上のポイント

- ボスを通常敵のvectorとは分離して管理
  - ボスは enemies には含めず，EntityContext で個別参照として保持した
  - 通常敵と異なるライフサイクルやUI連携，ボス戦状態との同期を扱えるようにするため
- 状態を「即時フラグ」ではなくフェーズとして扱う
  - active/defeatedのbool2つではなく，BossBattlePhaseにまとめたことで，現在のボス戦状態を1つの値で扱える
  - PlayingSceneの条件分岐が，isActive()でほとんど判定できるようになった

### week19残課題

week19 時点では，ボス戦の基礎は成立しているが，ボス戦関連の改善事項としては次のものが挙げられる：

- ボス行動パターンの複数化
- ボス戦専用BGM / SE
- ボス撃破演出の強化
- HPバーのデザイン改善
- ボス戦開始演出（警告表示やゲート閉鎖演出など）

### 次に実装する内容

現状でおおよその基盤ができているため，UXを向上させるための実装を予定している．
具体的には次の通り(順不同)：

- スプライトによるブロック・アイテムの描画
  - 現状は矩形の塗りつぶしのみ
- 背景の追加
  - 現状は真っ黒
  - 雲や空で遠近がわかるように描画
- SEやBGMの実装
- アイテム取得時のパーティクル
- 残機システム・プレイヤーの形態変化のスプライト差分

## Week20

### week20の概要

week20では，これまで実装してきたゲームロジックの上に，見た目とUXを改善するための描画・UI系の整備を進めた．
主な目的は矩形ベースの仮描画(SDL_RenderFillRect)から脱却し，プレイしていて最低限の視覚品質へ近づけることを目的とした．

具体的には次の要素を実装した：

- BossのHPバー表示
- Block/Itemのテクスチャ描画への移行および描画経路の整理
- RenderAssetContextを用いた描画アセット管理の導入
- ステージ遷移時のPlayer状態維持
- Boss周辺の描画・UIの改善

### Boss HPバーの追加

Boss戦中に，画面上部へHPバーを表示する処理を追加した．
実装はSDL_RenderFillRectベースで行い，以下の要素を描画している．
矩形の重ね合わせてバーを表現する際に，枠→背景→HPバーの描画順序にしている：

- HPバー
  - 白い外枠
  - 灰色の背景バー
  - 現在HPに応じた可変長のfill部分
- HP残量に応じた色変化
  - 高HP時：緑
  - 高HP時：黃
  - 低HP時：赤

これによりBoss戦の進行状況が視覚的に把握しやすくなった．
また，HPバーはワールド座標ではなく画面固定UIとして描画しているため，カメラ移動の影響を受けない．

### Block/Itemのテクスチャ描画経路の整備

従来，BlockやItemはSDL_RenderFillRectによる単純な矩形描画で仮表示していた．
week20では，これをスプライトベース描画へ移行するための基盤整備を行った．

Game::loadResources() にて，以下のテクスチャを読み込むようにした．

- Block用
  - Standable
  - Question
  - UsedQuestion
  - Breakable
  - DropThrough
  - Damage
  - Clear
- Item用
  - Coin
  - Mushroom
  - FireFlower

また，描画アセットをGameContext経由でScene/Systemから利用できるようにするため，以下の構造を導入した．

- BlockTextureContext
- ItemTextureContext
- RenderAssetContext
  - これはGameContextが読むこむようにして，各RendererがGameContextへ依存するのを避けている

これにより，Block/Itemの描画責務を今後BlockRenderer/ItemRendererのような専用経路へ切り出しやすい状態になった．

### RenderAssetContextの導入

描画用テクスチャをSceneやSystemにベタ渡しするのではなく，GameContext配下でまとめて扱う構造へ変更した．

導入した意図は次の通り：

- Block/Item描画で必要なテクスチャ群を一元管理する
- PlayingSceneが個別のテクスチャを直接握りすぎないようにする
  - 今後，背景・UI・パーティクルなどの描画系資産を整理しやすくする
  - レンダリング処理をPlayingSceneから切り離す

この変更により，ゲームロジック用のContextと描画アセット用のContextの境界を分離する方針にした．

### Player状態のステージ跨ぎ維持

これまではステージ遷移時にPlayerの状態が初期化され，Stage1でFire状態になってもStage2開始時にSmallへ戻っていた．
それではプレイヤーがアイテムを取得する意欲が削がれると考え，ステージをまたいだ際にSuper状態あるいはFire状態を維持するようにした．

これを修正するため，Playerのリセット処理を以下の2系統に分離した．

- resetForNewGame()
  - 完全初期化
  - PlayerForm を Small に戻す
- resetForStageTransition()
  - 位置・速度・一時状態のみリセット
  - PlayerForm は維持

これにより，取得した強化状態を維持したまま次のステージへ進めるようになった．

### Bossの視覚フィードバック改善

Boss被弾時に無敵時間を持たせるだけでなく，その状態がプレイヤーに分かるよう点滅描画を追加した．

実装方針はPlayerの無敵点滅と同様で，invincibleTimerを用いて描画ON/OFFを切り替えている．
これによりBossに攻撃が通っているか，現在は追撃できないかが視覚的に分かりやすくなった．

### 現時点の状況

week20終了時点で，以下は最低限成立している：

- Boss戦の進行がUIで把握できる
- Block/Itemのスプライト描画へ移行するためのアセット経路が通った
- 強化状態を維持したままステージ進行できる
- Bossの被弾状態が視覚的に分かる

現在はゲームロジックの作成から，見た目とUXを整える段階へ入ったと言える．

### 今後の課題

week20時点では，以下は引き続き改善余地がある．

- PlayerのSmall/Super/Fireに応じた見た目差分
- Block/Item描画の完全スプライト化
- 背景描画の追加
- SE/BGM実装
- アイテム取得時やブロック破壊時の演出
- UIレイアウト全体の整理

### week20まとめ

week20では，Boss戦のUI追加と，Block/Itemの描画アセット整理を中心に進めた．
これによりゲーム全体がデバッグモードから，プレイできる状態に進んでいる．

次の段階では，背景・スプライト差分・音・演出を加え，完成度を高めていく．

## Week21

### week21の概要

week21ではweek20で実装したゲームロジックに見た目とUXを改善する整備を進めた．
主にプレイヤーの形態変化に対応した見た目差分のスプライトシートの作成，プレイヤーの形態変化のロジック，ゲームのポーズ処理を実装した．

具体的には次の要素を実装した：

- プレイヤーの形態変化に対応した見た目差分のスプライトシートの作成
- プレイヤーの形態変化のロジック
- ゲームのポーズ処理

### プレイヤーの形態変化に対応した見た目差分のスプライトシートの作成

詳細はassetsディレクトリを参照．

スプライトはPlayerConfigにMetricsとしてまとめ，チビ状態で高さ80(px)，デカ状態で高さ120と定めた．これまでのプレイヤーのスプライトシートよりも若干大きい．幅は72で固定している．

各種の当たり判定の処理にはスプライトシートに応じた当たり判定用の矩形が必要であるため，Metricsの定義を取得するgetFormMetrics()を用意した．当初は現在のPlayerFormのMerticsのデータを取得するものだったが，形態変化時のめり込みを考慮する都合上，引数で渡されたFormのデータを返す関数に変更した．

### プレイヤーの形態変化のロジック

基本的にはこれまでアイテム取得→内部のフラグで状態を管理，となっていた処理に見た目の変化を加えた形である．

ただし見た目が変化する都合上，見た目が大きくなる変化をする場合，座標を予め計算しブロックにめり込む場合は形態変化を保留するロジックを実装した．基本的には頭の位置が上に伸びるため，プレイヤーの足元の座標を固定し，頭の座標を減算することで，プレイヤーのスプライトサイズの変更を考慮している．

また，PlayingSceneのupdate()の処理順序を見直し，

1. ブロック殴打
2. アイテム処理
3. プレイヤーの状態反映
4. 危険物判定

の流れに修正した．CollisionSystemのresolve()を分解して順序を変更することになった．予めCollisionSystemを作成したことで，修正は最小限ですだ．

### ゲームのポーズ処理

これまでゲーム中のポーズはポーズと言いながらも実際にはTitleSceneへ戻る処理であった．week21で実際にポーズするように機能を実装した．

具体的にはよくあるポーズの実装であり，ポーズ中はほとんどの更新処理をスキップすることでゲーム内の時間を止めている．また，描画は実際に行うが黒い半透明のオーバーレイを配置し，テキストでポーズ中であることを表現するようにした．

このオーバーレイの実装において，PlayingSceneはテキストの用意のみの責務であり，見た目のほとんどはRendererに実装したAPIで行っている．

### 現時点の状況

week21終了時点で

- Playerの形態変化に見た目の変更を加える
- ポーズ機能

を実装している

### week21の課題

week21での課題点は次の通り：

- ポーズ中の入力受付の見直し
- ポーズ中のキー入力の意図と役割を一致させる
- テストの実装

また，week22では簡単なSEを実装してさらにユーザの体験を向上させる．

## week22

### week22の概要

- Pause/Resume処理の修正
- SEを実装
- BGMを実装

### Pause/Resumeの修正

- Pause機能の調整を実施し，PlayingScene内でゲーム更新停止とオーバーレイ表示を，APIをRenderer/Sceneで分離して安定化
- ポーズ中入力と通常時入力を整理し，ゲームプレイ入力がポーズ中に混入しないよう修正(week21ではファイアボール発射を受け付けていた)
- Rendererに半透明オーバーレイ描画処理を追加し，Pause表示の視認性を改善

### SEの導入

- GameEvent/GameEventBufferにPlaySoundEventを追加し，SEをイベント駆動で再生する仕組みを実装
- SoundSystemを実装し，Jump/Stomp/Coin/ItemGet/Damage/Fireball/BlockHit/PauseOpen/PauseCloseのSEをロード・再生する基盤を作成
- プレイヤーのジャンプ成立タイミングをjumpStartedThisFrameで管理し，入力時ではなく行動成立時にSEを鳴らすよう修正
- tryTakeDamage()の戻り値をDamageResultに変更し，被ダメ時の状態分岐を明確化
  - これによって，将来的なダメージに応じた処理の変更も可能である
- 当初はSoundSystemでSDL_mixerを初期化していた
  - BGMの導入に当たり，SEとBGMの双方がSDL_mixerを触るため変更
  - SdlSystem側へSDL_mixerの初期化処理を移し，音声デバイス初期化責務を一元化

### BGMの導入

- MusicSystemを新規実装し，BGMのロード/再生/切替/停止/フェードアウト/一時停止/再開の基盤を作成
- GameContextにMusicSystemを追加し，Sceneと結びつけた
- Sceneに対応したBGMを流す，という方針であり，各SceneのonEnter()で制御できる形へ変更
  - PlayingScene::onEnter() で通常BGM，ボストリガ到達時にはボスBGMへ切り替える処理を追加

### SdlSystemの処理の変更

- week21まではif文で順番にSDLの各サブシステムを初期化し，RAIIに従って逆順で解放していた
- SDL_mixerを導入するに当たり，初期化処理が失敗した場合に初期化済みのサブシステムが開放されずに残ってしまう問題へ対処した
  - これまでの方法ではstd::runtime_error()でプログラムが強制終了するため，すでに初期化されていたサブシステムが残っていた
  - 実際にはプログラムが終了した時点でOSがメモリ上の残骸などを片付けるため，実害は現状ないがより「行儀の良い」初期化処理を実装した
- 具体的は，クラス内で構造体とその構造体に対応するコンストラクタ/デストラクタを実装
  - コンストラクタ→デストラクタの処理で初期化→破棄を必ず実装されるようにした
- RAIIに従いリソースを確保している
  - 初期化：Core→Img→Ttf→Mixerの順番
  - 破棄：Mixer→Ttf→Img→Coreの順番

```cpp
class SdlSystem{
public:
    // 内部に個別のRAIIラッパを初期化の順番で宣言する
    // 初期化順：上から下，破棄順：下から上(逆順でcleanupしていく)
    // SDL初期化
    struct Core{
        Core(){
            if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0){
                throw std::runtime_error(SDL_GetError());
            }
        }
        ~Core(){
            SDL_Quit();
        }
    };
    // SDL_IMG初期化
    struct Img{    
        Img(){
            if(!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)){
                throw std::runtime_error(IMG_GetError());
            }
        }
        ~Img(){
            IMG_Quit();
        }
    };
    // SDL_TTF初期化
    struct Ttf{
        Ttf(){
            if((TTF_Init() != 0)){
                throw std::runtime_error(TTF_GetError());
            }
        }
        ~Ttf(){
            TTF_Quit();
        }
    };
    // SDL_Mixer初期化
    struct Mixer{    
        Mixer(){
            // 44100Hz, ステレオ, バッファ2048サンプル(mp3を使う場合はMIX_INIT_MP3が必要)
            int mixFlags = MIX_INIT_OGG;
            if((Mix_Init(mixFlags) & mixFlags) != mixFlags) {
                throw std::runtime_error(Mix_GetError());
            }
            if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0){
                throw std::runtime_error(Mix_GetError());
           }
        }
        ~Mixer(){
            Mix_CloseAudio();   // Mixerデバイスの終了
            Mix_Quit();
        }
    };

    // RAII
    Core  core;     // 1番目に初期化，4番目に破棄
    Img   img;      // 2番目に初期化，3番目に破棄
    Ttf   ttf;      // 3番目に初期化，2番目に破棄
    Mixer mixer;    // 4番目に初期化，1番目に破棄
    
    // 二重解放防止用のコンストラクタ宣言
    SdlSystem() = default;
    ~SdlSystem() = default;
    SdlSystem(const SdlSystem&) = delete;
    SdlSystem& operator=(const SdlSystem&) = delete;
};
```

### week22の課題

- SdlSystem::Mixer内部の Mix_Init()→Mix_OpenAudio()の2段階初期化がまだ完全な例外安全になっていない
- Title/GameOver/Clear/Result各シーンのonEnter()でBGM再生を統一し，遷移が正しく行われているかを検証する
- Pause中にBGMを一時停止の実装の確認
- BGM切替は現状Mix_HaltMusic()ベースの即時切替なので，必要に応じてフェード制御へ演出的に改善できるかを検討する
- 形態変化/被ダメ/Pause/音再生などの回帰テストを実装したい

## week23

### week23の概要

### iCCPエラー

背景を読み込む際に次のようなエラーが生じた．

```Bash
libpng warning: iCCP: known incorrect sRGB profile
```

iCCPチャンクと呼ばれる，異なるデバイス間で色を再現するためのプロファイルが不正あるいは非標準であることの警告である．
libpngが検出していることがわかる．この警告が表示されても背景の描画に問題は生じなかった．色再現が不正確になる可能性があるが，肉眼で確認した限り色合いに問題はなかった．

対処方法としては，画像操作用のコマンドImageMagickを使用するのが手っ取り早いとネットでも掲載されている[LINK](https://qiita.com/takahashim/items/39534bd820f7fd71a5bb)．

次のコマンドによって，iCCPチャンクを取り除くことで警告が出なくなった．

```Bash
convert "forest.png" -strip "forest.png"
```

その他，png画像なのでファイル最適化用のコマンドラインロスレスツールのpngcrushも有効であることを確認しています：

```Bash
sudo apt update
sudo apt install pngcrush
pngcrush -n -q *.png  # どのファイルが警告を出すかチェック
pngcrush -ow -rem allb -reduce forest.png # 上書きで不正iCCPチャンクを除去
```

なお，元ファイルのバックアップは必ず取っておいたほうが良いです．

## アセット

詳細はATTRIBUTIONを参照．

- Red Hat Boy
- FREE PLATFORMER GAME TILESET

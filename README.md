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

Game は std::unique_ptr\<Scene\> scenes\[3\] と Scene* currentScene を持ち，changeScene() でシーンを切り替える．シーン切り替え時には onExit() / onEnter() を必ず呼ぶことで，各 Scene 自身が「入室時・退室時の初期化／後始末」を管理できるようにした．

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
   - GameContext の構築（Renderer / Input / Camera / WorldInfo / EntityContext / TextRenderContext / RandomContext）
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

## アセット

詳細はATTRIBUTIONを参照．

- Red Hat Boy
- FREE PLATFORMER GAME TILESET

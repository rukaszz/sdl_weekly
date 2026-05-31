#include <gtest/gtest.h>

#include <random>

#include "CameraShakeController.hpp"

/**
 * @brief Construct a new TEST object
 * 
 * カメラシェイクの仕組みチェック
 * →shake時の値の生成，reset時の値生成の結果をチェック
 */
TEST(CameraShakeControllerTests, ResetClearAppliedOffset){
    // カメラとカメラシェイクを宣言
    Camera camera{100.0, 50.0, 800.0, 600.0};
    CameraShakeController shake;
    // メルセンヌツイスタ
    std::mt19937 rng{9999};
    // 揺れ時間1.0s, 揺れの強さ10.0でシェイクスタート
    shake.start(1.0, 10.0);
    // 1フレーム分のdelta(1/60)と乱数生成器を与える
    shake.update(0.016, rng);

    // シェイク用のカメラ作成
    const Camera shaken = shake.applyToCamera(camera);  // 上記のcamera+オフセット付き
    // シェイクが適用されているか確認
    // ※乱数のオフセットがあるので，今のカメラと一致する確率はほぼ0
    EXPECT_TRUE(shaken.x != camera.x || shaken.y != camera.y); 

    // リセット
    shake.reset();
    const Camera reset = shake.applyToCamera(camera);   // オフセットがresetで0になっている
    // リセット直後にapplyToCamera()を呼んで，リセットが適用されているか見る
    EXPECT_DOUBLE_EQ(reset.x, camera.x);
    EXPECT_DOUBLE_EQ(reset.y, camera.y);
    EXPECT_DOUBLE_EQ(reset.width, camera.width);
    EXPECT_DOUBLE_EQ(reset.height, camera.height);
}

/**
 * @brief Construct a new TEST object
 * 
 * start()で指定した間隔だけ振動した後，振動が停止するかチェック
 */
TEST(CameraShakeControllerTests, ShakingStopAfterCertainNumberOfUpdate){
    // カメラとカメラシェイクを宣言
    Camera camera{0.0, 0.0, 800.0, 600.0};
    CameraShakeController shake;
    // メルセンヌツイスタ
    std::mt19937 rng{1};
    // 揺れ時間0.1s, 揺れの強さ10.0でシェイクスタート
    shake.start(0.1, 10.0);
    // 7フレーム分のdelta(1/60 * 7 = 0.112)と乱数生成器を与える
    shake.update(0.016*7, rng);
    // 指定期間0.1を超えているので，次のupdateはtimer<=0になりresetの分岐に進むはず
    shake.update(0.016, rng);

    // shakeがreset()されているはずなので，resultはcameraに一致するはず
    const Camera result = shake.applyToCamera(camera);
    EXPECT_DOUBLE_EQ(result.x, camera.x);
    EXPECT_DOUBLE_EQ(result.y, camera.y);
    EXPECT_DOUBLE_EQ(result.width, camera.width);
    EXPECT_DOUBLE_EQ(result.height, camera.height);
}

/**
 * @brief Construct a new TEST object
 * 
 * カメラシェイクのバリデーションのテスト
 */
TEST(CameraShakeControllerTests, InvalidStartInputShouldNotCreateShake){
    // カメラとカメラシェイクを宣言
    Camera camera{0.0, 0.0, 800.0, 600.0};
    CameraShakeController shake;
    // メルセンヌツイスタ
    std::mt19937 rng{1};
    // 揺れ時間1.0s, 揺れの強さ0.0でシェイクスタート→チェックに引っかかり即returnになる
    shake.start(1.0, 0.0);
    // 1フレーム分のdelta(1/60)と乱数生成器を与える
    shake.update(0.016, rng);
    // カメラシェイクが起きないはず
    const Camera result1 = shake.applyToCamera(camera);
    EXPECT_DOUBLE_EQ(result1.x, camera.x);
    EXPECT_DOUBLE_EQ(result1.y, camera.y);
    EXPECT_DOUBLE_EQ(result1.width, camera.width);
    EXPECT_DOUBLE_EQ(result1.height, camera.height);

    // 揺れ時間-1.0s, 揺れの強さ10.0でシェイクスタート→チェックに引っかかり即returnになる
    shake.start(-1.0, 10.0);
    // 1フレーム分のdelta(1/60)と乱数生成器を与える
    shake.update(0.016, rng);
    // カメラシェイクが起きないはず
    const Camera result2 = shake.applyToCamera(camera);
    EXPECT_DOUBLE_EQ(result2.x, camera.x);
    EXPECT_DOUBLE_EQ(result2.y, camera.y);
    EXPECT_DOUBLE_EQ(result2.width, camera.width);
    EXPECT_DOUBLE_EQ(result2.height, camera.height);
}

/**
 * @brief Construct a new TEST object
 * 
 * start()で指定した間隔だけ振動した後，振動が停止するかチェック
 */
TEST(CameraShakeControllerTests, CameraDoesNotShakeUnstarted){
    // カメラとカメラシェイクを宣言
    Camera camera{0.0, 0.0, 800.0, 600.0};
    CameraShakeController shake;
    // メルセンヌツイスタ
    std::mt19937 rng{1};
    // 揺れ時間0.1s, 揺れの強さ10.0でシェイクをスタートさせない
    // shake.start(0.1, 10.0);
    // 1フレーム分のdelta(1/60)と乱数生成器を与える
    shake.update(0.016, rng);
    // start()でdurationやMagnitudeを与えていないので揺れない(何もしない)はず
    const Camera result = shake.applyToCamera(camera);
    EXPECT_DOUBLE_EQ(result.x, camera.x);
    EXPECT_DOUBLE_EQ(result.y, camera.y);
    EXPECT_DOUBLE_EQ(result.width, camera.width);
    EXPECT_DOUBLE_EQ(result.height, camera.height);
}

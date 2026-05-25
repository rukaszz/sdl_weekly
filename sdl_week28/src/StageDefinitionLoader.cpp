#include "StageDefinitionLoader.hpp"

#include <fstream>
#include <stdexcept>
#include <string>

// nlohmann json
#include <nlohmann/json.hpp>
using nlohmann::json;

#include "StageDefinition.hpp"
#include "BackgroundId.hpp"

namespace{
    /**
     * @brief 文字列とEnemyTypeの対応付を行うヘルパ関数
     * 
     * @param s 
     * @return EnemyType 
     */
    EnemyType parseEnemyType(const std::string& s){
        if(s == "Walker") return EnemyType::Walker;
        if(s == "Chaser") return EnemyType::Chaser;
        if(s == "Jumper") return EnemyType::Jumper;
        if(s == "Turret") return EnemyType::Turret;
        throw std::runtime_error("Unknown EnemyType: " + s);
    }

    /**
     * @brief 文字列とItemTypeの対応付を行うヘルパ関数
     * 
     * @param s 
     * @return ItemType 
     */
    ItemType parseItemType(const std::string& s){
        if(s == "Coin")       return ItemType::Coin;
        if(s == "Mushroom")   return ItemType::Mushroom;
        if(s == "FireFlower") return ItemType::FireFlower;
        throw std::runtime_error("Unknown ItemType: " + s);
    }

    /**
     * @brief 文字列とBackgroundIdとの対応付を行うヘルパ関数
     * 
     * @param s 
     * @return BackgroundId 
     */
    BackgroundId parseBackgroundId(const std::string& s){
        if(s == "Forest")     return BackgroundId::Forest;
        if(s == "DarkForest") return BackgroundId::DarkForest;
        if(s == "HellForest") return BackgroundId::HellForest;
        throw std::runtime_error("Unknown BackgroundId: " + s);
    }

    /**
     * @brief 文字列とBgDecorationTypeとの対応付を行うヘルパ関数
     * 
     * @param s 
     * @return BgDecorationType 
     */
    BgDecorationType parseBgDecorationType(const std::string& s){
        if(s == "Cloud") return BgDecorationType::Cloud;
        if(s == "Star")  return BgDecorationType::Star;
        if(s == "DarkSun")  return BgDecorationType::DarkSun;
        throw std::runtime_error("Unknown BgDecorationType: " + s);
    }
}   // namespace

/**
 * @brief nlohmann jsonを用いたstageX.jsonのパース処理
 * 
 * @param path 
 * @return std::vector<StageDefinition> 
 */
std::vector<StageDefinition> StageDefinitionLoader::loadStagesFromJson(const std::string& path){
    // jsonファイル読み込み
    std::ifstream ifs(path);
    if(!ifs){
        throw std::runtime_error("Failed to open json: " + path);
    }
    // nlohmann jsonへ読み込んだ文字列を渡す
    json j;
    ifs >> j;

    // 出力用
    std::vector<StageDefinition> out;
    // jsonファイル解析
    for(const auto& stageJson : j.at("stages")){
        // StageDefinitionの各要素を取り出す
        StageDefinition sd{};
        sd.name          = stageJson.at("name").get<std::string>();
        sd.levelFile     = stageJson.at("levelFile").get<std::string>();
        // プレイヤーの初期位置
        sd.playerStart_X = stageJson.at("playerStart").at("x").get<double>();
        sd.playerStart_Y = stageJson.at("playerStart").at("y").get<double>();
        // 背景情報の取得(デフォルト値はforest)
        if(stageJson.contains("background")){
            sd.backgroundId = parseBackgroundId(stageJson.at("background").get<std::string>());
        }
        // 背景の装飾の取得
        if(stageJson.contains("bgDecorations")){
            for(const auto& bgDecoJson : stageJson.at("bgDecorations")){
                BgDecorationSpawn bds{};
                bds.world_X        = bgDecoJson.at("world_X").get<int>();
                bds.screen_Y       = bgDecoJson.at("screen_Y").get<int>();
                bds.parallaxFactor = bgDecoJson.at("parallaxFactor").get<double>();
                bds.type           = parseBgDecorationType(bgDecoJson.at("type").get<std::string>());
                sd.bgDecorations.push_back(bds);
            }
        }
        // ボスの情報取得
        if(stageJson.contains("boss")){
            const auto& bossJson = stageJson.at("boss");
            sd.bossBattleDef.enabled     = bossJson.at("enabled").get<bool>();
            sd.bossBattleDef.trigger_X   = bossJson.at("trigger_X").get<double>();
            sd.bossBattleDef.cameraMin_X = bossJson.at("cameraMin_X").get<double>();
            sd.bossBattleDef.cameraMax_X = bossJson.at("cameraMax_X").get<double>();
            sd.bossBattleDef.bossSpawn_X = bossJson.at("spawn_X").get<double>();
            sd.bossBattleDef.bossSpawn_Y = bossJson.at("spawn_Y").get<double>();
            sd.bossBattleDef.hp          = bossJson.at("hp").get<int>();
        }

        // enemiesの子要素分解
        if(stageJson.contains("enemies")){
            for(const auto& enemyJson : stageJson.at("enemies")){
                // EnemySpawnの各要素を取り出す
                EnemySpawn es{};
                es.type  = parseEnemyType(enemyJson.at("type").get<std::string>());
                es.x     = enemyJson.at("x").get<double>();
                es.y     = enemyJson.at("y").get<double>();
                es.speed = enemyJson.at("speed").get<double>();
                // sdへ戻す
                sd.enemySpawns.push_back(es);
            }
        }
        // itemsの小要素分解
        if(stageJson.contains("items")){
            for(const auto& itemJson : stageJson.at("items")){
                // ItemSpawnに対応する各要素を取り出す
                ItemSpawn is{};
                is.x    = itemJson.at("x").get<double>();
                is.y    = itemJson.at("y").get<double>();
                is.type = parseItemType(itemJson.at("type").get<std::string>());
                // sdへ戻す
                sd.itemSpawns.push_back(is);
            }
        }
        // sdが完成したらoutへ
        out.push_back(std::move(sd));
    }
    return out;
}

#include "StageDefinitionLoader.hpp"

#include <fstream>
#include <stdexcept>
#include <string>

// nlohmann json
#include <nlohmann/json.hpp>
using nlohmann::json;

#include "StageDefinition.hpp"

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
     * @brief 文字列とEnemyTypeの対応付を行うヘルパ関数
     * 
     * @param s 
     * @return ItemType 
     */
    ItemType parseItemType(const std::string& s){
        if(s == "Coin")     return ItemType::Coin;
        if(s == "Mushroom") return ItemType::Mushroom;
        throw std::runtime_error("Unknown ItemType: " + s);
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
        sd.playerStart_X = stageJson.at("playerStart").at("x").get<double>();
        sd.playerStart_Y = stageJson.at("playerStart").at("y").get<double>();

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

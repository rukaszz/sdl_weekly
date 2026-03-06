#include "StageDefinitionLoader.hpp"

#include <fstream>
#include <stdexcept>

// nlohmann json
#include "json.hpp"
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
        throw std::runtime_error("Unknown EnemyType" + s);
    }
}   // namespace

/**
 * @brief nlohmann jsonを用いたstageX.jsonのパース処理
 * 
 * @param path 
 * @return std::vector<StageDefinition> 
 */
std::vector<StageDefinition> StageLoader::loadStagesFromJson(const std::string& path){
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
    for(const auto& stages : j.at("stages")){
        // StageDefinitionの各要素を取り出す
        StageDefinition sd;
        sd.name          = stages.at("name").get<std::string>();
        sd.levelFile     = stages.at("levelFile").get<std::string>();
        sd.playerStart_X = stages.at("playerStart").at("x").get<double>();
        sd.playerStart_Y = stages.at("playerStart").at("y").get<double>();
        
        // enemiesの子要素の分解
        for(const auto& enemies : stages.at("enemies")){
            // EnemySpawnの各要素を取り出す
            EnemySpawn es;
            es.type  = parseEnemyType(enemies.at("type")/get<std::string>());
            es.x     = enemies.at("x").get<double>();
            es.y     = enemies.at("y").get<double>();
            es.speed = enemies.at("speed").get<double>();
            // sdへ戻す
            sd.enemySpawns.push_back(es);
        }
        // sdが完成したらoutへ
        out.push_vack(std::move(sd));
    }
    return out;
}

#include "GameUtil.hpp"
#include "BlockLevelLoader.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

/**
 * @brief assets/level/level*.txtを読み込む関数
 * ブロックの配置や種別を外部ファイルから読み取る
 * 
 * @param path 
 * @return std::vector<Block> 
 */
std::vector<Block> BlockLevelLoader::loadFromFile(const std::string& path){
    //ファイルオープン
    std::ifstream ifs(path);
    if(!ifs){
        throw std::runtime_error("Failed to open level file: " + path);
    }
    // 結果格納用
    std::vector<Block> result;
    // 行読み込み用
    std::string line;
    // 不具合追跡用変数
    int lineNo = 0;
    while(std::getline(ifs, line)){
        ++lineNo;
        // 空の行 or コメント(#)は無視
        auto pos = line.find('#');
        // findで見つからない→#が無い箇所のみ
        if(pos != std::string::npos){
            line = line.substr(0, pos);
        }
        // トリム
        GameUtil::trim(line);
        // 空白の行は何もしない(#以降は切り捨て)
        if(line.empty()){
            continue;
        }
        // 空白区切りの文字列を読み取り
        std::istringstream iss(line);
        char typeChar;
        double x, y, w, h;
        // 空白を区切り単位として>>で順番に格納
        if(!(iss >> typeChar >> x >> y >> w >> h)){
            std::cerr << "Level parse failed at " << path << ":" << lineNo << " -> '" << line << "'\n";
            // 失敗したら次へ
            continue;
        }
        // blockTypeは別途判定
        BlockType type;
        switch(typeChar){
        case 'S':
            type = BlockType::Standable;
            break;
        case 'T':
            type = BlockType::DropThrough;
            break;
        case 'D':
            type = BlockType::Damage;
            break;
        case 'C':
            type = BlockType::Clear;
            break;
        default:
            // 不明なタイプは無視
            continue;
        }

        // 読み取った結果を格納
        result.push_back(Block{x, y, w, h, type});
    }
    return result;
}

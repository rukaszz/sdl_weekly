#ifndef RENDERASSETCONTEXT_H
#define RENDERASSETCONTEXT_H

class Texture;

/**
 * @brief ブロックのテクスチャを管理
 * 
 */
struct BlockTextureContext{
    Texture& standable;
    Texture& question;
    Texture& usedQuestion;
    Texture& breakable;
    Texture& dropThrough;
    Texture& damage;
    Texture& clear;
};

/**
 * @brief アイテムのテクスチャを管理
 * 
 */
struct ItemTextureContext{
    Texture& coin;
    Texture& mushroom;
    Texture& fireFlower;
};

/**
 * @brief 背景のテクスチャを管理
 * 
 */
struct BackgroundTextureContext{
    Texture& forest;
    Texture& mountain;
    Texture& sky;
    Texture& darkForest;
    Texture& darkMountain;
    Texture& darkSky;
    Texture& hellForest;
    Texture& hellMountain;
    Texture& hellSky;
    Texture& cloudTexture;
    Texture& starTexture;
    Texture& darkSunTexture;
};

/**
 * @brief ブロック・アイテムのレンダリング用
 * 
 */
struct RenderAssetContext{
    BlockTextureContext blockTextures;
    ItemTextureContext itemTextures;
    BackgroundTextureContext bgTextures;
};

#endif  // RENDERASSETCONTEXT_H

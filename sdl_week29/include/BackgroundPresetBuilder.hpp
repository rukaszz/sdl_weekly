#ifndef BACKGROUNDPRESETBUILDER_H
#define BACKGROUNDPRESETBUILDER_H

#include <cstdint>

class BackgroundRenderer;
struct BackgroundTextureContext;
struct StageDefinition;
enum class BackgroundId : std::uint8_t;

namespace BackgroundPresetBuilder{
    // BackgroundRendererのclear〜addLayer/addDecorationまでを一括で行う
    void build(
        BackgroundRenderer& bgRenderer,
        const BackgroundTextureContext& textures,
        const StageDefinition& stageDef
    );
    // SimpleSceneBackground用のオーバーロード
    void build(
        BackgroundRenderer& bgRenderer,
        const BackgroundTextureContext& textures,
        BackgroundId id
    );
}

#endif  // BACKGROUNDPRESETBUILDER_H

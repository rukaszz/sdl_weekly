#ifndef BACKGROUNDPRESETBUILDER_H
#define BACKGROUNDPRESETBUILDER_H

class BackgroundRenderer;
struct BackgroundTextureContext;
struct StageDefinition;

namespace BackgroundPresetBuilder{
    // BackgroundRendererのclear〜addLayer/addDecorationまでを一括で行う
    void build(
        BackgroundRenderer& bgRenderer,
        const BackgroundTextureContext& textures,
        const StageDefinition& stageDef
    );
}

#endif  // BACKGROUNDPRESETBUILDER_H
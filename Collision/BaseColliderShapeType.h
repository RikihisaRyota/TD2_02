#pragma once
#include <vector>
#include "Vector2.h"
#include "Vector3.h"
#include <memory>

class BaseColliderShapeType {
public:
    enum class ShapeType
    {
        UNKNOWN = 0,
        BOX2D,
        MAPCHIP2D,
    };

    enum class ColliderType
    {
        UNKNOWN = 0, // 何もなし
        RIGID_BODY, // 体あり。これ同士はめり込み処理あり
        COLLIDER, // 体なし。あたり判定のみ
    };

    struct MAPCHIP2D
    {
        //std::vector<std::vector<std::unique_ptr<BaseColliderShapeType>>> shape_;
        std::vector<std::vector<uint32_t>> mapChip_;
     
        std::vector<int> noRigitBody_;
        std::vector<int> noCollider_;
        void SetMapChip(const std::vector<std::vector<uint32_t>>& mapchip);
        void SetNoRigitBody(int no);
        void SetNoCollider(int no);
        bool IsRigitBody(int y, int x);
        bool IsCollider(int y, int x);
    };

    ShapeType getShapeType() const;

    ColliderType GetColliderType() const;

    void SetColliderType(ColliderType type);

    void SetV2Info(const Vector2& center, const Vector2& scale, const Vector2& velocity = {});

public:

    ShapeType shapeType_;

    ColliderType colliderType_;

    MAPCHIP2D mapChip2D_;

    std::vector<int> iParas_;

    std::vector<float> fParas_;

    std::vector<Vector2> v2Paras_;

    std::vector<Vector3> v3Paras_;


};
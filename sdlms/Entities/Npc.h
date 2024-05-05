#pragma once

#include "Entity.h"
#include "wz/Property.hpp"
#include "Core/World.h"
#include "Components/AnimatedSprite.h"

class Npc : public Entity
{
public:
    Npc(wz::Node *node, World *world);

    std::unordered_map<std::u16string, std::u16string> str_map;
    Sprite* name=nullptr;
    Sprite* func=nullptr;
};

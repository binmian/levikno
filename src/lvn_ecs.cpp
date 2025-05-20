#include "lvn_ecs.h"

// [FILE]: lvn_ecs.cpp (Entity Component System)
// ------------------------------------------------------------

static size_t                  s_EntityIndexID = 0;
static size_t                  s_MaxEntityIDs = SIZE_MAX;
static LvnQueue<LvnEntity>     s_AvailableEntityIDs;
static LvnComponentManager     s_ComponentManager;

namespace lvn
{

LvnEntity createEntity()
{
    if (!s_AvailableEntityIDs.empty())
    {
        LvnEntity entity = s_AvailableEntityIDs.front();
        s_AvailableEntityIDs.pop();
        return entity;
    }

    LVN_CORE_ASSERT(s_EntityIndexID < s_MaxEntityIDs, "cannot create entity, maximum entity count (%zu) reached", s_MaxEntityIDs);
    return ++s_EntityIndexID;
}

void destroyEntity(LvnEntity entity)
{
    for (auto& comp : s_ComponentManager)
    {
        comp.second->entityDestroyed(entity);
    }

    s_AvailableEntityIDs.push(entity);
}

void setMaxEntityIdCount(size_t max)
{
    s_MaxEntityIDs = max;
}

size_t getMaxEntityIdCount()
{
    return s_MaxEntityIDs;
}

LvnComponentManager* getComponentManager()
{
    return &s_ComponentManager;
}

void ecsRestart()
{
    s_EntityIndexID = 0;
    s_MaxEntityIDs = SIZE_MAX;
    s_AvailableEntityIDs = LvnQueue<LvnEntity>();
    s_ComponentManager = LvnComponentManager();
}

} /* namespace lvn */

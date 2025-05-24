#ifndef HG_LVN_ECS_H
#define HG_LVN_ECS_H


// ------------------------------------------------------------
// Layout: lvn_ecs.h (Entity Component System)
// ------------------------------------------------------------
//
// [SECTION]: ECS (Entity Component System) Definitions & Implementation
// [SECTION]: ECS Functions

#include "levikno.h"

#include <type_traits>


// ------------------------------------------------------------
// [SECTION]: ECS (Entity Component System) Definitions & Implementation
// ------------------------------------------------------------

template <typename T>
class LvnComponentArray;
class LvnIComponentArray;
class LvnComponentManager;
typedef size_t LvnTypeId;
typedef size_t LvnEntity;


inline LvnTypeId i_NextTypeId = 0;

namespace lvn
{

namespace internal
{
    template<typename T>
    inline LvnTypeId getTypeIdImpl()
    {
        static const LvnTypeId id = i_NextTypeId++;
        return id;
    }
} /* namespace internal */

    template<typename T>
    LvnTypeId getTypeId()
    {
        return lvn::internal::getTypeIdImpl<std::remove_cv_t<std::remove_reference_t<T>>>();
    }

} /* namespace lvn */


class LvnIComponentArray
{
public:
    virtual ~LvnIComponentArray() = default;
    virtual void entityDestroyed(LvnEntity entity) = 0;
};

template <typename T>
class LvnComponentArray : public LvnIComponentArray
{
    static_assert(!std::is_pointer_v<T>, "cannot have pointer type as template parameter in component array");

private:
    LvnVector<T> m_Data;
    LvnHashMap<LvnEntity, size_t> m_EntityToIndex;
    LvnQueue<size_t> m_AvailableIndices;

public:
    void add_entity(LvnEntity entity, const T& comp)
    {
        LVN_CORE_ASSERT(!m_EntityToIndex.contains(entity), "entity already has component in component array");

        if (!m_AvailableIndices.empty())
        {
            const size_t index = m_AvailableIndices.front();
            m_AvailableIndices.pop();
            m_Data[index] = comp;
            m_EntityToIndex[entity] = index;
            return;
        }

        m_Data.push_back(comp);
        m_EntityToIndex[entity] = m_Data.size() - 1;
    }

    void remove_entity(LvnEntity entity)
    {
        LVN_CORE_ASSERT(m_EntityToIndex.contains(entity), "entity not found within component array");

        const size_t index = m_EntityToIndex[entity];
        LVN_CORE_ASSERT(index < m_Data.size(), "index out of vector size range");

        m_AvailableIndices.push(index);
        m_EntityToIndex.erase(entity);
    }

    T& get_entity_component(LvnEntity entity)
    {
        LVN_CORE_ASSERT(m_EntityToIndex.contains(entity), "entity not found within component array");

        const size_t index = m_EntityToIndex[entity];
        LVN_CORE_ASSERT(index < m_Data.size(), "index out of vector size range");

        return m_Data[index];
    }

    bool has_entity_with_component(LvnEntity entity)
    {
        return m_EntityToIndex.contains(entity);
    }

    virtual void entityDestroyed(LvnEntity entity) override
    {
        if (has_entity_with_component(entity))
            remove_entity(entity);
    }
};

class LvnComponentManager
{
private:
    LvnHashMap<LvnTypeId, LvnUniquePtr<LvnIComponentArray>> m_Components;
    LvnVector<LvnDoublePair<LvnTypeId, LvnIComponentArray*>> m_ComponentArray;

public:
    template <typename T>
    void add_component()
    {
        LvnTypeId id = lvn::getTypeId<T>();
        m_Components[id] = lvn::makeUniquePtr<LvnComponentArray<T>>();
        m_ComponentArray.push_back(LvnDoublePair<LvnTypeId, LvnIComponentArray*>{id, m_Components[id].get()});
    }

    template <typename T>
    void remove_component()
    {
        LVN_CORE_ASSERT(m_Components.contains(lvn::getTypeId<T>()), "component not found within registry");
        LvnTypeId id = lvn::getTypeId<T>();
        m_Components.erase(id);
        for (size_t i = 0; i < m_ComponentArray.size(); i++)
        {
            if (m_ComponentArray[i].first == id)
            {
                m_ComponentArray.erase_index(i);
                break;
            }
        }
    }

    template <typename T>
    LvnComponentArray<T>& get_component()
    {
        LVN_CORE_ASSERT(m_Components.contains(lvn::getTypeId<T>()), "component not found within registry");
        return *static_cast<LvnComponentArray<T>*>(m_Components[lvn::getTypeId<T>()].get());
    }

    template <typename T>
    bool has_component()
    {
        return m_Components.contains(lvn::getTypeId<T>());
    }

    auto begin() { return m_ComponentArray.begin(); }
    auto end()   { return m_ComponentArray.end(); }
};

namespace lvn
{
    // ------------------------------------------------------------
    // [SECTION]: ECS Functions
    // ------------------------------------------------------------

    LvnEntity               createEntity();
    void                    destroyEntity(LvnEntity entity);
    void                    setMaxEntityIdCount(size_t max);
    size_t                  getMaxEntityIdCount();
    LvnComponentManager*    getComponentManager();
    void                    ecsRestart();


    template <typename T>
    void entityAddComponent(LvnEntity entity, const T& comp)
    {
        LvnComponentManager* compManager = lvn::getComponentManager();
        if (!compManager->has_component<T>())
            compManager->add_component<T>();

        LvnComponentArray<T>& compArray = compManager->get_component<T>();
        compArray.add_entity(entity, comp);
    }

    template <typename T, typename... Args>
    void entityAddComponent(LvnEntity entity, const T& comp, const Args&... args)
    {
        LvnComponentManager* compManager = lvn::getComponentManager();
        if (!compManager->has_component<T>())
            compManager->add_component<T>();

        LvnComponentArray<T>& compArray = compManager->get_component<T>();
        compArray.add_entity(entity, comp);

        entityAddComponent(entity, args...);
    }

    template <typename T>
    void entityRemoveComponent(LvnEntity entity)
    {
        LvnComponentArray<T>& compArray = lvn::getComponentManager()->get_component<T>();
        compArray.remove_entity(entity);
    }

    template <typename T, typename T2, typename... Args>
    void entityRemoveComponent(LvnEntity entity)
    {
        LvnComponentArray<T>& compArray = lvn::getComponentManager()->get_component<T>();
        compArray.remove_entity(entity);

        entityRemoveComponent<T2, Args...>(entity);
    }

    template <typename T>
    T& entityGetComponent(LvnEntity entity)
    {
        LvnComponentArray<T>& compArray = lvn::getComponentManager()->get_component<T>();
        return compArray.get_entity_component(entity);
    }

    template <typename... Ts>
    void entityUpdateSystem(LvnEntity* pEntities, size_t entityCount, void (*func)(Ts&...))
    {
        for (size_t i = 0; i < entityCount; i++)
        {
            func(lvn::entityGetComponent<Ts>(pEntities[i])...);
        }
    }
} /* namespace lvn */

#endif

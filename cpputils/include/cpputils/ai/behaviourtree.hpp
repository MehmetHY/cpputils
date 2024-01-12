#pragma once

#include "../export.hpp"
#include <type_traits>
#include <concepts>
#include <memory>
#include <vector>
#include <functional>

namespace cu::ai
{

enum class BTStatus
{
    failure,
    running,
    success
};

class BTNode
{
public:
    virtual BTStatus tick() = 0;
};

template<typename TNode>
concept InheritsBTNode = std::is_base_of_v<BTNode, TNode>;

class BehaviourTree : public BTNode
{
    std::unique_ptr<BTNode> _root{};

public:
    template<InheritsBTNode TNode, typename... TArgs>
    TNode* createRoot(TArgs&&... args)
    {
        auto node = std::make_unique<TNode>(std::forward<TArgs>(args)...);
        auto ptr  = node.get();
        _root     = std::move(node);

        return ptr;
    }

    BTStatus tick() override
    {
        return nullptr == _root ? BTStatus::failure : _root->tick();
    }
};

class BTControlNode : public BTNode
{
protected:
    size_t                               _currentNodeIndex{};
    std::vector<std::unique_ptr<BTNode>> _children{};
public:
    virtual ~BTControlNode() noexcept = default;

    template<InheritsBTNode TNode, typename... TArgs>
    TNode* createChild(TArgs&&... args)
    {
        auto node = std::make_unique<TNode>(std::forward<TArgs>(args)...);
        auto ptr  = node.get();
        _children.push_back(std::move(node));

        return ptr;
    }
};

class BTFallback : public BTControlNode
{

public:
    BTStatus tick() override
    {
        if (_currentNodeIndex >= _children.size())
        {
            _currentNodeIndex = 0;

            return BTStatus::failure;
        }

        auto status = _children[_currentNodeIndex]->tick();

        switch (status)
        {
        case BTStatus::failure:
            ++this->_currentNodeIndex;

            if (_currentNodeIndex >= _children.size())
            {
                _currentNodeIndex = 0;

                return BTStatus::failure;
            }

            break;

        case BTStatus::success:
            _currentNodeIndex = 0;
            return BTStatus::success;

        default:
            break;
        }

        return BTStatus::running;
    }
};

class BTSequence : public BTControlNode
{
public:
    BTStatus tick() override
    {
        if (_currentNodeIndex >= _children.size())
        {
            _currentNodeIndex = 0;

            return BTStatus::failure;
        }

        auto status = _children[_currentNodeIndex]->tick();

        switch (status)
        {
        case BTStatus::failure:
            _currentNodeIndex = 0;

            return BTStatus::failure;

        case BTStatus::running:
            return BTStatus::running;

        case BTStatus::success:
            ++_currentNodeIndex;

            if (_currentNodeIndex >= _children.size())
            {
                _currentNodeIndex = 0;

                return BTStatus::success;
            }
        }

        return BTStatus::running;
    }
};

class BTAction : public BTNode
{
    std::function<BTStatus(void)> _action;

public:
    BTAction(std::function<BTStatus(void)> action) noexcept
        : _action{std::move(action)}
    {
    }

    virtual ~BTAction() noexcept = default;

    BTStatus tick() override
    {
        return _action();
    }
};

}
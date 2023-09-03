﻿#include "Component.h"

Component* Component::GetParent()
{
	return _parent;
}

Component* Component::GetRoot()
{
	if (_parent == NULL)
	{
		return this;
	}
	return _parent->GetRoot();
}

void Component::AttachToComponent(Component component)
{
	if (_parent == &component)
	{
		return;
	}
	DetachFromParent();

	component.AddComponent(this);
}

void Component::DetachFromParent()
{
	if (_parent)
	{
		_parent->RemoveComponent(this);
	}
}

std::vector<Component*> Component::GetComponentsInChildren()
{
	return _children;
}

void Component::EnsureAwaked()
{
	if (!_awaked)
	{
		_awaked = true;
		Awake();
		ForeachChild([](Component* c) {c->EnsureAwaked(); });
	}
}

void Component::EnsureStarted()
{
	if (!_started)
	{
		_started = true;
		Start();
		ForeachChild([](Component* c) {c->EnsureStarted(); });
	}
}

void Component::EnsureDestroy()
{
	Destroy();
	for (Component* c : _children)
	{
		c->EnsureDestroy();
	}

	_children.clear();
	DetachFromParent();
}

void Component::AddComponent(Component* component)
{
	component->_parent = this;
	_children.push_back(component);
}

void Component::RemoveComponent(Component* component)
{
	for (auto it = _children.begin(); it != _children.end(); it++)
	{
		if (*it == component)
		{
			_children.erase(it);
			break;
		}
	}
	component->_parent = nullptr;
}

#include "SceneGraph.h"

bool SceneGraph::Initialise(void)
{
	std::list<SceneNodePointer>::iterator it;
	for (it = _children.begin(); it != _children.end(); ++it)
	{
		bool noFail = it->get()->Initialise();
		if (!noFail)
		{
			return false;
		}
	}

	return true;
}

void SceneGraph::Update(FXMMATRIX& currentWorldTransformation)
{
	SceneNode::Update(currentWorldTransformation);
	XMMATRIX combineWorldTransform = XMLoadFloat4x4(&_combinedWorldTransformation);
	for (int index = 0; index < _children.size(); index++)
	{
		auto indexP = _children.begin();
		std::advance(indexP, index);
		SceneNodePointer childP = *indexP;
		childP->Update(combineWorldTransform);
	}
}

void SceneGraph::Render(void)
{
	for (int index = 0; index < _children.size(); index++)
	{
		auto indexP = _children.begin();
		std::advance(indexP, index);

		SceneNodePointer childP = *indexP;
		childP->Render();
	}
}

void SceneGraph::Add(SceneNodePointer node)
{
	_children.push_back(node);
}

void SceneGraph::Remove(SceneNodePointer node)
{
	for (int index = 0; index < _children.size(); index++)
	{
		auto indexP = _children.begin();
		std::advance(indexP, index);

		SceneNodePointer childP = *indexP;
		childP->Remove(node);

		if (childP == node)
		{
			_children.erase(indexP);
		}
	}
}

SceneNodePointer SceneGraph::Find(wstring name)
{
	if (name == _name)
	{
		return shared_from_this();
	}
	SceneNodePointer returnValue = nullptr;
	for (int index = 0; index < _children.size(); index++)
	{
		auto indexP = _children.begin();
		std::advance(indexP, index);

		SceneNodePointer childP = *indexP;
		returnValue = childP->Find(name);
	}
}

void SceneGraph::Shutdown(void)
{
	for (int index = 0; index < _children.size(); index++)
	{
		auto indexP = _children.begin();
		std::advance(indexP, index);

		SceneNodePointer childP = *indexP;
		childP->Shutdown();
	}
}
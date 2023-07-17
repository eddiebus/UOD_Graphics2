#include "SceneGraph.h"

bool SceneGraph::Initialise(void)
{
	std::list<SceneNodePointer>::iterator it;
	bool noError = false;
	for (it = _children.begin(); it != _children.end(); ++it)
	{
		noError= it->get()->Initialise();
		//There was an error
		if (!noError)
		{
			return false;
		}
	}

	return true;
}

void SceneGraph::Update(FXMMATRIX& currentWorldTransformation)
{
	SceneNode::Update(currentWorldTransformation);
	FXMMATRIX combineWorldTransform =  XMLoadFloat4x4(&_combinedWorldTransformation);
	std::list<SceneNodePointer>::iterator it;

	//Iterate through list
	for (it = _children.begin(); it != _children.end(); it++)
	{
		//Update objects
		it->get()->Update(combineWorldTransform);
	}
}

void SceneGraph::Render(void)
{
	std::list<SceneNodePointer>::iterator it;
	for (it = _children.begin(); it != _children.end(); it++)
	{
		it->get()->Render();
	}
}

void SceneGraph::Add(SceneNodePointer node)
{
	_children.push_back(node);
}

void SceneGraph::Remove(SceneNodePointer node)
{
	std::list<SceneNodePointer>::iterator it;
	for (it = _children.begin(); it != _children.end(); it++)
	{
		it->get()->Remove(node);
		SceneNodePointer thisNode = (SceneNodePointer)it->get();
		if (node == thisNode)
		{
			_children.erase(it);
		}
	}
	
}

SceneNodePointer SceneGraph::Find(wstring name)
{
	if (name == _name)
	{
		return shared_from_this();
	}
	std::list<SceneNodePointer>::iterator it;
	for (it = _children.begin(); it != _children.end(); it++)
	{
		it->get()->Find(name);
	}
}

void SceneGraph::Shutdown(void)
{
	std::list<SceneNodePointer>::iterator it;
	for (it = _children.begin(); it != _children.end(); it++)
	{
		it->get()->Shutdown();
	}
}
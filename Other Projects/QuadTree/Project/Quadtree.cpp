#include "Quadtree.h"
#include "Enemy.h"
#include "HelperFunctions.h"
#include <queue>

Quadtree::Quadtree(float topLeftX, float topLeftY, float bottomRightX, float bottomRightY)
{
	//Subdivide tree a few times.
	m_root = new node(topLeftX, topLeftY, bottomRightX, bottomRightY);

	//Start with a subdivided root.
	m_root->subdivide();
	m_foundEnemies = nullptr;
}

Quadtree::~Quadtree()
{

}

void Quadtree::draw()
{
	drawOne(m_root);
}

void Quadtree::drawOne(node* drawNode)
{
	if (drawNode == nullptr)
		return;

	DrawSquare(drawNode->topLeftPoint, drawNode->bottomRightPoint);

	for (int i = 0; i < 4; i++)
		drawOne(drawNode->children[i]);
}

void Quadtree::addEnemy(Enemy* newEnemy)
{
	//Non-adaptive
	//Just add to one node based on its origin and not its bounding box.
	float EnmX = newEnemy->GetPos().x;
	float EnmY = newEnemy->GetPos().y;

	//Assuming m_root will never be null.

	node* checkingNode = m_root;
	while (true)
	{
		//bool hasChildren = checkingNode->bottomLeft == nullptr && checkingNode->bottomRight == nullptr && checkingNode->topLeft == nullptr && checkingNode->topRight == nullptr;

		node* nextNode = checkingNode->pointInsideWhichNode(EnmX, EnmY);
		if (nextNode == nullptr)//This node has no children, so it is in this node
			break;
		checkingNode = nextNode;
	}

	//Add enemy to this node.
	checkingNode->data.push_back(newEnemy);
	if (checkingNode->data.size() >= 1)//Subdivide if more than one enemy in this zone.
	{
		checkingNode->subdivide();

		for (std::list<Enemy*>::iterator i = checkingNode->data.begin(); i != checkingNode->data.end(); i++)
			checkingNode->pointInsideWhichNode((*i)->GetPos().x, (*i)->GetPos().y)->data.push_back((*i));

		checkingNode->data.clear();

		for (int i = 0; i < 4; i++)
			if (checkingNode->children[i]->data.empty())//This created node is empty.
				if (!checkingNode->children[i]->hasChildren())
				{
					delete checkingNode->children[i];
					checkingNode->children[i] = nullptr;
				}
	}
}

//Searches for enemies and returns all of them in the rectangle specified.
std::list<Enemy*>* Quadtree::search(float topLeftX, float topLeftY, float bottomRightX, float bottomRightY)
{
	return nullptr;
}

//Converts circle into square to find nodes, then uses circle to compare radius with distance.
std::list<Enemy*>* Quadtree::searchCircle(float X, float Y, float Radius)
{
	//Create bounding box for circle, to be able to search the nodes.
	Vector2 topLeft;
	Vector2 bottomRight;
	topLeft.x = X - Radius;
	topLeft.y = Y - Radius;
	bottomRight.x = X + Radius;
	bottomRight.y = Y + Radius;

	Vector2 circleCenter;
	circleCenter.x = X;
	circleCenter.y = Y;

	delete m_foundEnemies;
	m_foundEnemies = new std::list<Enemy*>;

	std::queue<node*> nodeQueue;

	//Add the root to the queue.
	nodeQueue.push(m_root);

	//Search through all the nodes that overlap the circle, start at the root and check its children, then their children, etc.
	while (!nodeQueue.empty())
	{
		//Get the node at the front of the queue.
		node* thisNode = nodeQueue.front();

		//If this node has no children, then add this nodes children.
		if (!thisNode->hasChildren())
		{
			//Iteratre through enemies
			for (std::list<Enemy*>::iterator enemy = thisNode->data.begin(); enemy != thisNode->data.end(); enemy++)
			{
				//Check if the enemy is within the radius of the circle.
				if (Vector2::GetDistance((*enemy)->GetPos(), circleCenter) <= Radius)
				{
					m_foundEnemies->push_back(*enemy);
				}
			}
		}
		else//This node has some children, so check if they overlap.
		{
			for (int i = 0; i < 4; i++)
				if (thisNode->children[i] != nullptr)
					if (CircleSquareCollision(circleCenter, Radius, thisNode->children[i]->topLeftPoint, thisNode->children[i]->bottomRightPoint))
						nodeQueue.push(thisNode->children[i]);
		}
		nodeQueue.pop();
	}

	//delete m_foundEnemies;

	return m_foundEnemies;
}
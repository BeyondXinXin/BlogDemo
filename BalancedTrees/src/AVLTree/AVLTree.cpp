#include "AVLTree.h"

AVLTreeNode::AVLTreeNode(int k) :
	m_Value(k), m_Left(nullptr), m_Right(nullptr), m_Height(0) {
}

AVLTreeNode* AVLTreeNode::Insert(int k)
{
	if(k < m_Value)
	{
		if(m_Left == nullptr)
		{
			m_Left = new AVLTreeNode(k);
		}
		else
		{
			m_Left = m_Left->Insert(k);
		}
	}
	else if(k > m_Value)
	{
		if(m_Right == nullptr)
		{
			m_Right = new AVLTreeNode(k);
		}
		else
		{
			m_Right = m_Right->Insert(k);
		}
	}
	else {
		return this;
	}
	UpdateHeight();
	return Rote();
}

AVLTreeNode* AVLTreeNode::Remove(int k)
{
	AVLTreeNode* node;

	if(k < m_Value)
	{
		if(m_Left != nullptr)
		{
			m_Left = m_Left->Remove(k);
		}
	}
	else if(k > m_Value)
	{
		if(m_Right != nullptr)
		{
			m_Right = m_Right->Remove(k);
		}
	}
	else
	{
		if(m_Left == nullptr && m_Right == nullptr) // 子节点0
		{
			delete this;
			return nullptr;
		}
		else if(m_Left != nullptr && m_Right == nullptr) // 子节点1
		{
			AVLTreeNode* left = m_Left;
			delete this;
			node = left;
		}
		else if(m_Left == nullptr && m_Right != nullptr) // 子节点1
		{
			AVLTreeNode* right = m_Right;
			delete this;
			node = right;
		}
		else // 子节点2
		{
			AVLTreeNode* min = m_Right;
			while(min->GetLeft() != nullptr)
			{
				min = min->GetLeft();
			}
			m_Value = min->GetValue();
			m_Right = m_Right->Remove(min->GetValue());
		}
	}
	UpdateHeight();
	return Rote();
}

void AVLTreeNode::SetValue(int k) {
	m_Value = k;
}

void AVLTreeNode::SetLeft(AVLTreeNode* l) {
	m_Left = l;
}

void AVLTreeNode::SetRight(AVLTreeNode* r) {
	m_Right = r;
}

void AVLTreeNode::SetHeight(int h) {
	m_Height = h;
}

int AVLTreeNode::GetValue() const {
	return m_Value;
}

AVLTreeNode* AVLTreeNode::GetLeft() const {
	return m_Left;
}

AVLTreeNode* AVLTreeNode::GgetRight() const {
	return m_Right;
}

int AVLTreeNode::GetHeight() const {
	return m_Height;
}

int AVLTreeNode::GetBalanceFactor() const
{
	int leftH = m_Left != nullptr ? m_Left->GetHeight() : -1;
	int rightH = m_Right != nullptr ? m_Right->GetHeight() : -1;
	return leftH - rightH;
}

void AVLTreeNode::UpdateHeight()
{
	int leftH = m_Left != nullptr ? m_Left->GetHeight() : -1;
	int rightH = m_Right != nullptr ? m_Right->GetHeight() : -1;
	m_Height = std::max(leftH, rightH) + 1;
}

AVLTreeNode* AVLTreeNode::Rote()
{
	int balanceFactor = GetBalanceFactor();
	if (balanceFactor > 1) // 左偏树
	{
		if (m_Left->GetBalanceFactor() < 0)
		{
			m_Left->LeftRote();
		}
		return RightRote();
	}
	else if (balanceFactor < -1) // 右偏树
	{
		if (m_Right->GetBalanceFactor() > 0)
		{
			m_Right->RightRote();
		}
		return LeftRote();
	}
	return this;
}

AVLTreeNode* AVLTreeNode::RightRote()
{
	AVLTreeNode* child = m_Left;
	AVLTreeNode* grandChild = child->GgetRight();
	child->SetRight(this);
	this->SetLeft(grandChild);
	UpdateHeight();
	child->UpdateHeight();
	return child;
}

AVLTreeNode* AVLTreeNode::LeftRote()
{
	AVLTreeNode* child = m_Right;
	AVLTreeNode* grandChild = child->GetLeft();
	child->SetLeft(this);
	this->SetRight(grandChild);
	UpdateHeight();
	child->UpdateHeight();
	return child;
}

void AVLTreeNode::Print() const
{
	if (m_Right != nullptr) {
		m_Right->Print(prefix + (isLeft ? "│   " : "    "), false);
	}

	std::cout << prefix;
	std::cout << (isLeft ? "├──" : "└──");
	std::cout << m_Value << std::endl;

	if (m_Left != nullptr) {
		m_Left->Print(prefix + (isLeft ? "    " : "│   "), true);
	}
}

#ifndef AVLTree_H
#define AVLTree_H

#include <iostream>

class AVLTreeNode {
public:
	AVLTreeNode(int k);

	AVLTreeNode* Insert(int k);
	AVLTreeNode* Remove(int k);

	void SetValue(int k);
	void SetLeft(AVLTreeNode* l);
	void SetRight(AVLTreeNode* r);
	void SetHeight(int h);

	int GetValue() const;
	AVLTreeNode* GetLeft() const;
	AVLTreeNode* GgetRight() const;
	int GetHeight() const;
	int GetBalanceFactor()const;

	void Print() const;

private:
	void UpdateHeight();
	AVLTreeNode* Rote();
	AVLTreeNode* RightRote();
	AVLTreeNode* LeftRote();

private:
	int m_Value;
	AVLTreeNode* m_Left;
	AVLTreeNode* m_Right;
	int m_Height;
};

#endif // AVLTree_H
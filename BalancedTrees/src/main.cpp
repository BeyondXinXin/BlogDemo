#include <QApplication>
#include <QFile>
#include <QTextCodec>

#include "AVLTree/AVLTree.h"

int main(int, char* []) {
	AVLTreeNode root = AVLTreeNode(10);
	root.Insert(5);
	root.Insert(15);
	root.Insert(3);
	root.Insert(7);
	root.Insert(13);
	root.Insert(17);
	root.Insert(2);
	root.Insert(4);
	root.Insert(6);
	root.Insert(8);
	root.Insert(12);
	root.Insert(14);
	root.Insert(16);
	root.Insert(18);
	root.Insert(1);
	root.Insert(9);
	root.Insert(11);
	root.Print();

	return 1;
}




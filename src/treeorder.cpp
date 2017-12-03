#include <iostream>


/* A binary tree node has data, pointer to left child
   and a pointer to right child */
struct Node {
public:
	Node(int data, Node *left=nullptr, Node *right=nullptr):
		m_data(data), m_left(left), m_right(right) {}

	int get_data() const {return m_data;}
	void set_data(int data) {m_data = data;}

	Node *get_left() const {return m_left;}
	void set_left(Node *left) {m_left = left;}

	Node *get_right() const {return m_right;}
	void set_right(Node *right) {m_right = right;}

	void print_postorder(void) const {
		if (m_left)	m_left->print_postorder();
		if (m_right) m_right->print_postorder();
		std::cout << m_data << " ";
	}

	void print_preorder(void) const {
		std::cout << m_data << " ";
		if (m_left)	m_left->print_preorder();
		if (m_right) m_right->print_preorder();
	}

	void print_inorder(void) const {
		if (m_left)	m_left->print_inorder();
		std::cout << m_data << " ";
		if (m_right) m_right->print_inorder();
	}

private:
	int m_data;
	Node *m_left;
	Node *m_right;
};


int main() {
	Node *root = new Node(1);
	root->set_left(new Node(2));
	root->set_right(new Node(3));
	root->get_left()->set_left(new Node(4));
	root->get_left()->set_right(new Node(5));

	std::cout << "Preorder traversal of binary tree: ";
	root->print_preorder();
	std::cout << std::endl;

	std::cout << "Postorder traversal of binary tree: ";
	root->print_postorder();
	std::cout << std::endl;

	std::cout << "Inorder traversal of binary tree: ";
	root->print_inorder();
	std::cout << std::endl;

	return 0;
}

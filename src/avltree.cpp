#include <cstddef>
#include <cstring>
#include <iostream>
#include <ostream>


template<class T>
class Node
{
public:
	Node(const T &data, Node *left=nullptr, Node *right=nullptr) :
		m_data(data),
		m_left(left),
		m_right(right)
	{}

	const T &get_data() const {return m_data;}
	void set_data(T &data) {m_data = data;}

	Node *get_left() const {return m_left;}
	void set_left(Node *left) {m_left = left;}

	Node *get_right() const {return m_right;}
	void set_right(Node *right) {m_right = right;}

	void to_dot(const std::string &graph_name="to_dot") const {
		std::cout << "digraph " << graph_name << " {" << std::endl;
		std::cout << *this;
		std::cout << "}" << std::endl;
	}

	friend std::ostream & operator<<(std::ostream &os, const Node<T> &p)
	{
		p.to_dot_recurse(os);
		return os;
	}

private:
	void to_dot_recurse(std::ostream &os) const
	{
		std::cout << "\tp" << reinterpret_cast<uint64_t>(this) << " [label=<" << get_data() << ">];" << std::endl;

		if(get_left()) {
			get_left()->to_dot_recurse(os);
		}

		if(get_right()) {
			get_right()->to_dot_recurse(os);
		}

		if(get_left()) {
			std::cout << "\t"
			          << "p" << reinterpret_cast<uint64_t>(this) << " -> "
			          << "p" << reinterpret_cast<uint64_t>(get_left()) << ';' << std::endl;
		}

		if (get_right()) {
			std::cout << "\t"
			          << "p" << reinterpret_cast<uint64_t>(this) << " -> "
			          << "p" << reinterpret_cast<uint64_t>(get_right()) << ';' << std::endl;
		}
	}

private:
	T m_data;
	Node *m_left;
	Node *m_right;
};


template<class T>
class Tree
{
public:
	Tree(Node<T> *head=nullptr) :
		m_head (head)
	{}

	~Tree()
	{
		delete_recurse(m_head);
	}

	void to_dot(const std::string &graph_name="to_dot") const
	{
		if(m_head) {
			m_head->to_dot(graph_name);
		}
	}

	Tree<T> &push(const T &value)
	{
		m_head = push_recurse(m_head, value);
		return *this;
	}

private:
	void delete_recurse(Node<T> *node)
	{
		if (node) {
			delete_recurse(node->get_left());
			delete_recurse(node->get_right());
			delete node;
		}
	}

	std::size_t tree_depth(Node<T> *node) const
	{
		if (node) {
			std::size_t left_depth = tree_depth(node->get_left());
			std::size_t right_depth = tree_depth(node->get_right());
			return std::max(left_depth, right_depth) + 1;
		}
		else {
			return 0;
		}
	}

	Node<T> *push_recurse(Node<T> *node, const T &value) const
	{
		if (node == nullptr) {
			return new Node<T>(value);
		}

		Node<T> *left = node->get_left();
		Node<T> *right = node->get_right();

		if(value < node->get_data()) {
			node->set_left(push_recurse(left, value));
		}
		else {
			node->set_right(push_recurse(right, value));
		}

		int diff = tree_depth(right) - tree_depth(left);
		switch(diff) {
		case 2:
			{
				int diff_right = tree_depth(right->get_right()) - tree_depth(right->get_left());
				switch(diff_right) {
				case 1: return rotate_left(node);
				case -1: return rotate_right_left(node);
				default: break;
				}
			}
			break;

		case -2:
			{
				int diff_left = tree_depth(left->get_right()) - tree_depth(left->get_left());
				switch(diff_left) {
				case 1: return rotate_right(node);
				case -1: return rotate_left_right(node);
				default: break;
				}
			}
			break;

		default: break;
		}

		return node;
	}

	Node<T> *rotate_left(Node<T> *node) const
	{
		// Case where the node has a sub-tree depth difference of 2 and
		// the right son a difference of 1.
		//
		//  from:   Y         to:    X
		//         / \              / \
		//        a   X            Y   c
		//           / \          / \  c
		//          b   c        a   b
		//              c

		Node<T> *Y = node;
		Node<T> *X = Y->get_right();
		Node<T> *a = Y->get_left();
		Node<T> *b = X->get_left();
		Node<T> *c = X->get_right();

		Y->set_left(a);
		Y->set_right(b);
		X->set_right(c);
		X->set_left(Y);

		return X;
	}

	Node<T> *rotate_right(Node<T> *node) const
	{
		// Case where the node has a sub-tree depth difference of -2 and
		// the left son a difference of -1.
		//
		// from:    Y      to:    X
		//         / \		       / \
		//        X   c		   a   Y
		//       / \			   a  / \
		//      a   b		        b   c
		//      a

		Node<T> *Y = node;
		Node<T> *X = Y->get_left();
		Node<T> *a = X->get_left();
		Node<T> *b = X->get_right();
		Node<T> *c = Y->get_right();

		X->set_right(a);
		Y->set_left(b);
		Y->set_right(c);
		X->set_right(Y);

		return X;
	}

	Node<T> *rotate_right_left(Node<T> *node) const
	{
		// Case where the node has a sub-tree depth difference of 2 and
		// the right son a difference of -1.
		//
		// from:    Z      to:    X
		//         / \		 	   /   \
		//        a   Y	     Z     Y
		//           / \      / \   / \
		//          X   d    a   b c   d
 		//         / \           b
		//        b   c
		//        b

		node->set_right(rotate_right(node->get_right()));
		return rotate_left(node);
	}

	Node<T> *rotate_left_right(Node<T> *node) const
	{
		// Case where the node has a subtree depth difference of -2 and
		// the left son a difference of 1.
		//
		// from:    Z      to:    X
		//         / \          /   \
      //        Y   d        Y     Z
		//       / \          / \   / \
		//      a   X        a   b c   d
		//         / \             c
		//        b   c
		//            c

		node->set_left(rotate_left(node->get_left()));
		return rotate_right(node);
	}


private:
	Node<T> *m_head;
};



int main() {
	Node<int> *root = new Node<int>(1);
	root->set_left(new Node<int>(2));
	root->set_right(new Node<int>(3));
	root->get_left()->set_left(new Node<int>(4));
	root->get_left()->set_right(new Node<int>(5));

	Tree<int> tree;
	tree.push(0).push(2).push(4).push(6).push(5).push(5).push(7);
	tree.push(20).push(30).push(40).push(10);

	tree.to_dot();

	return 0;
}

#include <cstddef>
#include <cstring>
#include <iostream>
#include <ostream>


/**
 * Node class
 */
template<class T>
class Node
{
public:
	/**
	 * Node constructor.
	 */
	Node(const T &data) :
		m_data(data),
		m_left(nullptr),
		m_right(nullptr)
	{}

	/**
	 * Return the node payload.
	 */
	const T &get_data() const {return m_data;}

	/**
	 * Change the node's payload.
	 */
	void set_data(T &data) {m_data = data;}

	/**
	 * Return the left's sub-node.
	 */
	Node *get_left() const {return m_left;}

	/**
	 * Change the left's sub-node.
	 */
	void set_left(Node *left) {m_left = left;}

	/**
	 * Return the right's sub-node.
	 */
	Node *get_right() const {return m_right;}

	/**
	 * Change the right's sub-node.
	 */
	void set_right(Node *right) {m_right = right;}

	/**
	 * Print in the given stream the tree using the dot format
	 * (Graphviz).
	 */
	void to_dot(std::ostream &os, const std::string &graph_name="to_dot") const {
		std::cout << "digraph " << graph_name << " {" << std::endl;
		std::cout << *this;
		std::cout << "}" << std::endl;
	}

private:
	/**
	 * Overload of the operator<<. This method call the recursive
	 * function to_dot_recurse that scan the wall sub-nodes.
	 */
	friend std::ostream & operator<<(std::ostream &os, const Node<T> &p)
	{
		p.to_dot_recurse(os);
		return os;
	}

	/**
	 * Go through the tree to print all payloads in the dot format
	 * (Graphviz). The dot header of the diagram is not managed here.
	 */
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


/**
 * AVL Tree class.
 *
 * It points to a Node root.
 */
template<class T>
class AVLTree
{
public:
	/**
	 * AVLTree constructor
	 */
	AVLTree() :	m_head (nullptr) {}

	/**
	 * AVLTree destructor. Free all nodes recursively.
	 */
	~AVLTree()
	{
		delete_recurse(m_head);
	}

	/**
	 * Print to the dot format the tree. Payload T must implements the
	 * operator<< overloading.
	 */
	void to_dot(const std::string &graph_name="to_dot") const
	{
		if(m_head) {
			m_head->to_dot(std::cout, graph_name);
		}
	}

	/**
	 * Insert an element in the tree. Keep the node sorted. The
	 * insertion can execute one rotation after insertion to keep the
	 * tree properly balanced.
	 */
	AVLTree<T> &push(const T &value)
	{
		m_head = push_recurse(m_head, value);
		return *this;
	}

	/**
	 * Remove an element in the tree. It can execute a rotation during
	 * the ascent of each parent until the root after the node deletion.
	 */
	AVLTree<T> &remove(const T &value)
	{
		m_head = remove_recurse(m_head, value);
		return *this;
	}


private:

	/**
	 * Free the sub-nodes recursively and the given node.
	 */
	void delete_recurse(Node<T> *node)
	{
		if (node) {
			delete_recurse(node->get_left());
			delete_recurse(node->get_right());
			delete node;
		}
	}

	/**
	 * Return the depth of the given node.
	 */
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

	/**
	 * Push the value by recursively searching for the right place in
	 * the node. After insertion, apply the proper rotation depending
	 * on tree depth.
	 *
	 * There is four balance cases:
	 *
	 * - left rotation: the node has a 2 sub-tree depth difference and
	 *   the right son a 1 difference.
	 *
	 * - right rotation: The node has a -2 sub-tree depth difference
	 *   and the left son a -1 difference.
	 *
	 * - double right left rotation: The node has a 2 sub-tree depth
	 *   difference and the right son a -1 difference.
	 *
	 * - double left right rotation: The node has a -2 subtree depth
	 *   difference and the left son a -1 difference.
	 *
	 */
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
				case  1: return rotate_left(node);
				case -1: return rotate_right_left(node);
				default: break;
				}
			}
			break;

		case -2:
			{
				int diff_left = tree_depth(left->get_right()) - tree_depth(left->get_left());
				switch(diff_left) {
				case  1: return rotate_right(node);
				case -1: return rotate_left_right(node);
				default: break;
				}
			}
			break;

		default: break;
		}

		return node;
	}

	/**
	 * Search for the largest node. Mark to nullptr the parent that
	 * points to it and returns the pointer to the largest node.
	 */
	Node<T> *remove_largest(Node<T> *node) const
	{
		if (node == nullptr) {
			return nullptr;
		}

		Node<T> *right = node->get_right();

		if(right == nullptr) {
			return node;
		}

		Node<T> *rnode = remove_largest(right);
		if (rnode == right) {
			node->set_right(nullptr);
		}

		return rnode;
	}

	/**
	 * Remove a node and apply rotations during the ascent of each
	 * parent until the root to keep the tree properly balanced.
	 */
	Node<T> *remove_recurse(Node<T> *node, const T &value) const
	{
		Node<T> *left = node->get_left();
		Node<T> *right = node->get_right();

		if(value < node->get_data()) {
			node->set_left(remove_recurse(left, value));
		}
		else if(value > node->get_data()) {
			node->set_right(remove_recurse(right, value));
		}
		else {
			// We must find the largest payload of the left sub-nodes and
			// replace it within the found node. Then the tree has to be
			// balanced from the current node to the root during the
			// recursion unwinding.
			if(!right && !left) {
				// Node is a leaf, we delete it.
				delete node;
				return nullptr;
			}
			else if(right && !left) {
				Node<T> *rnode = remove_largest(node);
				T data = rnode->get_data();
				node->set_data(data);
				delete rnode;
			}
			else {
				Node<T> *rnode = remove_largest(node->get_left());
				T data = rnode->get_data();
				node->set_data(data);
				delete rnode;
			}
		}

		int diff = tree_depth(right) - tree_depth(left);
		switch(diff) {
		case 2:
			{
				int diff_right = tree_depth(right->get_right()) - tree_depth(right->get_left());
				switch(diff_right) {
				case  1: return rotate_left(node);
				case -1: return rotate_right_left(node);
				default: break;
				}
			}
			break;

		case -2:
			{
				int diff_left = tree_depth(left->get_right()) - tree_depth(left->get_left());
				switch(diff_left) {
				case  1: return rotate_right(node);
				case -1: return rotate_left_right(node);
				default: break;
				}
			}
			break;

		default: break;
		}

		return node;
	}


	/**
	 * Left rotation
	 *
	 * The node has a 2 sub-tree depth difference and
	 * the right son a 1 difference.
	 *
	 *      from:   Y         to:    X
	 *             / \              / \
	 *            a   X            Y   c
	 *               / \          / \  c
	 *              b   c        a   b
	 *                  c
	 */
	Node<T> *rotate_left(Node<T> *node) const
	{

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

	/**
	 * Right rotation
	 *
	 * The node has a -2 sub-tree depth difference and
	 * the left son a -1 difference.
	 *
	 *      from:    Y      to:   X
	 *              / \          / \
	 *             X   c        a   Y
	 *            / \           a  / \
	 *           a   b            b   c
	 *           a
	 */
	Node<T> *rotate_right(Node<T> *node) const
	{
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

	/**
	 * Right left double rotation
	 *
	 * The node has a 2 sub-tree depth difference and
	 * the right son a -1 difference.
	 *
	 *      from:    Z      to:    X
	 *              / \          /   \
	 *             a   Y        Z     Y
	 *                / \      / \   / \
	 *               X   d    a   b c   d
	 *              / \           b
	 *             b   c
	 *             b
	 */
	Node<T> *rotate_right_left(Node<T> *node) const
	{
		node->set_right(rotate_right(node->get_right()));
		return rotate_left(node);
	}

	/**
	 *
	 * Left right double rotation
	 *
	 * The node has a -2 subtree depth difference and
	 * the left son a -1 difference.
	 *
	 *     from:    Z      to:    X
	 *             / \          /   \
	 *            Y   d        Y     Z
	 *           / \          / \   / \
	 *          a   X        a   b c   d
	 *             / \             c
	 *            b   c
	 *                c
	 */
	Node<T> *rotate_left_right(Node<T> *node) const
	{
		node->set_left(rotate_left(node->get_left()));
		return rotate_right(node);
	}

private:
	Node<T> *m_head;
};



int main() {
	AVLTree<int> tree;
	tree.push(0).push(2).push(4).push(6).push(5).push(5).push(7);
	tree.push(20).push(30).push(40).push(10);

	tree.remove(6);

	tree.to_dot();

	return 0;
}

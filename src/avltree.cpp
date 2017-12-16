#include <cstddef>
#include <cstring>
#include <cstdlib>
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
	Node(const T &data, Node *parent=nullptr) :
		m_data(data),
		m_depth(1),
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
	 * Return the depth of the node.
	 */
	std::size_t get_depth() const {return m_depth;}

	/**
	 * Change the depth's of the node.
	 */
	void set_depth(std::size_t depth) {m_depth = depth;}

	/**
	 * Return the difference of sub-nodes depth.
	 */
	int64_t get_depth_diff() const {
		int64_t ld = 0, rd = 0;

		if (m_left)
			ld = m_left->get_depth();

		if (m_right)
			rd = m_right->get_depth();

		return rd - ld;
	}

	/**
	 * Update the depth value by checking left and right depth. (no
	 * recursion).
	 */
	void update_depth() {
		std::size_t ld = 0, rd = 0;

		if (m_left)
			ld = m_left->get_depth();

		if (m_right)
			rd = m_right->get_depth();

		set_depth(std::max(ld, rd) + 1);
	}

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
		std::cout << "digraph " << graph_name << " {\n"
		          << "\tnode [shape=circle];\n\n"
		          << *this
		          << "}" << std::endl;
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
		std::cout << "\tp" << reinterpret_cast<uint64_t>(this) << " ["
		          << "label=< "
		          << "<TABLE BORDER=\"0\">\n"
		          << "\t\t<TR><TD colspan=\"2\"><FONT POINT-SIZE=\"36\"><b>" << get_data() << "</b></FONT></TD></TR>\n"
		          << "\t\t<TR><TD colspan=\"2\"><FONT POINT-SIZE=\"24\">" << get_depth() << " / Δ:" << get_depth_diff() << "</FONT></TD></TR>\n"
		          << "\t\t<TR><TD PORT=\"left\">LEFT</TD>" << "<TD PORT=\"right\">RIGHT</TD></TR>\n"
		          << "\t\t</TABLE>" << " >];" << std::endl;

		if(get_left()) {
			get_left()->to_dot_recurse(os);
		}

		if(get_right()) {
			get_right()->to_dot_recurse(os);
		}

		if(get_left()) {
			std::cout << "\t"
			          << "p" << reinterpret_cast<uint64_t>(this) << ":left -> "
			          << "p" << reinterpret_cast<uint64_t>(get_left()) << ';' << std::endl;
		}

		if (get_right()) {
			std::cout << "\t"
			          << "p" << reinterpret_cast<uint64_t>(this) << ":right -> "
			          << "p" << reinterpret_cast<uint64_t>(get_right()) << ';' << std::endl;
		}
	}

private:
	T m_data;
	std::size_t m_depth;
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
		m_head = nullptr;
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
		m_head->update_depth();
		m_head = balance_tree(m_head);
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
		Y->update_depth();

		X->set_right(c);
		X->set_left(Y);
		X->update_depth();

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

		Y->set_left(b);
		Y->set_right(c);
		Y->update_depth();

		X->set_right(a);
		X->set_right(Y);
		X->update_depth();

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
		Node<T> *Z = node;
		Node<T> *Y = Z->get_right();
		Node<T> *X = Y->get_left();
		Node<T> *a = Z->get_left();
		Node<T> *b = X->get_left();
		Node<T> *c = X->get_right();
		Node<T> *d = Y->get_right();

		Z->set_left(a);
		Z->set_right(b);
		Z->update_depth();

		Y->set_left(c);
		Y->set_right(d);
		Y->update_depth();

		X->set_left(Z);
		X->set_right(Y);
		X->update_depth();

		return X;
	}

	/**
	 *
	 * Left right double rotation
	 *
	 * The node has a -2 subtree depth difference and
	 * the left son a 1 difference.
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
		Node<T> *Z = node;
		Node<T> *Y = Z->get_left();
		Node<T> *X = Y->get_right();
		Node<T> *a = Y->get_left();
		Node<T> *b = X->get_left();
		Node<T> *c = X->get_right();
		Node<T> *d = Z->get_right();

		Y->set_left(a);
		Y->set_right(b);
		Y->update_depth();

		Z->set_left(c);
		Z->set_right(d);
		Z->update_depth();

		X->set_left(Y);
		X->set_right(Z);
		X->update_depth();

		return X;
	}

	/**
	 * Balance the tree. Call the right rotation depending on the
	 * sub-nodes depth.
	 */
	Node<T> *balance_tree(Node<T> *node) const {
		if (!node) return nullptr;

		int64_t diff = node->get_depth_diff();
		if (diff < -1) {
			Node<T> *left = node->get_left();
			if (left) {
				int64_t diff_left = left->get_depth_diff();
				if(diff_left <= 0) {
					return rotate_right(node);
				}
				else {
					return rotate_left_right(node);
				}
			}
		}
		else if (diff > 1) {
			Node<T> *right = node->get_right();
			if (right) {
				int64_t diff_right = right->get_depth_diff();
				if(diff_right >= 0) {
					return rotate_left(node);
				}
				else {
					return rotate_right_left(node);
				}
			}
		}

		return node;
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
			return new Node<T>(value, node);
		}

		if(value < node->get_data()) {
			node->set_left(push_recurse(node->get_left(), value));
		}
		else {
			node->set_right(push_recurse(node->get_right(), value));
		}

		node->update_depth();

		return balance_tree(node);
	}

	/**
	 * Search for the largest node. Mark to nullptr the parent that
	 * points to it and return the pointer to the largest node.
	 */
	Node<T> *remove_largest(Node<T> *node) const
	{
		if (node == nullptr) {
			return nullptr;
		}

		Node<T> *right = node->get_right();

		if(!right) {
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
				// Node has only a right sub-node. The tree is an AVL, so
				// the depth of this sub-node is 1. We can safely remove
				// the right node.
				T data = right->get_data();
				node->set_data(data);
				node->set_right(nullptr);
				delete right;
			}
			else {
				Node<T> *rnode = remove_largest(node->get_left());
				T data = rnode->get_data();
				node->set_data(data);
				if(left == rnode)	node->set_left(rnode->get_left());
				delete rnode;
			}
		}

		return balance_tree(node);
	}


private:
	Node<T> *m_head;
};



int main() {
	std::srand(0);

	AVLTree<int> tree;
	for(std::size_t i=0; i<100;i++) {
		tree.push((std::rand() % 100));
	}

	tree.to_dot();
	return 0;
}

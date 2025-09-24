#include <bits/stdc++.h>

struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
};

void preorderIter(TreeNode *root) {
    if (root == nullptr) return;
    std::stack<TreeNode *> st;
    st.push(root);
    while (!st.empty()) {
        auto node = st.top();
        st.pop();

        std::cout << node->val << ' ';

        if (node->right != nullptr) st.push(node->right);
        if (node->left != nullptr) st.push(node->left);
    }
}

int main() {
    /*
           1
          / \
         2   3
        / \
       4   5
    */
    TreeNode *root = new TreeNode(1);
    root->left = new TreeNode(2);
    root->right = new TreeNode(3);
    root->left->left = new TreeNode(4);
    root->left->right = new TreeNode(5);

    std::cout << "先序遍历结果: ";
    preorderIter(root);
    std::cout << std::endl;

    delete root->left->left;
    delete root->left->right;
    delete root->left;
    delete root->right;
    delete root;

    return 0;
}

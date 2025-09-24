#include <bits/stdc++.h>

struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
};

void postorderIter(TreeNode *root) {
    if (!root) return;
    std::stack<TreeNode *> st1, st2;

    st1.push(root);

    while (!st1.empty()) {
        auto node = st1.top();
        st1.pop();
        st2.push(node);

        if (node->left != nullptr) st1.push(node->left);
        if (node->right != nullptr) st1.push(node->right);
    }

    while (!st2.empty()) {
        auto node = st2.top();
        st2.pop();
        std::cout << node->val << ' ';
    }
}

// 测试
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

    std::cout << "后序遍历结果: ";
    postorderIter(root);
    std::cout << std::endl;

    // 释放内存
    delete root->left->left;
    delete root->left->right;
    delete root->left;
    delete root->right;
    delete root;

    return 0;
}

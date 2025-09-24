#include <bits/stdc++.h>

struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
};

void inorderIter(TreeNode *root) {
    auto cur = root;
    std::stack<TreeNode *> st;
    while (cur != nullptr || !st.empty()) {
        while (cur) {
            st.push(cur);
            cur = cur->left;
        }

        cur = st.top();
        st.pop();
        std::cout << cur->val << ' ';
        cur = cur->right;    
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

    std::cout << "中序遍历结果: ";
    inorderIter(root);
    std::cout << std::endl;

    // 释放内存
    delete root->left->left;
    delete root->left->right;
    delete root->left;
    delete root->right;
    delete root;

    return 0;
}

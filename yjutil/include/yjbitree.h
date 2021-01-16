#ifndef YJBITREE_H_
#define YJBITREE_H_

#include "yjdef.h"
#include <iostream>
#include <vector>

namespace yjutil {
/**
 * @defgroup data_structure Self-defined struct
 * @ingroup yjutil
 * @{
 */

/**
 * @brief Define and implement a binary tree
 */
template <typename T>
struct _bitree {
    _bitree *m_left_child;
    _bitree *m_right_child;
    T m_value;
    _bitree(T e) : m_value(e), m_left_child(NULL), m_right_child(NULL) {}
    ~_bitree() {
        //析构要销毁左右子树
        SAFE_DELETE(m_left_child);
        SAFE_DELETE(m_right_child);
    }

    /// 构造二叉树（用递归）
    // 先造树根，再造左子树，接着右子树，直到返回NULL
    static _bitree *CreateTree(std::vector<T> vec, int up_pos, int down_pos) {
        int _cur_pos = (up_pos + down_pos) / 2;
        // 递归终止
        if (up_pos - down_pos < 0)
            return NULL;
        _bitree *_root = new _bitree(vec.at(_cur_pos));
        // 构造左子树
        _root->m_left_child = CreateTree(vec, _cur_pos - 1, down_pos);
        // 构造右子树
        _root->m_right_child = CreateTree(vec, up_pos, _cur_pos + 1);
        return _root;
    }

    /// 插入元素
    bool InsertElem(T e) {
        if (m_value == e)
            return false;
        if (m_value < e) {
            if (m_right_child == NULL)
                m_right_child = new _bitree(e);
            else
                m_right_child->InsertElem(e);
        } else {
            if (m_left_child == NULL)
                m_left_child = new _bitree(e);
            else
                m_left_child->InsertElem(e);
        }
        return true;
    }

    /// 深度优先探索
    void DFS_print(std::ostream &os) {
        using namespace std;
        vector<_bitree *> _tmp;
        _tmp.push_back(this);
        while (_tmp.size() > 0) {
            vector<_bitree *> _tmp_t;
            typename vector<_bitree *>::iterator _it = _tmp.begin();
            for (; _it != _tmp.end(); ++_it) {
                os << (*_it)->m_value << ",";
                if ((*_it)->m_left_child != NULL)
                    _tmp_t.push_back((*_it)->m_left_child);
                if ((*_it)->m_right_child != NULL)
                    _tmp_t.push_back((*_it)->m_right_child);
            }
            os << endl;
            _tmp.swap(_tmp_t);
        }
    }

    /// 友元函数模版重载输出运算符
    template <typename E>
    friend std::ostream &operator<<(std::ostream &os, const _bitree<E> &e);
};

/// 输出二叉树
template <class T>
std::ostream &operator<<(std::ostream &os, const yjutil::_bitree<T> &e) {
    _bitree<T>(e).DFS_print(os);
    return os;
}

/**
 * @}
 */

} // namespace yjutil

#endif
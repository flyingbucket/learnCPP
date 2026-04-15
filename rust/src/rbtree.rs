use std::sync::Arc;

#[derive(Clone, Debug, PartialEq)]
pub enum Color {
    Red,
    Black,
}

#[derive(Clone, Debug, PartialEq)]
pub enum RBTree<T> {
    Empty,
    // 颜色, 左子树, 节点值, 右子树
    Node(Color, Arc<RBTree<T>>, T, Arc<RBTree<T>>),
}

impl<T: Clone + Ord> Default for RBTree<T> {
    fn default() -> Self {
        Self::new()
    }
}

impl<T: Clone + Ord> RBTree<T> {
    pub fn new() -> Self {
        Self::Empty
    }

    /// 查找树中是否包含指定的值
    pub fn contains(&self, x: &T) -> bool {
        match self {
            Self::Empty => false,
            Self::Node(_, left, val, right) => {
                if x < val {
                    left.contains(x)
                } else if x > val {
                    right.contains(x)
                } else {
                    true
                }
            }
        }
    }

    /// 函数式插入
    pub fn insert(&self, x: T) -> Self {
        // 内部递归插入函数
        fn ins<T: Ord + Clone>(tree: &RBTree<T>, x: T) -> RBTree<T> {
            match tree {
                RBTree::Empty => {
                    // 遇到空节点，插入一个红色的新节点
                    RBTree::Node(
                        Color::Red,
                        Arc::new(RBTree::Empty),
                        x,
                        Arc::new(RBTree::Empty),
                    )
                }
                RBTree::Node(color, left, val, right) => {
                    if x < *val {
                        // 插入左子树，然后触发 balance
                        RBTree::balance(
                            color.clone(),
                            Arc::new(ins(left, x.clone())),
                            val.clone(),
                            right.clone(),
                        )
                    } else if x > *val {
                        // 插入右子树，然后触发 balance
                        RBTree::balance(
                            color.clone(),
                            left.clone(),
                            val.clone(),
                            Arc::new(ins(right, x.clone())),
                        )
                    } else {
                        // 元素已存在，直接返回当前树的克隆
                        tree.clone()
                    }
                }
            }
        }
        // 执行插入，并强制根节点染黑
        match ins(self, x) {
            Self::Empty => unreachable!("ins function should never return an Empty tree!"),
            Self::Node(_, left, val, right) => Self::Node(Color::Black, left, val, right),
        }
    }

    /// Okasaki 4-case balance function
    fn balance(color: Color, left: Arc<Self>, val: T, right: Arc<Self>) -> Self {
        use Color::{Black, Red};
        use RBTree::Node;

        if color == Black {
            // left left Red
            if let Node(Red, ref ll, ref l_val, ref lr) = *left
                && let Node(Red, ref lll, ref ll_val, ref llr) = **ll
            {
                return Node(
                    Red,
                    Arc::new(Node(Black, lll.clone(), ll_val.clone(), llr.clone())),
                    l_val.clone(),
                    Arc::new(Node(Black, lr.clone(), val, right.clone())),
                );
            }

            // left right Red
            if let Node(Red, ref ll, ref l_val, ref lr) = *left
                && let Node(Red, ref lrl, ref lr_val, ref lrr) = **lr
            {
                return Node(
                    Red,
                    Arc::new(Node(Black, ll.clone(), l_val.clone(), lrl.clone())),
                    lr_val.clone(),
                    Arc::new(Node(Black, lrr.clone(), val, right.clone())),
                );
            }

            // right left Red
            if let Node(Red, ref rl, ref r_val, ref rr) = *right
                && let Node(Red, ref rll, ref rl_val, ref rlr) = **rl
            {
                return Node(
                    Red,
                    Arc::new(Node(Black, left.clone(), val, rll.clone())),
                    rl_val.clone(),
                    Arc::new(Node(Black, rlr.clone(), r_val.clone(), rr.clone())),
                );
            }

            // right rigt Red
            if let Node(Red, ref rl, ref r_val, ref rr) = *right
                && let Node(Red, ref rrl, ref rr_val, ref rrr) = **rr
            {
                return Node(
                    Red,
                    Arc::new(Node(Black, left.clone(), val, rl.clone())),
                    r_val.clone(),
                    Arc::new(Node(Black, rrl.clone(), rr_val.clone(), rrr.clone())),
                );
            }
        }

        Node(color, left, val, right)
    }
}

#[cfg(test)]
mod proptest_v_rb_tree {
    use super::*;
    use proptest::prelude::*;

    /// 验证是否满足二叉搜索树的排序性质：左子树 < 根 < 右子树
    fn is_bst<T: Ord + Clone>(tree: &RBTree<T>, min: Option<T>, max: Option<T>) -> bool {
        match tree {
            RBTree::Empty => true,
            RBTree::Node(_, left, val, right) => {
                // 检查当前节点值是否在 (min, max) 区间内
                if let Some(ref lo) = min
                    && val <= lo
                {
                    return false;
                }
                if let Some(ref hi) = max
                    && val >= hi
                {
                    return false;
                }

                // 递归检查：
                // 左子树的所有值必须小于当前 val
                // 右子树的所有值必须大于当前 val
                is_bst(left, min, Some(val.clone())) && is_bst(right, Some(val.clone()), max)
            }
        }
    }
    fn validate_all_invariants<T: Ord + Clone>(tree: &RBTree<T>) {
        match tree {
            RBTree::Empty => return,
            RBTree::Node(color, _, _, _) => {
                // 性质 2: 根节点是黑色
                assert_eq!(*color, Color::Black, "Root must be black");
            }
        }
        // 性质：是 BST
        assert!(is_bst(tree, None, None));

        // 性质 4: 无双红
        assert_no_double_red(tree);

        // 性质 5: 黑高一致
        assert!(
            check_black_height_generic(tree).is_some(),
            "Black height unbalanced"
        );
    }

    // 这里由于是在测试环境下，我们可以稍微放开限制
    fn check_black_height_generic<T>(tree: &RBTree<T>) -> Option<usize> {
        match tree {
            RBTree::Empty => Some(1),
            RBTree::Node(color, left, _, right) => {
                let lh = check_black_height_generic(left)?;
                let rh = check_black_height_generic(right)?;
                if lh != rh {
                    return None;
                }
                Some(if *color == Color::Black { lh + 1 } else { lh })
            }
        }
    }

    fn assert_no_double_red<T>(tree: &RBTree<T>) {
        if let RBTree::Node(Color::Red, left, _, right) = tree
            && (matches!(**left, RBTree::Node(Color::Red, ..))
                || matches!(**right, RBTree::Node(Color::Red, ..)))
        {
            panic!("Double Red detected!");
        }
        if let RBTree::Node(_, left, _, right) = tree {
            assert_no_double_red(left);
            assert_no_double_red(right);
        }
    }

    proptest! {
        /// 测试随机插入一系列 i32，并验证每一步后的红黑树性质
        #[test]
        fn test_rb_invariants_randomly(values in prop::collection::vec(any::<i32>(), 0..500)) {
            let mut tree = RBTree::new();
            for val in values {
                tree = tree.insert(val);
                // 每一插必验，确保平衡操作在任何时刻都没有破坏结构
                validate_all_invariants(&tree);
            }
        }

        /// 专门测试极端情况：连续升序（最容易导致树倾斜的情况）
        #[test]
        fn test_rb_invariants_sequential(size in 0..500usize) {
            let mut tree = RBTree::new();
            for i in 0..size {
                tree = tree.insert(i);
            }
            validate_all_invariants(&tree);
        }
    }
}

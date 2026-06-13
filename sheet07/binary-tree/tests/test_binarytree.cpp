#include "../binary-tree.hpp"
#include <gtest/gtest.h>

namespace binary_tree {
std::string visit(const Tree& tree, Tree::Traversal order)
{
    testing::internal::CaptureStdout();
    tree.traverse(order);
    return testing::internal::GetCapturedStdout();
}

TEST(BINARY_TREE, NonCopyableMovableNode)
{
    ASSERT_FALSE(std::is_copy_constructible_v<Tree::Node>);
    ASSERT_FALSE(std::is_copy_assignable_v<Tree::Node>);
    ASSERT_FALSE(std::is_move_constructible_v<Tree::Node>);
    ASSERT_FALSE(std::is_move_assignable_v<Tree::Node>);
}

TEST(BINARY_TREE, Empty)
{
    Tree tree;
    ASSERT_EQ(visit(tree, Tree::Traversal::PREORDER), "");
    ASSERT_EQ(visit(tree, Tree::Traversal::INORDER), "");
    ASSERT_EQ(visit(tree, Tree::Traversal::POSTORDER), "");
}

TEST(BINARY_TREE, SingleElementValue)
{
    Tree tree;
    const Tree::Node& node = tree.insert(42);
    ASSERT_EQ(&tree.find(42), &node);
    ASSERT_EQ(visit(tree, Tree::Traversal::PREORDER), "(42)");
    ASSERT_EQ(visit(tree, Tree::Traversal::INORDER), "(42)");
    ASSERT_EQ(visit(tree, Tree::Traversal::POSTORDER), "(42)");
}

TEST(BINARY_TREE, SingleElementNode)
{
    Tree tree;
    Tree::Node* ptr = new Tree::Node(42);
    const Tree::Node& node = tree.insert(ptr);
    ASSERT_EQ(ptr, &node); // expected to __NOT__ copy the node
    ASSERT_EQ(&tree.find(42), &node);
    ASSERT_EQ(visit(tree, Tree::Traversal::PREORDER), "(42)");
    ASSERT_EQ(visit(tree, Tree::Traversal::INORDER), "(42)");
    ASSERT_EQ(visit(tree, Tree::Traversal::POSTORDER), "(42)");

    tree.remove(42);
    ASSERT_EQ(visit(tree, Tree::Traversal::PREORDER), "");
    ASSERT_EQ(tree.size(), 0);
}

#define EXPECT_THROW_WITH_MESSAGE(statement, txt)     \
    try {                                             \
        statement;                                    \
        FAIL() << "Expected exception!";              \
    } catch (std::runtime_error & msg) {              \
        ASSERT_EQ(std::string_view(msg.what()), txt); \
    } catch (...) {                                   \
        FAIL() << "Expected runtime_error!";          \
    }

TEST(BINARY_TREE, Exception)
{
    Tree tree;
    EXPECT_THROW_WITH_MESSAGE(tree.find(42), "unable to find element");
    EXPECT_THROW_WITH_MESSAGE(tree.remove(42), "unable to remove element");
}

TEST(BINARY_TREE, MultiElements)
{
    Tree tree;
    tree.insert(new Tree::Node(42));
    tree.insert(10);
    tree.insert(3);
    tree.insert(19);
    tree.insert(18);
    tree.insert(17);
    tree.insert(16);
    tree.insert(new Tree::Node(100));
    tree.insert(new Tree::Node(110));
    tree.insert(new Tree::Node(103));
    tree.insert(new Tree::Node(1000));
    ASSERT_EQ(visit(tree, Tree::Traversal::PREORDER), "(42)(10)(3)(19)(18)(17)(16)(100)(110)(103)(1000)");
    ASSERT_EQ(visit(tree, Tree::Traversal::INORDER), "(3)(10)(16)(17)(18)(19)(42)(100)(103)(110)(1000)");
    ASSERT_EQ(visit(tree, Tree::Traversal::POSTORDER), "(3)(16)(17)(18)(19)(10)(103)(1000)(110)(100)(42)");
    ASSERT_EQ(tree.size(), 11);

    tree.remove(1000);
    tree.remove(103);
    tree.remove(110);
    tree.remove(100);

    ASSERT_EQ(visit(tree, Tree::Traversal::PREORDER), "(42)(10)(3)(19)(18)(17)(16)");
    ASSERT_EQ(visit(tree, Tree::Traversal::INORDER), "(3)(10)(16)(17)(18)(19)(42)");
    ASSERT_EQ(visit(tree, Tree::Traversal::POSTORDER), "(3)(16)(17)(18)(19)(10)(42)");
}

TEST(BINARY_TREE, Mirror)
{
    Tree tree;
    tree.insert(new Tree::Node(42));
    tree.insert(10);
    tree.insert(3);
    tree.insert(19);
    tree.insert(18);
    tree.insert(17);
    tree.insert(16);
    tree.insert(new Tree::Node(100));
    tree.insert(new Tree::Node(110));
    tree.insert(new Tree::Node(103));
    tree.insert(new Tree::Node(1000));
    ASSERT_EQ(visit(tree, Tree::Traversal::INORDER), "(3)(10)(16)(17)(18)(19)(42)(100)(103)(110)(1000)");
    ASSERT_EQ(visit(tree.mirror(), Tree::Traversal::INORDER), "(1000)(110)(103)(100)(42)(19)(18)(17)(16)(10)(3)");
    ASSERT_EQ(tree.mirror().size(), 11);
}
} // namespace binary_tree

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
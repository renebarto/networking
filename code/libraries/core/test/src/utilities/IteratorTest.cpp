#include "GoogleTest.h"

#include "core/utilities/Iterator.h"

namespace core {
namespace utilities {

class IteratorTest : public ::testing::Test
{
};

TEST_F(IteratorTest, Construct)
{
    std::vector<int> container({1, 2, 3, 4, 5});
    Iterator<std::vector<int>> Iterator(container);
    EXPECT_TRUE(Iterator.AtBegin());
    EXPECT_FALSE(Iterator.AtEnd());
}

TEST_F(IteratorTest, ConstructEmptyContainer)
{
    std::vector<int> container;
    Iterator<std::vector<int>> Iterator(container);
    EXPECT_TRUE(Iterator.AtBegin());
    EXPECT_TRUE(Iterator.AtEnd());
}

TEST_F(IteratorTest, PrefixIncrement)
{
    std::vector<int> container({1, 2, 3});
    Iterator<std::vector<int>> Iterator(container);
    EXPECT_TRUE(Iterator.AtBegin());
    EXPECT_FALSE(Iterator.AtEnd());
    EXPECT_EQ(2, ++Iterator);
    EXPECT_FALSE(Iterator.AtBegin());
    EXPECT_FALSE(Iterator.AtEnd());
    EXPECT_EQ(3, ++Iterator);
    EXPECT_FALSE(Iterator.AtBegin());
    EXPECT_FALSE(Iterator.AtEnd());
    EXPECT_THROW(++Iterator, std::runtime_error);
    EXPECT_FALSE(Iterator.AtBegin());
    EXPECT_TRUE(Iterator.AtEnd());
}

TEST_F(IteratorTest, PostfixIncrement)
{
    std::vector<int> container({1, 2, 3});
    Iterator<std::vector<int>> Iterator(container);
    EXPECT_TRUE(Iterator.AtBegin());
    EXPECT_FALSE(Iterator.AtEnd());
    EXPECT_EQ(1, Iterator++);
    EXPECT_FALSE(Iterator.AtBegin());
    EXPECT_FALSE(Iterator.AtEnd());
    EXPECT_EQ(2, Iterator++);
    EXPECT_FALSE(Iterator.AtBegin());
    EXPECT_FALSE(Iterator.AtEnd());
    EXPECT_EQ(3, Iterator++);
    EXPECT_FALSE(Iterator.AtBegin());
    EXPECT_TRUE(Iterator.AtEnd());
}

TEST_F(IteratorTest, Reset)
{
    std::vector<int> container({1, 2, 3});
    Iterator<std::vector<int>> Iterator(container);
    EXPECT_EQ(1, Iterator++);
    EXPECT_EQ(2, Iterator++);
    EXPECT_EQ(3, Iterator++);
    Iterator.Reset();
    EXPECT_TRUE(Iterator.AtBegin());
    EXPECT_FALSE(Iterator.AtEnd());
}

TEST_F(IteratorTest, ChangeValue)
{
    std::vector<int> container({1, 2, 3});
    Iterator<std::vector<int>> Iterator(container);
    Iterator++ = 2;
    EXPECT_EQ(2, container[0]);
    EXPECT_EQ(2, container[1]);
    EXPECT_EQ(3, container[2]);
}

class ConstIteratorTest : public ::testing::Test
{
public:
};

TEST_F(ConstIteratorTest, Construct)
{
    std::vector<int> container({1, 2, 3, 4, 5});
    ConstIterator<std::vector<int>> Iterator(container);
    EXPECT_TRUE(Iterator.AtBegin());
    EXPECT_FALSE(Iterator.AtEnd());
}

TEST_F(ConstIteratorTest, ConstructEmptyContainer)
{
    std::vector<int> container({});
    ConstIterator<std::vector<int>> Iterator(container);
    EXPECT_TRUE(Iterator.AtBegin());
    EXPECT_TRUE(Iterator.AtEnd());
}

TEST_F(ConstIteratorTest, PrefixIncrement)
{
    std::vector<int> container({1, 2, 3});
    ConstIterator<std::vector<int>> Iterator(container);
    EXPECT_TRUE(Iterator.AtBegin());
    EXPECT_FALSE(Iterator.AtEnd());
    EXPECT_EQ(2, ++Iterator);
    EXPECT_FALSE(Iterator.AtBegin());
    EXPECT_FALSE(Iterator.AtEnd());
    EXPECT_EQ(3, ++Iterator);
    EXPECT_FALSE(Iterator.AtBegin());
    EXPECT_FALSE(Iterator.AtEnd());
    EXPECT_THROW(++Iterator, std::runtime_error);
    EXPECT_FALSE(Iterator.AtBegin());
    EXPECT_TRUE(Iterator.AtEnd());
}

TEST_F(ConstIteratorTest, PostfixIncrement)
{
    std::vector<int> container({1, 2, 3});
    ConstIterator<std::vector<int>> Iterator(container);
    EXPECT_TRUE(Iterator.AtBegin());
    EXPECT_FALSE(Iterator.AtEnd());
    EXPECT_EQ(1, Iterator++);
    EXPECT_FALSE(Iterator.AtBegin());
    EXPECT_FALSE(Iterator.AtEnd());
    EXPECT_EQ(2, Iterator++);
    EXPECT_FALSE(Iterator.AtBegin());
    EXPECT_FALSE(Iterator.AtEnd());
    EXPECT_EQ(3, Iterator++);
    EXPECT_FALSE(Iterator.AtBegin());
    EXPECT_TRUE(Iterator.AtEnd());
}

TEST_F(ConstIteratorTest, Reset)
{
    std::vector<int> container({1, 2, 3});
    ConstIterator<std::vector<int>> Iterator(container);
    EXPECT_EQ(1, Iterator++);
    EXPECT_EQ(2, Iterator++);
    EXPECT_EQ(3, Iterator++);
    Iterator.Reset();
    EXPECT_TRUE(Iterator.AtBegin());
    EXPECT_FALSE(Iterator.AtEnd());
}

TEST_F(ConstIteratorTest, ChangeValue)
{
    std::vector<int> container({1, 2, 3});
    ConstIterator<std::vector<int>> Iterator(container);
    // Iterator++ = 2; Fails compilation
    EXPECT_EQ(1, container[0]);
    EXPECT_EQ(2, container[1]);
    EXPECT_EQ(3, container[2]);
}

} // namespace utilities
} // namespace core

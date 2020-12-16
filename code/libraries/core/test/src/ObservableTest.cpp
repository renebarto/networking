#include "GoogleTest.h"

#include "core/Observable.h"

namespace core {

using Value = int;

TEST(observableTest, Construction)
{
    Observable<Value> target;
    EXPECT_EQ(size_t{0}, target.Count());
}

TEST(observableTest, Add)
{
    int value1 = 123;
    int value2 = 234;
    Observable<Value> target;

    target.Add(&value1);
    target.Add(&value2);
    EXPECT_EQ(size_t{2}, target.Count());
    std::vector<Value *> result;
    target.ForAll([&result](Value * value){ result.push_back(value); });
    EXPECT_EQ(size_t{2}, result.size());
    EXPECT_EQ(&value1, result[0]);
    EXPECT_EQ(&value2, result[1]);
}

TEST(observableTest, Remove)
{
    int value1 = 123;
    int value2 = 234;
    int value3 = 345;
    Observable<Value> target;

    target.Add(&value1);
    target.Add(&value2);
    target.Remove(&value3);
    EXPECT_EQ(size_t{2}, target.Count());
    std::vector<Value *> result;
    target.ForAll([&result](Value * value){ result.push_back(value); });
    EXPECT_EQ(size_t{2}, result.size());
    EXPECT_EQ(&value1, result[0]);
    EXPECT_EQ(&value2, result[1]);

    target.Remove(&value1);
    EXPECT_EQ(size_t{1}, target.Count());
    result.clear();
    target.ForAll([&result](Value * value){ result.push_back(value); });
    EXPECT_EQ(size_t{1}, result.size());
    EXPECT_EQ(&value2, result[0]);

    target.Remove(&value2);
    EXPECT_EQ(size_t{0}, target.Count());
    result.clear();
    target.ForAll([&result](Value * value){ result.push_back(value); });
    EXPECT_EQ(size_t{0}, result.size());
}

TEST(observableTest, ForAll)
{
    int value1 = 123;
    int value2 = 234;
    Observable<Value> target;

    target.Add(&value1);
    target.Add(&value2);
    std::vector<Value *> result;
    target.ForAll([&result](Value * value){ result.push_back(value); });
    EXPECT_EQ(size_t{2}, result.size());
    EXPECT_EQ(&value1, result[0]);
    EXPECT_EQ(&value2, result[1]);
}

} // namespace core

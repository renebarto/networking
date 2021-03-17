#include "GoogleTest.h"

#include "utility/Observable.h"

namespace utility {

using Value = int;

TEST(observableTest, Construction)
{
    Observable<Value> target;
    EXPECT_EQ(size_t{0}, target.TotalSubscribers());
}

TEST(observableTest, Subscribe)
{
    int value1 = 123;
    int value2 = 234;
    Observable<Value> target;

    target.Subscribe(&value1);
    target.Subscribe(&value2);
    EXPECT_EQ(size_t{2}, target.TotalSubscribers());
    std::vector<Value *> result;
    target.ForAll([&result](Value * value){ result.push_back(value); });
    EXPECT_EQ(size_t{2}, result.size());
    EXPECT_EQ(&value1, result[0]);
    EXPECT_EQ(&value2, result[1]);
}

TEST(observableTest, Unsubscribe)
{
    int value1 = 123;
    int value2 = 234;
    int value3 = 345;
    Observable<Value> target;

    target.Subscribe(&value1);
    target.Subscribe(&value2);
    target.Unsubscribe(&value3);
    EXPECT_EQ(size_t{2}, target.TotalSubscribers());
    std::vector<Value *> result;
    target.ForAll([&result](Value * value){ result.push_back(value); });
    EXPECT_EQ(size_t{2}, result.size());
    EXPECT_EQ(&value1, result[0]);
    EXPECT_EQ(&value2, result[1]);

    target.Unsubscribe(&value1);
    EXPECT_EQ(size_t{1}, target.TotalSubscribers());
    result.clear();
    target.ForAll([&result](Value * value){ result.push_back(value); });
    EXPECT_EQ(size_t{1}, result.size());
    EXPECT_EQ(&value2, result[0]);

    target.Unsubscribe(&value2);
    EXPECT_EQ(size_t{0}, target.TotalSubscribers());
    result.clear();
    target.ForAll([&result](Value * value){ result.push_back(value); });
    EXPECT_EQ(size_t{0}, result.size());
}

TEST(observableTest, ForAll)
{
    int value1 = 123;
    int value2 = 234;
    Observable<Value> target;

    target.Subscribe(&value1);
    target.Subscribe(&value2);
    std::vector<Value *> result;
    target.ForAll([&result](Value * value){ result.push_back(value); });
    EXPECT_EQ(size_t{2}, result.size());
    EXPECT_EQ(&value1, result[0]);
    EXPECT_EQ(&value2, result[1]);
}

} // namespace utility

#include "GoogleTest.h"

#include "core/Nullable.h"

namespace core {

class NullableTest : public ::testing::Test
{
};

class Value
{
public:
    Value() : m_value() {}
    // cppcheck-suppress noExplicitConstructor
    Value(int value) : m_value(value) {}
    Value(const Value & other) : m_value(other.m_value) {}
    Value(Value && other) : m_value(other.m_value) { other.m_value = 0; }
    Value & operator = (int value)
    {
        m_value = value;
        return *this;
    }
    Value & operator = (const Value & other)
    {
        if (&other != this)
            m_value = other.m_value;
        return *this;
    }
    Value & operator = (Value && other)
    {
        if (&other != this)
        {
            m_value = other.m_value;
            other.m_value = 0;
        }
        return *this;
    }

    bool operator == (const Value & other) const { return m_value == other.m_value; }
    bool operator != (const Value & other) const { return !operator == (other); }

    operator int () const { return m_value; }

private:
    int m_value;
};

inline void PrintTo(const Value & value, std::ostream & stream)
{
    stream << static_cast<int>(value);
}

TEST_F(NullableTest, Construction)
{
    Nullable<Value> target;

    EXPECT_FALSE(target.HasValue());
}

TEST_F(NullableTest, ConstructionValue)
{
    Value value = 1234;
    Nullable<Value> target(value);

    EXPECT_TRUE(target.HasValue());
    EXPECT_EQ(value, target.Value());
}

TEST_F(NullableTest, ConstructionCopy)
{
    Value value = 1234;
    Nullable<Value> other(value);
    Nullable<Value> target(other);

    EXPECT_TRUE(target.HasValue());
    EXPECT_EQ(value, target.Value());

    EXPECT_TRUE(other.HasValue());
    EXPECT_EQ(value, other.Value());
}

TEST_F(NullableTest, ConstructionMove)
{
    Value value = 1234;
    Nullable<Value> other(value);
    Nullable<Value> target(std::move(other));

    EXPECT_TRUE(target.HasValue());
    EXPECT_EQ(value, target.Value());

    EXPECT_FALSE(other.HasValue());
}

TEST_F(NullableTest, AssignmentValue)
{
    Value value = 1234;
    Nullable<Value> target;
    target = value;

    EXPECT_TRUE(target.HasValue());
    EXPECT_EQ(value, target.Value());
}

TEST_F(NullableTest, AssignmentCopy)
{
    Value value = 1234;
    Nullable<Value> other(value);
    Nullable<Value> target;
    target = other;

    EXPECT_TRUE(target.HasValue());
    EXPECT_EQ(value, target.Value());

    EXPECT_TRUE(other.HasValue());
    EXPECT_EQ(value, other.Value());
}

TEST_F(NullableTest, AssignmentMove)
{
    Value value = 1234;
    Nullable<Value> other(value);
    Nullable<Value> target;
    target = std::move(other);

    EXPECT_TRUE(target.HasValue());
    EXPECT_EQ(value, target.Value());

    EXPECT_FALSE(other.HasValue());
}

TEST_F(NullableTest, SetNull)
{
    Value value = 1234;
    Nullable<Value> target(value);

    EXPECT_TRUE(target.HasValue());
    EXPECT_EQ(value, target.Value());

    target.SetNull();

    EXPECT_FALSE(target.HasValue());
}

TEST_F(NullableTest, ValueRefConst)
{
    Value value = 1234;
    const Nullable<Value> target(value);
    const Value &constRef = value;

    EXPECT_TRUE(target.HasValue());
    EXPECT_EQ(value, target.Value());
    EXPECT_EQ(constRef, target.Value());
}

TEST_F(NullableTest, ValueRef)
{
    Value value = 1234;
    Nullable<Value> target(value);
    Value & ref = target.Value();

    EXPECT_TRUE(target.HasValue());
    EXPECT_EQ(value, target.Value());
    EXPECT_EQ(value, ref);
    ref = 123;
    EXPECT_EQ(123, ref);
    EXPECT_EQ(123, target.Value());
}

} // namespace core

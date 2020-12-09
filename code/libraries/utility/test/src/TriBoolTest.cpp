#include "GoogleTest.h"

#include "utility/TriBool.h"

namespace utility {

TEST(TriBoolTest, Construct) {
    TriBool value;
    EXPECT_EQ(TriBool::false_value, value.value);
    EXPECT_FALSE(value.IsTrue());
    EXPECT_TRUE(value.IsFalse());
    EXPECT_FALSE(value.IsIndeterminate());
    EXPECT_FALSE(static_cast<bool>(value));
    EXPECT_FALSE(indeterminate(value));
}

TEST(TriBoolTest, ConstructBoolFalse) {
    TriBool value(false);
    EXPECT_EQ(TriBool::false_value, value.value);
    EXPECT_FALSE(value.IsTrue());
    EXPECT_TRUE(value.IsFalse());
    EXPECT_FALSE(value.IsIndeterminate());
    EXPECT_FALSE(static_cast<bool>(value));
    EXPECT_FALSE(indeterminate(value));
}

TEST(TriBoolTest, ConstructBoolTrue) {
    TriBool value(true);
    EXPECT_EQ(TriBool::true_value, value.value);
    EXPECT_TRUE(value.IsTrue());
    EXPECT_FALSE(value.IsFalse());
    EXPECT_FALSE(value.IsIndeterminate());
    EXPECT_TRUE(static_cast<bool>(value));
    EXPECT_FALSE(indeterminate(value));
}

TEST(TriBoolTest, ConstructIndeterminate) {
    TriBool value(indeterminate);
    EXPECT_EQ(TriBool::indeterminate_value, value.value);
    EXPECT_FALSE(value.IsTrue());
    EXPECT_FALSE(value.IsFalse());
    EXPECT_TRUE(value.IsIndeterminate());
    EXPECT_FALSE(static_cast<bool>(value));
    EXPECT_TRUE(indeterminate(value));
}

TEST(TriBoolTest, OperatorNot) {
    EXPECT_EQ(TriBool(true), !TriBool(false));
    EXPECT_EQ(TriBool(false), !TriBool(true));
    EXPECT_TRUE((!TriBool(indeterminate)).IsIndeterminate());
}

TEST(TriBoolTest, OperatorEquals) {
    TriBool falseValue(false);
    TriBool trueValue(true);
    TriBool indeterminateValue(indeterminate);
    TriBool falseValue2(false);
    TriBool trueValue2(true);
    TriBool indeterminateValue2(indeterminate);

    EXPECT_TRUE(falseValue == falseValue2);
    EXPECT_TRUE(trueValue == trueValue2);
    EXPECT_TRUE((indeterminateValue == indeterminateValue2).IsIndeterminate());
    EXPECT_FALSE(falseValue == trueValue);
    EXPECT_TRUE((falseValue == indeterminateValue).IsIndeterminate());
    EXPECT_FALSE(trueValue == falseValue);
    EXPECT_TRUE((trueValue == indeterminateValue).IsIndeterminate());
}

TEST(TriBoolTest, OperatorNotEquals) {
    TriBool falseValue(false);
    TriBool trueValue(true);
    TriBool indeterminateValue(indeterminate);
    TriBool falseValue2(false);
    TriBool trueValue2(true);
    TriBool indeterminateValue2(indeterminate);

    EXPECT_FALSE(falseValue != falseValue2);
    EXPECT_FALSE(trueValue != trueValue2);
    EXPECT_TRUE((indeterminateValue != indeterminateValue2).IsIndeterminate());
    EXPECT_TRUE(falseValue != trueValue);
    EXPECT_TRUE((falseValue != indeterminateValue).IsIndeterminate());
    EXPECT_TRUE(trueValue != falseValue);
    EXPECT_TRUE((trueValue != indeterminateValue).IsIndeterminate());
}

TEST(TriBoolTest, OperatorLogicalAnd) {
    TriBool falseValue(false);
    TriBool trueValue(true);
    TriBool indeterminateValue(indeterminate);
    TriBool falseValue2(false);
    TriBool trueValue2(true);
    TriBool indeterminateValue2(indeterminate);

    EXPECT_FALSE(falseValue && falseValue2);
    EXPECT_FALSE(falseValue && trueValue2);
    EXPECT_FALSE(trueValue && falseValue2);
    EXPECT_TRUE(trueValue && trueValue2);
    EXPECT_FALSE(falseValue && false);
    EXPECT_FALSE(falseValue && true);
    EXPECT_FALSE(trueValue && false);
    EXPECT_TRUE(trueValue && true);
    EXPECT_FALSE(false && falseValue);
    EXPECT_FALSE(true && falseValue);
    EXPECT_FALSE(false && trueValue);
    EXPECT_TRUE(true && trueValue);
    EXPECT_TRUE((indeterminateValue && indeterminateValue2).IsIndeterminate());
    EXPECT_FALSE(falseValue && indeterminateValue);
    EXPECT_TRUE((trueValue && indeterminateValue).IsIndeterminate());
    EXPECT_FALSE(indeterminateValue && falseValue);
    EXPECT_TRUE((indeterminateValue && trueValue).IsIndeterminate());
}

TEST(TriBoolTest, OperatorLogicalOr) {
    TriBool falseValue(false);
    TriBool trueValue(true);
    TriBool indeterminateValue(indeterminate);
    TriBool falseValue2(false);
    TriBool trueValue2(true);
    TriBool indeterminateValue2(indeterminate);

    EXPECT_FALSE(falseValue || falseValue2);
    EXPECT_TRUE(falseValue || trueValue2);
    EXPECT_TRUE(trueValue || falseValue2);
    EXPECT_TRUE(trueValue || trueValue2);
    EXPECT_FALSE(falseValue || false);
    EXPECT_TRUE(falseValue || true);
    EXPECT_TRUE(trueValue || false);
    EXPECT_TRUE(trueValue || true);
    EXPECT_FALSE(false || falseValue);
    EXPECT_TRUE(true || falseValue);
    EXPECT_TRUE(false || trueValue);
    EXPECT_TRUE(true || trueValue);
    EXPECT_TRUE((indeterminateValue || indeterminateValue2).IsIndeterminate());
    EXPECT_TRUE((falseValue || indeterminateValue).IsIndeterminate());
    EXPECT_TRUE(trueValue || indeterminateValue);
    EXPECT_TRUE((indeterminateValue || falseValue).IsIndeterminate());
    EXPECT_TRUE(indeterminateValue || trueValue);
}

TEST(TriBoolTest, StreamingOutOperator) {
    TriBool falseValue(false);
    TriBool trueValue(true);
    TriBool indeterminateValue(indeterminate);
    std::string falseString("false");
    std::string trueString("true");
    std::string indeterminateString("indeterminate");

    std::ostringstream str;

    str << falseValue << " " << trueValue << " " << indeterminateValue;

    EXPECT_EQ(falseString + " " + trueString + " " + indeterminateString, str.str());
}

TEST(TriBoolTest, StreamingOutOperatorIndeterminate) {
    std::ostringstream str;
    std::string indeterminateString("indeterminate");

    str << indeterminate;

    EXPECT_EQ(indeterminateString, str.str());
}

TEST(TriBoolTest, StreamingInOperator) {
    TriBool falseValue(false);
    TriBool trueValue(true);
    TriBool falseValue2;
    TriBool trueValue2;
    TriBool indeterminateValue2;
    std::string falseString("false");
    std::string trueString("true");
    std::string indeterminateString("indeterminate");

    std::istringstream str(falseString + " " + trueString + " " + indeterminateString);

    str >> falseValue2 >> trueValue2 >> indeterminateValue2;

    EXPECT_EQ(falseValue, falseValue2);
    EXPECT_EQ(trueValue, trueValue2);
    EXPECT_TRUE(indeterminate(indeterminateValue2));
}

} // namespace utility

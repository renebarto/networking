#include "GoogleTest.h"

#include "osal/TypeInfo.h"

namespace osal
{
    
static void TypeInfoDummyFunction(int)
{
}

class TypeInfoDummy
{
public:
    TypeInfoDummy()
    {
    }
};

TEST(TypeInfoTest, FunctionName)
{
    std::string typeName = type(TypeInfoDummyFunction);

    EXPECT_EQ("void __cdecl(int)", typeName);
}

TEST(TypeInfoTest, ClassName)
{
    TypeInfoDummy a;
    std::string typeName = type(a);

    EXPECT_EQ("osal::TypeInfoDummy", typeName);
}

TEST(TypeInfoTest, PrintBool)
{
    std::ostringstream stream;
    stream << std::boolalpha << true << " " << false;
    std::string text = stream.str();

    EXPECT_EQ("true false", text);
}

} // namespace osal

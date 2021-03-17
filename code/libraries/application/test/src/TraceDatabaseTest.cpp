#include "GoogleTest.h"

#include "application/TraceDatabase.h"

namespace application {

TEST(TraceDatabaseTest, LoadEmptyDatabase)
{
    std::string path = std::string(TEST_DATA_DIR) + "/TraceDatabaseEmpty.json";
    TraceDatabase database;
    EXPECT_TRUE(database.Load(path));

}

} // namespace application

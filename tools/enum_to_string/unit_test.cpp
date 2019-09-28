
#include <gmock/gmock.h>
#include <sstream>

#include "logic.hpp"


TEST(EnumFinderTest, emptyEnum)
{
    std::stringstream str("enum E {};");

    auto r = find_enum(str);

    ASSERT_EQ(r.size(), 1);
    EXPECT_EQ(r.front().name, "E");
    EXPECT_TRUE(r.front().entries.empty());
}


TEST(EnumFinderTest, emptyEnumClass)
{
    std::stringstream str("enum class E {};");

    auto r = find_enum(str);

    ASSERT_EQ(r.size(), 1);
    EXPECT_EQ(r.front().name, "E");
    EXPECT_TRUE(r.front().entries.empty());
}


TEST(EnumFinderTest, enumWithEntries)
{
    std::stringstream str("enum E {a, b, c};");

    auto r = find_enum(str);

    ASSERT_EQ(r.size(), 1);

    const Enum& e = r.front();
    EXPECT_EQ(e.name, "E");

    const auto& ents = e.entries;
    ASSERT_EQ(ents.size(), 3);

    EXPECT_EQ(ents[0], "a");
    EXPECT_EQ(ents[1], "b");
    EXPECT_EQ(ents[2], "c");
}


TEST(EnumFinderTest, mixedEnums)
{
    std::stringstream str("enum Abc {}; enum class QWE {q, w, e, qqqq}; enum E {a, b, c,};");

    auto r = find_enum(str);

    ASSERT_EQ(r.size(), 3);

    EXPECT_EQ(r[0].name, "Abc");
    EXPECT_EQ(r[1].name, "QWE");
    EXPECT_EQ(r[2].name, "E");

    EXPECT_EQ(r[0].entries.size(), 0);
    EXPECT_EQ(r[1].entries.size(), 4);
    EXPECT_EQ(r[2].entries.size(), 3);
}

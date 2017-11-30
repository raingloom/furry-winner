#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>
#include "Monopoly.cpp"
//modules when?
//oh. Rust already has them. (and ADTs. and pattern matching. and linear types.)

using namespace std;
using namespace monopoly;

TEST_CASE("empty game")
{
    Game game({});
    CHECK(game.isGameOver());
    CHECK_EQ(game.run(),nobody);
}

#include "test_harness.h"
#include <algds/version.h>

static void algds_test_version(void) {
    const char *version_call1 = algds_version();
    const char *expected_version = "0.1.0";

    REQUIRE(version_call1 != NULL);

    CHECK(strlen(version_call1) != 0);
    CHECK_STR_EQ(expected_version, version_call1);

    const char *version_call2 = algds_version();
    CHECK(version_call1 == version_call2);
}

int main(void) {
    algds_test_version();
    return TEST_STATUS;
}

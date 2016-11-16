/*
 *	MetaCall Library by Parra Studios
 *	Copyright (C) 2016 Vicente Eduardo Ferrer Garcia <vic798@gmail.com>
 *
 *	A library for providing a foreign function interface calls.
 *
 */

#include <gmock/gmock.h>
#include <cs-loader-test/environment.hpp>

int main(int argc, char * argv[])
{
	::testing::InitGoogleMock(&argc, argv);
	::testing::AddGlobalTestEnvironment(new environment());
	return RUN_ALL_TESTS();
}
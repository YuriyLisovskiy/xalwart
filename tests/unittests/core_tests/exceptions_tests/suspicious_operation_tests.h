/*
 * Copyright (c) 2019 Yuriy Lisovskiy
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef WASP_UNIT_TESTS_CORE_TESTS_EXCEPTIONS_TESTS_SUSPICIOUS_OPERATION_TESTS_H
#define WASP_UNIT_TESTS_CORE_TESTS_EXCEPTIONS_TESTS_SUSPICIOUS_OPERATION_TESTS_H

#include <gtest/gtest.h>

#include "../../globals.h"
#include "../../../../src/core/exceptions.h"


__UNIT_TESTS_BEGIN__

class SuspiciousOperationTestCase : public ::testing::Test
{
public:
	const char* FullErrorMessage = "SuspiciousOperation: Test error message from SuspiciousOperationTestCase";
	const char* ErrorFunction = "testFunc";
	const char* ErrorFile = "test_file.cpp";
	const size_t ErrorLine = 1;

	wasp::SuspiciousOperation ConstCharConstructorError;
	wasp::SuspiciousOperation StringConstructorError;

	explicit SuspiciousOperationTestCase()
		: ConstCharConstructorError("Test error message from SuspiciousOperationTestCase", this->ErrorLine, this->ErrorFunction, this->ErrorFile),
		  StringConstructorError(std::string("Test error message from SuspiciousOperationTestCase"), this->ErrorLine, this->ErrorFunction, this->ErrorFile)
	{
	}
};

TEST_F(SuspiciousOperationTestCase, TestWhat)
{
	ASSERT_STREQ(this->ConstCharConstructorError.what(), this->FullErrorMessage);
	ASSERT_STREQ(this->StringConstructorError.what(), this->FullErrorMessage);
}

TEST_F(SuspiciousOperationTestCase, TestLine)
{
	ASSERT_EQ(this->ConstCharConstructorError.line(), this->ErrorLine);
	ASSERT_EQ(this->StringConstructorError.line(), this->ErrorLine);
}

TEST_F(SuspiciousOperationTestCase, TestFunction)
{
	ASSERT_EQ(this->ConstCharConstructorError.function(), this->ErrorFunction);
	ASSERT_EQ(this->StringConstructorError.function(), this->ErrorFunction);
}

TEST_F(SuspiciousOperationTestCase, TestFile)
{
	ASSERT_EQ(this->ConstCharConstructorError.file(), this->ErrorFile);
	ASSERT_EQ(this->StringConstructorError.file(), this->ErrorFile);
}

__UNIT_TESTS_END__


#endif // WASP_UNIT_TESTS_CORE_TESTS_EXCEPTIONS_TESTS_SUSPICIOUS_OPERATION_TESTS_H
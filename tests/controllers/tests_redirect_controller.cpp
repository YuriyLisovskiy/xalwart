/**
 * controllers/tests_redirect_controller.cpp
 *
 * Copyright (c) 2019 Yuriy Lisovskiy
 */

#include <map>

#include <gtest/gtest.h>

#include <xalwart.base/logger.h>
#include <xalwart.base/collections/dict.h>
#include <xalwart.base/collections/multi_dict.h>

#include "../../src/controllers/redirect.h"
#include "../../src/conf/settings.h"

using namespace xw;


http::HttpRequest make_request(const conf::Settings* settings, const std::string& method)
{
	auto empty_parameters = http::HttpRequest::Parameters<std::string, std::string>(
		collections::Dict<std::string, std::string>(),
		collections::MultiValueDict<std::string, std::string>()
	);

	auto empty_map = collections::Dict<std::string, std::string>();
	return http::HttpRequest(
		settings,
		method,
		"/hello",
		1, 1,
		"",
		true,
		"",
		empty_map,
		empty_parameters,
		empty_parameters,
		http::HttpRequest::Parameters<std::string, files::UploadedFile>(
			collections::Dict<std::string, files::UploadedFile>(),
			collections::MultiValueDict<std::string, files::UploadedFile>()
		),
		{}
	);
}

struct RedirectControllerTestSettings : public conf::Settings
{
	RedirectControllerTestSettings() : conf::Settings("./")
	{
		auto lc = log::Config();
		lc.enable_info = false;
		lc.enable_debug = false;
		lc.enable_warning = false;
		lc.enable_error = false;
		lc.enable_fatal = false;
		lc.enable_print = false;
		this->LOGGER = std::make_shared<log::Logger>(lc);
	}
};


class RedirectControllerWithDefaultParamsTestCase : public ::testing::Test
{
protected:
	ctrl::RedirectController* controller = nullptr;
	RedirectControllerTestSettings* settings = nullptr;

	void SetUp() override
	{
		this->settings = new RedirectControllerTestSettings();
		this->controller = new ctrl::RedirectController(this->settings, "/hello");
	}

	void TearDown() override
	{
		delete this->settings;
		delete this->controller;
	}
};

TEST_F(RedirectControllerWithDefaultParamsTestCase, GetTest)
{
	auto request = make_request(this->settings, "get");
	this->controller->setup(&request);
	auto response = this->controller->get(nullptr);

	ASSERT_TRUE(dynamic_cast<http::HttpResponseRedirect*>(response.value.get()));
}

TEST_F(RedirectControllerWithDefaultParamsTestCase, PostTest)
{
	auto request = make_request(this->settings, "post");
	this->controller->setup(&request);
	auto response = this->controller->post(nullptr);

	ASSERT_TRUE(dynamic_cast<http::HttpResponseRedirect*>(response.value.get()));
}

TEST_F(RedirectControllerWithDefaultParamsTestCase, PutTest)
{
	auto request = make_request(this->settings, "put");
	this->controller->setup(&request);
	auto response = this->controller->put(nullptr);

	ASSERT_TRUE(dynamic_cast<http::HttpResponseRedirect*>(response.value.get()));
}

TEST_F(RedirectControllerWithDefaultParamsTestCase, PatchTest)
{
	auto request = make_request(this->settings, "patch");
	this->controller->setup(&request);
	auto response = this->controller->patch(nullptr);

	ASSERT_TRUE(dynamic_cast<http::HttpResponseRedirect*>(response.value.get()));
}

TEST_F(RedirectControllerWithDefaultParamsTestCase, DeleteTest)
{
	auto request = make_request(this->settings, "delete");
	this->controller->setup(&request);
	auto response = this->controller->delete_(nullptr);

	ASSERT_TRUE(dynamic_cast<http::HttpResponseRedirect*>(response.value.get()));
}

TEST_F(RedirectControllerWithDefaultParamsTestCase, HeadTest)
{
	auto request = make_request(this->settings, "head");
	this->controller->setup(&request);
	auto response = this->controller->head(nullptr);

	ASSERT_TRUE(dynamic_cast<http::HttpResponseRedirect*>(response.value.get()));
}

TEST_F(RedirectControllerWithDefaultParamsTestCase, OptionsTest)
{
	auto request = make_request(this->settings, "options");
	this->controller->setup(&request);
	auto response = this->controller->options(nullptr);

	ASSERT_TRUE(dynamic_cast<http::HttpResponseRedirect*>(response.value.get()));
}

TEST_F(RedirectControllerWithDefaultParamsTestCase, TraceTest)
{
	auto request = make_request(this->settings, "trace");
	auto response = this->controller->trace(nullptr);

	ASSERT_EQ(response.value, nullptr);
}

TEST_F(RedirectControllerWithDefaultParamsTestCase, AllowedMethodsTest)
{
	auto expected = std::vector<std::string>{"GET", "POST", "PUT", "PATCH", "DELETE", "HEAD", "OPTIONS"};
	auto actual = this->controller->allowed_methods();

	ASSERT_EQ(expected.size(), actual.size());

	for (size_t i = 0; i < expected.size(); i++)
	{
		ASSERT_EQ(expected[i], actual[i]);
	}
}

TEST_F(RedirectControllerWithDefaultParamsTestCase, SetupAndDispatchAllowedTest)
{
	auto request = make_request(this->settings, "get");

	ASSERT_THROW(this->controller->dispatch(nullptr), NullPointerException);

	this->controller->setup(&request);
	auto response = this->controller->dispatch(nullptr);

	ASSERT_EQ(response.value->status(), 302);
}


TEST_F(RedirectControllerWithDefaultParamsTestCase, DispatchTraceNotAllowedTest)
{
	auto request = make_request(this->settings, "trace");
	this->controller->setup(&request);

	auto response = this->controller->dispatch(nullptr);

	ASSERT_EQ(response.value->status(), 405);
}

TEST_F(RedirectControllerWithDefaultParamsTestCase, GetRedirectUrlTest)
{
	auto request = make_request(this->settings, "get");
	this->controller->setup(&request);
	ASSERT_EQ(this->controller->get_redirect_url(), "/hello");
}


class RedirectControllerPermanentAndQueryStringTestCase : public ::testing::Test
{
protected:
	ctrl::RedirectController* controller = nullptr;
	RedirectControllerTestSettings* settings = nullptr;

	void SetUp() override
	{
		this->settings = new RedirectControllerTestSettings();
		this->controller = new ctrl::RedirectController(
			this->settings, "/hello/world", true, true
		);
	}

	void TearDown() override
	{
		delete this->settings;
		delete this->controller;
	}
};

TEST_F(RedirectControllerPermanentAndQueryStringTestCase, GetTest)
{
	auto request = make_request(this->settings, "get");
	this->controller->setup(&request);
	auto response = this->controller->get(nullptr);

	ASSERT_TRUE(dynamic_cast<http::HttpResponsePermanentRedirect*>(response.value.get()));
}

TEST_F(RedirectControllerPermanentAndQueryStringTestCase, PostTest)
{
	auto request = make_request(this->settings, "post");
	this->controller->setup(&request);
	auto response = this->controller->post(nullptr);

	ASSERT_TRUE(dynamic_cast<http::HttpResponsePermanentRedirect*>(response.value.get()));
}

TEST_F(RedirectControllerPermanentAndQueryStringTestCase, PutTest)
{
	auto request = make_request(this->settings, "put");
	this->controller->setup(&request);
	auto response = this->controller->put(nullptr);

	ASSERT_TRUE(dynamic_cast<http::HttpResponsePermanentRedirect*>(response.value.get()));
}

TEST_F(RedirectControllerPermanentAndQueryStringTestCase, PatchTest)
{
	auto request = make_request(this->settings, "patch");
	this->controller->setup(&request);
	auto response = this->controller->patch(nullptr);

	ASSERT_TRUE(dynamic_cast<http::HttpResponsePermanentRedirect*>(response.value.get()));
}

TEST_F(RedirectControllerPermanentAndQueryStringTestCase, DeleteTest)
{
	auto request = make_request(this->settings, "delete");
	this->controller->setup(&request);
	auto response = this->controller->delete_(nullptr);

	ASSERT_TRUE(dynamic_cast<http::HttpResponsePermanentRedirect*>(response.value.get()));
}

TEST_F(RedirectControllerPermanentAndQueryStringTestCase, HeadTest)
{
	auto request = make_request(this->settings, "head");
	this->controller->setup(&request);
	auto response = this->controller->head(nullptr);

	ASSERT_TRUE(dynamic_cast<http::HttpResponsePermanentRedirect*>(response.value.get()));
}

TEST_F(RedirectControllerPermanentAndQueryStringTestCase, OptionsTest)
{
	auto request = make_request(this->settings, "options");
	this->controller->setup(&request);
	auto response = this->controller->options(nullptr);

	ASSERT_TRUE(dynamic_cast<http::HttpResponsePermanentRedirect*>(response.value.get()));
}

TEST_F(RedirectControllerPermanentAndQueryStringTestCase, GetRedirectUrlTest)
{
	auto empty_parameters = http::HttpRequest::Parameters<std::string, std::string>(
		collections::Dict<std::string, std::string>(),
		collections::MultiValueDict<std::string, std::string>()
	);
	auto empty_map = collections::Dict<std::string, std::string>();
	auto* request = new http::HttpRequest(
		this->settings,
		"get",
		"/hello",
		1, 1,
		"param1=100&q=find_user",
		true,
		"",
		empty_map,
		empty_parameters,
		empty_parameters,
		http::HttpRequest::Parameters<std::string, files::UploadedFile>(
			collections::Dict<std::string, files::UploadedFile>(),
			collections::MultiValueDict<std::string, files::UploadedFile>()
		),
		{}
	);
	this->controller->setup(request);

	ASSERT_EQ(this->controller->get_redirect_url(), "/hello/world?param1=100&q=find_user");

	delete request;
}


class RedirectControllerEmptyUrlTestCase : public ::testing::Test
{
protected:
	ctrl::RedirectController* controller = nullptr;
	RedirectControllerTestSettings* settings = nullptr;

	void SetUp() override
	{
		this->settings = new RedirectControllerTestSettings();
		this->controller = new ctrl::RedirectController(this->settings, "");
	}

	void TearDown() override
	{
		delete this->settings;
		delete this->controller;
	}
};

TEST_F(RedirectControllerEmptyUrlTestCase, GetTest)
{
	auto request = make_request(this->settings, "get");
	this->controller->setup(&request);
	auto response = this->controller->get(nullptr);

	ASSERT_TRUE(dynamic_cast<http::HttpResponseGone*>(response.value.get()));
}

TEST_F(RedirectControllerEmptyUrlTestCase, PostTest)
{
	auto request = make_request(this->settings, "post");
	this->controller->setup(&request);
	auto response = this->controller->post(nullptr);

	ASSERT_TRUE(dynamic_cast<http::HttpResponseGone*>(response.value.get()));
}

TEST_F(RedirectControllerEmptyUrlTestCase, PutTest)
{
	auto request = make_request(this->settings, "put");
	this->controller->setup(&request);
	auto response = this->controller->put(nullptr);

	ASSERT_TRUE(dynamic_cast<http::HttpResponseGone*>(response.value.get()));
}

TEST_F(RedirectControllerEmptyUrlTestCase, PatchTest)
{
	auto request = make_request(this->settings, "patch");
	this->controller->setup(&request);
	auto response = this->controller->patch(nullptr);

	ASSERT_TRUE(dynamic_cast<http::HttpResponseGone*>(response.value.get()));
}

TEST_F(RedirectControllerEmptyUrlTestCase, DeleteTest)
{
	auto request = make_request(this->settings, "delete");
	this->controller->setup(&request);
	auto response = this->controller->delete_(nullptr);

	ASSERT_TRUE(dynamic_cast<http::HttpResponseGone*>(response.value.get()));
}

TEST_F(RedirectControllerEmptyUrlTestCase, HeadTest)
{
	auto request = make_request(this->settings, "head");
	this->controller->setup(&request);
	auto response = this->controller->head(nullptr);

	ASSERT_TRUE(dynamic_cast<http::HttpResponseGone*>(response.value.get()));
}

TEST_F(RedirectControllerEmptyUrlTestCase, OptionsTest)
{
	auto request = make_request(this->settings, "options");
	this->controller->setup(&request);
	auto response = this->controller->options(nullptr);

	ASSERT_TRUE(dynamic_cast<http::HttpResponseGone*>(response.value.get()));
}

TEST_F(RedirectControllerEmptyUrlTestCase, GetRedirectUrlTest)
{
	auto request = make_request(this->settings, "get");
	this->controller->setup(&request);

	ASSERT_EQ(this->controller->get_redirect_url(), "");
}
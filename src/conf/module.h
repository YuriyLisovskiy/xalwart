/**
 * conf/module.h
 *
 * Copyright (c) 2019-2021 Yuriy Lisovskiy
 *
 * Configuration of separate module with urls, commands and other
 * stuff.
 */

#pragma once

// Base libraries.
#include <xalwart.base/string_utils.h>

// Module definitions.
#include "./_def_.h"

// Framework libraries.
#include "./interfaces.h"
#include "../urls/pattern.h"
#include "../controllers/controller.h"
#include "../commands/command.h"


__CONF_BEGIN__

// TESTME: ModuleConfig
// TODO: docs for 'ModuleConfig'
// Derived class must contain constructor with pointer
// to 'conf::Settings' parameter.
class ModuleConfig : public IModuleConfig
{
private:
	bool _is_initialized;

	friend class conf::Settings;

	std::vector<std::shared_ptr<urls::IPattern>> _urlpatterns;
	std::vector<std::shared_ptr<cmd::AbstractCommand>> _commands;
	std::vector<std::function<void()>> _sub_modules_to_init;

	inline std::shared_ptr<IModuleConfig> _find_module(const std::string& module)
	{
		auto result = std::find_if(
			this->settings->MODULES.begin(), this->settings->MODULES.end(),
			[&module](const std::shared_ptr<IModuleConfig>& entry) -> bool {
				return entry->get_name() == module;
			}
		);
		if (result == this->settings->MODULES.end())
		{
			throw ImproperlyConfigured(
				"module is used but was not registered: " + module, _ERROR_DETAILS_
			);
		}

		return *result;
	}

protected:
	std::string module_name;
	conf::Settings* settings;

protected:
	inline explicit ModuleConfig(conf::Settings* settings) : _is_initialized(false), settings(settings)
	{
	}

	template <
		typename ControllerType, typename ...RequestArgs, typename ...ControllerArgs,
		typename = std::enable_if<std::is_base_of<ctrl::Controller<RequestArgs...>, ControllerType>::value>
	>
	inline void url(const std::string& pattern, const std::string& name, ControllerArgs ...controller_args)
	{
		ctrl::Handler<RequestArgs...> controller_handler = [controller_args...](
			http::Request* request,
			const std::tuple<RequestArgs...>& request_args,
			const conf::Settings* settings_pointer
		) -> std::unique_ptr<http::HttpResponse>
		{
			ControllerType controller(settings_pointer, controller_args...);
			return std::apply(
				[&controller, request](RequestArgs ...a) mutable -> auto
				{
					return controller.dispatch(request, a...);
				},
				request_args
			);
		};

		this->_urlpatterns.push_back(std::make_shared<urls::Pattern<RequestArgs...>>(
			pattern.starts_with("/") ? pattern : "/" + pattern,
			controller_handler,
			name.empty() ? demangle(typeid(ControllerType).name()) : name
		));
	}

	inline void include(const std::string& module, const std::string& prefix, const std::string& namespace_="")
	{
		this->_sub_modules_to_init.emplace_back([this, prefix, namespace_, module]() -> void {
			auto app = this->_find_module(module);
			auto included_urlpatterns = app->get_urlpatterns();
			auto ns = namespace_.empty() ? app->get_name() : namespace_;
			for (const auto& pattern : included_urlpatterns)
			{
				pattern->add_namespace(ns);
				pattern->add_prefix(str::rtrim(prefix.starts_with("/") ? prefix : "/" + prefix, "/"));
				this->_urlpatterns.push_back(pattern);
			}
		});
	}

	template <cmd::command_type CommandType, typename ...Args>
	inline void command(Args&& ...args)
	{
		this->_commands.push_back(std::make_shared<CommandType>(std::forward<Args>(args)...));
	}

	virtual inline void urlpatterns()
	{
	}

	virtual inline void commands()
	{
	}

public:
	[[nodiscard]]
	inline bool ready() const override
	{
		return this->_is_initialized;
	}

	[[nodiscard]]
	inline std::string get_name() const final
	{
		return this->module_name;
	}

	[[nodiscard]]
	std::vector<std::shared_ptr<urls::IPattern>> get_urlpatterns() final;

	[[nodiscard]]
	std::vector<std::shared_ptr<cmd::AbstractCommand>> get_commands() final;

	virtual void init(const std::string& name);
};

__CONF_END__

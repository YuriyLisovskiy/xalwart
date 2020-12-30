/**
 * commands/app_command.cpp
 *
 * Copyright (c) 2019-2020 Yuriy Lisovskiy
 */

#include "./app_command.h"


__COMMANDS_BEGIN__

AppCommand::AppCommand(
	apps::IAppConfig* app_cfg, conf::Settings* settings, const std::string& cmd_name, const std::string& help
) : BaseCommand(cmd_name, help)
{
	this->app_config = app_cfg;
	this->settings = settings;
}

__COMMANDS_END__

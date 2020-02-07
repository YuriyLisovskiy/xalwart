/*
 * Copyright (c) 2019-2020 Yuriy Lisovskiy
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

/**
 * An implementation of core/managements/commands/app_command.h
 */

#include "./app_command.h"


__CORE_COMMANDS_BEGIN__

AppCommand::AppCommand(
	apps::IAppConfig* app_cfg, conf::Settings* settings, const std::string& cmd_name, const std::string& help
) : Command(settings, cmd_name, help)
{
	this->app_config = app_cfg;
}

__CORE_COMMANDS_END__

/*
 * Copyright (c) 2020 Yuriy Lisovskiy
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
 * render/loader.h
 *
 * Purpose:
 * Searches for template using template name and a vector of
 * directories where given template can be located.
 */

#pragma once

// Module definitions.
#include "./_def_.h"

// Wasp libraries.
#include "./base.h"


__RENDER_BEGIN__

class Loader : public ILoader
{
public:
	ITemplate* get_template(
		const std::string& template_name,
		const std::vector<std::string>& dirs,
		BaseEngine* engine
	) override;
};

__RENDER_END__

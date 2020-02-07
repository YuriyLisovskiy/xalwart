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
 * core/datetime/predefined/tz_abbr_to_offset.h
 *
 * Purpose: offsets of some known time zones.
 */

#pragma once

// c++ libraries.
#include <map>

// Module definitions.
#include "../_def_.h"

// Wasp libraries.
#include "../../../collections/dict.h"


__DATETIME_INTERNAL_BEGIN__

extern collections::Dict<std::string, std::string> TZ_ABBR_TO_OFFSET;

__DATETIME_INTERNAL_END__

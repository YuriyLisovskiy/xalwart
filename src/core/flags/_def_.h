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
 * _def_.h
 * Purpose: core/flags module's definitions.
 */

#pragma once

#include "../_def_.h"


/// wasp::flags
#define __FLAGS_BEGIN__ __WASP_BEGIN__ namespace flags {
#define __FLAGS_END__ } __WASP_END__

/// wasp::flags::internal
#define __FLAGS_INTERNAL_BEGIN__ __FLAGS_BEGIN__ namespace internal {
#define __FLAGS_INTERNAL_END__ } __FLAGS_END__

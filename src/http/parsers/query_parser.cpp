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

#include "query_parser.h"


__INTERNAL_BEGIN__

query_parser::query_parser()
{
	this->dict = new Dict<std::string, std::string>(true);
	this->multi_dict = new MultiValueDict<std::string, std::string>(true);
}

query_parser::~query_parser()
{
	delete this->dict;
	delete this->multi_dict;
}

void query_parser::parse(const std::string& content)
{
	if (content.empty())
	{
		return;
	}

	auto begin = content.begin();
	auto end = content.end();
	std::string item_key, item_value;
	query_parser::state st = query_parser::state::s_key;
	while (begin != end)
	{
		char input = *begin++;
		switch (st)
		{
			case query_parser::state::s_key:
				if (input == '=')
				{
					st = query_parser::state::s_val;
				}
				else
				{
					item_key.push_back(input);
				}
				break;
			case query_parser::state::s_val:
				if (input == '&')
				{
					this->append_parameter(item_key, item_value);
					item_key.clear();
					item_value.clear();
					st = query_parser::state::s_key;
				}
				else
				{
					item_value.push_back(input);
				}
				break;
		}
	}

	this->append_parameter(item_key, item_value);
}

void query_parser::append_parameter(const std::string& key, const std::string& value)
{
	if (!this->dict->contains(key))
	{
		this->dict->set(key, value);
	}

	this->multi_dict->append(key, value);
}

__INTERNAL_END__
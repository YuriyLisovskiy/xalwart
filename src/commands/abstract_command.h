/**
 * commands/abstract_command.h
 *
 * Copyright (c) 2019-2021 Yuriy Lisovskiy
 *
 * The base class from which all commands ultimately derive.
 */

#pragma once

// C++ libraries.
#include <string>

// Base libraries.
#include <xalwart.base/options.h>

// Module definitions.
#include "./_def_.h"

// Framework libraries.
#include "./flags/flag_set.h"


__COMMANDS_BEGIN__

// TESTME: AbstractCommand
// Use this class if you want access to all of the mechanisms which
// parse the command-line arguments and work out what code to call in
// response; if you don't need to change any of that behavior,
// consider using one of the subclasses defined in this file.
class AbstractCommand
{
public:

	// Returns command flags.
	[[nodiscard]]
	virtual inline Options get_options() const
	{
		return {};
	}

	// Returns usage based on flag_set.
	std::string usage();

	// Returns help message for command.
	[[nodiscard]]
	inline std::string help_message() const
	{
		return this->help;
	}

	// Returns full command's name.
	[[nodiscard]]
	inline std::string name() const
	{
		return this->label;
	}

	void run_from_argv(int argc, char** argv, bool is_verbose = false);

protected:
	std::shared_ptr<flags::FlagSet> flag_set;
	bool is_created;

	// Index of item in argv to parse from.
	size_t parse_from;

	std::string help;
	std::string label;

	explicit AbstractCommand(const std::string& cmd_name, const std::string& help);

	// Creates flags if they are not created yet.
	void create_flags();

	// Override in child class to add more commands.
	virtual inline void add_flags()
	{
	}

	// Validates arguments before processing the command.
	virtual inline void validate() const
	{
	}

	// The actual logic of the command. Subclasses must implement
	// this method.
	virtual void handle() = 0;
};

template <typename T>
concept command_type = std::is_base_of_v<AbstractCommand, T>;

__COMMANDS_END__

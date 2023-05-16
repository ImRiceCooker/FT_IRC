#pragma once

typedef enum e_mode_type
{
	MODE_TYPE_ERR,
	PLUS_I,
	MINUS_I,
} t_mode_type;

typedef struct s_mode
{
	t_mode_type mode_type;
	std::string target;
	std::string option;
	std::string param;
} t_mode;

#pragma once

enum e_mode_val { N_MODE_TYPE = 10};

typedef enum e_mode_type
{
	PLUS_I,
	MINUS_I,
	PLUS_T,
	MINUS_T,
	PLUS_L,
	MINUS_L,
	PLUS_O,
	MINUS_O,
	PLUS_K,
	MINUS_K,
	MODE_TYPE_ERR
}	t_mode_type;

typedef enum e_mode_err_type
{
	CMD_EMPTY,
	CMD_TOO_MANY,
	WRONG_MSG
}	t_mode_err_type;


typedef struct s_mode
{
	t_mode_type mode_type;
	std::string target;
	std::string option;
	std::string param;
} t_mode;

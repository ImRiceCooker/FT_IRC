#pragma once

#include "Udata.hpp"
#include "User.hpp"
#include "Color.hpp"

class Sender
{
public:
	static Event mode_wrong_message(const uintptr_t &ident, const char &mode_option);
	static Event pong(const uintptr_t &socket, const std::string &msg); //@Todo: pong_message
	static Event command_no_origin_specified_409(const User &sender, const std::string &command);
	static Event command_not_registered_451(const User &sender, const std::string &command);
	static Event command_not_registered_451(const uintptr_t &sock, const std::string &command);
	static Event command_empty_argument_461(const User &sender, const std::string &command);
	static Event command_empty_argument_461(const uintptr_t &sock, const std::string &command);
	static Event welcome_message_connect(const User &sender);
	static Event user_302_message(const User &sender);
	static Event nick_well_message(const User &sender, const User &receiver, const std::string &new_nick);
	static Event nick_error_message(const User &sender, const std::string &new_nick);
	static Event nick_error_message2(const User &sender, const std::string &new_nick); //@Todo: nick_error_message_steal
	static Event nick_error_message(const uintptr_t &sock, const std::string &new_nick);
	static Event nick_wrong_message(const User &sender, const std::string &new_nick);
	static Event nick_wrong_message(const uintptr_t &sock, const std::string &new_nick);
	static Event password_incorrect_464(const uintptr_t &sock);
	static Event quit_leaver_message(const User &sender, std::string leave_message);
	static Event quit_member_message(const User &sender, const User &receiver, std::string leave_message);
	static Event privmsg_bot_message(const User &sender, const std::string &msg);
	static Event privmsg_p2p_message(const User &sender, const User &target, const std::string &msg);
	static Event privmsg_channel_message(const User &sender, const User &receiver, const std::string &channel, const std::string &msg);
	static Event privmsg_no_user_error_message(const User &sender, const std::string &target);

	static Event join_message(const User &sender, const User &receiver, const std::string &channel);
	static Event join_invalid_channel_name_message(const User &sender, const std::string invaild_channel);
	static Event part_message(const User &sender, const User &receiver, const std::string &channel, const std::string &msg);
	static Event kick_message(const User &sender, const User &receiver, const std::string &subject, const std::string &channel, const std::string &msg);
	static Event kick_error_not_op_message(const User &sender, const std::string &host, const std::string &channel);
	static Event kick_error_no_user_message(const User &sender, const std::string &host, const std::string &target, const std::string &channel);
	static Event topic_message(const User &sender, const User &receiver, const std::string &channel, const std::string &topic);
	static Event topic_access_error(const User &sender, const std::string &channel);

	static Event notice_p2p_message(const User &sender, const User &target, const std::string &msg);
	static Event notice_channel_message(const User &sender, const User &receiver, const std::string &channel, const std::string &msg);
	static Event notice_no_nick_message(const User &sender, const User &receiver);
	static Event no_channel_message(const User &sender, const std::string &channel);
	static Event no_user_message(const User &sender, const std::string &target);
	static Event mode_error_not_op_message(const User &sender, const std::string &channel);
	static Event command_too_many_argument_461(const uintptr_t &sock, const std::string &command);
	static Event mode_no_user_message(const User &sender, const std::string &target);
	static Event mode_syntax_error(const User &sender, const std::string &target, const std::string &mode_option, const std::string &description, const std::string &err_syntax);
	static Event mode_syntax_error_l_negative_num(const User &sender, const std::string &target, const std::string &mode_option, const std::string &mode_param);

	static std::string join_353_message(const User &sender, const std::string &chan_name, const std::string &chan_status, const std::string &chan_user_list);
	static std::string join_366_message(const User &sender, const std::string &chan_name);
	static Event mode_message(const User &sender, const User &receiver, const std::string &channel, const std::string &mode_type);

	static Event already_in_channel_message(const User &target_user, const std::string &channel);
	static Event invite_message(const User &sender, const User &receiver, const std::string &channel);
	static Event invitee_message(const User &sender, const User &receiver, const std::string &channel);
	static Event invite_no_user_message(const User &sender, const std::string &target_user);
	static Event invitor_message(const User &invitor, const User &target_user, const std::string &channel);

	static Event cannot_join_message(const User &receiver, const std::string &channel);
	static Event cannot_join_message_key(const User &receiver, const std::string &channel);
	static Event cannot_join_message_limit(const User &receiver, const std::string &channel);

private:
	static const std::string server_name_;
};

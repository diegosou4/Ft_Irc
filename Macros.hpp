/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Macros.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/27 12:11:32 by cbouvet           #+#    #+#             */
/*   Updated: 2025/06/27 12:13:04 by cbouvet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

//General:
#ifndef NULL
# define NULL		0
#endif
//Socket setup related:
#define LOCALHOST	"127.0.0.1"
#define BUFFSIZE	1000
#define REMOVAL		-2
//Colours:
#define PURPLE	"\001\033[1;38;2;209;174;231m\002"
#define GREY	"\001\033[1;37m\002"
#define RED		"\001\033[1;31m\002"
#define R		"\001\033[1;00m\002"
//Output messages:
#define ONLINE_OPTS		"\n\n1 - Log in    |    2 - Register"
#define INSTRUCTIONS	"Please enter using PASS"
#define PASS_EXPECT		"Expected: PASS <password>"
#define NICK_EXPECT		"Expected: NICK <nickname>"
#define USER_EXPECT		"Expected: USER <username> <hostname> <servername> :<realname>"
#define JOIN_EXPECT		"Expected: JOIN <#channelname>"

#define RPL_WELCOME		001
#define WELCOME \
"\n          ▗▄▄▄▖▗▄▄▖  ▗▄▄▖           \n\
            █  ▐▌ ▐▌▐▌              \n\
            █  ▐▛▀▚▖▐▌              \n\
          ▗▄█▄▖▐▌ ▐▌▝▚▄▄▖           \n\
                                    \n\
▗▖ ▗▖▗▄▄▄▖▗▖    ▗▄▄▖ ▗▄▖ ▗▖  ▗▖▗▄▄▄▖\n\
▐▌ ▐▌▐▌   ▐▌   ▐▌   ▐▌ ▐▌▐▛▚▞▜▌▐▌   \n\
▐▌ ▐▌▐▛▀▀▘▐▌   ▐▌   ▐▌ ▐▌▐▌  ▐▌▐▛▀▀▘\n\
▐▙█▟▌▐▙▄▄▖▐▙▄▄▖▝▚▄▄▖▝▚▄▞▘▐▌  ▐▌▐▙▄▄▖\n\
                                    \n"
#define	RPL_CHANMODE	324 //when MODE is required
#define RPL_CREATTIME	329 // sent with MODE or INFO
#define	RPL_NOTOPIC		331	//sent after JOIN if no topic
#define	RPL_TOPIC		332 //sent after JOIN if topic
#define	RPL_INVITING	341	//after successfully invite
#define RPL_NAMREPLY	353 //after JOIN/NAMES command
#define RPL_ENDOFNAMES	366 //after 353

#define ERR_NOSUCHNICK		401 // user/channel doesn't exist
#define ERR_NOSUCHCHAN		403 //channel doesn't exist - JOIN/MODE/NICK
#define ERR_CANTSENDTOCHAN	404	//privmsg to channel failed

#define	ERR_NONICK			431 //NICK command used w/ nick
#define ERR_INVALIDNICK		432	//NICK - invalid chars
#define ERR_NICKINUSE		433 //NICK in use

#define	ERR_USERNOTINCHAN	441	//KICK a non-member
#define	ERR_NOTINCHAN		442 //PRIVMSG/LEAVE when ur not in
#define	ERR_USERINCHAN		443 //INVITE someone already in

#define	ERR_NEEDMOREPARAMS	461	//not enough params with cmd
#define	ERR_ALREADYAUTHED	462 //PASS/NICK/USER once already ACTIVE
#define	ERR_WRONGPASS		464	//wrong password

#define ERR_KEYSET			467 // channel already has password set
#define	ERR_CHANISFULL		471	//JOIN/INVITE when channel is full
#define	ERR_UNKNOWNMODE		472	//invalid MODE flag
#define	ERR_INVITEONLYCHAN	473 // try to join invite only channel
#define	ERR_BADCHANKEY		475	//invalid chan pass
#define	ERR_NOTCHANOP		482	// MODE/KICK by a non-op

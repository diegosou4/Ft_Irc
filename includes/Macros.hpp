/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Macros.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/27 12:11:32 by cbouvet           #+#    #+#             */
/*   Updated: 2025/07/05 16:42:39 by cbouvet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <map>
#include <iostream>

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

#define DIGIT_CHARS	"01234567789"
#define ALPHA_CHARS "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
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


#define SUCCESS			000
#define RPL_WELCOME		001

#define	RPL_CHANMODE	324
#define RPL_CREATTIME	329
#define	RPL_NOTOPIC		331
#define	RPL_TOPIC		332
#define	RPL_INVITING	341
#define RPL_NAMREPLY	353
#define RPL_ENDOFNAMES	366

#define ERR_NOSUCHNICK		401
#define ERR_NOSUCHCHAN		403
#define ERR_CANTSENDTOCHAN	404

#define ERR_UNKNOWNCOMMAND	421

#define	ERR_NONICK			431
#define ERR_INVALIDNICK		432
#define ERR_NICKINUSE		433

#define	ERR_USERNOTINCHAN	441
#define	ERR_NOTINCHAN		442
#define	ERR_USERINCHAN		443

#define	ERR_NOTAUTHED		451

#define	ERR_NEEDMOREPARAMS	461
#define	ERR_ALREADYAUTHED	462
#define	ERR_WRONGPASS		464

#define ERR_KEYSET			467
#define	ERR_CHANISFULL		471
#define	ERR_UNKNOWNMODE		472
#define	ERR_INVITEONLYCHAN	473
#define	ERR_BADCHANKEY		475
#define	ERR_NOTCHANOP		482

static std::map<int, std::string> create_errmsg()
{
	std::map<int, std::string> errmap;
	errmap[401] = ":No such nick/channel";
	errmap[403] = ":No such channel";
	errmap[404] = ":Cannot send to channel";
	errmap[421] = ":Unknown command";
	errmap[431] = ":No nickname given";
	errmap[432] = ":Erroneous nickname";
	errmap[433] = ":Nickname is already in use";
	errmap[441] = ":They aren't on that channel";
	errmap[442] = ":You're not on that channel";
	errmap[443] = ":is already on channel";
	errmap[451] = ":You have not registered";
	errmap[461] = ":Not enough parameters";
	errmap[462] = ":You may not reregister";
	errmap[464] = ":Password incorrect";
	errmap[467] = ":Channel key already set";
	errmap[471] = ":Cannot join channel (+l)";
	errmap[472] = ":is unknown mode char to me";
	errmap[473] = ":Cannot join channel (+i)";
	errmap[475] = ":Cannot join channel (+k)";
	errmap[482] = ":You're not channel operator";

	return (errmap);
}

static const std::map<int, std::string> ErrMsg = create_errmsg();

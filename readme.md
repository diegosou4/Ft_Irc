RUN:
valgrind --track-fds=yes ./ircserv 6667 5555

Checklist:
| Category       | Command                            | Expected Behavior                   | Notes                             | State|
|:---------------|:-----------------------------------|:------------------------------------|:----------------------------------|:-----|
| Authentication | NICK Alice                         | Sets nickname                       | Store nickname                    |OK|
| Authentication | USER alice 0 * :Alice              | Sets user info                      | Required before joining           |OK|
| Authentication | NICK                               | ERR_NONICKNAMEGIVEN                 | Missing nick                      |OK|
| Authentication | USER                               | ERR_NEEDMOREPARAMS                  | Incomplete USER cmd               |OK|
| Authentication | NICK Bob                           | ERR_NICKNAMEINUSE                   | Collision check                   |OK|
| JOIN/Channels  | JOIN #test                         | Joins or creates #test              | Must reply with RPL_TOPIC + NAMES |OK|
| JOIN/Channels  | JOIN 0                             | Leaves all channels                 | Clears user’s channels;           |XX|
| JOIN/Channels  | JOIN                               | ERR_NEEDMOREPARAMS                  | Missing channel name              |OK|
| JOIN/Channels  | JOIN #wrong pass                   | ERR_BADCHANKEY                      | Must reject                       |OK|
| JOIN/Channels  | JOIN #chan key                     | Joins with password                 | If mode +k is active              |OK|
| JOIN/Channels  | JOIN #chan                         | ERR_CHANNELISFULL                   | If +l active and full             |??|
| PART/QUIT      | PART #test                         | User leaves #test                   | Notify others                     |OK|
| PART/QUIT      | PART                               | ERR_NEEDMOREPARAMS                  | No channel provided               |OK|
| PART/QUIT      | QUIT :Bye                          | Disconnects user, broadcasts QUIT   | Should part from all channels     |XX|
| PART/QUIT      | QUIT                               | Graceful quit                       | Even without reason               |OK|
| Messaging      | PRIVMSG #test :hello               | Message sent to all in #test        | Only if user in channel           |OK|
| Messaging      | PRIVMSG                            | ERR_NORECIPIENT                     | Missing target                    |XX|
| Messaging      | PRIVMSG Alice                      | ERR_NOSUCHNICK                      | Target check                      |OK|
| Messaging      | NOTICE                             | Same validation, no replies         | Not implemented                   |XX|
| MODE           | MODE #chan                         | Returns current modes               | Includes creation time            |OK|
| MODE           | MODE #chan +i                      | Sets invite-only                    | Only operator can                 |OK|
| MODE           | MODE #chan +l 10                   | Set limit                           | Cap to 50 if implemented          |XX|
| MODE           | MODE #chan -k                      | Remove key                          |                                   |OK|
| MODE           | MODE #chan +o Bob                  | Make Bob operator                   | Bob must be in channel            |XX|
| MODE           | MODE #chan -o Bob                  | Remove op status                    |                                   |??|
| MODE           | MODE #chan +z                      | ERR_UNKNOWNMODE                     | Invalid mode                      |OK|
| MODE           | MODE #chan +i+k                    | Accepts combined flags              | Correct parsing                   |OK|
| MODE           | MODE #chan ++o                     | ERR_UNKNOWNCOMMAND                  | Catch malformed signs             |OK|
| TOPIC/INVITE   | TOPIC #chan :New Topic             | Sets topic                          | Only op if +t active              |OK|
| TOPIC/INVITE   | TOPIC #chan                        | Returns topic                       | If none, empty                    |OK|
| TOPIC/INVITE   | INVITE Bob #chan                   | Invite sent                         | Only if user is op                |OK|
| TOPIC/INVITE   | JOIN #chan                         | Must be invited                     | If +i active                      |OK|
| NAMES          | NAMES                              | Lists all visible users in channels | One by one                        |OK|
| NAMES          | NAMES #chan                        | RPL_NAMREPLY + RPL_ENDOFNAMES       | Shows users in that channel       |OK|
| Edge Cases     | Change nick after joining          | Updates everywhere                  | Including modes, name lists       |XX|
| Edge Cases     | MODE #chan +o Bob (Bob already op) | No duplicate error                  | No reply                          |XX|
| Edge Cases     | MODE #chan -o Bob (Bob not op)     | Silent                              | Valid behavior                    |XX|
| Edge Cases     | Channel auto-destroy when empty    | Channel is removed                  | If last user parts                |OK|
| Edge Cases     | Join channel with invalid name     | ERR_NOSUCHCHANNEL or custom         | Validate against '#'              |OK|
| Edge Cases     | JOIN #a\r\nJOIN #b                 | Both handled                        | Your line splitting logic         |XX|

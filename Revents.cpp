/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Revents.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/20 12:41:33 by cbouvet           #+#    #+#             */
/*   Updated: 2025/06/29 23:37:37 by cbouvet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//----------------------------Poll/revents-------------------------------------
#include "Server.hpp"

// Loops around all pollfds for revent activity - if found, sends to relevant event-managing method

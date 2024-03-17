/*
 * std/set.h
 *
 *  Created on: 17 Oct 2022
 *      Author: Nick Pelling
 */

#ifndef STD_SET_H_
#define STD_SET_H_

enum
{
	std_set_implements =
		std_container_implements_name
};

#define STD_SET_JUMPTABLE \
		.pachContainerName = "set"

#endif /* STD_SET_H_ */

/*
 * std/priority_queue.h
 *
 *  Created on: 17 Oct 2022
 *      Author: Nick Pelling
 */

#ifndef STD_PRIORITY_QUEUE_H_
#define STD_PRIORITY_QUEUE_H_

enum
{
	std_priorityqueue_implements =
		std_container_implements_name
};

#define STD_PRIORITYQUEUE_JUMPTABLE \
		.pachContainerName = "priority queue"

#endif /* STD_PRIORITY_QUEUE_H_ */

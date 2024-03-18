#ifndef _STD_ENUMS_H_
#define _STD_ENUMS_H_

typedef enum
{
	std_container_implements_name = 1 << 0,
	std_container_implements_construct = 1 << 1,
	std_container_implements_push_front = 1 << 2,
	std_container_implements_push_back = 1 << 3,
	std_container_implements_push = 1 << 4,
	std_container_implements_pop_front = 1 << 5,
	std_container_implements_pop_back = 1 << 6,
	std_container_implements_pop = 1 << 7,
	std_container_implements_at = 1 << 8,
	std_container_implements_empty = 1 << 9,
	std_container_implements_reserve = 1 << 10,
	std_container_implements_fit = 1 << 11,
	std_container_implements_ranged_sort = 1 << 12,
	std_container_implements_forward_construct = 1 << 13,
	std_container_implements_forward_next = 1 << 14,
	std_container_implements_forward_prev = 1 << 15,
	std_container_implements_forward_range = 1 << 16,
	std_container_implements_reverse_construct = 1 << 17,
	std_container_implements_reverse_next = 1 << 18,
	std_container_implements_reverse_prev = 1 << 19,
	std_container_implements_reverse_range = 1 << 20,

	std_container_implements_pushpop_front = std_container_implements_push_front | std_container_implements_pop_front,
	std_container_implements_pushpop_back = std_container_implements_push_back | std_container_implements_pop_back,
	std_container_implements_pushpop = std_container_implements_push | std_container_implements_pop,
	std_container_implements_forward_constructnextprev = std_container_implements_forward_construct | std_container_implements_forward_next | std_container_implements_forward_prev,
	std_container_implements_reverse_constructnextprev = std_container_implements_reverse_construct | std_container_implements_reverse_next | std_container_implements_reverse_prev,

} std_container_implements_t;

typedef enum
{
	std_container_has_no_handlers,
	std_container_has_itemhandler,
	std_container_has_memoryhandler,
	std_container_has_itemhandler_memoryhandler,
	std_container_has_lockhandler,
	std_container_has_lockhandler_itemhandler,
	std_container_has_lockhandler_memoryhandler,
	std_container_has_lockhandler_itemhandler_memoryhandler,

	std_container_has_MAX
} std_container_has_t;

#endif /* _STD_ENUMS_H_ */

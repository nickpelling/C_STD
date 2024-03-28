#ifndef _STD_ENUMS_H_
#define _STD_ENUMS_H_

#include <stdint.h>		// for uint8_t
#include <stdbool.h>	// for true/false

#define STD_XMACRO_ENUM(ENUM)				ENUM,
#define STD_XMACRO_ENUM_TO_TYPE(ENUM)		typedef uint8_t (*ENUM##_t)[1U + (ENUM)];

#define STD_TYPE_MAKE_PREFIX(PREFIX, VALUE)	typedef uint8_t (*PREFIX##_##VALUE##_t)[1U + (VALUE)]

// -----------------------------------------------------------------

STD_TYPE_MAKE_PREFIX(std_bool, false);
STD_TYPE_MAKE_PREFIX(std_bool, true);
#define std_bool_0_t	std_bool_false_t
#define std_bool_1_t	std_bool_true_t
#define STD_ENUM_BOOL_SET(BOOL)		std_bool_##BOOL##_t
#define STD_ENUM_BOOL_GET(FIELD)	((bool)(sizeof(*FIELD) - 1U))

// -----------------------------------------------------------------

typedef enum
{
	std_container_implements_name = 1 << 0,
	std_container_implements_construct = 1 << 1,
	std_container_implements_destruct = 1 << 2,
	std_container_implements_push_front = 1 << 3,
	std_container_implements_push_back = 1 << 4,
	std_container_implements_push = 1 << 5,
	std_container_implements_pop_front = 1 << 6,
	std_container_implements_pop_back = 1 << 7,
	std_container_implements_pop = 1 << 8,
	std_container_implements_at = 1 << 9,
	std_container_implements_reserve = 1 << 10,
	std_container_implements_fit = 1 << 11,
	std_container_implements_ranged_sort = 1 << 12,
	std_container_implements_forward_construct = 1 << 13,
	std_container_implements_forward_next = 1 << 14,
	std_container_implements_forward_prev = 1 << 15,
	std_container_implements_forward_range = 1 << 16,
	std_container_implements_forward_erase = 1 << 17,
	std_container_implements_forward_insert_after = 1 << 18,
	std_container_implements_forward_insert_before = 1 << 19,
	std_container_implements_reverse_construct = 1 << 20,
	std_container_implements_reverse_next = 1 << 21,
	std_container_implements_reverse_prev = 1 << 22,
	std_container_implements_reverse_range = 1 << 23,
	std_container_implements_reverse_erase = 1 << 24,
	std_container_implements_reverse_insert_after = 1 << 25,
	std_container_implements_reverse_insert_before = 1 << 26,
	std_container_implements_default_itemhandler = 1 << 27,

	std_container_implements_pushpop_front = std_container_implements_push_front | std_container_implements_pop_front,
	std_container_implements_pushpop_back = std_container_implements_push_back | std_container_implements_pop_back,
	std_container_implements_pushpop = std_container_implements_push | std_container_implements_pop,
	std_container_implements_forward_constructnextprev = std_container_implements_forward_construct | std_container_implements_forward_next | std_container_implements_forward_prev,
	std_container_implements_reverse_constructnextprev = std_container_implements_reverse_construct | std_container_implements_reverse_next | std_container_implements_reverse_prev,

} std_container_implements_t;

// -----------------------------------------------------------------

#define STD_CONTAINER_XMACRO(X)				\
	X(std_container_enum_deque)				\
	X(std_container_enum_list)				\
	X(std_container_enum_priority_queue)	\
	X(std_container_enum_queue)				\
	X(std_container_enum_stack)				\
	X(std_container_enum_vector)			\
	X(std_container_enum_bitarray)			\
											\
	X(std_container_enum_set)				\
	X(std_container_enum_unordered_set)		\
	X(std_container_enum_multiset)			\
	X(std_container_enum_map)				\
	X(std_container_enum_unordered_map)		\
											\
	X(std_container_enum_pool)				\
	X(std_container_enum_ring)				\
	X(std_container_enum_heap)				\
	X(std_container_enum_graph)

typedef enum
{
	STD_CONTAINER_XMACRO(STD_XMACRO_ENUM)

	std_container_enum_MAX
} std_container_enum_t;

STD_CONTAINER_XMACRO(STD_XMACRO_ENUM_TO_TYPE)

#define STD_ENUM_CONTAINER_SET(ENUM)	ENUM##_t
#define STD_ENUM_CONTAINER_GET(FIELD)	((std_container_enum_t)(sizeof(*FIELD) - 1U))

// -----------------------------------------------------------------

#define STD_HAS_XMACRO(X)							\
	X(std_container_has_no_handlers)				\
	X(std_container_has_itemhandler)				\
	X(std_container_has_memoryhandler)				\
	X(std_container_has_memoryhandler_itemhandler)	\
	X(std_container_has_lockhandler)				\
	X(std_container_has_lockhandler_itemhandler)	\
	X(std_container_has_lockhandler_memoryhandler)	\
	X(std_container_has_lockhandler_memoryhandler_itemhandler)

typedef enum
{
	STD_HAS_XMACRO(STD_XMACRO_ENUM)

	std_container_has_MAX
} std_container_has_t;

STD_HAS_XMACRO(STD_XMACRO_ENUM_TO_TYPE)

#define STD_ENUM_HAS_SET(ENUM)			ENUM##_t
#define STD_ENUM_HAS_GET(FIELD)			((std_container_has_t)(sizeof(*FIELD) - 1U))

// -----------------------------------------------------------------

#define STD_ITERATOR_XMACRO(X)			\
	X(std_iterator_enum_forward)		\
	X(std_iterator_enum_reverse)

typedef enum
{
	STD_ITERATOR_XMACRO(STD_XMACRO_ENUM)

	std_iterator_enum_MAX
} std_iterator_enum_t;

STD_ITERATOR_XMACRO(STD_XMACRO_ENUM_TO_TYPE)

#define STD_ENUM_ITERATOR_SET(ENUM)		ENUM##_t
#define STD_ENUM_ITERATOR_GET(FIELD)	((std_iterator_enum_t)(sizeof(*FIELD) - 1U))

// -----------------------------------------------------------------

#endif /* _STD_ENUMS_H_ */

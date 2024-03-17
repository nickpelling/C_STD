/*
 * std/typelist.h
 *
 *  Created on: 3 Nov 2022
 *      Author: Nick Pelling
 */

#ifndef STD_TYPELIST_H_
#define STD_TYPELIST_H_

#define TYPELIST_LEN(TYPELIST)	sizeof(((TYPELIST *)0U)->achLen)

#define TYPELIST_BASE(TYPE)	\
	struct	\
	{	\
		char achLen[1U];	\
		void * pstParent;	\
		TYPE * pstType;	\
	}

#define TYPELIST_APPEND(PARENT, TYPE)	\
	struct	\
	{	\
		char achLen[TYPELIST_LEN(PARENT) + 1U];	\
		PARENT * pstParent;	\
		TYPE * pstType;	\
	}

#define TYPELIST_REMOVELAST(TYPELIST)	STD_TYPEOF(((TYPELIST *)0U)->pstParent[0])

#define TYPELIST_1(T1,...)				TYPELIST_BASE(T1)
#define TYPELIST_2(T1,T2,...)			TYPELIST_APPEND(TYPELIST_BASE(T2), T1)
#define TYPELIST_3(T1,T2,T3,...)		TYPELIST_APPEND(TYPELIST_APPEND(TYPELIST_BASE(T3), T2), T1)
#define TYPELIST_4(T1,T2,T3,T4,...)		TYPELIST_APPEND(TYPELIST_APPEND(TYPELIST_APPEND(TYPELIST_BASE(T4), T3), T2), T1)
#define TYPELIST_5(T1,T2,T3,T4,T5,...)	TYPELIST_APPEND(TYPELIST_APPEND(TYPELIST_APPEND(TYPELIST_APPEND(TYPELIST_BASE(T5), T4), T3), T2), T1)
#define TYPELIST_SELECT(A,B,C,D,E,F,...)	TYPELIST_##F(A,B,C,D,E,F)
#define TYPELIST(...)	TYPELIST_SELECT(__VA_ARGS__,5,4,3,2,1)

#define TYPELIST_TYPE1(TYPELIST)	STD_TYPEOF(((TYPELIST *)0U)->pstType[0])
#define TYPELIST_TYPE2(TYPELIST)	STD_TYPEOF(((TYPELIST *)0U)->pstParent->pstType[0])
#define TYPELIST_TYPE3(TYPELIST)	STD_TYPEOF(((TYPELIST *)0U)->pstParent->pstParent->pstType[0])
#define TYPELIST_TYPE4(TYPELIST)	STD_TYPEOF(((TYPELIST *)0U)->pstParent->pstParent->pstParent->pstType[0])
#define TYPELIST_TYPE5(TYPELIST)	STD_TYPEOF(((TYPELIST *)0U)->pstParent->pstParent->pstParent->pstParent->pstType[0])


#endif /* STD_TYPELIST_H_ */

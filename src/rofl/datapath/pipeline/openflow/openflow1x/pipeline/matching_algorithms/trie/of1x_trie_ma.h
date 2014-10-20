#ifndef __OF1X_TRIE_MATCH_H__
#define __OF1X_TRIE_MATCH_H__

#include "rofl.h"
#include "../matching_algorithms.h"
#include "../../of1x_flow_table.h"


//Data structures
typedef struct of1x_trie_leaf{

	//Match
	of1x_match_t match;

	//Flow entry
	of1x_flow_entry_t* entry;

	//Inner max priority
	uint32_t inner_max_priority;

	//Inner branch(es)
	struct of1x_trie_leaf* inner;

	//Next
	struct of1x_trie_leaf* next;

	//Parent
	struct of1x_trie_leaf* parent;
}of1x_trie_leaf_t;

typedef struct of1x_trie{
	//Active tree
	of1x_trie_leaf_t* active;

	//Unactive
	of1x_trie_leaf_t* secondary;
}of1x_trie_t;

//C++ extern C
ROFL_BEGIN_DECLS

//Nothing needed here

//C++  xtern C
ROFL_END_DECLS

#endif //TRIE_MATCH

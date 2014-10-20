#ifndef __OF1X_TRIE_MATCH_PP_H__
#define __OF1X_TRIE_MATCH_PP_H__

#include "rofl.h"
#include "../../of1x_pipeline.h"
#include "../../of1x_flow_table.h"
#include "../../of1x_flow_entry.h"
#include "../../of1x_match_pp.h"
#include "../../of1x_group_table.h"
#include "../../of1x_instruction_pp.h"
#include "../../../of1x_async_events_hooks.h"
#include "../../../../../platform/lock.h"
#include "../../../../../platform/likely.h"
#include "../../../../../platform/memory.h"
#include "of1x_trie_ma.h"

//C++ extern C
ROFL_BEGIN_DECLS

/* FLOW entry lookup entry point */
static inline of1x_flow_entry_t* of1x_find_best_match_trie_ma(of1x_flow_table_t *const table, datapacket_t *const pkt){

	of1x_flow_entry_t* best_match = NULL;
	int64_t match_priority = -1;
	struct of1x_trie* trie = ((of1x_trie_t*)table->matching_aux[0]);
	struct of1x_trie_leaf* curr = trie->active;

	if(curr){
OF1X_TRIE_CHECK_LEAF:
		//Check inner
		if( ((int64_t)curr->inner_max_priority) > match_priority){

			//Check match
			if(__of1x_check_match(pkt, &curr->match)){
				if(entry)
					best_match = curr->entry

				if(curr->inner){
					curr = curr->inner;
					goto OF1X_TRIE_CHECK_LEAF;
				}
			}
			goto OF1X_TRIE_NEXT;
		}

OF1X_TRIE_NEXT:
		//Check next
		curr = curr->next;
		if(curr)
			goto OF1X_TRIE_CHECK_LEAF;
		else{
			//Go to parent
			curr = curr->parent;
			if(!curr->parent)
				goto OF1X_TRIE_END;
			else
				goto OF1X_TRIE_NEXT;
		}
	}

OF1X_TRIE_END:
	return best_match;
}

//C++ extern C
ROFL_END_DECLS

#endif //OF1X_TRIE_MATCH_PP

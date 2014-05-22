#include "pex_connected.h"

#include <string.h>
#include "platform/lock.h"
#include "platform/memory.h"

//Destroy a PEX port name list
void pex_port_name_list_destroy(pex_port_name_list_t* list)
{
	if(list)
	{
		platform_free_shared(list->names);
		platform_free_shared(list);
	}
}

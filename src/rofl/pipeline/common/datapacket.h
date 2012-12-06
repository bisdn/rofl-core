#ifndef __DATAPACKET_H__
#define __DATAPACKET_H__


//Typedef to void. This is dependant to the version of the pipeline
typedef void of_packet_matches_t;
typedef void of_write_actions_t;
typedef void platform_datapacket_state_t; 


typedef struct {

	//Generic parts of an Openflow (version agnostic)
	of_packet_matches_t* matches;
	of_write_actions_t* write_actions;
	
	//Platform specific part (non-OF related and plat. agnostic)
	platform_datapacket_state_t* platform_state;

} datapacket_t;

#endif //DATAPACKET


/*
 * cofpacketqueuelist.h
 *
 *  Created on: 30.04.2013
 *      Author: andi
 */

#ifndef COFPACKETQUEUELIST_H_
#define COFPACKETQUEUELIST_H_ 1

#include <ostream>

#include "rofl/common/coflist.h"
#include "rofl/common/openflow/cofpacketqueue.h"

namespace rofl {
namespace openflow {

class cofpacket_queue_list :
		public coflist<cofpacket_queue>
{
private:

	uint8_t of_version;

public:


	/**
	 *
	 */
	cofpacket_queue_list(
		uint8_t of_version);



	/**
	 *
	 */
	virtual
	~cofpacket_queue_list();



	/**
	 *
	 */
	cofpacket_queue_list(
		cofpacket_queue_list const& qpl);



	/**
	 */
	cofpacket_queue_list&
	operator= (
		cofpacket_queue_list const& qpl);


public:


	/**
	 *
	 */
	virtual size_t
	length() const;



	/**
	 *
	 */
	virtual void
	unpack(
		uint8_t *buf, size_t buflen);



	/**
	 *
	 */
	virtual void
	pack(
		uint8_t *buf,
		size_t buflen) const;


public:


	/**
	 *
	 */
	cofpacket_queue&
	find_packet_queue(
		uint32_t queue_id);


	/**
	 *
	 */
	friend std::ostream&
	operator<< (std::ostream& os, cofpacket_queue_list const& pql)
	{
		os << "PacketQueueList: ";
		for (cofpacket_queue_list::const_iterator
				it = pql.begin(); it != pql.end(); ++it) {
			os << (*it) << " ";
		}
		return os;
	};
};

}; // end of namespace openflow
}; // end of namespace rofl

#endif /* COFPACKETQUEUELIST_H_ */

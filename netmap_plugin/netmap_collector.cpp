// log4cpp logging facility
#include "log4cpp/Category.hh"
#include "log4cpp/Appender.hh"
#include "log4cpp/FileAppender.hh"
#include "log4cpp/OstreamAppender.hh"
#include "log4cpp/Layout.hh"
#include "log4cpp/BasicLayout.hh"
#include "log4cpp/PatternLayout.hh"
#include "log4cpp/Priority.hh"

// For support uint32_t, uint16_t
#include <sys/types.h>

// For config map operations
#include <string>
#include <map>

#include <stdio.h>
#include <iostream>
#include <string>
#define NETMAP_WITH_LIBS

#include <net/netmap_user.h>
#include <boost/thread.hpp>

#include "../fastnetmon_packet_parser.h"

// For pooling operations
#include <poll.h>

// For support: IPPROTO_TCP, IPPROTO_ICMP, IPPROTO_UDP
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "netmap_collector.h"

// Get log4cpp logger from main programm
extern log4cpp::Category& logger;

// Global configuration map 
extern std::map<std::string, std::string> configuration_map;

// This variable name should be uniq for every plugin!
process_packet_pointer netmap_process_func_ptr = NULL;

void start_netmap_collection(process_packet_pointer func_ptr) {
    // remove IT
    struct pfring_pkthdr l2tp_header;
    fastnetmon_parse_pkt((u_char*)NULL, &l2tp_header, 4, 1, 0);

    logger<< log4cpp::Priority::INFO<<"Netmap plugin started";
    netmap_process_func_ptr = func_ptr;

    std::string netmap_plugin_config_param = "";

    if (configuration_map.count("some_plugin_param_from_global_config") != 0) {
        netmap_plugin_config_param = configuration_map[ "some_plugin_param_from_global_config" ];
    }

    // We should fill this structure for passing to FastNetMon
    simple_packet current_packet;
    
    current_packet.src_ip = 0;
    current_packet.dst_ip = 0;

    current_packet.ts.tv_sec = 0;
    current_packet.ts.tv_usec = 0;
    current_packet.flags = 0;

    // There we store packet length or total length of aggregated stream
    current_packet.length = 128;
    
    // Number of received packets, it's not equal to 1 only for aggregated data like netflow
    current_packet.number_of_packets = 1;

    // If your data sampled
    current_packet.sample_ratio = 1;

    /* ICMP */
    current_packet.protocol = IPPROTO_ICMP; 
    
    /* TCP */
    current_packet.protocol = IPPROTO_TCP;
    current_packet.source_port = 0;
    current_packet.destination_port = 0;

    /* UDP */
    current_packet.protocol = IPPROTO_UDP;
    current_packet.source_port = 0;
    current_packet.destination_port = 0;

    netmap_process_func_ptr(current_packet);
}

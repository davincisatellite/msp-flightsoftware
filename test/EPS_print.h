#ifndef EPS_PRINT_H
#define EPS_PRINT_H
#include <cstdio>

void print_array(uint8_t *v, uint8_t n);
void print_VIPD_data(char* name, EPS::VIPD vipd, int i=-1);
void print_BPD_data(char* name, EPS::BPD bpd);
void print_CCD_data(char* name, EPS::CCD ccd);
void print_CCSD_data(char* name, EPS::CCSD ccsd);
void print_command(uint8_t stid, uint8_t ivid, uint8_t cc, uint8_t bid);
void print_command_5_bytes(uint8_t stid, uint8_t ivid, uint8_t cc, uint8_t bid, uint8_t fifth_byte);
void print_command_6_bytes(uint8_t stid, uint8_t ivid, uint8_t cc, uint8_t bid, uint8_t fifth_byte, uint8_t sixth_byte);
void print_standard_reply(EPS::standard_reply reply);
void print_system_status(EPS::system_status_reply reply);
void print_overcurrent_reply(EPS::overcurrent_reply reply);
void print_pbu_abf_placed_state(EPS::pbu_abf_placed_state reply);
void print_pbu_housekeeping_data_reply(EPS::pbu_housekeeping_data_reply reply);
void print_pcu_housekeeping_data_reply(EPS::pcu_housekeeping_data_reply reply);
void print_pdu_housekeeping_data_reply(EPS::pdu_housekeeping_data_reply reply);
void print_piu_housekeeping_data_reply(EPS::piu_housekeeping_data_reply reply);
void print_config_reply(EPS::config_reply reply);

#endif
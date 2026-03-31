#include "EPS_print.h"
#include "../src/EPS.h"
#include "../src/Console.h"
#include <cstdio>

// there are some methods to print the data on the screen
void print_array(uint8_t *v, uint8_t n) {
    char buf[200];
    int offset = 0;
    for(uint8_t i=0;i<n;i++)
    {
        offset += sprintf(buf + offset, "%x ", v[i]);
        if (offset >= sizeof(buf)) {
            Console::log("overflow");
            break; // prevent overflow
        }
    }
    Console::log("Raw bytes: %d bytes", n);
    Console::log(buf);
}
void print_VIPD_data(char* name, EPS::VIPD vipd, int i=-1) {
    char buf1[70];
    if(i!=-1)
        sprintf(buf1, "%s%d:   VOLT: %d V | CURR: %d A | POWER: %d W", name, i, vipd.volt, vipd.curr, vipd.power);
    else
        sprintf(buf1, "%s:   VOLT: %d V | CURR: %d A | POWER: %d W", name, vipd.volt, vipd.curr, vipd.power);
    Console::log(buf1);
}
void print_BPD_data(char* name, EPS::BPD bpd) {
    char buf1[60],buf2[70],buf3[70],buf4[70];
    Console::log("%s:",name);
    print_VIPD_data("    VIP_BP_INPUT", bpd.vip_bp_input);
    Console::log("    STAT_BP: %x", bpd.stat_bp);
    sprintf(buf1, "    VOLT_CELLS 1,2,3,4:  %d V | %d V |  %d V | %d V", bpd.volt_cell1, bpd.volt_cell2, bpd.volt_cell3, bpd.volt_cell4);
    sprintf(buf2, "    Internal temperature of battery:                 %d C", bpd.bat_temp1);
    sprintf(buf3, "    Temperature in between the center battery cells: %d C", bpd.bat_temp2);
    sprintf(buf4, "    Temperature on the front of the battery pack:    %d C", bpd.bat_temp3);
    Console::log(buf1);
    Console::log(buf2);
    Console::log(buf3);
    Console::log(buf4);
}
void print_CCD_data(char* name, EPS::CCD ccd) {
    char buf1[60],buf2[60];
    Console::log("%s:",name);
    print_VIPD_data("    VIP_CC_OUTPUT", ccd.vip_cc_output);
    sprintf(buf1, "    VOLT_IN_MPPT:  %d V  |  CURR_IN_MPPT:  %d V", ccd.volt_in_mppt, ccd.curr_in_mppt);
    Console::log(buf1);
    sprintf(buf2, "    VOLT_OUT_MPPT: %d V  |  CURR_OUT_MPPT: %d V", ccd.volt_out_mppt, ccd.curr_out_mppt);
    Console::log(buf2);
}
void print_CCSD_data(char* name, EPS::CCSD ccsd) {
    char buf1[60],buf2[60];
    Console::log("%s:",name);
    sprintf(buf1, "    VOLT_IN_MPPT:  %d V  |  CURR_IN_MPPT:  %d V", ccsd.volt_in_mppt, ccsd.curr_in_mppt);
    Console::log(buf1);
    sprintf(buf2, "    VOLT_OUT_MPPT: %d V  |  CURR_OUT_MPPT: %d V", ccsd.volt_out_mppt, ccsd.curr_out_mppt);
    Console::log(buf2);
}
void print_command(uint8_t stid, uint8_t ivid, uint8_t cc, uint8_t bid) {
    Console::log("--- Command Data ---");
    Console::log("STID: %x | IVID: %x | CC: %x | BID: %x", stid, ivid, cc, bid);
}
void print_command_5_bytes(uint8_t stid, uint8_t ivid, uint8_t cc, uint8_t bid, uint8_t fifth_byte) {
    Console::log("--- Command Data ---");
    Console::log("STID: %x | IVID: %x | CC: %x | BID: %x | 5th byte: %x", stid, ivid, cc, bid, fifth_byte);
}
void print_command_6_bytes(uint8_t stid, uint8_t ivid, uint8_t cc, uint8_t bid, uint8_t fifth_byte, uint8_t sixth_byte) {
    Console::log("--- Command Data ---");
    Console::log("STID: %x | IVID: %x | CC: %x | BID: %x | 5th byte: %x | 6th byte: %x", stid, ivid, cc, bid, fifth_byte, sixth_byte);
}
void print_standard_reply(EPS::standard_reply reply) {
    Console::log("--- Standard Reply Data ---");
    Console::log("STID: %x | IVID: %x | RC: %x | BID: %x | STAT: %x | Error: %x", reply.stid, reply.ivid, reply.rc, reply.bid, reply.stat, reply.error);
}


void print_system_status(EPS::system_status_reply reply) {
    Console::log("--- System status Data ---");
    Console::log("STID: %x | IVID: %x | RC: %x | BID: %x | STAT: %x | Error: %x", reply.stid, reply.ivid, reply.rc, reply.bid, reply.stat, reply.error);

    //Console::log("MODE: %d", reply.mode);
    if (reply.mode==0)
        Console::log("MODE: 0 -> Startup");
    else if (reply.mode==1)
        Console::log("MODE: 1 -> Nominal");
    else if (reply.mode==2)
        Console::log("MODE: 2 -> Safety");
    else if (reply.mode==3)
        Console::log("MODE: 3 -> Emergency Low Power");
    else
        Console::log("MODE: %d -> Invalid mode", reply.mode);

    //Console::log("CONF: %d", reply.conf);
    if (reply.conf==0)
        Console::log("CONF: 0 -> Parameters have NOT been altered since the last load/save.");
    else if (reply.conf==1)
        Console::log("CONF: 1 -> Parameters have been altered since the last load/save.");
    else
        Console::log("CONF: %d -> Invalid conf", reply.conf);

    //Console::log("RESET_CAUSE:     %d", reply.reset_cause);
    if (reply.reset_cause==0)
        Console::log("RESET_CAUSE:     %d -> power-on; system returned from an unpowered state", reply.reset_cause);
    else if (reply.reset_cause==1)
        Console::log("RESET_CAUSE:     %d -> watchdog; system was reset due to watchdog timeout", reply.reset_cause);
    else if (reply.reset_cause==2)
        Console::log("RESET_CAUSE:     %d -> commanded; system was reset due a reset command", reply.reset_cause);
    else if (reply.reset_cause==3)
        Console::log("RESET_CAUSE:     %d -> control system reset; an upset in the EPS control system caused a reset", reply.reset_cause);
    else if (reply.reset_cause==4)
        Console::log("RESET_CAUSE:     %d -> emlopo; emergency, input voltage dropped below the threshold", reply.reset_cause);
    else
        Console::log("RESET_CAUSE:     %d -> invalid response", reply.reset_cause);

    Console::log("UPTIME:          %d s", reply.uptime);
    Console::log("(SYS) ERROR:     %d", reply.sys_error);
    Console::log("RC_CNT_PWRON:    %d", reply.rc_cnt_pwron);
    Console::log("RC_CNT_WDG:      %d", reply.rc_cnt_wdg);
    Console::log("RC_CNT_CMD:      %d", reply.rc_cnt_cmd);
    Console::log("RC_CNT_MCU:      %d", reply.rc_cnt_mcu);
    Console::log("RC_CNT_EMLOPO:   %d", reply.rc_cnt_emlopo);
    Console::log("PREVCMD_ELAPSED: %d s", reply.prevcmd_elapsed);
}

void print_overcurrent_reply(EPS::overcurrent_reply reply) {
    Console::log("--- PIU Overcurrent Reply Data ---");
    Console::log("STID: %x | IVID: %x | RC: %x | BID: %x | STAT: %x | Error: %x", reply.stid, reply.ivid, reply.rc, reply.bid, reply.stat, reply.error);
    Console::log("STAT_OB_ON: %x  |  STAT_OB_OCF: %x", reply.stat_ob_on, reply.stat_ob_ocf);
    Console::log("Overcurrent fault count of output bus channels:");
    for(int i=0;i<8;i++) {
        char buf1[60];
        sprintf(buf1, "    OCF_CNT_CH%d: %d | OCF_CNT_CH%d: %d", i*2, reply.ocf_cnt_ch[i*2], i*2+1, reply.ocf_cnt_ch[i*2+1]);
        Console::log(buf1);
    }
}
void print_pbu_abf_placed_state(EPS::pbu_abf_placed_state reply) {
    Console::log("--- PBU ABF Placed State Reply Data ---");
    Console::log("STID: %x | IVID: %x | RC: %x | BID: %x | STAT: %x | Error: %x", reply.stid, reply.ivid, reply.rc, reply.bid, reply.stat, reply.error);
    Console::log("STAT_OB_ON: %x  |  STAT_OB_OCF: %x", reply.abf_placed_0, reply.abf_placed_1);
    if(reply.abf_placed_0==0xAB)
        Console::log("is ABD placed?: yes");
    else if (reply.abf_placed_0==0x00)
        Console::log("is ABD placed?: no");
    else if(reply.abf_placed_1==0xAB)
        Console::log("is ABD placed?: yes, but abf_placed_0 is invalid");
    else if (reply.abf_placed_1==0x00)
        Console::log("is ABD placed?: no, but abf_placed_0 is invalid");
    else
        Console::log("is ABD placed?: unknown, both abf_placed_0 and 1 are invalid");
}
void print_pbu_housekeeping_data_reply(EPS::pbu_housekeeping_data_reply reply) {
    char buf1[60];
    Console::log("--- PBU Housekeeping Data (engineering form)---");



    Console::log("STID: %x | IVID: %x | RC: %x | BID: %x | STAT: %x | Error: %x", reply.stid, reply.ivid, reply.rc, reply.bid, reply.stat, reply.error);
    sprintf(buf1, "VOLT_BRDSUP: %d V  |  TEMP: %d C", reply.volt_brdsup, reply.temp);
    Console::log(buf1);
    print_VIPD_data("VIP_INPUT", reply.vip_input);
    Console::log("STAT_BU: %x", reply.stat_bu);
    print_BPD_data("BP1", reply.bp[0]);
    print_BPD_data("BP2", reply.bp[1]);
    print_BPD_data("BP3", reply.bp[2]);
}

void print_pcu_housekeeping_data_reply(EPS::pcu_housekeeping_data_reply reply) {
    char buf1[60];
    Console::log("--- PCU Housekeeping Data (engineering form)---");
    Console::log("STID: %x | IVID: %x | RC: %x | BID: %x | STAT: %x | Error: %x", reply.stid, reply.ivid, reply.rc, reply.bid, reply.stat, reply.error);
    sprintf(buf1, "VOLT_BRDSUP: %d V  |  TEMP: %d C", reply.volt_brdsup, reply.temp);
    Console::log(buf1);
    print_VIPD_data("VIP_OUTPUT", reply.vip_output);
    print_CCD_data("CC1", reply.cc[0]);
    print_CCD_data("CC2", reply.cc[1]);
    print_CCD_data("CC3", reply.cc[2]);
    print_CCD_data("CC4", reply.cc[3]);
}

void print_pdu_housekeeping_data_reply(EPS::pdu_housekeeping_data_reply reply) {
    char buf1[60];
    Console::log("--- PDU Housekeeping Data (engineering form)---");
    Console::log("STID: %x | IVID: %x | RC: %x | BID: %x | STAT: %x | Error: %x", reply.stid, reply.ivid, reply.rc, reply.bid, reply.stat, reply.error);
    sprintf(buf1, "VOLT_BRDSUP: %d V  |  TEMP: %d C", reply.volt_brdsup, reply.temp);
    Console::log(buf1);
    print_VIPD_data("VIP_INPUT", reply.vip_input);
    Console::log("STAT_CH_ON: %x  |  STAT_CH_OCF: %x", reply.stat_ch_on, reply.stat_ch_ocf);
    for(int i=0;i<7;i++)
        print_VIPD_data("VIP_VD", reply.vip_vd[i], i);
    for(int i=0;i<16;i++)
        print_VIPD_data("VIP_CH", reply.vip_ch[i], i);
}
void print_piu_housekeeping_data_reply(EPS::piu_housekeeping_data_reply reply) {
    char buf1[60],buf2[70],buf3[70];
    Console::log("--- PIU Housekeeping Data (engineering form)---");
    Console::log("STID: %x | IVID: %x | RC: %x | BID: %x | STAT: %x | Error: %x", reply.stid, reply.ivid, reply.rc, reply.bid, reply.stat, reply.error);
    sprintf(buf1, "VOLT_BRDSUP: %d V  |  TEMP: %d C", reply.volt_brdsup, reply.temp);
    Console::log(buf1);
    print_VIPD_data("VIP_DIST_INPUT", reply.vip_dist_input);
    print_VIPD_data("VIP_BATT_INPUT", reply.vip_bat_input);
    Console::log("STAT_CH_ON: %x  |  STAT_CH_OCF: %x", reply.stat_ch_on, reply.stat_ch_ocf);
    sprintf(buf2, "BAT_STAT: %x  |  BAT_TEMP2: %d  |  BAT_TEMP3: %d", reply.bat_stat, reply.bat_temp2, reply.bat_temp3);
    Console::log(buf2);
    sprintf(buf3, "VOLT voltage domains 0,1,2:  %d V  |  %d V  |  %d V ", reply.volt_vd[0], reply.volt_vd[1], reply.volt_vd[2]);
    Console::log(buf3);
    for(int i=0;i<9;i++)
        print_VIPD_data("VIP_CH", reply.vip_ch[i], i);
    print_CCSD_data("CC1", reply.cc[0]);
    print_CCSD_data("CC2", reply.cc[1]);
    print_CCSD_data("CC3", reply.cc[2]);
}
void print_config_reply(EPS::config_reply reply) {

    Console::log("STID: %x | IVID: %x | RC: %x | BID: %x | STAT: %x | Error: %x", reply.stid, reply.ivid, reply.rc, reply.bid, reply.stat, reply.error);
    Console::log("Config Parameter ID: %x", reply.par_id);
    ParameterType p_type = EPS::getConfigParameterType(reply.par_id);
    AccessType access_type = EPS::getAccessType(reply.par_id);
    char buf[50];

    sprintf(buf, "Config Parameter Value: %d", reply.par_value);
    Console::log(buf);
//    if(p_type==Float || p_type==Double)
//    	Console::log("Config Parameter Value: %f", reply.par_id);
    if(p_type==UInt8)
        sprintf(buf, "Config Parameter Value: %d", reply.par_value.ui8);
    else if (p_type==UInt16)
        sprintf(buf, "Config Parameter Value: %d", reply.par_value.ui16);
    else if (p_type==Int8)
        sprintf(buf, "Config Parameter Value: %d", reply.par_value.i8);
    else if (p_type==Int16)
        sprintf(buf, "Config Parameter Value: %d", reply.par_value.ui16);
    else
        sprintf(buf, "Config Parameter Value: invalid?");
//    	Console::log("Config Parameter Value: %d", reply.par_value);
//    else
//    	Console::log("Config Parameter Value: %d", reply.par_value);

    if(access_type==ReadWrite)
        Console::log("Parameter Length: %d  | Access type: ReadWrite", EPS::get_param_length(p_type));
    else
        Console::log("Parameter Length: %d  | Access type: Read Only", EPS::get_param_length(p_type));
}
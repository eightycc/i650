//
//  module_top.cpp
//  i650_sim_cpp
//
//  Created by Robert Abeles on 7/27/15.
//  Copyright © 2015 Robert Abeles. All rights reserved.
//

#include "sim_includes.hpp"
#include "module_includes.hpp"

ModuleTop::ModuleTop(SimModelPtr m)
  : SimModule(m, "top", nullptr)
{
    opCtlModule    = new ModuleOperatorCtl(m, "oc", this);
    accModule      = new ModuleAccumulator(m, "acc", this);
    timingModule   = new ModuleTiming(m, "time", this);
    genStoreModule = new ModuleGenStore(m, "gs", this);
    addrRegModule  = new ModuleAddrReg(m, "ar", this);
    adderInAModule = new ModuleAdderInA(m, "adda", this);
    adderInBModule = new ModuleAdderInB(m, "addb", this);
    adderModule    = new ModuleAdder(m, "adder", this);
    storeSelModule = new ModuleStoreSelect(m, "ss", this);
    distModule     = new ModuleDistributor(m, "dist", this);
    progRegModule  = new ModuleProgReg(m, "ps", this);
    errStopModule  = new ModuleErrorStop(m, "es", this);
    opRegModule    = new ModuleOpReg(m, "or", this);
    ctlCommModule  = new ModuleCtlComm(m, "cc", this);
    
    // Operator control panel inputs
    new SimWire(m, "oc_digit_num", this, "time/digit_num", "oc/digit_num");
    new SimWire(m, "oc_e_digit_num", this, "time/e_digit_num", "oc/e_digit_num");
    new SimWire(m, "oc_word_num", this, "time/word_num", "oc/word_num");
    new SimWire(m, "oc_upper_word", this, "time/upper_word", "oc/upper_word");
    new SimWire(m, "oc_sector_num", this, "time/sector_num", "oc/sector_num");
    new SimWire(m, "oc_sel_store", this, "ss/select_out", "oc/sel_store");
    new SimWire(m, "oc_acc_ontime", this, "acc/ontime_out", "oc/acc_ontime");
    new SimWire(m, "oc_pgm_step_ontime", this, "ps/ontime_out", "oc/pgm_step_ontime");
    new SimWire(m, "oc_restart_reset", this, "es/restart_reset", "oc/restart_reset");
    
    // General storage module inputs
    new SimWire(m, "gs_digit_num",  this, "time/digit_num",  "gs/digit_num");
    new SimWire(m, "gs_e_digit_num", this, "time/e_digit_num", "gs/e_digit_num");
    new SimWire(m, "gs_word_num",   this, "time/word_num",   "gs/word_num");
    new SimWire(m, "gs_sector_num", this, "time/sector_num", "gs/sector_num");
    new SimWire(m, "gs_addr_th", this, "ar/addr_th", "gs/addr_th");
    new SimWire(m, "gs_addr_h", this, "ar/addr_h", "gs/addr_h");
    new SimWire(m, "gs_addr_t", this, "ar/addr_t", "gs/addr_t");
    new SimWire(m, "gs_dist_early", this, "dist/early_out", "gs/dist_early");
    
    // Accumulator module inputs
    new SimWire(m, "acc_digit_num", this, "time/digit_num",  "acc/digit_num");
    new SimWire(m, "acc_e_digit_num", this, "time/e_digit_num", "acc/e_digit_num");
    new SimWire(m, "acc_upper_word", this, "time/upper_word", "acc/upper_word");
    // readEarly
    new SimWire(m, "acc_adder", this, "adder/sum_out", "acc/adder");
    // zeroAcc
    // zeroShiftCtr
    new SimWire(m, "acc_acc_reset", this, "oc/acc_reset", "acc/acc_reset");
    
    // Address register module inputs
    new SimWire(m, "ar_digit_num", this, "time/digit_num", "ar/digit_num");
    new SimWire(m, "ar_word_num",   this, "time/word_num",   "ar/word_num");
    new SimWire(m, "ar_sector_num", this, "time/sector_num", "ar/sector_num");
    new SimWire(m, "ar_error_reset", this, "oc/error_reset", "ar/error_reset");
    // restartA
    new SimWire(m, "ar_set_8000", this, "oc/set_8000", "ar/set_8000");
    new SimWire(m, "ar_reset_0000", this, "oc/reset_0000", "ar/reset_0000");
    // tluBandChange
    new SimWire(m, "ar_read_in_ar", this, "or/ri_addr_reg", "ar/read_in_ar");
    new SimWire(m, "ar_ps_ped", this, "ps/ped_out", "ar/ps_ped");
    new SimWire(m, "ar_xfer_key", this, "oc/xfer_key", "ar/xfer_key");
    new SimWire(m, "ar_oc_addr_th", this, "oc/addr_th", "ar/oc_addr_th");
    new SimWire(m, "ar_oc_addr_h", this, "oc/addr_h", "ar/oc_addr_h");
    new SimWire(m, "ar_oc_addr_t", this, "oc/addr_t", "ar/oc_addr_t");
    new SimWire(m, "ar_oc_addr_u", this, "oc/addr_u", "ar/oc_addr_u");
   
    // Adder Input Mux A
    new SimWire(m, "adda_acc_early", this, "acc/early_out", "adda/acc_early");
    new SimWire(m, "adda_acc_ontime", this, "acc/ontime_out", "adda/acc_ontime");
    new SimWire(m, "adda_ps_early", this, "ps/early_out", "adda/ps_early");
    new SimWire(m, "adda_addr_u", this, "ar/addr_u", "adda/addr_u");
    new SimWire(m, "adda_sel_store", this, "ss/select_out", "adda/sel_store");
    //SimPortIn<bool> *accTrueAdd;
    //SimPortIn<bool> *accCompAdd;
    //SimPortIn<bool> *leftShift;
    //SimPortIn<bool> *pgmStepAdd;
    //SimPortIn<bool> *shiftCount;
    //SimPortIn<bool> *selStoreAdd;
    
    // Adder Input Mux B
    new SimWire(m, "addb_dist_early", this, "dist/early_out", "addb/dist_early");
    new SimWire(m, "addb_dist_ontime", this, "dist/ontime_out", "addb/dist_ontime");
    // specialDigit
    // dist_ontime_add
    // dist_true_add
    // dist_comp_add
    // upperLower
    // distBlank
    // earlyDistZero
    
    // Adder
    new SimWire(m, "adder_digit_num", this, "time/digit_num", "adder/digit_num");
    new SimWire(m, "adder_upper_word", this, "time/upper_word", "adder/upper_word");
    new SimWire(m, "adder_entry_a", this, "adda/entry_a", "adder/entry_a");
    new SimWire(m, "adder_entry_b", this, "addb/entry_b", "adder/entry_b");
    // tluOn
    // leftShiftOn
    // carryBlank
    // carryInsert
    // zeroInsert
    new SimWire(m, "adder_ovflw_stop_sw", this, "oc/ovflw_sw_stop", "adder/ovflw_stop_sw");
    new SimWire(m, "adder_ovflw_sense_sw", this, "oc/ovflw_sw_sense", "adder/ovflw_sense_sw");
    // shiftOvflw
    // multDivOff
    // quotDigitOn
    // distTrueAdd
    // accTrueAdd
    new SimWire(m, "adder_error_reset", this, "oc/error_reset", "adder/error_reset");
    
    // Distributor
    new SimWire(m, "dist_digit_num", this, "time/digit_num", "dist/digit_num");
    new SimWire(m, "dist_e_digit_num", this, "time/e_digit_num", "dist/e_digit_num");
    new SimWire(m, "dist_store_select", this, "ss/select_out", "dist/store_select");
    new SimWire(m, "dist_acc_ontime", this, "acc/ontime_out", "dist/acc_ontime");
    // acc_readin
    // acc_ri_start
    // acc_ri_end
    // ss_readin
    new SimWire(m, "dist_acc_reset", this, "oc/acc_reset", "dist/acc_reset");

    // Storage source selction mux
    new SimWire(m, "ss_digit_num",  this, "time/digit_num",  "ss/digit_num");
    new SimWire(m, "ss_acc_ontime", this, "acc/ontime_out", "ss/acc_ontime");
    new SimWire(m, "ss_dist_ontime", this, "dist/ontime_out", "ss/dist_ontime");
    new SimWire(m, "ss_gs_out", this, "gs/gen_store_out", "ss/gen_store_out");
    new SimWire(m, "ss_data_switches", this, "oc/data_out", "ss/data_switches");
    //PortInBool *accPlus;
    //PortInBool *accMinus;
    
    new SimWire(m, "ss_addr_no_800x", this, "ar/addr_no_800x", "ss/addr_no_800x");
    new SimWire(m, "ss_addr_8000", this, "ar/addr_8000", "ss/addr_8000");
    new SimWire(m, "ss_addr_8001", this, "ar/addr_8001", "ss/addr_8001");
    new SimWire(m, "ss_addr_8002_3", this, "ar/addr_8002_3", "ss/addr_8002_3");
    //PortInBool *addrHot8000;
    
    // Program step register
    new SimWire(m, "ps_digit_num", this, "time/digit_num", "ps/digit_num");
    new SimWire(m, "ps_e_digit_num", this, "time/e_digit_num", "ps/e_digit_num");
    new SimWire(m, "ps_adder", this, "adder/sum_out", "ps/adder");
    new SimWire(m, "ps_store_select", this, "ss/select_out", "ps/store_select");
    new SimWire(m, "ps_prog_reset", this, "oc/pgm_reset", "ps/prog_reset");
    // rips
    // progAdd
    
    // Error Stop
    new SimWire(m, "es_digit_num", this, "time/digit_num", "es/digit_num");
    new SimWire(m, "es_lower_word", this, "time/lower_word", "es/lower_word");
    new SimWire(m, "es_err_sw_sense", this, "oc/err_sw_sense", "es/err_sw_sense");
    new SimWire(m, "es_err_reset", this, "oc/error_reset", "es/err_reset");
    new SimWire(m, "es_err_sense_reset", this, "oc/err_sense_reset", "es/err_sense_reset");
    // clockError
    // errorStopSignal
    new SimWire(m, "es_oc_busy", this, "oc/busy", "es/oc_busy");
    
    // Operation register
    new SimWire(m, "or_digit_num", this, "time/digit_num", "or/digit_num");
    new SimWire(m, "or_prog_reg_ped", this, "ps/ped_out", "or/prog_reg_ped");
    // restartA
    // restartB
    // dAlt
    // iAlt
    // tluBandChange
    new SimWire(m, "or_prog_reset", this, "oc/pgm_reset", "or/prog_reset");
    
    // Control Commutator
    new SimWire(m, "cc_digit_num", this, "time/digit_num", "cc/digit_num");
    new SimWire(m, "cc_lower_word", this, "time/lower_word", "cc/lower_word");
    new SimWire(m, "cc_err_reset", this, "oc/error_reset", "cc/err_reset");
    new SimWire(m, "cc_pgm_reset", this, "oc/pgm_reset", "cc/pgm_reset");
    new SimWire(m, "cc_disp_sw_ro", this, "oc/disp_sw_readout", "cc/disp_sw_readout");
    new SimWire(m, "cc_disp_sw_ri", this, "oc/disp_sw_readin", "cc/disp_sw_readin");
    new SimWire(m, "cc_hc_sw_half", this, "oc/hc_sw_half", "cc/hc_sw_half");
    new SimWire(m, "cc_pgm_start", this, "oc/pgm_start", "cc/pgm_start");
    new SimWire(m, "cc_pgm_stop", this, "oc/pgm_stop", "cc/pgm_stop");
    // pgm_stop_addr
    new SimWire(m, "cc_opreg_t", this, "or/opreg_t", "cc/opreg_t");
    new SimWire(m, "cc_opreg_u", this, "or/opreg_u", "cc/opreg_u");
    new SimWire(m, "cc_addr_no_800x", this, "ar/addr_no_800x", "cc/addr_no_800x");
    //new SimWire(m, "cc_pgm_restart", this, "")
    // arithRestart
    // decodeRestarts
    new SimWire(m, "cc_addr_invalid", this, "ar/addr_invalid", "cc/addr_invalid");
    // opStop
    
    new SimWire(m, "cc_run_control", this, "oc/run_control", "cc/run_control");
    // useDforI
    
    new SimWire(m, "cc_err_sns_restart", this, "es/err_sense_restart", "cc/err_sns_restart");
    new SimWire(m, "cc_err_stop", this, "es/err_stop", "cc/err_stop");
    new SimWire(m, "cc_dist_back", this, "dist/dist_back", "cc/dist_back");
}
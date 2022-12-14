/*
 *  yosys -- Yosys Open SYnthesis Suite
 *
 *  Copyright (C) 2020 Karol Gugala <kgugala@antmicro.com>
 *
 *  Permission to use, copy, modify, and/or distribute this software for any
 *  purpose with or without fee is hereby granted, provided that the above
 *  copyright notice and this permission notice appear in all copies.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */
#include "kernel/register.h"
#include "kernel/celltypes.h"
#include "kernel/rtlil.h"
#include "kernel/log.h"

USING_YOSYS_NAMESPACE
PRIVATE_NAMESPACE_BEGIN

struct SynthQuickLogicPass : public ScriptPass {

    SynthQuickLogicPass() : ScriptPass("synth_quicklogic", "Synthesis for QuickLogic FPGAs") {}

    void help() override
    {
        log("\n");
        log("   synth_quicklogic [options]\n");
        log("This command runs synthesis for QuickLogic FPGAs\n");
        log("\n");
        log("    -top <module>\n");
        log("         use the specified module as top module\n");
        log("\n");
        log("    -family <family>\n");
        log("        run synthesis for the specified QuickLogic architecture\n");
        log("        generate the synthesis netlist for the specified family.\n");
        log("        supported values:\n");
        log("        - pp3: PolarPro 3 \n");
        log("        - ap: ArcticPro \n");
        log("        - ap2: ArcticPro 2 \n");
        log("        - ap3: ArcticPro 3 \n");
        log("        - qlf_k4n8: qlf_k4n8 \n");
        log("\n");
        log("    -no_ff_map\n");
        log("        By default ff techmap is turned on. Specifying this switch turns it off.\n");
        log("\n");
        log("    -no_abc_opt\n");
        log("        By default most of ABC logic optimization features is\n");
        log("        enabled. Specifying this switch turns them off.\n");
        log("\n");
        log("    -edif <file>\n");
        log("        write the design to the specified edif file. writing of an output file\n");
        log("        is omitted if this parameter is not specified.\n");
        log("\n");
        log("    -blif <file>\n");
        log("        write the design to the specified BLIF file. writing of an output file\n");
        log("        is omitted if this parameter is not specified.\n");
        log("\n");
        log("    -verilog <file>\n");
        log("        write the design to the specified verilog file. writing of an output file\n");
        log("        is omitted if this parameter is not specified.\n");
        log("\n");
        log("    -no_adder\n");
        log("        By default use adder cells in output netlist.\n");
        log("        Specifying this switch turns it off.\n");
        log("\n");
        log("    -infer_dbuff\n");
        log("        Infer d_buff for const driver IO signals (applicable for AP, AP2 & AP3 device)\n");
        log("\n");
        log("\n");
        log("The following commands are executed by this synthesis command:\n");
        help_script();
        log("\n");
    }

    string top_opt, edif_file, blif_file, family, currmodule, verilog_file;
    bool inferAdder, infer_dbuff;
    bool abcOpt;
    bool noffmap;

    void clear_flags() YS_OVERRIDE
    {
        top_opt = "-auto-top";
        edif_file = "";
        blif_file = "";
        verilog_file = "";
        currmodule = "";
        family = "pp3";
        inferAdder = true;
        abcOpt = true;
	noffmap = false;
        infer_dbuff = false;
    }

    void execute(std::vector<std::string> args, RTLIL::Design *design) YS_OVERRIDE
    {
        string run_from, run_to;
        clear_flags();

        size_t argidx;
        for (argidx = 1; argidx < args.size(); argidx++)
        {
            if (args[argidx] == "-top" && argidx+1 < args.size()) {
                top_opt = "-top " + args[++argidx];
                continue;
            }
            if (args[argidx] == "-edif" && argidx+1 < args.size()) {
                edif_file = args[++argidx];
                continue;
            }

            if (args[argidx] == "-family" && argidx+1 < args.size()) {
                family = args[++argidx];
                continue;
            }
            if (args[argidx] == "-blif" && argidx+1 < args.size()) {
                blif_file = args[++argidx];
                continue;
            }
            if (args[argidx] == "-verilog" && argidx+1 < args.size()) {
                verilog_file = args[++argidx];
                continue;
            }
            if (args[argidx] == "-no_adder") {
                inferAdder = false;
                continue;
            }
            if (args[argidx] == "-infer_dbuff") {
                infer_dbuff = true;
                continue;
            }
            if (args[argidx] == "-no_abc_opt") {
                abcOpt = false;
                continue;
            }
            if (args[argidx] == "-no_ff_map") {
                noffmap = true;
                continue;
            }
            
            break;
        }
        extra_args(args, argidx, design);

        if (!design->full_selection())
            log_cmd_error("This command only operates on fully selected designs!\n");

        log_header(design, "Executing SYNTH_QUICKLOGIC pass.\n");
        log_push();

        run_script(design, run_from, run_to);

        log_pop();
    }

    void script() YS_OVERRIDE
    {
        if (check_label("begin")) {
            std::string readVelArgs;
            readVelArgs = " +/quicklogic/" + family + "_cells_sim.v";
            
            run("read_verilog -lib -specify +/quicklogic/cells_sim.v" + readVelArgs);
            run(stringf("hierarchy -check %s", help_mode ? "-top <top>" : top_opt.c_str()));
        }

        if (check_label("prepare"))  {
            run("proc");
            run("flatten");
            if(family == "pp3" || family == "ap") {
                run("tribuf -logic");
            }
            run("opt_expr");
            run("opt_clean");
            run("deminout");
            run("opt");
        }

        if (check_label("coarse")) {
            run("opt_expr");
            run("opt_clean");
            run("check");
            run("opt");
            run("wreduce -keepdc");
            run("peepopt");
            run("pmuxtree");
            run("opt_clean");

            run("alumacc");
            run("opt");
            run("fsm");
            run("opt -fast");
            run("memory -nomap");
            run("opt_clean");
        }

        if (check_label("map_bram", "(skip if -nobram)") && family == "pp3") {
            run("memory_bram -rules +/quicklogic/" + family + "_brams.txt");
            run("pp3_braminit");
            run("techmap -map +/quicklogic/" + family + "_brams_map.v");
        }

        if (check_label("map_ffram")) {
            run("opt -fast -mux_undef -undriven -fine");
            run("memory_map -iattr -attr !ram_block -attr !rom_block -attr logic_block "
                    "-attr syn_ramstyle=auto -attr syn_ramstyle=registers "
                    "-attr syn_romstyle=auto -attr syn_romstyle=logic");
            run("opt -undriven -fine");
        }

        if (check_label("map_gates")) {
            if (inferAdder && family != "pp3" && family != "ap") {
                run("techmap -map +/techmap.v -map +/quicklogic/" + family + "_arith_map.v");
            } else {
                run("techmap");
            }
            run("opt -fast");
            if (family == "pp3" || family == "ap") {
                run("muxcover -mux8 -mux4");
            } else {
                run("opt_expr -clkinv");
                run("opt -fast");
                run("opt_expr");
                run("opt_merge");
                run("opt_rmdff");
                run("opt_clean");
                run("opt");
            }
        }

        if (check_label("map_ffs")) {
            if (family == "pp3" || family == "ap") {
                run("opt_expr -clkinv");
                run("dff2dffe");
            } else {
                if(family != "qlf_k4n8") {
                    run("dff2dffe -direct-match $_DFF_*");
                }
            }

            std::string techMapArgs = " -map +/quicklogic/" + family;

            techMapArgs += "_ffs_map.v";

            if (family == "qlf_k4n8") {
                run("shregmap -minlen 8 -maxlen 8");
            }
            if(!noffmap) {
                run("techmap " + techMapArgs);
            }

            run("opt_expr -mux_undef");
            run("simplemap");
            if(family != "ap" && family != "pp3") {
                run("opt_expr");
                run("opt_merge");
                run("opt_rmdff");
                run("opt_clean");
                run("opt");
            }
        }

        if (check_label("map_luts")) {
            std::string techMapArgs = " -map +/quicklogic/" + family + "_latches_map.v";
            if(family != "qlf_k4n8") {
                run("techmap " + techMapArgs);

                if (abcOpt) {
                    std::string lutDefs = "+/quicklogic/" + family + "_lutdefs.txt";
                    rewrite_filename(lutDefs);

                    std::string abcArgs = "+read_lut," + lutDefs + ";"
                        "strash;ifraig;scorr;dc2;dretime;strash;dch,-f;if;mfs2;" // Common Yosys ABC script
                        "sweep;eliminate;if;mfs;lutpack;" // Optimization script
                        "dress"; // "dress" to preserve names

                    run("abc -script " + abcArgs);
                } else {
                    if (family == "pp3" || family == "ap") {
                        run("abc -luts 1,2,2,4:8");
                    } else if (family == "ap2") {
                        run("abc -dress -lut 4:5 -dff"); //-luts 5,4,4,1,3
                    } else {
                        run("abc -dress -lut 4 -dff");
                    }
                }
            } else {
                run("abc -lut 4 ");
            }

            techMapArgs = " -map +/quicklogic/" + family + "_ffs_map.v";

            if (family == "qlf_k4n8") {
                run("shregmap -minlen 8 -maxlen 8");
            }
            if(!noffmap) {
                run("techmap " + techMapArgs);
	    }
	
            run("clean");
            if(family != "pp3" && family != "ap") {
                run("opt_lut");
            }
        }

        if (check_label("map_cells")) {

            std::string techMapArgs;
            if(family != "qlf_k4n8") {
                techMapArgs = " -map +/quicklogic/" + family + "_cells_map.v";
            }
            if(family == "qlf_k4n8" && family != "pp3" && family != "ap") {
                if(family == "qlf_k4n8") {
                    techMapArgs += " -D NO_LUT -map +/quicklogic/ap3_lut_map.v";
                } else {
                    techMapArgs += " -D NO_LUT -map +/quicklogic/" + family + "_lut_map.v";
                }
            } else {
                techMapArgs += " -map +/quicklogic/" + family + "_lut_map.v";
            }
            run("techmap" + techMapArgs);
            run("clean");
            if (!edif_file.empty()) {
                run("quicklogic_eqn");
            }
        }

        if (check_label("check")) {
            run("autoname");
            run("hierarchy -check");
            run("stat");
            run("check -noinit");
        }

        if (check_label("iomap")) {
            if (family == "pp3") {
                run("clkbufmap -buf $_BUF_ Y:A -inpad ckpad Q:P");
                run("iopadmap -bits -outpad outpad A:P -inpad inpad Q:P -tinoutpad bipad EN:Q:A:P A:top");
            } else {
                if (family != "qlf_k4n8") {
                    run("clkbufmap -buf $_BUF_ Y:A -inpad ck_buff Q:A");
                    string ioTechmapFile;
                    if(infer_dbuff) {
                        run("iopadmap -bits -outpad $__out_buff A:Q -inpad $__in_buff Q:A");
                        ioTechmapFile = family + "_io_map_dbuff.v";
                        std::string techMapArgs = " -map +/quicklogic/" + ioTechmapFile + " -autoproc ";
                        run("techmap" + techMapArgs);
                    } else {
                        run("iopadmap -bits -outpad out_buff A:Q -inpad in_buff Q:A");
                    }
                }
            } 
        }

        if (check_label("finalize")) {
            if(family != "qlf_k4n8") {
                run("splitnets -ports -format ()");
                run("setundef -zero -params -undriven");
                run("hilomap -hicell logic_1 a -locell logic_0 a -singleton A:top");
                run("opt_clean -purge");
            }
            run("check");
        }

        if (check_label("edif")) {
            if (!edif_file.empty() || help_mode)
                run(stringf("write_edif -nogndvcc -attrprop -pvector par %s %s", this->currmodule.c_str(), edif_file.c_str()));
        }

        if (check_label("blif")) {
            if (!blif_file.empty() || help_mode) {
                if(family == "qlf_k4n8" && family != "pp3") {
                    run(stringf("opt_clean -purge"),
                            "                                 (qlf_k4n8 mode)");
                    if (inferAdder) {
                        run(stringf("write_blif -param %s", help_mode ? "<file-name>" : blif_file.c_str()));
                    } else {
                        run(stringf("write_blif %s", help_mode ? "<file-name>" : blif_file.c_str()));
                    }

                } else {
                    run(stringf("write_blif -attr -param %s %s", top_opt.c_str(), blif_file.c_str()));
                }
            }
        }

        if (check_label("verilog")) {
            if (!verilog_file.empty()) {
                run("write_verilog -noattr -nohex " + verilog_file);
            }
        }
    }

} SynthQuicklogicPass;

PRIVATE_NAMESPACE_END

/*
 *  yosys -- Yosys Open SYnthesis Suite
 *
 *  Copyright (C) 2012  Clifford Wolf <clifford@clifford.at>
 *  Copyright (C) 2020  The Symbiflow Authors
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

#include "get_cells.h"
#include "get_count.h"
#include "get_nets.h"
#include "get_pins.h"
#include "get_ports.h"
#include "selection_to_tcl_list.h"

USING_YOSYS_NAMESPACE

PRIVATE_NAMESPACE_BEGIN

struct DesignIntrospection {
    DesignIntrospection() {}
    GetNets get_nets_cmd;
    GetPorts get_ports_cmd;
    GetCells get_cells_cmd;
    GetPins get_pins_cmd;
    GetCount get_count_cmd;
    SelectionToTclList selection_to_tcl_list_cmd;
} DesignIntrospection;

PRIVATE_NAMESPACE_END

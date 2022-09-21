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
#include "get_pins.h"

USING_YOSYS_NAMESPACE

std::string GetPins::TypeName() { return "pin"; }

std::string GetPins::SelectionType() { return "c"; }

void GetPins::ExecuteSelection(RTLIL::Design *design, const CommandArgs &args)
{
    (void)design;
    (void)args;
}

GetPins::SelectionObjects GetPins::ExtractSelection(RTLIL::Design *design, const CommandArgs &args)
{
    SelectionObjects selected_objects;
    for (auto obj : args.selection_objects) {
        size_t port_separator = obj.find_last_of('/');
        std::string cell = obj.substr(0, port_separator);
        std::string port = obj.substr(port_separator + 1);
        SelectionObjects selection{RTLIL::unescape_id(design->top_module()->name) + "/" + SelectionType() + ":" + cell};
        extra_args(selection, 0, design);
        ExtractSingleSelection(selected_objects, design, port, args);
    }
    if (selected_objects.size() == 0 and !args.is_quiet) {
        log_warning("Couldn't find matching pin.\n");
    }
    return selected_objects;
}

void GetPins::ExtractSingleSelection(SelectionObjects &objects, RTLIL::Design *design, const std::string &port_name, const CommandArgs &args)
{
    if (design->selected_modules().empty()) {
        if (!args.is_quiet) {
            log_warning("Specified %s not found in design\n", TypeName().c_str());
        }
    }
    for (auto module : design->selected_modules()) {
        for (auto cell : module->selected_cells()) {
            if (!cell->hasPort(RTLIL::escape_id(port_name))) {
                continue;
            }
            if (args.filters.size() > 0) {
                Filter filter = args.filters.at(0);
                std::string attr_value = cell->get_string_attribute(RTLIL::IdString(RTLIL::escape_id(filter.first)));
                if (attr_value.compare(filter.second)) {
                    continue;
                }
            }
            std::string pin_name(RTLIL::unescape_id(cell->name) + "/" + port_name);
            objects.push_back(pin_name);
        }
    }
}

# This script segment is generated automatically by AutoPilot

set axilite_register_dict [dict create]
set port_control {
x0 { 
	dir I
	width 64
	depth 1
	mode ap_none
	offset 16
	offset_end 27
}
x1 { 
	dir I
	width 64
	depth 1
	mode ap_none
	offset 28
	offset_end 39
}
x2 { 
	dir I
	width 64
	depth 1
	mode ap_none
	offset 40
	offset_end 51
}
x3 { 
	dir I
	width 64
	depth 1
	mode ap_none
	offset 52
	offset_end 63
}
w { 
	dir I
	width 64
	depth 1
	mode ap_none
	offset 64
	offset_end 75
}
y { 
	dir I
	width 64
	depth 1
	mode ap_none
	offset 76
	offset_end 87
}
ap_start { }
ap_done { }
ap_ready { }
ap_continue { }
ap_idle { }
}
dict set axilite_register_dict control $port_control



# Definitional proc to organize widgets for parameters.
proc init_gui { IPINST } {
  ipgui::add_param $IPINST -name "Component_Name"
  #Adding Page
  set Page_0 [ipgui::add_page $IPINST -name "Page 0"]
  ipgui::add_param $IPINST -name "ANODES_0" -parent ${Page_0}
  ipgui::add_param $IPINST -name "ANODES_1" -parent ${Page_0}
  ipgui::add_param $IPINST -name "C_S00_AXI_ADDR_WIDTH" -parent ${Page_0}
  ipgui::add_param $IPINST -name "C_S00_AXI_DATA_WIDTH" -parent ${Page_0}
  ipgui::add_param $IPINST -name "SEGMENTS_0" -parent ${Page_0}
  ipgui::add_param $IPINST -name "SEGMENTS_1" -parent ${Page_0}


}

proc update_PARAM_VALUE.ANODES_0 { PARAM_VALUE.ANODES_0 } {
	# Procedure called to update ANODES_0 when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.ANODES_0 { PARAM_VALUE.ANODES_0 } {
	# Procedure called to validate ANODES_0
	return true
}

proc update_PARAM_VALUE.ANODES_1 { PARAM_VALUE.ANODES_1 } {
	# Procedure called to update ANODES_1 when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.ANODES_1 { PARAM_VALUE.ANODES_1 } {
	# Procedure called to validate ANODES_1
	return true
}

proc update_PARAM_VALUE.C_S00_AXI_ADDR_WIDTH { PARAM_VALUE.C_S00_AXI_ADDR_WIDTH } {
	# Procedure called to update C_S00_AXI_ADDR_WIDTH when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.C_S00_AXI_ADDR_WIDTH { PARAM_VALUE.C_S00_AXI_ADDR_WIDTH } {
	# Procedure called to validate C_S00_AXI_ADDR_WIDTH
	return true
}

proc update_PARAM_VALUE.C_S00_AXI_DATA_WIDTH { PARAM_VALUE.C_S00_AXI_DATA_WIDTH } {
	# Procedure called to update C_S00_AXI_DATA_WIDTH when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.C_S00_AXI_DATA_WIDTH { PARAM_VALUE.C_S00_AXI_DATA_WIDTH } {
	# Procedure called to validate C_S00_AXI_DATA_WIDTH
	return true
}

proc update_PARAM_VALUE.SEGMENTS_0 { PARAM_VALUE.SEGMENTS_0 } {
	# Procedure called to update SEGMENTS_0 when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.SEGMENTS_0 { PARAM_VALUE.SEGMENTS_0 } {
	# Procedure called to validate SEGMENTS_0
	return true
}

proc update_PARAM_VALUE.SEGMENTS_1 { PARAM_VALUE.SEGMENTS_1 } {
	# Procedure called to update SEGMENTS_1 when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.SEGMENTS_1 { PARAM_VALUE.SEGMENTS_1 } {
	# Procedure called to validate SEGMENTS_1
	return true
}


proc update_MODELPARAM_VALUE.ANODES_0 { MODELPARAM_VALUE.ANODES_0 PARAM_VALUE.ANODES_0 } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.ANODES_0}] ${MODELPARAM_VALUE.ANODES_0}
}

proc update_MODELPARAM_VALUE.ANODES_1 { MODELPARAM_VALUE.ANODES_1 PARAM_VALUE.ANODES_1 } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.ANODES_1}] ${MODELPARAM_VALUE.ANODES_1}
}

proc update_MODELPARAM_VALUE.SEGMENTS_0 { MODELPARAM_VALUE.SEGMENTS_0 PARAM_VALUE.SEGMENTS_0 } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.SEGMENTS_0}] ${MODELPARAM_VALUE.SEGMENTS_0}
}

proc update_MODELPARAM_VALUE.SEGMENTS_1 { MODELPARAM_VALUE.SEGMENTS_1 PARAM_VALUE.SEGMENTS_1 } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.SEGMENTS_1}] ${MODELPARAM_VALUE.SEGMENTS_1}
}

proc update_MODELPARAM_VALUE.C_S00_AXI_DATA_WIDTH { MODELPARAM_VALUE.C_S00_AXI_DATA_WIDTH PARAM_VALUE.C_S00_AXI_DATA_WIDTH } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.C_S00_AXI_DATA_WIDTH}] ${MODELPARAM_VALUE.C_S00_AXI_DATA_WIDTH}
}

proc update_MODELPARAM_VALUE.C_S00_AXI_ADDR_WIDTH { MODELPARAM_VALUE.C_S00_AXI_ADDR_WIDTH PARAM_VALUE.C_S00_AXI_ADDR_WIDTH } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.C_S00_AXI_ADDR_WIDTH}] ${MODELPARAM_VALUE.C_S00_AXI_ADDR_WIDTH}
}


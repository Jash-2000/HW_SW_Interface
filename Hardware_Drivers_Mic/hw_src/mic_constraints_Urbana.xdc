##Omnidirectional Microphone (Urbana)

set_property -dict { PACKAGE_PIN E12    IOSTANDARD LVCMOS33 } [get_ports { mic_clk }]; #IO_25_35 Sch=m_clk
set_property -dict { PACKAGE_PIN D12    IOSTANDARD LVCMOS33 } [get_ports { mic_data_in }]; #IO_L24N_T3_35 Sch=m_data
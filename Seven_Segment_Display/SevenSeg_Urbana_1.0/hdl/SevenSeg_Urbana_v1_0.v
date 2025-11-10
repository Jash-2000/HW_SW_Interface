
`timescale 1 ns / 1 ps

	module SevenSeg_Urbana_v1_0 #
	(
		// Users to add parameters here

		// User parameters ends
		// Do not modify the parameters beyond this line
		parameter integer ANODES_0 		= 4,
		parameter integer ANODES_1 		= 4,
		parameter integer SEGMENTS_0	= 8,
		parameter integer SEGMENTS_1	= 8,

		// Parameters of Axi Slave Bus Interface S00_AXI
		parameter integer C_S00_AXI_DATA_WIDTH	= 32,
		parameter integer C_S00_AXI_ADDR_WIDTH	= 4
	)
	(
		// Users to add ports here
		output wire [ANODES_0 - 1 : 0] D0_AN,
		output wire [ANODES_1 - 1 : 0] D1_AN,
		output wire [SEGMENTS_0 - 1 : 0] D0_SEG,
		output wire [SEGMENTS_1 - 1 : 0] D1_SEG,

		// User ports ends
		// Do not modify the ports beyond this line
		

		// Ports of Axi Slave Bus Interface S00_AXI
		input wire  s00_axi_aclk,
		input wire  s00_axi_aresetn,
		input wire [C_S00_AXI_ADDR_WIDTH-1 : 0] s00_axi_awaddr,
		input wire [2 : 0] s00_axi_awprot,
		input wire  s00_axi_awvalid,
		output wire  s00_axi_awready,
		input wire [C_S00_AXI_DATA_WIDTH-1 : 0] s00_axi_wdata,
		input wire [(C_S00_AXI_DATA_WIDTH/8)-1 : 0] s00_axi_wstrb,
		input wire  s00_axi_wvalid,
		output wire  s00_axi_wready,
		output wire [1 : 0] s00_axi_bresp,
		output wire  s00_axi_bvalid,
		input wire  s00_axi_bready,
		input wire [C_S00_AXI_ADDR_WIDTH-1 : 0] s00_axi_araddr,
		input wire [2 : 0] s00_axi_arprot,
		input wire  s00_axi_arvalid,
		output wire  s00_axi_arready,
		output wire [C_S00_AXI_DATA_WIDTH-1 : 0] s00_axi_rdata,
		output wire [1 : 0] s00_axi_rresp,
		output wire  s00_axi_rvalid,
		input wire  s00_axi_rready
	);
// Instantiation of Axi Bus Interface S00_AXI
	SevenSeg_Urbana_v1_0_S00_AXI # ( 
		.C_S_AXI_DATA_WIDTH(C_S00_AXI_DATA_WIDTH),
		.C_S_AXI_ADDR_WIDTH(C_S00_AXI_ADDR_WIDTH)
	) SevenSeg_Urbana_v1_0_S00_AXI_inst (
		.S_AXI_ACLK(s00_axi_aclk),
		.S_AXI_ARESETN(s00_axi_aresetn),
		.S_AXI_AWADDR(s00_axi_awaddr),
		.S_AXI_AWPROT(s00_axi_awprot),
		.S_AXI_AWVALID(s00_axi_awvalid),
		.S_AXI_AWREADY(s00_axi_awready),
		.S_AXI_WDATA(s00_axi_wdata),
		.S_AXI_WSTRB(s00_axi_wstrb),
		.S_AXI_WVALID(s00_axi_wvalid),
		.S_AXI_WREADY(s00_axi_wready),
		.S_AXI_BRESP(s00_axi_bresp),
		.S_AXI_BVALID(s00_axi_bvalid),
		.S_AXI_BREADY(s00_axi_bready),
		.S_AXI_ARADDR(s00_axi_araddr),
		.S_AXI_ARPROT(s00_axi_arprot),
		.S_AXI_ARVALID(s00_axi_arvalid),
		.S_AXI_ARREADY(s00_axi_arready),
		.S_AXI_RDATA(s00_axi_rdata),
		.S_AXI_RRESP(s00_axi_rresp),
		.S_AXI_RVALID(s00_axi_rvalid),
		.S_AXI_RREADY(s00_axi_rready),
		.data_out(data_out)
	);

	// Add user logic here
	reg [SEGMENTS_0 - 1 : 0] seg_0;
	reg [ANODES_0 - 1 : 0] an_0;
	reg [SEGMENTS_1 - 1 : 0] seg_1;
	reg [ANODES_1 - 1 : 0] an_1;
	wire [C_S00_AXI_DATA_WIDTH - 1 : 0] data_out;

	always @(posedge s00_axi_aclk or negedge s00_axi_aresetn) begin
		if(~s00_axi_aresetn) begin
			seg_0 <= 0;
			an_0  <= 0;
			seg_1 <= 1;
			an_1  <= 1;
		end else begin
			seg_0 <= data_out[SEGMENTS_0 - 1 : 0];
			seg_1 <= data_out[SEGMENTS_1 + SEGMENTS_0 - 1 : SEGMENTS_0];
			an_0  <= data_out[SEGMENTS_1 + SEGMENTS_0 + ANODES_0 - 1 : SEGMENTS_0 + SEGMENTS_1];
			an_1  <= data_out[SEGMENTS_1 + SEGMENTS_0 + ANODES_0 + ANODES_1 - 1 : SEGMENTS_0 + SEGMENTS_1 + ANODES_0];
		end
	end

	assign D0_AN = an_0;
	assign D1_AN = an_1;
	assign D0_SEG = seg_0;
	assign D1_SEG = seg_1;

	// User logic ends

	endmodule

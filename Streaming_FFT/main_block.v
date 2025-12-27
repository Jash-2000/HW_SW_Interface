//----------------------------------------------------------------------
//  Main Module: FFT Processing with AXI Interface
//----------------------------------------------------------------------
module main_block #(
    parameter WIDTH = 32,
    parameter FFT_SIZE = 1024,
    parameter HALF_SIZE = 513
)(
    input wire          clk,
    input wire          rst_n,
    
    // AXI Stream Input Interface
    input wire  [WIDTH-1:0] s_axis_data,
    input wire              s_axis_valid,
    output reg              s_axis_ready,
    
    // AXI-Lite Slave Interface (for CPU access)
    input wire  [31:0]      s_axi_araddr,
    input wire              s_axi_arvalid,
    output reg              s_axi_arready,
    output reg  [31:0]      s_axi_rdata,
    output reg  [1:0]       s_axi_rresp,
    output reg              s_axi_rvalid,
    input wire              s_axi_rready
);

    //------------------------------------------------------------------
    //  Internal Signals
    //------------------------------------------------------------------
    reg         reset;
    wire        fft_di_en;
    wire [WIDTH-1:0] fft_di_re;
    wire [WIDTH-1:0] fft_di_im;
    wire        fft_do_en;
    wire [WIDTH-1:0] fft_do_re;
    wire [WIDTH-1:0] fft_do_im;
    
    reg  [9:0]  input_count;
    reg  [9:0]  output_count;
    wire [9:0]  bit_reversed_addr;  // Changed to wire
    
    // Storage for half FFT bins (513 bins)
    reg  [WIDTH-1:0] fft_bins_re [0:HALF_SIZE-1];
    reg  [WIDTH-1:0] fft_bins_im [0:HALF_SIZE-1];
    reg  [WIDTH-1:0] fft_magnitude [0:HALF_SIZE-1];
    
    // Max bin tracking
    reg  [9:0]  max_bin_index;
    reg  [WIDTH-1:0] max_magnitude;
    reg         processing_done;
    
    integer i;
    
    //------------------------------------------------------------------
    //  Reset Logic
    //------------------------------------------------------------------
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n)
            reset <= 1'b1;
        else
            reset <= 1'b0;
    end
    
    //------------------------------------------------------------------
    //  Input State Machine
    //------------------------------------------------------------------
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            input_count <= 10'd0;
            s_axis_ready <= 1'b1;
        end else begin
            if (s_axis_valid && s_axis_ready) begin
                if (input_count == FFT_SIZE - 1) begin
                    input_count <= 10'd0;
                    s_axis_ready <= 1'b1;
                end else begin
                    input_count <= input_count + 1'b1;
                end
            end
        end
    end
    
    // Connect to FFT input
    assign fft_di_en = s_axis_valid && s_axis_ready;
    assign fft_di_re = s_axis_data;
    assign fft_di_im = 32'd0;  // All real input
    
    //------------------------------------------------------------------
    //  FFT Module Instance
    //------------------------------------------------------------------
    FFT #(
        .WIDTH(WIDTH)
    ) fft_inst (
        .clock(clk),
        .reset(reset),
        .di_en(fft_di_en),
        .di_re(fft_di_re),
        .di_im(fft_di_im),
        .do_en(fft_do_en),
        .do_re(fft_do_re),
        .do_im(fft_do_im)
    );
      
    //----------------------------------------------------------------
    // Bit Reversal Module
    //-----------------------------------------------------------------
    bit_reverse_addr bra(
        .addr(output_count),
        .addr_rev(bit_reversed_addr)
    );  

    //------------------------------------------------------------------
    //  Magnitude Calculation and Max Finding
    //------------------------------------------------------------------
    reg [1:0] mag_state;
    reg [9:0] mag_index;
    reg [WIDTH-1:0] re_squared, im_squared;
    reg [WIDTH-1:0] magnitude_temp;
    
    localparam MAG_IDLE = 2'd0;
    localparam MAG_CALC = 2'd1;
    localparam MAG_FIND_MAX = 2'd2;
    localparam MAG_DONE = 2'd3;

    //------------------------------------------------------------------
    //  Output Processing: Capture and Reorder FFT Output
    //------------------------------------------------------------------
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            output_count <= 10'd0;
            processing_done <= 1'b0;
        end else begin
            if (fft_do_en) begin
                // Store only first half (513 bins for real input)
                if (bit_reversed_addr < HALF_SIZE) begin
                    // Scale up from 1/N by left shift 10 (multiply by 1024)
                    fft_bins_re[bit_reversed_addr] <= fft_do_re <<< 10;
                    fft_bins_im[bit_reversed_addr] <= fft_do_im <<< 10;
                end
                
                if (output_count == FFT_SIZE - 1) begin
                    output_count <= 10'd0;
                    processing_done <= 1'b1;  // Trigger magnitude calculation
                end else begin
                    output_count <= output_count + 1'b1;
                end
            end else if (mag_state == MAG_DONE) begin
                processing_done <= 1'b0;
            end
        end
    end
    
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            mag_state <= MAG_IDLE;
            mag_index <= 10'd0;
            max_bin_index <= 10'd0;
            max_magnitude <= 32'd0;
        end else begin
            case (mag_state)
                MAG_IDLE: begin
                    if (processing_done) begin
                        mag_state <= MAG_CALC;
                        mag_index <= 10'd0;
                        max_magnitude <= 32'd0;
                    end
                end
                
                MAG_CALC: begin
                    // simple L1 norm calculated for simplicity
                    re_squared = (fft_bins_re[mag_index][WIDTH-1]) ? 
                                 -fft_bins_re[mag_index] : fft_bins_re[mag_index];
                    im_squared = (fft_bins_im[mag_index][WIDTH-1]) ? 
                                 -fft_bins_im[mag_index] : fft_bins_im[mag_index];
                    magnitude_temp = re_squared + im_squared;
                    fft_magnitude[mag_index] <= magnitude_temp;
                    mag_state <= MAG_FIND_MAX;
                end
                
                MAG_FIND_MAX: begin
                    if (fft_magnitude[mag_index] > max_magnitude) begin
                        max_magnitude <= fft_magnitude[mag_index];
                        max_bin_index <= mag_index;
                    end
                    
                    if (mag_index == HALF_SIZE - 1) begin
                        mag_state <= MAG_DONE;
                    end else begin
                        mag_index <= mag_index + 1'b1;
                        mag_state <= MAG_CALC;
                    end
                end
                
                MAG_DONE: begin
                    // Stay here until processing_done is cleared
                    if (!processing_done)
                        mag_state <= MAG_IDLE;
                end
            endcase
        end
    end
    
    //------------------------------------------------------------------
    //  AXI-Lite Slave Interface for CPU Access
    //------------------------------------------------------------------
    localparam ADDR_MAX_BIN = 32'h0000;
    localparam ADDR_MAX_MAG = 32'h0004;
    localparam ADDR_STATUS  = 32'h0008;
    
    // Read Address Channel - Fixed: removed assign from always block
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            s_axi_arready <= 1'b0;
        end else begin
            s_axi_arready <= 1'b1;
        end
    end
    
    // Read Data Channel
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            s_axi_rdata <= 32'd0;
            s_axi_rresp <= 2'b00;
            s_axi_rvalid <= 1'b0;
        end else begin
            if (s_axi_arvalid && s_axi_arready && !s_axi_rvalid) begin
                // New read request
                s_axi_rvalid <= 1'b1;
                s_axi_rresp <= 2'b00;  // OKAY response
                
                case (s_axi_araddr[5:2])
                    4'h0: s_axi_rdata <= {22'd0, max_bin_index}; //ADDR_MAX_BIN
                    4'h1: s_axi_rdata <= max_magnitude; // ADDR_MAX_MAG
                    4'h2:  s_axi_rdata <= {30'd0, mag_state}; // ADDR_STATUS
                    default: begin
                        s_axi_rdata <= 32'd0;
                        s_axi_rresp <= 2'b10;  // SLVERR
                    end
                endcase
            end else if (s_axi_rready && s_axi_rvalid) begin
                // Read data accepted by master
                s_axi_rvalid <= 1'b0;
            end
        end
    end

endmodule
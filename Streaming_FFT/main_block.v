`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Author: Jash Shah
// Date Modified: 23 Dec, 2025
// Description: Main module for wake word detection system
//              - Receives microphone data via AXI stream
//              - Performs FFT processing
//              - Applies 5-way averaging
//              - Stores processed data in BRAM
//              - Interfaces with Wake_Word detection module
//              - CPU control via AXI-Lite interface
//////////////////////////////////////////////////////////////////////////////////
/*
| CPU action      | Write location | Data | Hardware effect           |
| --------------- | -------------- | ---- | ------------------------- |
| Start detection | 0x00           | 1    | FSM starts audio pipeline |
| Stop detection  | 0x04           | 1    | FSM aborts to IDLE        |
| Check state     | 0x08           | —    | Observe FSM progress      |
| Read result     | 0x0C           | —    | Wake-word classification  |
| Monitor frames  | 0x10           | —    | Debug/progress info       |
*/

module wake_word_detector #(
    parameter WIDTH = 32,   // Width of a single data
    parameter FFT_SIZE = 1024,  // 1024 point FFT implemented
    parameter REDUCED_FFT_SIZE = 515,  // 513 for real outputs + 2 padding
    parameter AVERAGED_SIZE = 103,      // After 5-way averaging
    parameter NUM_FRAMES = 20,  // Number of frames required for successful wake-word detection
    parameter BRAM_SIZE = 2060  // 103 * 20
)(
    (* X_INTERFACE_INFO = "xilinx.com:signal:clock:1.0 sys_clk CLK" *)
    (* X_INTERFACE_PARAMETER = "ASSOCIATED_BUSIF s_axi_cpu:s_axis_stream, ASSOCIATED_RESET sys_rst, FREQ_HZ 100000000" *)
    input wire sys_clk,
    input wire sys_rst,
    
    // AXI Stream interface (from microphone)
    input wire [31:0] s_axis_stream_tdata,
    input wire s_axis_stream_tvalid,
    output wire s_axis_stream_tready,
    
    // AXI-Lite CPU interface
    input  wire [11:0]  s_axi_cpu_awaddr,
    input  wire         s_axi_cpu_awvalid,
    output wire         s_axi_cpu_awready,
    input  wire [31:0]  s_axi_cpu_wdata,
    input  wire         s_axi_cpu_wvalid,
    output wire         s_axi_cpu_wready,
    output wire [1:0]   s_axi_cpu_bresp,
    output wire         s_axi_cpu_bvalid,
    input  wire         s_axi_cpu_bready,
    input  wire [11:0]  s_axi_cpu_araddr,
    input  wire         s_axi_cpu_arvalid,
    output wire         s_axi_cpu_arready,
    output wire [31:0]  s_axi_cpu_rdata,
    output wire [1:0]   s_axi_cpu_rresp,
    output wire         s_axi_cpu_rvalid,
    input  wire         s_axi_cpu_rready,
    
    // Interrupt output
    output reg interrupt
);

    //==========================================================================
    // State Machine States
    //==========================================================================
    localparam STATE_IDLE           = 4'd0;
    localparam STATE_COLLECT_FFT    = 4'd1;
    localparam STATE_WAIT_FFT       = 4'd2;
    localparam STATE_REORDER        = 4'd3;
    localparam STATE_AVERAGE        = 4'd4;
    localparam STATE_STORE_BRAM     = 4'd5;
    localparam STATE_CHECK_FRAMES   = 4'd6;
    localparam STATE_CALL_WAKEWORD  = 4'd7;
    localparam STATE_WAIT_WAKEWORD  = 4'd8;
    localparam STATE_DONE           = 4'd9;
    
    reg [3:0] state;
    reg [3:0] next_state;
    
    //==========================================================================
    // Control Registers (CPU accessible)
    //==========================================================================
    reg start_processing;      // Register 0: Write 1 to start
    reg stop_processing;       // Register 1: Write 1 to stop
    reg [31:0] status_reg;     // Register 2: Status
    reg [31:0] result_reg;     // Register 3: Wake word result
    reg [31:0] frame_count;    // Register 4: Current frame count
    
    //==========================================================================
    // FFT Module Signals
    //==========================================================================
    reg fft_di_en;
    reg [WIDTH-1:0] fft_di_re;
    reg [WIDTH-1:0] fft_di_im;
    wire fft_do_en;
    wire [WIDTH-1:0] fft_do_re;
    wire [WIDTH-1:0] fft_do_im;
    
    //==========================================================================
    // Bit Reversal Buffer
    //==========================================================================
    reg [WIDTH-1:0] reorder_buffer_re [0:FFT_SIZE-1];
    reg [WIDTH-1:0] reorder_buffer_im [0:FFT_SIZE-1];
    reg [9:0] reorder_write_idx;
    reg [9:0] reorder_read_idx;
    reg [9:0] address_rev;
    reg reorder_complete;
    
    //==========================================================================
    // Averaging Logic
    //==========================================================================
    reg [WIDTH-1:0] avg_buffer [0:REDUCED_FFT_SIZE-1];
    reg [31:0] avg_accumulator_re;
    reg [31:0] avg_accumulator_im;
    reg [31:0] avg_magnitude;
    reg [9:0] avg_input_idx;
    reg [6:0] avg_output_idx;
    reg avg_complete;
    
    //==========================================================================
    // BRAM Interface
    //==========================================================================
    reg bram_we;
    reg [11:0] bram_addr;
    reg [31:0] bram_din;
    wire [31:0] bram_dout;
    
    //==========================================================================
    // Frame Counter
    //==========================================================================
    reg [4:0] current_frame;
    
    //==========================================================================
    // FFT Input Counter
    //==========================================================================
    reg [9:0] fft_input_counter;
    reg [9:0] fft_output_counter;
    
    //==========================================================================
    // Wake Word Module Interface
    //==========================================================================
    wire ww_done;
    wire [31:0] ww_result;
    reg ww_bram_req;
    wire [11:0] ww_bram_addr;
    assign ww_bram_addr = bram_addr;
    assign bram_dout = bram_din;

    //==========================================================================
    // AXI Stream Control
    //==========================================================================
    assign s_axis_stream_tready = (state == STATE_COLLECT_FFT) ? 1'b1 : 1'b1;
    
    //==========================================================================
    // FFT Module Instantiation
    //==========================================================================
    FFT #(
        .WIDTH(WIDTH)
    ) fft_inst (
        .clock(sys_clk),
        .reset(~sys_rst),
        .di_en(fft_di_en),
        .di_re(fft_di_re),
        .di_im(fft_di_im),
        .do_en(fft_do_en),
        .do_re(fft_do_re),
        .do_im(fft_do_im)
    );
    
    //==========================================================================
    // Bit Reversal Module Instantiation
    //==========================================================================
    bit_reverse_addr #(.ADDR_WIDTH(10)) u_rev (
        .addr(reorder_read_idx),
        .addr_rev(address_rev)
    );

    //==========================================================================
    // Wake Word Module Instantiation (Placeholder)
    //==========================================================================
    Wake_Word wake_word_inst (
        .clock(sys_clk),
        .reset(~sys_rst),
        .done(ww_done),
        .result(ww_result),
        .bram_addr(ww_bram_addr),
        .bram_data(bram_dout)
    );
    
    //==========================================================================
    // BRAM Instantiation
    //==========================================================================
    xpm_memory_sdpram #(
        .MEMORY_SIZE(BRAM_SIZE * 32),
        .MEMORY_PRIMITIVE("auto"),
        .CLOCKING_MODE("common_clock"),
        .MEMORY_INIT_FILE("none"),
        .MEMORY_INIT_PARAM(""),
        .USE_MEM_INIT(1),
        .WAKEUP_TIME("disable_sleep"),
        .MESSAGE_CONTROL(0),
        .WRITE_DATA_WIDTH_A(32),
        .BYTE_WRITE_WIDTH_A(32),
        .ADDR_WIDTH_A(12),
        .READ_DATA_WIDTH_B(32),
        .ADDR_WIDTH_B(12),
        .READ_RESET_VALUE_B("0"),
        .READ_LATENCY_B(2),
        .WRITE_MODE_B("no_change")
    ) bram_inst (
        .sleep(1'b0),
        .clka(sys_clk),
        .ena(bram_we || ww_bram_req),
        .wea(bram_we),
        .addra(bram_addr),
        .dina(bram_din),
        .injectsbiterra(1'b0),
        .injectdbiterra(1'b0),
        .clkb(sys_clk),
        .rstb(1'b0),
        .enb(1'b1),
        .regceb(1'b1),
        .addrb(ww_bram_addr),
        .doutb(bram_dout),
        .sbiterrb(),
        .dbiterrb()
    );
    
    //==========================================================================
    // Bit Reversal Function
    //==========================================================================
    function [9:0] bit_reverse;
        input [9:0] index;
        integer i;
        begin
            bit_reverse = 10'd0;
            for (i = 0; i < 10; i = i + 1) begin
                bit_reverse[i] = index[9-i];
            end
        end
    endfunction
    
    //==========================================================================
    // Main State Machine
    //==========================================================================
    always @(posedge sys_clk) begin
        if (!sys_rst) begin
            state <= STATE_IDLE;
        end else begin
            state <= next_state;
        end
    end
    
    always @(*) begin
        next_state = state;
        case (state)
            STATE_IDLE: begin
                if (start_processing && !stop_processing)
                    next_state = STATE_COLLECT_FFT;
            end
            
            STATE_COLLECT_FFT: begin
                if (stop_processing)
                    next_state = STATE_IDLE;
                else if (fft_input_counter >= FFT_SIZE)
                    next_state = STATE_WAIT_FFT;
            end
            
            STATE_WAIT_FFT: begin
                if (stop_processing)
                    next_state = STATE_IDLE;
                else if (fft_output_counter >= FFT_SIZE)
                    next_state = STATE_REORDER;
            end
            
            STATE_REORDER: begin
                if (stop_processing)
                    next_state = STATE_IDLE;
                else if (reorder_complete)
                    next_state = STATE_AVERAGE;
            end
            
            STATE_AVERAGE: begin
                if (stop_processing)
                    next_state = STATE_IDLE;
                else if (avg_complete)
                    next_state = STATE_STORE_BRAM;
            end
            
            STATE_STORE_BRAM: begin
                if (stop_processing)
                    next_state = STATE_IDLE;
                else
                    next_state = STATE_CHECK_FRAMES;
            end
            
            STATE_CHECK_FRAMES: begin
                if (stop_processing)
                    next_state = STATE_IDLE;
                else if (current_frame >= NUM_FRAMES)
                    next_state = STATE_CALL_WAKEWORD;
                else
                    next_state = STATE_COLLECT_FFT;
            end
            
            STATE_CALL_WAKEWORD: begin
                next_state = STATE_WAIT_WAKEWORD;
            end
            
            STATE_WAIT_WAKEWORD: begin
                if (ww_done)
                    next_state = STATE_DONE;
            end
            
            STATE_DONE: begin
                next_state = STATE_IDLE;
            end
        endcase
    end
    
    //==========================================================================
    // FFT Input Logic
    //==========================================================================
    always @(posedge sys_clk) begin
        if (!sys_rst) begin
            fft_di_en <= 1'b0;
            fft_di_re <= 32'd0;
            fft_di_im <= 32'd0;
            fft_input_counter <= 10'd0;
        end else begin
            case (state)
                STATE_COLLECT_FFT: begin
                    if (s_axis_stream_tvalid && fft_input_counter < FFT_SIZE) begin
                        fft_di_en <= 1'b1;
                        fft_di_re <= s_axis_stream_tdata;
                        fft_di_im <= 32'd0;
                        fft_input_counter <= fft_input_counter + 1;
                    end else begin
                        fft_di_en <= 1'b0;
                    end
                end
                
                default: begin
                    fft_di_en <= 1'b0;
                    fft_input_counter <= 10'd0;
                end
            endcase
        end
    end
    
    //==========================================================================
    // FFT Output Collection (Bit-Reversed)
    //==========================================================================
    always @(posedge sys_clk) begin
        if (!sys_rst) begin
            fft_output_counter <= 10'd0;
            reorder_write_idx <= 10'd0;
        end else begin
            case (state)
                STATE_WAIT_FFT: begin
                    if (fft_do_en) begin
                        reorder_buffer_re[reorder_write_idx] <= fft_do_re;
                        reorder_buffer_im[reorder_write_idx] <= fft_do_im;
                        reorder_write_idx <= reorder_write_idx + 1;
                        fft_output_counter <= fft_output_counter + 1;
                    end
                end
                
                default: begin
                    fft_output_counter <= 10'd0;
                    reorder_write_idx <= 10'd0;
                end
            endcase
        end
    end
    
    //==========================================================================
    // Reordering Logic (Bit-Reversed to Natural Order)
    //==========================================================================
    always @(posedge sys_clk) begin
        if (!sys_rst) begin
            reorder_read_idx <= 10'd0;
            reorder_complete <= 1'b0;
        end else begin
            case (state)
                STATE_REORDER: begin
                    if (reorder_read_idx < REDUCED_FFT_SIZE - 2) begin
                        avg_buffer[reorder_read_idx] <= reorder_buffer_re[address_rev];
                        reorder_read_idx <= reorder_read_idx + 1;
                    end else if (reorder_read_idx < REDUCED_FFT_SIZE) begin
                        // Pad with zeros
                        avg_buffer[reorder_read_idx] <= 32'd0;
                        reorder_read_idx <= reorder_read_idx + 1;
                    end else begin
                        reorder_complete <= 1'b1;
                    end
                end
                
                default: begin
                    reorder_read_idx <= 10'd0;
                    reorder_complete <= 1'b0;
                end
            endcase
        end
    end
    
    //==========================================================================
    // 5-way Averaging Logic
    //==========================================================================
    always @(posedge sys_clk) begin
        if (!sys_rst) begin
            avg_input_idx <= 10'd0;
            avg_output_idx <= 7'd0;
            avg_complete <= 1'b0;
            avg_magnitude <= 32'd0;
        end else begin
            case (state)
                STATE_AVERAGE: begin
                    if (avg_output_idx < AVERAGED_SIZE) begin
                        // Simple averaging: sum 5 consecutive values
                        if (avg_input_idx == 0) begin
                            avg_magnitude <= avg_buffer[avg_output_idx * 5];
                            avg_input_idx <= 1;
                        end else if (avg_input_idx < 5) begin
                            avg_magnitude <= avg_magnitude + avg_buffer[avg_output_idx * 5 + avg_input_idx];
                            avg_input_idx <= avg_input_idx + 1;
                        end else begin
                            // Divide by 5 (~0.203)
                            avg_magnitude <= (avg_magnitude  * 13) >> 6;  // Approximation
                            avg_input_idx <= 0;
                            avg_output_idx <= avg_output_idx + 1;
                        end
                    end else begin
                        avg_complete <= 1'b1;
                    end
                end
                
                default: begin
                    avg_input_idx <= 10'd0;
                    avg_output_idx <= 7'd0;
                    avg_complete <= 1'b0;
                end
            endcase
        end
    end
    
    //==========================================================================
    // BRAM Storage Logic
    //==========================================================================
    reg [6:0] bram_store_idx;
    
    always @(posedge sys_clk) begin
        if (!sys_rst) begin
            bram_we <= 1'b0;
            bram_addr <= 12'd0;
            bram_din <= 32'd0;
            bram_store_idx <= 7'd0;
        end else begin
            case (state)
                STATE_STORE_BRAM: begin
                    if (bram_store_idx < AVERAGED_SIZE) begin
                        bram_we <= 1'b1;
                        bram_addr <= current_frame * AVERAGED_SIZE + bram_store_idx;
                        bram_din <= avg_buffer[bram_store_idx];
                        bram_store_idx <= bram_store_idx + 1;
                    end else begin
                        bram_we <= 1'b0;
                    end
                end
                
                default: begin
                    bram_we <= 1'b0;
                    bram_store_idx <= 7'd0;
                end
            endcase
        end
    end
    
    //==========================================================================
    // Frame Counter
    //==========================================================================
    always @(posedge sys_clk) begin
        if (!sys_rst || state == STATE_IDLE) begin
            current_frame <= 5'd0;
        end else if (state == STATE_CHECK_FRAMES) begin
            current_frame <= current_frame + 1;
        end
    end
    
    //==========================================================================
    // Wake Word Module Control
    //==========================================================================
    always @(posedge sys_clk) begin
        if (!sys_rst) begin
            result_reg <= 32'd0;
            interrupt <= 1'b0;
        end else begin
            case (state)
                
                STATE_WAIT_WAKEWORD: begin
                    if (ww_done) begin
                        result_reg <= ww_result;
                        if (ww_result == 32'd4) begin
                            interrupt <= 1'b1;
                        end
                    end
                end
                
                STATE_IDLE: begin
                    interrupt <= 1'b0;
                end
            endcase
        end
    end
    
    //==========================================================================
    // AXI-Lite Write Interface
    //==========================================================================
    reg do_write;
    reg write_ready;
    reg [2:0] write_addr;
    reg [31:0] write_data;
    
    wire do_write_next = s_axi_cpu_awvalid && s_axi_cpu_wvalid && write_ready;
    wire write_ready_next = s_axi_cpu_awvalid && s_axi_cpu_wvalid && !write_ready;
    
    always @(posedge sys_clk) begin
        if (!sys_rst) begin
            write_ready <= 1'b0;
            do_write <= 1'b0;
            write_data <= 32'b0;
            write_addr <= 3'b0;
            start_processing <= 1'b0;
            stop_processing <= 1'b0;
        end else begin
            write_ready <= write_ready_next;
            do_write <= do_write_next;
            
            if (do_write_next) begin
                write_data <= s_axi_cpu_wdata;
                write_addr <= s_axi_cpu_awaddr[4:2];
            end
            
            // Register writes
            if (do_write && write_addr == 3'd0) begin
                start_processing <= write_data[0];
            end
            if (do_write && write_addr == 3'd1) begin
                stop_processing <= write_data[0];
            end
            
            // Auto-clear stop after processing
            if (state == STATE_IDLE) begin
                stop_processing <= 1'b0;
            end
        end
    end
    
    assign s_axi_cpu_awready = write_ready;
    assign s_axi_cpu_wready = write_ready;
    
    // Write response
    reg write_ack_valid;
    
    always @(posedge sys_clk) begin
        if (!sys_rst) begin
            write_ack_valid <= 1'b0;
        end else if (do_write) begin
            write_ack_valid <= 1'b1;
        end else if (write_ack_valid && s_axi_cpu_bready) begin
            write_ack_valid <= 1'b0;
        end
    end
    
    assign s_axi_cpu_bvalid = write_ack_valid;
    assign s_axi_cpu_bresp = 2'd0;
    
    //==========================================================================
    // AXI-Lite Read Interface
    //==========================================================================
    reg [2:0] read_addr;
    reg read_ready;
    reg do_read;
    
    wire do_read_next = read_ready && s_axi_cpu_arvalid;
    wire read_ready_next = s_axi_cpu_arvalid && !read_ready;
    
    always @(posedge sys_clk) begin
        if (!sys_rst) begin
            read_ready <= 1'b0;
            do_read <= 1'b0;
            read_addr <= 3'd0;
        end else begin
            read_ready <= read_ready_next;
            do_read <= do_read_next;
            if (do_read_next) begin
                read_addr <= s_axi_cpu_araddr[4:2];
            end
        end
    end
    
    assign s_axi_cpu_arready = read_ready;
    
    // Read data multiplexer
    reg [31:0] read_data;
    reg read_ack_valid;
    
    // Status register encoding
    always @(*) begin
        status_reg = {28'd0, state};
    end
    
    always @(*) begin
        frame_count = {27'd0, current_frame};
    end
    
    wire [31:0] read_data_next = 
        (read_addr == 3'd0) ? {31'd0, start_processing} :
        (read_addr == 3'd1) ? {31'd0, stop_processing} :
        (read_addr == 3'd2) ? status_reg :
        (read_addr == 3'd3) ? result_reg :
        (read_addr == 3'd4) ? frame_count :
        32'd0;
    
    always @(posedge sys_clk) begin
        if (!sys_rst) begin
            read_data <= 32'd0;
            read_ack_valid <= 1'b0;
        end else if (do_read) begin
            read_data <= read_data_next;
            read_ack_valid <= 1'b1;
        end else if (read_ack_valid && s_axi_cpu_rready) begin
            read_ack_valid <= 1'b0;
        end
    end
    
    assign s_axi_cpu_rvalid = read_ack_valid;
    assign s_axi_cpu_rresp = 2'd0;
    assign s_axi_cpu_rdata = read_data;

endmodule
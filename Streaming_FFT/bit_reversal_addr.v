module bit_reverse_addr #(
    parameter ADDR_WIDTH = 10
)(
    input  wire [ADDR_WIDTH-1:0] addr,
    output wire [ADDR_WIDTH-1:0] addr_rev
);

    genvar i;
    generate
        for (i = 0; i < ADDR_WIDTH; i = i + 1) begin : REV
            assign addr_rev[i] = addr[ADDR_WIDTH-1-i];
        end
    endgenerate

endmodule

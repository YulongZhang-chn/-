#Load the synthesized netlist
open_checkpoint ../design.dcp

#Place design using the bookshelf
place_design -placement ../design.pl

#Route design
route_design

#Routing Report
report_route_status

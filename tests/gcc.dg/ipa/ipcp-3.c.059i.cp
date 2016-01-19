
IPA constant propagation start:
Determining dynamic type for call: foo (&gi, _4);
  Starting walk at: foo (&gi, _4);
  instance pointer: &gi  Outer instance pointer: gi offset: 0 (bits) vtbl reference: 
Determining dynamic type for call: foo (&gi, _4);
  Starting walk at: foo (&gi, _4);
  instance pointer: _4  Outer instance pointer: _4 offset: 0 (bits) vtbl reference: 
Determining dynamic type for call: mark_cell (interp_2(D), c_3(D));
  Starting walk at: mark_cell (interp_2(D), c_3(D));
  instance pointer: interp_2(D)  Outer instance pointer: interp_2(D) offset: 0 (bits) vtbl reference: 
Determining dynamic type for call: mark_cell (interp_2(D), c_3(D));
  Starting walk at: mark_cell (interp_2(D), c_3(D));
  instance pointer: c_3(D)  Outer instance pointer: c_3(D) offset: 0 (bits) vtbl reference: 
Determining dynamic type for call: never_ever (interp_7(D), _4);
  Starting walk at: never_ever (interp_7(D), _4);
  instance pointer: interp_7(D)  Outer instance pointer: interp_7(D) offset: 0 (bits) vtbl reference: 
Determining dynamic type for call: never_ever (interp_7(D), _4);
  Starting walk at: never_ever (interp_7(D), _4);
  instance pointer: _4  Outer instance pointer: _4 offset: 0 (bits) vtbl reference: 

IPA structures before propagation:

Jump functions:
  Jump functions of caller  abort/6:
  Jump functions of caller  main/5:
    callsite  main/5 -> foo/3 : 
       param 0: CONST: &gi
         Alignment: 4, misalignment: 0
       param 1: UNKNOWN
         Unknown alignment
    callsite  main/5 -> getnull/4 : 
  Jump functions of caller  getnull/4:
  Jump functions of caller  foo/3:
    callsite  foo/3 -> mark_cell/2 : 
       param 0: PASS THROUGH: 0, op nop_expr, agg_preserved
         Unknown alignment
       param 1: PASS THROUGH: 1, op nop_expr, agg_preserved
         Unknown alignment
  Jump functions of caller  mark_cell/2:
    callsite  mark_cell/2 -> never_ever/1 : 
       param 0: PASS THROUGH: 0, op nop_expr, agg_preserved
         Unknown alignment
       param 1: UNKNOWN
         Unknown alignment
  Jump functions of caller  never_ever/1:
    callsite  never_ever/1 -> abort/6 : 

 Propagating constants:

Not considering main for cloning; no hot calls.
Function getnull/4 is not versionable, reason: not a tree_versionable_function.
Not considering never_ever for cloning; no hot calls.

overall_size: 41, max_new_size: 11001
 - context independent values, size: 5, time_benefit: 1
     Decided to specialize for all known contexts, code not going to grow.
 - context independent values, size: 17, time_benefit: 1
     Decided to specialize for all known contexts, code not going to grow.

IPA lattices after all propagation:

Lattices:
  Node: main/5:
  Node: getnull/4:
  Node: foo/3:
    param [0]: &gi [loc_time: 0, loc_size: 0, prop_time: 0, prop_size: 0]
         ctxs: VARIABLE
         Alignment 4, misalignment 0
        AGGS VARIABLE
    param [1]: VARIABLE
         ctxs: VARIABLE
         Alignment unusable
        AGGS VARIABLE
  Node: mark_cell/2:
    param [0]: &gi [loc_time: 0, loc_size: 0, prop_time: 0, prop_size: 0]
         ctxs: VARIABLE
         Alignment 4, misalignment 0
        AGGS VARIABLE
    param [1]: VARIABLE
         ctxs: VARIABLE
         Alignment unusable
        AGGS VARIABLE
  Node: never_ever/1:
    param [0]: BOTTOM
         ctxs: BOTTOM
         Alignment unusable
        AGGS BOTTOM
    param [1]: BOTTOM
         ctxs: BOTTOM
         Alignment unusable
        AGGS BOTTOM

IPA decision stage:

 - Creating a specialized node of foo/3 for all known contexts.
    replacing param #0 interp with const &gi
 - Creating a specialized node of mark_cell/2 for all known contexts.
    replacing param #0 interp with const &gi
Propagated alignment info for function mark_cell.constprop/8:
  param 0: align: 4, misalign: 0
Propagated alignment info for function foo.constprop/7:
  param 0: align: 4, misalign: 0
Propagated alignment info for function foo/3:
  param 0: align: 4, misalign: 0
Propagated alignment info for function mark_cell/2:
  param 0: align: 4, misalign: 0

IPA constant propagation end

Reclaiming functions: foo/3 mark_cell/2
Reclaiming variables:
Clearing address taken flags:
Symbol table:

mark_cell.constprop.1/8 (mark_cell.constprop) @0x142714190
  Type: function definition analyzed
  Visibility:
  References: gi/0 (addr)
  Referring: 
  Clone of mark_cell/2
  Availability: local
  First run: 0
  Function flags: local
  Called by: foo.constprop.0/7 (1.00 per call) 
  Calls: never_ever/1 
foo.constprop.0/7 (foo.constprop) @0x142714000
  Type: function definition analyzed
  Visibility:
  References: gi/0 (addr)
  Referring: 
  Clone of foo/3
  Availability: local
  First run: 0
  Function flags: local
  Called by: main/5 (99.00 per call) 
  Calls: mark_cell.constprop.1/8 (1.00 per call) 
abort/6 (abort) @0x142603af0
  Type: function
  Visibility: external public
  References: 
  Referring: 
  Availability: not_available
  First run: 0
  Function flags:
  Called by: never_ever/1 (1.00 per call) 
  Calls: 
main/5 (main) @0x142603960
  Type: function definition analyzed
  Visibility: externally_visible public
  References: gi/0 (addr)
  Referring: 
  Availability: available
  First run: 0
  Function flags: body only_called_at_startup executed_once only_called_at_startup
  Called by: 
  Calls: foo.constprop.0/7 (99.00 per call) getnull/4 (99.00 per call) 
getnull/4 (getnull) @0x1426037d0
  Type: function definition analyzed
  Visibility: prevailing_def_ironly
  References: 
  Referring: 
  Availability: local
  First run: 0
  Function flags: body local
  Called by: main/5 (99.00 per call) 
  Calls: 
foo/3 (foo) @0x142603640
  Type: function
  Body removed by symtab_remove_unreachable_nodes
  Visibility: prevailing_def_ironly
  References: 
  Referring: 
  Availability: not_available
  First run: 0
  Function flags: body
  Called by: 
  Calls: 
mark_cell/2 (mark_cell) @0x1426034b0
  Type: function
  Body removed by symtab_remove_unreachable_nodes
  Visibility: prevailing_def_ironly
  References: 
  Referring: 
  Availability: not_available
  First run: 0
  Function flags: body
  Called by: 
  Calls: 
never_ever/1 (never_ever) @0x142603320
  Type: function definition analyzed
  Visibility: externally_visible public
  References: 
  Referring: 
  Availability: available
  First run: 0
  Function flags: body executed_once
  Called by: mark_cell.constprop.1/8 
  Calls: abort/6 (1.00 per call) 
gi/0 (gi) @0x142700000
  Type: variable definition analyzed
  Visibility: externally_visible public common
  References: 
  Referring: main/5 (addr)foo.constprop.0/7 (addr)mark_cell.constprop.1/8 (addr)
  Availability: overwritable
  Varpool flags:
main ()
{
  int i;
  struct Pcc_cell * _4;

  <bb 2>:
  goto <bb 4>;

  <bb 3>:
  _4 = getnull ();
  foo (&gi, _4);
  i_5 = i_1 + 1;

  <bb 4>:
  # i_1 = PHI <0(2), i_5(3)>
  if (i_1 <= 99)
    goto <bb 3>;
  else
    goto <bb 5>;

  <bb 5>:
  return 0;

}


getnull ()
{
  <bb 2>:
  return 0B;

}


never_ever (int * interp, struct PMC * pmc)
{
  <bb 2>:
  abort ();

}



;; Function getnull (getnull, funcdef_no=3, decl_uid=1484, cgraph_uid=3, symbol_order=4)

Modification phase of node getnull/4
getnull ()
{
  <bb 2>:
  return 0B;

}



;; Function never_ever (never_ever, funcdef_no=0, decl_uid=1464, cgraph_uid=0, symbol_order=1) (executed once)

Modification phase of node never_ever/1
never_ever (int * interp, struct PMC * pmc)
{
  <bb 2>:
  abort ();

}



;; Function mark_cell.constprop (mark_cell.constprop.1, funcdef_no=5, decl_uid=1521, cgraph_uid=7, symbol_order=8)

Modification phase of node mark_cell.constprop/8
mark_cell.constprop (int * interp, struct Pcc_cell * c)
{
  int * interp;
  long int _2;
  struct PMC * _3;
  unsigned int _4;
  unsigned int _5;

  <bb 8>:

  <bb 2>:
  if (c_1(D) != 0B)
    goto <bb 3>;
  else
    goto <bb 7>;

  <bb 3>:
  _2 = c_1(D)->type;
  if (_2 == 4)
    goto <bb 4>;
  else
    goto <bb 7>;

  <bb 4>:
  _3 = c_1(D)->p;
  if (_3 != 0B)
    goto <bb 5>;
  else
    goto <bb 7>;

  <bb 5>:
  _4 = _3->flags;
  _5 = _4 & 16384;
  if (_5 == 0)
    goto <bb 6>;
  else
    goto <bb 7>;

  <bb 6>:
  never_ever (&gi, _3);

  <bb 7>:
  return;

}



;; Function foo.constprop (foo.constprop.0, funcdef_no=6, decl_uid=1520, cgraph_uid=6, symbol_order=7)

Modification phase of node foo.constprop/7
foo.constprop (struct Pcc_cell * c)
{
  int * interp;

  <bb 3>:

  <bb 2>:
  mark_cell (&gi, c_1(D));
  return;

}



;; Function main (main, funcdef_no=4, decl_uid=1486, cgraph_uid=4, symbol_order=5) (executed once)

Modification phase of node main/5
main ()
{
  int i;
  struct Pcc_cell * _4;

  <bb 2>:
  goto <bb 4>;

  <bb 3>:
  _4 = getnull ();
  foo (&gi, _4);
  i_5 = i_1 + 1;

  <bb 4>:
  # i_1 = PHI <0(2), i_5(3)>
  if (i_1 <= 99)
    goto <bb 3>;
  else
    goto <bb 5>;

  <bb 5>:
  return 0;

}



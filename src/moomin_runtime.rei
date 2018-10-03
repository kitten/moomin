open Moomin_react_types;

/* Creates a render loop given a root element.
   The loop contains a setup method and a draw method and
   sets up the state for its reconciliation */
let render: elementT => reprocessingLoopT(recNodeT);

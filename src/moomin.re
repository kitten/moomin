open Moomin_react_types;

module Utils = Reprocessing_Utils;
module Constants = Reprocessing_Constants;
module Draw = Reprocessing_Draw;
module Env = Reprocessing_Env;
module Common = Reprocessing_Common;
module Events = Reprocessing_Events;

module Runtime = Moomin_runtime;
module Colors = Moomin_colors;
module ReasonReact = Moomin_react.ReasonReact;
module ReactDOMRe = Moomin_react_dom.ReactDOMRe;

let run = (root: elementT) => {
  let loop = Runtime.render(root);

  Reprocessing.run(
    ~setup=loop.setup,
    ~draw=loop.draw,
    ()
  );
};

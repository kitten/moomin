open Moomin_react_types;

module Utils = Reprocessing.Utils;
module Constants = Reprocessing.Constants;
module Draw = Reprocessing.Draw;
module Env = Reprocessing.Env;
module Events = Reprocessing.Events;

module Std = Moomin_std;
module Runtime = Moomin_runtime;
module Animated = Moomin_animated;
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

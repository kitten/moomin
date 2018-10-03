open Moomin_react;
open Moomin_react_types;
open Moomin_react_dom;

module Setup = {
  let component = ReasonReact.statelessComponent("Std.Setup");

  let make = (~width, ~height, ~background, children: array(elementT)) => {
    ...component,
    initialState: (glEnv) => {
      Reprocessing.Env.size(~width, ~height, glEnv);
      Reprocessing.Draw.background(background, glEnv);
      ()
    },
    render: self => {
      Reprocessing.Draw.background(background, self.glEnv);
      <g>...children</g>
    }
  };
};

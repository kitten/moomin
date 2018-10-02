open Moomin_react;
open Moomin_react_types;

exception UnknownElementTag;

type elementPropsT = {
  fill: option(Reprocessing.colorT),
  stroke: option(Reprocessing.colorT),
  strokeWidth: option(int),
  strokeLinecap: option(Reprocessing.strokeCapT),
  x: float,
  y: float,
  width: float,
  height: float
};

let applyStyles = (props, glEnv) => {
  switch (props.fill) {
  | None => ()
  | Some(fill) => Reprocessing.Draw.fill(fill, glEnv)
  };

  switch (props.stroke) {
  | None => ()
  | Some(stroke) => Reprocessing.Draw.stroke(stroke, glEnv)
  };

  switch (props.strokeWidth) {
  | None => ()
  | Some(weight) => Reprocessing.Draw.strokeWeight(weight, glEnv)
  };

  switch (props.strokeLinecap) {
  | None => ()
  | Some(cap) => Reprocessing.Draw.strokeCap(cap, glEnv)
  };
};

let drawGroup = (props, glEnv) => {
  applyStyles(props, glEnv);
  Reprocessing.Draw.translate(~x=props.x, ~y=props.y, glEnv);
};

let drawRect = (props, glEnv) => {
  applyStyles(props, glEnv);
  Reprocessing.Draw.rectf(
    ~pos=(props.x, props.y),
    ~width=props.width,
    ~height=props.height,
    glEnv
  );
};

module ReactDOMRe = {
  let props = (
    ~fill=?,
    ~stroke=?,
    ~strokeWidth=?,
    ~strokeLinecap=?,
    ~x=0.,
    ~y=0.,
    ~width=0.,
    ~height=0.,
    ()
  ): elementPropsT => {
    fill, stroke, strokeWidth, strokeLinecap,
    x, y, width, height
  };

  let defaultProps = props(());

  let createElement = (tag: string, ~props=defaultProps, children: array(elementT)) => {
    ...ReasonReact.null,
    name: tag,
    render: (~path as _path, ~glEnv) =>
      switch (tag) {
      | "rect" => {
        drawRect(props, glEnv);
        C_NULL
      }
      | "g" => {
        drawGroup(props, glEnv);
        C_MANY(children)
      }
      | x when x === ReasonReact.fragment => C_MANY(children)
      | _ => raise(UnknownElementTag)
      }
  };
};

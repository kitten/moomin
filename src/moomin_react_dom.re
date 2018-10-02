open Moomin_react;
open Moomin_react_types;

exception UnknownElementTag;

type elementPropsT = {
  body: option(string),
  font: option(Reprocessing.fontT),
  image: option(Reprocessing.imageT),
  fill: option(Reprocessing.colorT),
  stroke: option(Reprocessing.colorT),
  strokeWidth: option(int),
  strokeLinecap: option(Reprocessing.strokeCapT),
  rotate: option(float),
  scaleX: option(float),
  scaleY: option(float),
  shearX: option(float),
  shearY: option(float),
  x: float,
  y: float,
  x1: float,
  y1: float,
  x2: float,
  y2: float,
  x3: float,
  y3: float,
  cx: float,
  cy: float,
  rx: float,
  ry: float,
  r: float,
  width: option(float),
  height: option(float)
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

let applyTransforms = (props, glEnv) => {
  Reprocessing.Draw.translate(~x=props.x, ~y=props.y, glEnv);

  switch (props.rotate) {
  | None => ()
  | Some(rotate) => Reprocessing.Draw.rotate(rotate, glEnv)
  };

  switch (props.scaleX, props.scaleY) {
  | (None, None) => ()
  | (Some(x), None) => Reprocessing.Draw.scale(~x, ~y=1., glEnv)
  | (None, Some(y)) => Reprocessing.Draw.scale(~x=1., ~y, glEnv)
  | (Some(x), Some(y)) => Reprocessing.Draw.scale(~x, ~y, glEnv)
  };

  switch (props.shearX, props.shearY) {
  | (None, None) => ()
  | (Some(x), None) => Reprocessing.Draw.shear(~x, ~y=1., glEnv)
  | (None, Some(y)) => Reprocessing.Draw.shear(~x=1., ~y, glEnv)
  | (Some(x), Some(y)) => Reprocessing.Draw.shear(~x, ~y, glEnv)
  };
};

let drawGroup = (props, glEnv) => {
  applyStyles(props, glEnv);
  applyTransforms(props, glEnv);
};

let optionalFloatToInt = (x: option(float)) =>
  switch (x) {
  | Some(x) => Some(int_of_float(x))
  | None => None
  };

let drawImage = (props, glEnv) =>
  switch (props.image) {
  | Some(image) => {
    applyStyles(props, glEnv);
    applyTransforms(props, glEnv);
    Reprocessing.Draw.image(
      image,
      ~pos=(0, 0),
      ~width=?optionalFloatToInt(props.width),
      ~height=?optionalFloatToInt(props.height),
      glEnv
    );
  }
  | None => ()
  };

let drawRect = (props, ~width=0., ~height=0., glEnv) => {
  applyStyles(props, glEnv);
  applyTransforms(props, glEnv);

  Reprocessing.Draw.rectf(
    ~pos=(0., 0.),
    ~width,
    ~height,
    glEnv
  );
};

let drawLine = (props, glEnv) => {
  applyStyles(props, glEnv);
  applyTransforms(props, glEnv);

  Reprocessing.Draw.linef(
    ~p1=(props.x1, props.y1),
    ~p2=(props.x2, props.y2),
    glEnv
  );
};

let drawTriangle = (props, glEnv) => {
  applyStyles(props, glEnv);
  applyTransforms(props, glEnv);

  Reprocessing.Draw.trianglef(
    ~p1=(props.x1, props.y1),
    ~p2=(props.x2, props.y2),
    ~p3=(props.x3, props.y3),
    glEnv
  );
};

let drawEllipse = (props, ~rx, ~ry, glEnv) => {
  applyStyles(props, glEnv);
  applyTransforms(props, glEnv);

  Reprocessing.Draw.ellipsef(
    ~center=(props.cx, props.cy),
    ~radx=rx,
    ~rady=ry,
    glEnv
  );
};

let drawText = (props, glEnv) => {
  switch (props.body) {
  | Some(body) => {
    applyStyles(props, glEnv);
    applyTransforms(props, glEnv);
    Reprocessing.Draw.text(~font=?props.font, ~body, ~pos=(0, 0), glEnv);
  }
  | None => ()
  };
};

module ReactDOMRe = {
  let props = (
    ~body=?, ~font=?, ~image=?, ~fill=?, ~stroke=?,
    ~strokeWidth=?, ~strokeLinecap=?,
    ~rotate=?, ~scaleX=?, ~scaleY=?, ~shearX=?, ~shearY=?,
    ~x1=0., ~y1=0., ~x2=0., ~y2=0., ~x3=0., ~y3=0.,
    ~cx=0., ~cy=0., ~rx=0., ~ry=0., ~x=0., ~y=0.,
    ~r=0., ~width=?, ~height=?,
    ()
  ): elementPropsT => {
    body, font, image, fill, stroke, strokeWidth,
    strokeLinecap, rotate, scaleX, scaleY,
    shearX, shearY, cx, cy, rx, ry, r,
    x1, y1, x2, y2, x3, y3, x, y, width, height
  };

  let defaultProps = props(());

  let createElement = (tag: string, ~props=defaultProps, children: array(elementT)) => {
    ...ReasonReact.null,
    name: tag,
    render: (~path as _path, ~glEnv) =>
      switch (tag) {
      | "rect" => {
        drawRect(props, ~width=?props.width, ~height=?props.height, glEnv);
        C_NULL
      }
      | "image" => {
        drawImage(props, glEnv);
        C_NULL
      }
      | "line" => {
        drawLine(props, glEnv);
        C_NULL
      }
      | "triangle" => {
        drawTriangle(props, glEnv);
        C_NULL
      }
      | "ellipse" => {
        drawEllipse(props, ~rx=props.rx, ~ry=props.ry, glEnv);
        C_NULL
      }
      | "circle" => {
        drawEllipse(props, ~rx=props.r, ~ry=props.r, glEnv);
        C_NULL
      }
      | "text" => {
        drawText(props, glEnv);
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

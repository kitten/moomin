open Moomin;

module TestB = {
  type state = {
    x: float,
    y: float
  };

  let component = ReasonReact.reducerComponent("TestB");

  let make = (~x, ~y, _children) => {
    ...component,
    initialState: _glEnv => {
      x: float_of_int(x),
      y: float_of_int(y)
    },
    reducer: (_action: unit, state) => state,
    willReceiveProps: self => {
      x: self.state.x +. 1.,
      y: self.state.y +. 1.
    },
    render: self => {
      <rect
        x={self.state.x}
        y={self.state.y}
        width={50.}
        height={50.}
        fill={Constants.blue}
      />
    }
  };
};

module Test = {
  let component = ReasonReact.statelessComponent("Test");

  let make = (_children) => {
    ...component,
    render: _self => {
      <>
        <text body="Hello world!" />

        <g fill={Constants.red}>

          <g x={10.} rotate={0.1}>
            <TestB x={10} y={10} />
            <TestB x={10} y={70} />
          </g>

          <line
            x1={150.} y1={0.}
            x2={100.} y2={500.}
            stroke={Constants.blue}
          />

          <ellipse
            cx={100.} cy={100.}
            rx={25.} ry={30.}
          />

          <circle
            cx={100.} cy={160.}
            r={20.}
          />

          <rect shearX={2.} width={50.} height={50.} y={100.} />
        </g>
      </>
    }
  };
};

run(<Test />);

open Moomin;

module Square = {
  type state = {
    rotate: float
  };

  let component = ReasonReact.reducerComponent("TestC");

  let make = (_children) => {
    ...component,
    initialState: _glEnv => { rotate: 0. },
    reducer: (_action: unit, state) => state,
    willReceiveProps: self => { rotate: self.state.rotate +. 0.1 },
    render: self => {
      <rect
        x={5.}
        y={5.}
        width={20.}
        height={20.}
        rotate={self.state.rotate}
        fill={Constants.black}
      />
    }
  };
};

module TestC = {
  let component = ReasonReact.statelessComponent("TestC");

  let make = (~x, ~y, _children) => {
    ...component,
    render: _self => {
      <>
        <rect
          x={x}
          y={y}
          width={50.}
          height={50.}
          fill={Constants.blue}
        />

        {x >= 40. ? <Square /> : ReasonReact.null}
      </>
    }
  };
};

module TestB = {
  type state = {
    x: float,
    y: float,
    move: (float, float)
  };

  let component = ReasonReact.reducerComponent("TestB");

  let make = (~x, ~y, ~magnitude=1., _children) => {
    ...component,
    initialState: _glEnv => {
      x: float_of_int(x),
      y: float_of_int(y),
      move: (magnitude, magnitude)
    },
    reducer: (_action: unit, state) => state,
    willReceiveProps: self => {
      let move = if (self.state.x >= 50. || self.state.x <= 0.) {
        let (moveX, moveY) = self.state.move;
        (moveX *. -1., moveY *. -1.)
      } else {
        self.state.move
      };

      let (moveX, moveY) = move;

      {
        move: move,
        x: self.state.x +. moveX,
        y: self.state.y +. moveY
      }
    },
    render: self => {
      <TestC
        x={self.state.x}
        y={self.state.y}
      />
    }
  };
};

module Test = {
  let component = ReasonReact.statelessComponent("Test");

  let make = (children) => {
    ...component,
    render: _self => {
      <>
        <text body="Hello world!" />

        <g fill={Constants.red}>

          <g x={10.} rotate={0.1}>
            ...children
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

run(
  <Test>
    <TestB x={10} y={10} />
    <TestB x={10} y={70} magnitude={1.5} />
  </Test>
);

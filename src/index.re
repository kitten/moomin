open Moomin;

module TestB = {
  let component = ReasonReact.statelessComponent("TestB");

  let make = (~x, ~y, _children) => {
    ...component,
    render: _self => {
      <rect
        x={float_of_int(x)}
        y={float_of_int(y)}
        width={50.}
        height={50.}
        fill={Constants.green}
      />
    }
  };
};

module Test = {
  let component = ReasonReact.statelessComponent("Test");

  let make = (_children) => {
    ...component,
    render: ({ glEnv }) => {
      Draw.background(Constants.black, glEnv);

      <g x={10.}>
        <TestB x={10} y={10} />
        <TestB x={10} y={70} />

        <rect
          x={70.}
          y={10.}
          width={50.}
          height={50.}
          fill={Constants.blue}
          stroke={Constants.red}
          strokeWidth={2}
        />
      </g>
    }
  };
};

run(<Test />);

open Moomin_react_types;

let makeSelf = (glEnv: Reprocessing.glEnvT, state: 'a) => {
  state: state,
  glEnv: glEnv
};

let getElementKey = (element: elementT, index: int) =>
  element.name ++ "(" ++ switch (element.key) {
  | Some(x) => x
  | None => string_of_int(index)
  } ++ ")";

let getPop = (map: stateMapT('a), key: string, default: 'a) =>
  switch (StateMap.get(map, key)) {
  | Some(res) => {
    StateMap.remove(map, key);
    res
  }
  | None => default
  };

module ReasonReact = {
  let null: elementT = {
    name: "null",
    key: None,
    initialState: (~path as _path, ~glEnv as _glEnv) => (),
    render: (~path as _path, ~glEnv as _glEnv) => C_NULL,
    unmount: (~path as _path) => ()
  };

  let element = (~key=?, component): elementT => {
    let name = component.internal.name;
    let states = component.internal.states;

    {
      name,
      key,
      initialState: (~path, ~glEnv) => {
        let state = component.initialState(glEnv);
        StateMap.set(states, path, state);
      },
      render: (~path, ~glEnv): childrenT => {
        let state = StateMap.getExn(states, path);
        let element = component.render(makeSelf(glEnv, state));
        C_SINGLE(element)
      },
      unmount: (~path) => {
        StateMap.remove(states, path);
      }
    }
  };

  let fragment = "moomin%fragment";

  let statelessComponent = (componentName: string): componentT('state) => {
    internal: {
      name: componentName,
      states: StateMap.make()
    },
    initialState: _glEnv => (),
    render: _self => null
  };
};
